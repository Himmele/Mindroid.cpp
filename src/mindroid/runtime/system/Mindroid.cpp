/*
 * Copyright (C) 2018 E.S.R.Labs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <mindroid/runtime/system/Mindroid.h>
#include <mindroid/runtime/system/Runtime.h>
#include <mindroid/lang/IllegalArgumentException.h>
#include <mindroid/os/Handler.h>
#include <mindroid/os/HandlerThread.h>
#include <mindroid/os/Parcel.h>
#include <mindroid/net/ServerSocket.h>
#include <mindroid/net/Socket.h>
#include <mindroid/net/InetSocketAddress.h>
#include <mindroid/net/URI.h>
#include <mindroid/net/URISyntaxException.h>
#include <mindroid/io/DataInputStream.h>
#include <mindroid/io/DataOutputStream.h>
#include <mindroid/io/IOException.h>
#include <mindroid/io/EOFException.h>
#include <mindroid/util/Log.h>
#include <mindroid/util/concurrent/Executors.h>
#include <mindroid/util/concurrent/Promise.h>
#include <mindroid/util/concurrent/locks/ReentrantLock.h>
#include <mindroid/util/concurrent/locks/Condition.h>

namespace mindroid {

const char* const Mindroid::TAG = "Mindroid";
const sp<String> Mindroid::TIMEOUT = String::valueOf("timeout");
sp<HandlerThread> Mindroid::sThread = nullptr;
sp<Handler> Mindroid::sExecutor = nullptr;

Mindroid::Mindroid() : mLock(new ReentrantLock()) {
}

void Mindroid::start() {
    sThread = new HandlerThread("ThreadPoolExecutorDaemon");
    sThread->start();
    sExecutor = new Handler(sThread->getLooper());

    uint32_t nodeId = mRuntime->getNodeId();
    sp<Configuration> configuration = mRuntime->getConfiguration();
    if (configuration != nullptr) {
        mConfiguration = configuration->plugins->get(String::valueOf("mindroid"));
        if (mConfiguration != nullptr) {
            sp<HashSet<uint64_t>> ids = new HashSet<uint64_t>();
            auto itr = mConfiguration->services->iterator();
            while (itr.hasNext()) {
                auto entry = itr.next();
                sp<Configuration::Service> service = entry.getValue();
                if (service->node->id == nodeId) {
                    uint64_t id = ((uint64_t) nodeId << 32) | (service->id & 0xFFFFFFFFL);
                    ids->add(id);
                } else {
                    try {
                        sp<URI> descriptor = new URI(service->interfaceDescriptor);
                        sp<URI> uri = new URI(String::valueOf("mindroid"), String::format("%u.%u", service->node->id, service->id), String::format("/if=%s", descriptor->getPath()->substring(1)->c_str()), nullptr, nullptr);
                        sp<IBinder> proxy = Binder::Proxy::create(uri);
                        mRuntime->addService(new URI(String::valueOf("mindroid"), service->name, nullptr, nullptr, nullptr), proxy);
                    } catch (const Exception& e) {
                        Log::println('E', TAG, "Binder proxy registration failed: %s", service->name->c_str());
                    }
                }
            }
            mRuntime->addIds(ids);

            sp<Configuration::Node> node = mConfiguration->nodes->get(nodeId);
            try {
                mServer = new Server(mRuntime);
                mServer->start(node->uri);
            } catch (const IOException& e) {
                Log::println('E', TAG, "IOException");
            }
        }
    }
}

void Mindroid::stop() {
    mServer->shutdown();
    sThread->quit();
}

void Mindroid::attachProxy(uint64_t proxyId, const sp<Binder::Proxy>& proxy) {
    AutoLock autoLock(mLock);
    uint32_t nodeId = (uint32_t) ((proxy->getId() >> 32) & 0xFFFFFFFFL);
    if (!mProxies->containsKey(nodeId)) {
        mProxies->put(nodeId, new HashMap<uint64_t, wp<IBinder>>());
    }
    mProxies->get(nodeId)->put(proxyId, proxy);
}

void Mindroid::detachProxy(uint64_t proxyId, uint64_t binderId) {
    // TODO: Lazy connection shutdown for clients without proxies.
//    AutoLock autoLock(mLock);
//    uint32_t nodeId = (uint32_t) ((binderId >> 32) & 0xFFFFFFFFL);
//    if (mProxies->containsKey(nodeId)) {
//        sp<HashMap<uint64_t, wp<IBinder>>> proxies = mProxies->get(nodeId);
//        proxies->remove(proxyId);
//        if (proxies->isEmpty()) {
//            mProxies->remove(nodeId);
//            sp<Client> client = mClients->get(nodeId);
//            if (client != nullptr) {
//                client->shutdown();
//                mClients->remove(nodeId);
//            }
//        }
//    } else {
//        sp<Client> client = mClients->get(nodeId);
//        if (client != nullptr) {
//            client->shutdown();
//            mClients->remove(nodeId);
//        }
//    }
}

sp<Promise<sp<Parcel>>> Mindroid::transact(const sp<IBinder>& binder, int32_t what, const sp<Parcel>& data, int32_t flags) {
    uint32_t nodeId = (uint32_t) ((binder->getId() >> 32) & 0xFFFFFFFFL);
    sp<Client> client = mClients->get(nodeId);
    if (client == nullptr) {
        sp<Configuration::Node> node;
        if (mConfiguration != nullptr && (node = mConfiguration->nodes->get(nodeId)) != nullptr) {
            if (!mClients->containsKey(nodeId)) {
                try {
                    sp<Client> client = new Client(this, node->id);
                    client->start(node->uri);
                    mClients->put(nodeId, client);
                } catch (const IOException& e) {
                    throw RemoteException("Binder transaction failure");
                }
            }
            client = mClients->get(nodeId);
        } else {
            throw RemoteException("Binder transaction failure");
        }
    }
    return client->transact(binder, what, data, flags);
}

void Mindroid::onShutdown(const sp<Client>& client) {
    mClients->remove(client->getNodeId());
}

sp<Mindroid::Message> Mindroid::Message::newMessage(const sp<DataInputStream>& inputStream) {
    int32_t type = inputStream->readInt();
    sp<String> uri = inputStream->readUTF();
    int32_t transactionId = inputStream->readInt();
    int32_t what = inputStream->readInt();
    int32_t size = inputStream->readInt();
    sp<ByteArray> data = new ByteArray(size);
    inputStream->readFully(data, 0, size);
    return new Message(type, uri, transactionId, what, data, size);
}

void Mindroid::Message::write(const sp<DataOutputStream>& outputStream) {
    outputStream->writeInt(this->type);
    outputStream->writeUTF(this->uri);
    outputStream->writeInt(this->transactionId);
    outputStream->writeInt(this->what);
    outputStream->writeInt(this->size);
    outputStream->write(this->data, 0, this->size);
    outputStream->flush();
}

Mindroid::Server::Server(const sp<Runtime>& runtime) : AbstractServer(), mRuntime(runtime) {
}

void Mindroid::Server::onTransact(const sp<Bundle>& context, const sp<InputStream>& inputStream, const sp<OutputStream>& outputStream) {
    if (!context->containsKey("dataInputStream")) {
        sp<DataInputStream> dataInputStream = new DataInputStream(inputStream);
        context->putObject("dataInputStream", dataInputStream);
    }
    if (!context->containsKey("datOutputStream")) {
        sp<DataOutputStream> dataOutputStream = new DataOutputStream(outputStream);
        context->putObject("dataOutputStream", dataOutputStream);
    }
    sp<DataInputStream> dataInputStream = object_cast<DataInputStream>(context->getObject("dataInputStream"));
    sp<DataOutputStream> dataOutputStream = object_cast<DataOutputStream>(context->getObject("dataOutputStream"));

    try {
        sp<Message> message = Message::newMessage(dataInputStream);

        if (message->type == Mindroid::Message::MESSAGE_TYPE_TRANSACTION) {
            try {
                sp<IBinder> binder = mRuntime->getBinder(URI::create(message->uri));
                if (binder != nullptr) {
                    sp<Promise<sp<Parcel>>> result = binder->transact(message->what, Parcel::obtain(message->data), 0);
                    if (result != nullptr) {
                        try {
                            result->then([=] (const sp<Parcel>& value, const sp<Exception>& exception) {
                                AutoLock autoLock(mLock);
                                if (exception == nullptr) {
                                    Message::newMessage(message->uri, message->transactionId, message->what, value->getByteArray(), value->size())->write(dataOutputStream);
                                } else {
                                    Message::newExceptionMessage(message->uri, message->transactionId, message->what, BINDER_TRANSACTION_FAILURE)->write(dataOutputStream);
                                }
                            });
                        } catch (const IOException& e) {
                            try {
                                object_cast<Client::Connection>(context->getObject("connection"))->close();
                            } catch (const IOException& ignore) {
                            }
                        }
                    }
                } else {
                    AutoLock autoLock(mLock);
                    Message::newExceptionMessage(message->uri, message->transactionId, message->what, BINDER_TRANSACTION_FAILURE)->write(dataOutputStream);
                }
            } catch (const IllegalArgumentException& e) {
                Log::e(TAG, "IllegalArgumentException");
                AutoLock autoLock(mLock);
                Message::newExceptionMessage(message->uri, message->transactionId, message->what, BINDER_TRANSACTION_FAILURE)->write(dataOutputStream);
            } catch (const RemoteException& e) {
                Log::e(TAG, "RemoteException");
                AutoLock autoLock(mLock);
                Message::newExceptionMessage(message->uri, message->transactionId, message->what, BINDER_TRANSACTION_FAILURE)->write(dataOutputStream);
            }
        } else {
            Log::e(TAG, "Invalid message type: %d", message->type);
        }
    } catch (const IOException& e) {
        if (DEBUG) {
            Log::e(TAG, "IOException");
        }
        throw e;
    }
}

Mindroid::Client::Client(const sp<Mindroid>& plugin, uint32_t nodeId) : AbstractClient(nodeId),
        mPlugin(plugin),
        mTransactionIdGenerator(new AtomicInteger(1)) {
}

void Mindroid::Client::shutdown() {
    mPlugin->onShutdown(this);
    mPlugin.clear();

    auto itr = mTransactions->iterator();
    while (itr.hasNext()) {
        auto entry = itr.next();
        sp<Promise<sp<Parcel>>> promise = entry.getValue();
        promise->completeWith(sp<Exception>(new RemoteException()));
    }

    sExecutor->post([=] {
        AbstractClient::shutdown();
    });
}

sp<Promise<sp<Parcel>>> Mindroid::Client::transact(const sp<IBinder>& binder, int32_t what, const sp<Parcel>& data, int32_t flags) {
    sp<Bundle> context = getContext();
    if (!context->containsKey("datOutputStream")) {
        sp<DataOutputStream> dataOutputStream = new DataOutputStream(getOutputStream());
        context->putObject("dataOutputStream", dataOutputStream);
    }
    sp<DataOutputStream> dataOutputStream = object_cast<DataOutputStream>(context->getObject("dataOutputStream"));

    const int32_t transactionId = mTransactionIdGenerator->getAndIncrement();
    sp<Promise<sp<Parcel>>> result;
    if (flags == Binder::FLAG_ONEWAY) {
        result = nullptr;
    } else {
        sp<Promise<sp<Parcel>>> promise = new Promise<sp<Parcel>>(Executors::SYNCHRONOUS_EXECUTOR);
        result = promise->orTimeout(data->getLongExtra(Mindroid::TIMEOUT, Mindroid::DEFAULT_TRANSACTION_TIMEOUT))
        ->then([=] (const sp<Parcel>& value, const sp<Exception>& exception) {
            mTransactions->remove(transactionId);
        });
        mTransactions->put(transactionId, promise);
    }
    try {
        AutoLock autoLock(mLock);
        Message::newMessage(binder->getUri()->toString(), transactionId, what, data->getByteArray(), data->size())->write(dataOutputStream);
    } catch (const IOException& e) {
        if (result != nullptr) {
            result->completeWith(e);
            mTransactions->remove(transactionId);
        }
        shutdown();
        throw RemoteException(e);
    }
    return result;
}

void Mindroid::Client::onTransact(const sp<Bundle>& context, const sp<InputStream>& inputStream, const sp<OutputStream>& outputStream) {
    if (!context->containsKey("dataInputStream")) {
        sp<DataInputStream> dataInputStream = new DataInputStream(inputStream);
        context->putObject("dataInputStream", dataInputStream);
    }
    sp<DataInputStream> dataInputStream = object_cast<DataInputStream>(context->getObject("dataInputStream"));

    try {
        sp<Message> message = Message::newMessage(dataInputStream);

        sp<Promise<sp<Parcel>>> promise = mTransactions->get(message->transactionId);
        if (promise != nullptr) {
            mTransactions->remove(message->transactionId);
            if (message->type == Message::MESSAGE_TYPE_TRANSACTION) {
                promise->complete(Parcel::obtain(message->data)->asInput());
            } else {
                promise->completeWith(sp<Exception>(new RemoteException()));
            }
        } else {
            Log::e(TAG, "Invalid transaction id: %d", message->transactionId);
        }
    } catch (const IOException& e) {
        if (DEBUG) {
            Log::e(TAG, "IOException");
        }
        throw e;
    }
}

} /* namespace mindroid */
