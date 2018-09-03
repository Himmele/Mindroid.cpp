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

#ifndef MINDROID_RUNTIME_SYSTEM_IO_ABSTRACTSERVER_H_
#define MINDROID_RUNTIME_SYSTEM_IO_ABSTRACTSERVER_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/lang/ByteArray.h>
#include <mindroid/lang/Thread.h>
#include <mindroid/os/Binder.h>
#include <mindroid/util/HashMap.h>

namespace mindroid {

class Bundle;
class ServerSocket;
class Socket;
class InputStream;
class OutputStream;

class AbstractServer : public Object {
public:
    static const char* TAG;

    AbstractServer() = default;
    void start(const sp<String>& uri);
    void shutdown();

    virtual void onTransact(const sp<Bundle>& context, const sp<InputStream>& inputStream, const sp<OutputStream>& outputStream) = 0;

    class Connection : public Thread {
    public:
        Connection(const sp<Socket>& socket, const sp<AbstractServer>& server);
        void close();
        void run() override;

        sp<Bundle> getContext() {
            return mContext;
        }

        sp<InputStream> getInputStream() {
            return mInputStream;
        }

        sp<OutputStream> getOutputStream() {
            return mOutputStream;
        }

    private:
        sp<Bundle> mContext;
        sp<Socket> mSocket;
        sp<AbstractServer> mServer;
        sp<InputStream> mInputStream;
        sp<OutputStream> mOutputStream;
    };

protected:
   static const bool DEBUG = false;

private:
   sp<ServerSocket> mServerSocket;
   sp<Thread> mThread;
   // FIXME: Fix concurrent access to collections.
   sp<HashSet<sp<Connection>>> mConnections = new HashSet<sp<Connection>>();
};

} /* namespace mindroid */

#endif /* MINDROID_RUNTIME_SYSTEM_IO_ABSTRACTSERVER_H_ */
