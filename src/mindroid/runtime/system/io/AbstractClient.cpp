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

#include <mindroid/runtime/system/io/AbstractClient.h>
#include <mindroid/lang/IllegalArgumentException.h>
#include <mindroid/net/URI.h>
#include <mindroid/net/URISyntaxException.h>
#include <mindroid/io/IOException.h>
#include <mindroid/util/Log.h>
#include <mindroid/util/concurrent/Executors.h>
#include <mindroid/util/concurrent/Promise.h>

namespace mindroid {

const char* AbstractClient::TAG = "Client";

AbstractClient::AbstractClient(uint32_t nodeId) :
            mNodeId(nodeId),
            mSocket(new Socket()) {
}

void AbstractClient::start(const sp<String>& uri) {
    TAG = "Client";
    try {
        sp<URI> url = new URI(uri);
        if (!(String::valueOf("tcp")->equals(url->getScheme()))) {
            throw IllegalArgumentException(String::format("Invalid URI scheme: %s", url->getScheme()->c_str()));
        }
        mHost = url->getHost();
        mPort = url->getPort();

        try {
            mSocket->connect(new InetSocketAddress(mHost, mPort));
            mConnection = new Connection(mSocket, this);
            onConnected();
        } catch (const IOException& e) {
            Log::e(TAG, "IOException");
            shutdown(new IOException(e));
            throw;
        }
    } catch (const URISyntaxException& e) {
        throw IOException(String::format("Invalid URI: %s", (uri != nullptr) ? uri->c_str() : "nullptr"));
    }
}

void AbstractClient::shutdown(const sp<Exception>& cause) {
    if (mConnection != nullptr) {
        try {
            mConnection->close();
        } catch (const IOException& ignore) {
        }
    }
    mIsClosed = true;

    onDisconnected(cause);
}

AbstractClient::Connection::Connection(const sp<Socket>& socket, const sp<AbstractClient>& client) :
        mContext(new Bundle()) {
    setName(String::format("Client [%s <<>> %s]", socket->getLocalSocketAddress()->toString(), socket->getRemoteSocketAddress()->toString()));
    mContext->putObject("connection", this);
    mSocket = socket;
    mClient = client;
    try {
        mInputStream = mSocket->getInputStream();
        mOutputStream = mSocket->getOutputStream();
        mRemoteSocketAddress = mSocket->getRemoteSocketAddress();
    } catch (const IOException& e) {
        Log::d(TAG, "Failed to set up connection");
        try {
            close();
        } catch (const IOException& ignore) {
        }
        throw;
    }
    Thread::start();
}

void AbstractClient::Connection::close() {
    mContext->clear();
    if (DEBUG) {
        Log::d(TAG, "Closing connection");
    }
    interrupt();
    if (mInputStream != nullptr) {
        try {
            mInputStream->close();
        } catch (const IOException& ignore) {
        }
    }
    if (mOutputStream != nullptr) {
        try {
            mOutputStream->close();
        } catch (const IOException& ignore) {
        }
    }
    if (!mSocket->isClosed()) {
        try {
            mSocket->shutdownInput();
        } catch (const IOException& ignore) {
        }
        try {
            mSocket->shutdownOutput();
        } catch (const IOException& ignore) {
        }
    }
    try {
        mSocket->close();
    } catch (const IOException& e) {
        Log::e(TAG, "Cannot close socket");
    }
    join();
    mClient.clear();
    if (DEBUG) {
        Log::d(TAG, "Connection has been closed");
    }
}

void AbstractClient::Connection::run() {
    while (!isInterrupted()) {
        try {
            mClient->onTransact(mContext, mInputStream, mOutputStream);
        } catch (const IOException& e) {
            if (DEBUG) {
                Log::e(TAG, "IOException");
            }
            mClient->shutdown(new IOException(e));
            break;
        }
    }

    if (DEBUG) {
        Log::d(TAG, "Connection has been terminated");
    }
}

} /* namespace mindroid */
