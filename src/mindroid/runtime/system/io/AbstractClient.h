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

#ifndef MINDROID_RUNTIME_SYSTEM_IO_ABSTRACTCLIENT_H_
#define MINDROID_RUNTIME_SYSTEM_IO_ABSTRACTCLIENT_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/lang/ByteArray.h>
#include <mindroid/lang/Thread.h>
#include <mindroid/io/InputStream.h>
#include <mindroid/io/OutputStream.h>
#include <mindroid/os/Binder.h>
#include <mindroid/util/HashMap.h>

namespace mindroid {

class Bundle;
class Socket;

class AbstractClient : public Object {
public:
    static const char* TAG;

    AbstractClient(uint32_t nodeId);
    void start(const sp<String>& uri);
    virtual void shutdown();

    uint32_t getNodeId() {
        return mNodeId;
    }

    virtual void onTransact(const sp<Bundle>& context, const sp<InputStream>& inputStream, const sp<OutputStream>& outputStream) = 0;

    class Connection : public Thread {
    public:
        Connection(const sp<Socket>& socket, const sp<AbstractClient>& client);
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
        sp<AbstractClient> mClient;
        sp<InputStream> mInputStream;
        sp<OutputStream> mOutputStream;

        friend class AbstractClient;
    };

    sp<Bundle> getContext() {
        return mConnection->mContext;
    }

    sp<InputStream> getInputStream() {
        return mConnection->mInputStream;
    }

    sp<OutputStream> getOutputStream() {
        return mConnection->mOutputStream;
    }

protected:
    static const bool DEBUG = false;

private:
    uint32_t mNodeId;
    sp<Socket> mSocket;
    sp<String> mHost;
    int32_t mPort = -1;
    sp<Connection> mConnection;
};

} /* namespace mindroid */

#endif /* MINDROID_RUNTIME_SYSTEM_IO_ABSTRACTCLIENT_H_ */
