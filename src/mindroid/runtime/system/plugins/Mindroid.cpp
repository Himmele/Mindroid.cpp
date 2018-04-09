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

#include <mindroid/runtime/system/plugins/Mindroid.h>
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
                mServer = new Server();
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
                    sp<Client> client = new Client(node->id, this);
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

void Mindroid::Server::start(const sp<String>& uri) {
    sp<URI> url;
    try {
        url = new URI(uri);
    } catch (const URISyntaxException& e) {
        throw IOException(String::format("Invalid URI: %s", (uri != nullptr) ? uri->c_str() : "nullptr"));
    }

    if (String::valueOf("tcp")->equals(url->getScheme())) {
        try {
            mServerSocket = new ServerSocket();
            mServerSocket->setReuseAddress(true);
            mServerSocket->bind(new InetSocketAddress(InetAddress::getByName(url->getHost()), url->getPort()));

            mThread = new Thread([=] {
                while (!mThread->isInterrupted()) {
                    try {
                        sp<Socket> socket = mServerSocket->accept();
                        if (DEBUG) {
                            Log::d(TAG, "New connection from %s", socket->getRemoteSocketAddress()->toString()->c_str());
                        }
                        sp<Connection> connection = new Connection(socket, this);
                        connection->start();
                        mConnections->add(connection);
                    } catch (const IOException& e) {
                        Log::e(TAG, "IOException");
                    }
                }
            }, String::format("Server [%s]", mServerSocket->getLocalSocketAddress()->toString()->c_str()));
            mThread->start();
        } catch (const IOException& e) {
            Log::e(TAG, "Cannot bind to server socket on port %u", url->getPort());
        }
    } else {
        throw IllegalArgumentException(String::format("Invalid URI scheme: %s", url->getScheme()->c_str()));
    }
}

void Mindroid::Server::shutdown() {
    auto itr = mConnections->iterator();
    while (itr.hasNext()) {
        sp<Connection> connection = itr.next();
        connection->shutdown();
    }
    try {
        mServerSocket->close();
    } catch (const IOException& e) {
        Log::e(TAG, "Cannot close server socket");
    }
    mThread->interrupt();
    mThread->join();
}

Mindroid::Server::Connection::Connection(const sp<Socket>& socket, const sp<Server>& server) :
    mSocket(socket),
    mServer(server) {
}

void Mindroid::Server::Connection::start() {
    try {
        mReader = new Reader(String::format("Server.Reader: %s << %s", mSocket->getLocalSocketAddress()->toString()->c_str(),
                mSocket->getRemoteSocketAddress()->toString()->c_str()),
                mSocket->getInputStream(),
                this);
        mWriter = new Writer(String::format("Server.Writer: %s << %s", mSocket->getLocalSocketAddress()->toString()->c_str(),
                mSocket->getRemoteSocketAddress()->toString()->c_str()),
                mSocket->getOutputStream(),
                this);
        mReader->start();
        mWriter->start();
    } catch (const IOException& e) {
        Log::d(TAG, "Failed to set up connection");
        shutdown();
    }
}

void Mindroid::Server::Connection::shutdown() {
    if (DEBUG) {
        Log::d(TAG, "Disconnecting from %s", mSocket->getRemoteSocketAddress()->toString()->c_str());
    }
    sp<Connection> holder = this;
    mServer->mConnections->remove(this);

    sExecutor->post([=] {
        sp<Connection> connection = holder;
        try {
            mSocket->close();
        } catch (const IOException& e) {
            Log::e(TAG, "Cannot close socket");
        }
        if (mReader != nullptr) {
            try {
                if (DEBUG) {
                    Log::d(TAG, "Shutting down reader");
                }
                mReader->shutdown();
                if (DEBUG) {
                    Log::d(TAG, "Reader has been shut down");
                }
                mReader = nullptr;
            } catch (const Exception& e) {
                Log::e(TAG, "Cannot shutdown reader");
            }
        }
        if (mWriter != nullptr) {
            try {
                if (DEBUG) {
                    Log::d(TAG, "Shutting down writer");
                }
                mWriter->shutdown();
                if (DEBUG) {
                    Log::d(TAG, "Writer has been shut down");
                }
                mWriter = nullptr;
            } catch (const Exception& e) {
                Log::e(TAG, "Cannot shutdown writer");
            }
        }
        connection.clear();
    });

    mServer.clear();
}

Mindroid::Server::Connection::Reader::Reader(const sp<String>& name, const sp<InputStream>& inputStream, const sp<Connection>& connection) :
        Thread(name),
        mConnection(connection) {
    mInputStream = new DataInputStream(inputStream);
}

void Mindroid::Server::Connection::Reader::shutdown() {
    interrupt();
    try {
        mInputStream->close();
    } catch (const IOException& ignore) {
    }
    join();
    mConnection.clear();
}

void Mindroid::Server::Connection::Reader::run() {
    sp<Runtime> runtime = Runtime::getRuntime();

    while (!isInterrupted()) {
        try {
            int32_t type = mInputStream->readInt();
            sp<String> uri = mInputStream->readUTF();
            int32_t transactionId = mInputStream->readInt();
            int32_t what = mInputStream->readInt();
            int32_t size = mInputStream->readInt();
            sp<ByteArray> data = new ByteArray(size);
            mInputStream->readFully(data, 0, size);

            if (type == Mindroid::Message::MESSAGE_TYPE_TRANSACTION) {
                try {
                    sp<IBinder> binder = runtime->getBinder(URI::create(uri));
                    if (binder != nullptr) {
                        sp<Promise<sp<Parcel>>> result = binder->transact(what, Parcel::obtain(data), 0);
                        if (result != nullptr) {
                            result->then([=] (const sp<Parcel>& value, const sp<Exception>& exception) {
                                if (exception == nullptr) {
                                    mConnection->mWriter->write(Message::newMessage(uri, transactionId, what, value->toByteArray()));
                                } else {
                                    mConnection->mWriter->write(Message::newExceptionMessage(uri, transactionId, what, mConnection->BINDER_TRANSACTION_FAILURE));
                                }
                            });
                        }
                    } else {
                        mConnection->mWriter->write(Message::newExceptionMessage(uri, transactionId, what, mConnection->BINDER_TRANSACTION_FAILURE));
                    }
                } catch (const IllegalArgumentException& e) {
                    Log::e(TAG, "IllegalArgumentException");
                    mConnection->mWriter->write(Message::newExceptionMessage(uri, transactionId, what, mConnection->BINDER_TRANSACTION_FAILURE));
                } catch (const RemoteException& e) {
                    Log::e(TAG, "RemoteException");
                    mConnection->mWriter->write(Message::newExceptionMessage(uri, transactionId, what, mConnection->BINDER_TRANSACTION_FAILURE));
                }
            } else {
                Log::e(TAG, "Invalid message type: %d", type);
            }
        } catch (const IOException& e) {
            if (DEBUG) {
                Log::e(TAG, "IOException");
            }
            mConnection->shutdown();
            break;
        }
    }

    if (DEBUG) {
        Log::d(TAG, "Reader is terminating");
    }
}

Mindroid::Server::Connection::Writer::Writer(const sp<String>& name, const sp<OutputStream>& outputStream, const sp<Connection>& connection) :
        Thread(name),
        mConnection(connection) {
    mOutputStream = new DataOutputStream(outputStream);
    mLock = new ReentrantLock();
    mCondition = mLock->newCondition();
}

void Mindroid::Server::Connection::Writer::shutdown() {
    interrupt();
    {
        AutoLock autoLock(mLock);
        mCondition->signal();
    }
    try {
        mOutputStream->close();
    } catch (const IOException& ignore) {
    }
    join();
    mConnection.clear();
}

void Mindroid::Server::Connection::Writer::write(const sp<Mindroid::Message>& message) {
    AutoLock autoLock(mLock);
    mQueue->add(message);
    mCondition->signal();
}

void Mindroid::Server::Connection::Writer::run() {
    sp<Message> message;

    while (!isInterrupted()) {
        {
            AutoLock autoLock(mLock);
            while (!isInterrupted() && mQueue->isEmpty()) {
                mCondition->await();
            }
            if (isInterrupted()) {
                break;
            }

            message = mQueue->get(0);
            mQueue->remove(0);
        }

        try {
            mOutputStream->writeInt(message->type);
            mOutputStream->writeUTF(message->uri);
            mOutputStream->writeInt(message->transactionId);
            mOutputStream->writeInt(message->what);
            mOutputStream->writeInt(message->data->size());
            mOutputStream->write(message->data);
            mOutputStream->flush();
        } catch (const IOException& e) {
            Log::e(TAG, "IOException");
            mConnection->shutdown();
            break;
        }
    }

    if (DEBUG) {
        Log::d(TAG, "Writer is terminating");
    }
}

Mindroid::Client::Client(uint32_t nodeId, const sp<Mindroid>& plugin) :
            mNodeId(nodeId),
            mPlugin(plugin),
            mTransactionIdGenerator(new AtomicInteger(1)) {
}

void Mindroid::Client::start(const sp<String>& uri) {
    try {
        sp<URI> url = new URI(uri);
        if (!(String::valueOf("tcp")->equals(url->getScheme()))) {
            throw IllegalArgumentException(String::format("Invalid URI scheme: %s", url->getScheme()->c_str()));
        }
        mHost = url->getHost();
        mPort = url->getPort();
        mSocket = new Socket();
        mConnection = new Connection();

        mThread = new Thread([=] {
            try {
                mSocket->connect(new InetSocketAddress(mHost, mPort));
                mConnection->start(mSocket, this);
            } catch (const IOException& e) {
                Log::e(TAG, "IOException");
                shutdown();
            }
        }, String::format("Client [%s]", uri->c_str()));
        mThread->start();
    } catch (const URISyntaxException& e) {
        throw IOException(String::format("Invalid URI: %s", (uri != nullptr) ? uri->c_str() : "nullptr"));
    }
}

void Mindroid::Client::shutdown() {
    sp<Client> holder = this;
    mPlugin->onShutdown(this);

    sExecutor->post([=] {
        sp<Client> client = holder;
        if (mConnection != nullptr) {
            mConnection->shutdown();
        }

        mThread->interrupt();
        mThread->join();

        auto itr = mTransactions->iterator();
        while (itr.hasNext()) {
            auto entry = itr.next();
            sp<Promise<sp<Parcel>>> promise = entry.getValue();
            promise->completeWith(sp<Exception>(new RemoteException()));
        }
        client.clear();
    });
}

sp<Promise<sp<Parcel>>> Mindroid::Client::transact(const sp<IBinder>& binder, int32_t what, const sp<Parcel>& data, int32_t flags) {
    const int32_t transactionId = mTransactionIdGenerator->getAndIncrement();
    sp<Promise<sp<Parcel>>> result;
    if (flags == Binder::FLAG_ONEWAY) {
        result = nullptr;
    } else {
        result = new Promise<sp<Parcel>>(Executors::SYNCHRONOUS_EXECUTOR);
        mTransactions->put(transactionId, result);
    }
    mConnection->mWriter->write(Message::newMessage(binder->getUri()->toString(), transactionId, what, data->toByteArray()));
    return result;
}

Mindroid::Client::Connection::Connection() {
    mReader = new Reader();
    mWriter = new Writer();
}

void Mindroid::Client::Connection::start(const sp<Socket>& socket, const sp<Client>& client) {
    mSocket = socket;
    mClient = client;
    try {
        mReader->start(String::format("Client.Reader: %s << %s", mSocket->getLocalSocketAddress()->toString()->c_str(),
                mSocket->getRemoteSocketAddress()->toString()->c_str()),
                socket->getInputStream(),
                this);
        mWriter->start(String::format("Client.Writer: %s << %s", mSocket->getLocalSocketAddress()->toString()->c_str(),
                mSocket->getRemoteSocketAddress()->toString()->c_str()),
                socket->getOutputStream(),
                this);
    } catch (const IOException& e) {
        Log::d(TAG, "Failed to set up connection");
        mClient->shutdown();
    }
}

void Mindroid::Client::Connection::shutdown() {
    if (mSocket != nullptr) {
        try {
            mSocket->close();
        } catch (const IOException& e) {
            Log::e(TAG, "Cannot close socket");
        }
    }
    if (mReader != nullptr) {
        try {
            if (DEBUG) {
                Log::d(TAG, "Shutting down reader");
            }
            mReader->shutdown();
            if (DEBUG) {
                Log::d(TAG, "Reader has been shut down");
            }
        } catch (const Exception& e) {
            Log::e(TAG, "Cannot shutdown reader");
        }
    }
    if (mWriter != nullptr) {
        try {
            if (DEBUG) {
                Log::d(TAG, "Shutting down writer");
            }
            mWriter->shutdown();
            if (DEBUG) {
                Log::d(TAG, "Writer has been shut down");
            }
        } catch (const Exception& e) {
            Log::e(TAG, "Cannot shutdown writer");
        }
    }
    mClient.clear();
}

void Mindroid::Client::Connection::Reader::start(const sp<String>& name, const sp<InputStream>& inputStream, const sp<Connection>& connection) {
    Thread::setName(name);
    mConnection = connection;
    mInputStream = new DataInputStream(inputStream);
    Thread::start();
}

void Mindroid::Client::Connection::Reader::shutdown() {
    interrupt();
    if (mInputStream != nullptr) {
        try {
            mInputStream->close();
        } catch (const IOException& ignore) {
        }
    }
    join();
    mConnection.clear();
}

void Mindroid::Client::Connection::Reader::run() {
    while (!isInterrupted()) {
        try {
            int32_t type = mInputStream->readInt();
            sp<String> uri = mInputStream->readUTF();
            int32_t transactionId = mInputStream->readInt();
            __attribute__((unused)) int32_t what = mInputStream->readInt();
            int32_t size = mInputStream->readInt();
            sp<ByteArray> data = new ByteArray(size);
            mInputStream->readFully(data, 0, size);

            sp<Promise<sp<Parcel>>> promise = mConnection->mClient->mTransactions->get(transactionId);
            if (promise != nullptr) {
                if (type == Message::MESSAGE_TYPE_TRANSACTION) {
                    promise->complete(Parcel::obtain(data)->asInput());
                } else {
                    promise->completeWith(sp<Exception>(new RemoteException()));
                }
                mConnection->mClient->mTransactions->remove(transactionId);
            } else {
                Log::e(TAG, "Invalid transaction id: %d", transactionId);
            }
        } catch (const IOException& e) {
            if (DEBUG) {
                Log::e(TAG, "IOException");
            }
            mConnection->mClient->shutdown();
            break;
        }
    }

    if (DEBUG) {
        Log::d(TAG, "Reader is terminating");
    }
}

Mindroid::Client::Connection::Writer::Writer() :
        mLock(new ReentrantLock()),
        mCondition(mLock->newCondition()) {
}

void Mindroid::Client::Connection::Writer::start(const sp<String>& name, const sp<OutputStream>& outputStream, const sp<Connection>& connection) {
    setName(name);
    mConnection = connection;
    mOutputStream = new DataOutputStream(outputStream);
    Thread::start();
}

void Mindroid::Client::Connection::Writer::shutdown() {
    interrupt();
    {
        AutoLock autoLock(mLock);
        mCondition->signal();
    }
    if (mOutputStream != nullptr) {
        try {
            mOutputStream->close();
        } catch (const IOException& ignore) {
        }
    }
    join();
    mConnection.clear();
}

void Mindroid::Client::Connection::Writer::write(const sp<Mindroid::Message>& message) {
    AutoLock autoLock(mLock);
    mQueue->add(message);
    mCondition->signal();
}

void Mindroid::Client::Connection::Writer::run() {
    sp<Message> message;

    while (!isInterrupted()) {
        {
            AutoLock autoLock(mLock);
            while (!isInterrupted() && mQueue->isEmpty()) {
                mCondition->await();
            }
            if (isInterrupted()) {
                break;
            }

            message = mQueue->get(0);
            mQueue->remove(0);
        }

        try {
            mOutputStream->writeInt(message->type);
            mOutputStream->writeUTF(message->uri);
            mOutputStream->writeInt(message->transactionId);
            mOutputStream->writeInt(message->what);
            mOutputStream->writeInt(message->data->size());
            mOutputStream->write(message->data);
            mOutputStream->flush();
        } catch (const IOException& e) {
            Log::e(TAG, "IOException");
            mConnection->mClient->shutdown();
            break;
        }
    }

    if (DEBUG) {
        Log::d(TAG, "Writer is terminating");
    }
}

} /* namespace mindroid */
