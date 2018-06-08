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

#ifndef MINDROID_RUNTIME_SYSTEM_PLUGINS_MINDROID_H_
#define MINDROID_RUNTIME_SYSTEM_PLUGINS_MINDROID_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/lang/ByteArray.h>
#include <mindroid/lang/Thread.h>
#include <mindroid/os/Binder.h>
#include <mindroid/runtime/system/Plugin.h>
#include <mindroid/runtime/system/Configuration.h>
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
    static const uint64_t BINDER_TRANSACTION_TIMEOUT = 10000;

    Mindroid();
    virtual ~Mindroid() = default;

    void start() override;

    void stop() override;

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

    class Message : public Object {
    public:
        static const int32_t MESSAGE_TYPE_TRANSACTION = 1;
        static const int32_t MESSAGE_TYPE_EXCEPTION_TRANSACTION = 2;

        Message(int32_t type, const sp<String>& uri, int32_t transactionId, int32_t what, const sp<ByteArray>& data) :
                type(type),
                uri(uri),
                transactionId(transactionId),
                what(what),
                data(data) {
        }

        static sp<Message> newMessage(const sp<String>& uri, int32_t transactionId, int32_t what, const sp<ByteArray>& data) {
            return new Message(MESSAGE_TYPE_TRANSACTION, uri, transactionId, what, data);
        }

        static sp<Message> newExceptionMessage(const sp<String>& uri, int32_t transactionId, int32_t what, const sp<ByteArray>& data) {
            return new Message(MESSAGE_TYPE_EXCEPTION_TRANSACTION, uri, transactionId, what, data);
        }

        int32_t type;
        sp<String> uri;
        int32_t transactionId;
        int32_t what;
        sp<ByteArray> data;
    };

    class Server : public Object {
    public:
        Server() = default;
        void start(const sp<String>& uri);
        void shutdown();

        class Connection : public Object {
        public:
            Connection(const sp<Socket>& socket, const sp<Server>& server);
            void start();
            void shutdown();

            class Reader : public Thread {
            public:
                Reader(const sp<String>& name, const sp<InputStream>& inputStream, const sp<Connection>& connection);
                void shutdown();
                void run() override;

            private:
                sp<Connection> mConnection;
                sp<DataInputStream> mInputStream;
            };

            class Writer : public Thread {
            public:
                Writer(const sp<String>& name, const sp<OutputStream>& outputStream, const sp<Connection>& connection);
                void shutdown();
                void write(const sp<Mindroid::Message>& message);
                void run() override;

            private:
                sp<Connection> mConnection;
                sp<DataOutputStream> mOutputStream;
                sp<LinkedList<sp<Message>>> mQueue = new LinkedList<sp<Message>>();
                sp<Lock> mLock;
                sp<Condition> mCondition;
            };

        private:
            sp<ByteArray> BINDER_TRANSACTION_FAILURE = String::valueOf("Binder transaction failure")->getBytes();
            sp<Socket> mSocket;
            sp<Server> mServer;
            sp<Reader> mReader;
            sp<Writer> mWriter;
        };

    private:
       static const bool DEBUG = false;

       sp<Thread> mThread;
       sp<ServerSocket> mServerSocket;
       // FIXME: Control concurrent access to collections.
       sp<HashSet<sp<Connection>>> mConnections = new HashSet<sp<Connection>>();
    };

    class Client : public Object {
    public:
        Client(uint32_t nodeId, const sp<Mindroid>& plugin);
        void start(const sp<String>& uri);
        void shutdown();

        sp<Promise<sp<Parcel>>> transact(const sp<IBinder>& binder, int32_t what, const sp<Parcel>& data, int32_t flags);

        uint32_t getNodeId() {
            return mNodeId;
        }

        class Connection : public Object {
        public:
            Connection();
            void start(const sp<Socket>& socket, const sp<Client>& client);
            void shutdown();

            class Reader : public Thread {
            public:
                Reader() = default;
                void start(const sp<String>& name, const sp<InputStream>& inputStream, const sp<Connection>& connection);
                void shutdown();
                void run() override;

            private:
                sp<Connection> mConnection;
                sp<DataInputStream> mInputStream;
            };

            class Writer : public Thread {
            public:
                Writer();
                void start(const sp<String>& name, const sp<OutputStream>& outputStream, const sp<Connection>& connection);
                void shutdown();
                void write(const sp<Mindroid::Message>& message);
                void run() override;

            private:
                sp<Connection> mConnection;
                sp<DataOutputStream> mOutputStream;
                sp<LinkedList<sp<Mindroid::Message>>> mQueue = new LinkedList<sp<Mindroid::Message>>();
                sp<Lock> mLock;
                sp<Condition> mCondition;
            };

        private:
            sp<Socket> mSocket;
            sp<Client> mClient;
            sp<Reader> mReader;
            sp<Writer> mWriter;

            friend class Client;
        };

    private:
        static const bool DEBUG = false;

        uint32_t mNodeId;
        sp<Thread> mThread;
        sp<Socket> mSocket;
        sp<String> mHost;
        int32_t mPort = -1;
        sp<Connection> mConnection;
        sp<Mindroid> mPlugin;
        sp<AtomicInteger> mTransactionIdGenerator;
        // FIXME: Control concurrent access to collections.
        sp<HashMap<int32_t, sp<Promise<sp<Parcel>>>>> mTransactions = new HashMap<int32_t, sp<Promise<sp<Parcel>>>>();
    };

private:
    void onShutdown(const sp<Client>& client);

    sp<Configuration::Plugin> mConfiguration;
    sp<Server> mServer;
    // FIXME: Control concurrent access to collections.
    sp<HashMap<uint32_t, sp<Client>>> mClients = new HashMap<uint32_t, sp<Client>>();
    sp<HashMap<uint32_t, sp<HashMap<uint64_t, wp<IBinder>>>>> mProxies = new HashMap<uint32_t, sp<HashMap<uint64_t, wp<IBinder>>>>();
    sp<Lock> mLock;
    static sp<HandlerThread> sThread;
    static sp<Handler> sExecutor;
};

} /* namespace mindroid */

#endif /* MINDROID_RUNTIME_SYSTEM_PLUGINS_MINDROID_H_ */
