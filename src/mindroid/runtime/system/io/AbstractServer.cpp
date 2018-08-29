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

#include <mindroid/runtime/system/io/AbstractServer.h>
#include <mindroid/lang/IllegalArgumentException.h>
#include <mindroid/net/ServerSocket.h>
#include <mindroid/net/Socket.h>
#include <mindroid/net/InetSocketAddress.h>
#include <mindroid/net/URI.h>
#include <mindroid/net/URISyntaxException.h>
#include <mindroid/io/InputStream.h>
#include <mindroid/io/OutputStream.h>
#include <mindroid/io/IOException.h>
#include <mindroid/util/Log.h>
#include <mindroid/util/concurrent/Executors.h>
#include <mindroid/util/concurrent/Promise.h>

namespace mindroid {

const char* AbstractServer::TAG = "Server";

void AbstractServer::start(const sp<String>& uri) {
    TAG = "Server";
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
                        mConnections->add(new Connection(socket, this));
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

void AbstractServer::shutdown() {
    auto itr = mConnections->iterator();
    while (itr.hasNext()) {
        sp<Connection> connection = itr.next();
        try {
            connection->close();
        } catch (const IOException& ignore) {
        }
        itr.remove();
    }

    try {
        mServerSocket->close();
    } catch (const IOException& e) {
        Log::e(TAG, "Cannot close server socket");
    }
    mThread->interrupt();
    mThread->join();
}

AbstractServer::Connection::Connection(const sp<Socket>& socket, const sp<AbstractServer>& server) :
        mContext(new Bundle()),
        mSocket(socket),
        mServer(server) {
    mContext->putObject("connection", this);
    try {
        mInputStream = mSocket->getInputStream();
        mOutputStream = mSocket->getOutputStream();
    } catch (const IOException& e) {
        Log::d(TAG, "Failed to set up connection");
        try {
            close();
        } catch (IOException ignore) {
        }
        throw e;
    }
    Thread::start();
}

void AbstractServer::Connection::close() {
    mContext->clear();
    if (DEBUG) {
        Log::d(TAG, "Disconnecting from %s", mSocket->getRemoteSocketAddress()->toString()->c_str());
    }

    interrupt();
    try {
        mSocket->close();
    } catch (const IOException& e) {
        Log::e(TAG, "Cannot close socket");
    }
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
    join();
    mServer.clear();
    if (DEBUG) {
        Log::d(TAG, "Connection has been closed");
    }
}

void AbstractServer::Connection::run() {
    while (!isInterrupted()) {
        try {
            mServer->onTransact(mContext, mInputStream, mOutputStream);
        } catch (const IOException& e) {
            if (DEBUG) {
                Log::e(TAG, "IOException");
            }
            mServer->mConnections->remove(this);
            try {
                close();
            } catch (IOException ignore) {
            }
            break;
        }
    }

    if (DEBUG) {
        Log::d(TAG, "Connection has been terminated");
    }
}

} /* namespace mindroid */
