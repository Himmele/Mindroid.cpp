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

#ifndef MINDROID_RUNTIME_SYSTEM_MINDROID_H_
#define MINDROID_RUNTIME_SYSTEM_MINDROID_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/lang/ByteArray.h>
#include <mindroid/os/Binder.h>
#include <mindroid/runtime/system/Plugin.h>
#include <mindroid/runtime/system/io/AbstractServer.h>
#include <mindroid/runtime/system/io/AbstractClient.h>
#include <mindroid/runtime/system/ServiceDiscoveryConfigurationReader.h>
#include <mindroid/util/HashMap.h>
#include <mindroid/util/HashSet.h>
#include <mindroid/util/LinkedList.h>

namespace mindroid {

class Thread;
class ServerSocket;
class Socket;
class InputStream;
class OutputStream;
class DataInputStream;
class DataOutputStream;
class Lock;
class Condition;
class AtomicInteger;
class Handler;
class HandlerThread;

class Mindroid : public Plugin {
public:
    static const char* const TAG;
    static const sp<String> TIMEOUT;
    static const uint64_t DEFAULT_TRANSACTION_TIMEOUT = 10000;

    Mindroid();
    virtual ~Mindroid() = default;

    sp<Promise<sp<Void>>> start(const sp<URI>& uri, const sp<Bundle>& extras) override;
    sp<Promise<sp<Void>>> stop(const sp<URI>& uri, const sp<Bundle>& extras) override;

    void attachBinder(const sp<Binder>& binder) override {
    }

    void detachBinder(uint64_t id) override {
    }

    void attachProxy(uint64_t proxyId, const sp<Binder::Proxy>& proxy) override;
    void detachProxy(uint64_t proxyId, uint64_t binderId) override;

    sp<Binder> getStub(const sp<Binder>& service) override {
        return nullptr;
    }

    sp<IInterface> getProxy(const sp<IBinder>& binder) override {
        return nullptr;
    }

    sp<Promise<sp<Parcel>>> transact(const sp<IBinder>& binder, int32_t what, const sp<Parcel>& data, int32_t flags) override;
    void link(const sp<IBinder>& binder, const sp<IBinder::Supervisor>& supervisor, const sp<Bundle>& extras) override;
    bool unlink(const sp<IBinder>& binder, const sp<IBinder::Supervisor>& supervisor, const sp<Bundle>& extras) override;
    sp<Promise<sp<Void>>> connect(const sp<URI>& node, const sp<Bundle>& extras) override;
    sp<Promise<sp<Void>>> disconnect(const sp<URI>& node, const sp<Bundle>& extras) override;

    class Message : public Object {
    public:
        static const int32_t MESSAGE_TYPE_TRANSACTION = 1;
        static const int32_t MESSAGE_TYPE_EXCEPTION_TRANSACTION = 2;
        static const int32_t MAX_MESSAGE_SIZE = 64 * 1024 * 1024; //64MB

        Message(int32_t type, const sp<String>& uri, int32_t transactionId, int32_t what, const sp<ByteArray>& data, size_t size) :
                type(type),
                uri(uri),
                transactionId(transactionId),
                what(what),
                data(data),
                size(size) {
        }

        static sp<Message> newMessage(const sp<String>& uri, int32_t transactionId, int32_t what, const sp<ByteArray>& data) {
            return newMessage(uri, transactionId, what, data, data->size());
        }

        static sp<Message> newMessage(const sp<String>& uri, int32_t transactionId, int32_t what, const sp<ByteArray>& data, size_t size) {
            return new Message(MESSAGE_TYPE_TRANSACTION, uri, transactionId, what, data, size);
        }

        static sp<Message> newExceptionMessage(const sp<String>& uri, int32_t transactionId, int32_t what, const sp<ByteArray>& data) {
            return newExceptionMessage(uri, transactionId, what, data, data->size());
        }

        static sp<Message> newExceptionMessage(const sp<String>& uri, int32_t transactionId, int32_t what, const sp<ByteArray>& data, size_t size) {
            return new Message(MESSAGE_TYPE_EXCEPTION_TRANSACTION, uri, transactionId, what, data, size);
        }

        static sp<Message> newMessage(const sp<DataInputStream>& inputStream);

        void write(const sp<DataOutputStream>& outputStream);

        int32_t type;
        sp<String> uri;
        int32_t transactionId;
        int32_t what;
        sp<ByteArray> data;
        size_t size;
    };

    class Server : public AbstractServer {
    public:
        Server(const sp<Runtime>& runtime);
        void onConnected(const sp<AbstractServer::Connection>& connection) override;
        void onDisconnected(const sp<AbstractServer::Connection>& connection, const sp<Exception>& cause) override;
        void onTransact(const sp<Bundle>& context, const sp<InputStream>& inputStream, const sp<OutputStream>& outputStream) override;

    private:
        const sp<ByteArray> BINDER_TRANSACTION_FAILURE = String::valueOf("Binder transaction failure")->getBytes();
        sp<Runtime> mRuntime;
        sp<ReentrantLock> mLock = new ReentrantLock();
    };

    class Client : public AbstractClient {
    public:
        Client(const sp<Mindroid>& plugin, uint32_t nodeId);
        void shutdown(const sp<Exception>& cause) override;

        sp<Promise<sp<Parcel>>> transact(const sp<IBinder>& binder, int32_t what, const sp<Parcel>& data, int32_t flags);
        void onConnected() override;
        void onDisconnected(const sp<Exception>& cause) override;
        void onTransact(const sp<Bundle>& context, const sp<InputStream>& inputStream, const sp<OutputStream>& outputStream) override;

    private:
        sp<Mindroid> mPlugin;
        sp<AtomicInteger> mTransactionIdGenerator;
        // FIXME: Fix concurrent access to collections.
        sp<HashMap<int32_t, sp<Promise<sp<Parcel>>>>> mTransactions = new HashMap<int32_t, sp<Promise<sp<Parcel>>>>();
        sp<ReentrantLock> mLock = new ReentrantLock();
    };

private:
    void onShutdown(const sp<Client>& client);

    sp<ServiceDiscoveryConfigurationReader::Configuration> mConfiguration;
    sp<Server> mServer;
    // FIXME: Fix concurrent access to collections.
    sp<HashMap<uint32_t, sp<Client>>> mClients = new HashMap<uint32_t, sp<Client>>();
    sp<HashMap<uint32_t, sp<HashMap<uint64_t, wp<IBinder>>>>> mProxies = new HashMap<uint32_t, sp<HashMap<uint64_t, wp<IBinder>>>>();
    sp<Lock> mLock;
    static sp<HandlerThread> sThread;
    static sp<Handler> sExecutor;
};

} /* namespace mindroid */

#endif /* MINDROID_RUNTIME_SYSTEM_MINDROID_H_ */
