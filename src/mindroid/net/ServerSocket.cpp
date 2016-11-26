/*
 * Copyright (C) 2012 Daniel Himmelein
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

#include "mindroid/net/ServerSocket.h"
#include "mindroid/net/Socket.h"
#include "mindroid/net/SocketAddress.h"
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

namespace mindroid {

ServerSocket::ServerSocket() :
        mIsBound(false),
        mIsClosed(false),
        mReuseAddress(false) {
    mSocketId = ::socket(AF_INET, SOCK_STREAM, 0);
}

ServerSocket::ServerSocket(uint16_t port) :
        mIsBound(false),
        mIsClosed(false),
        mReuseAddress(false) {
    mSocketId = ::socket(AF_INET, SOCK_STREAM, 0);
    bind(port);
}

ServerSocket::ServerSocket(uint16_t port, int backlog) :
        mIsBound(false),
        mIsClosed(false),
        mReuseAddress(false) {
    mSocketId = ::socket(AF_INET, SOCK_STREAM, 0);
    bind(port, backlog);
}

ServerSocket::ServerSocket(const sp<String>& host, uint16_t port, int backlog) :
        mIsBound(false),
        mIsClosed(false),
        mReuseAddress(false) {
    mSocketId = ::socket(AF_INET, SOCK_STREAM, 0);
    bind(host, port, backlog);
}

ServerSocket::~ServerSocket() {
    close();
}

bool ServerSocket::bind(uint16_t port, int backlog) {
    return bind(nullptr, port, backlog);
}

bool ServerSocket::bind(const sp<String>& host, uint16_t port, int backlog) {
    if (mIsBound) {
        return false;
    }

    if (mSocketId < 0) {
        return false;
    }

    int value = mReuseAddress;
    setsockopt(mSocketId, SOL_SOCKET, SO_REUSEADDR, (char*) &value, sizeof(value));

    sp<SocketAddress> socketAddress = (host == nullptr) ?
            new SocketAddress(port) : new SocketAddress(host, port);

    if (::bind(mSocketId, (struct sockaddr*) &socketAddress->mSocketAddress, sizeof(socketAddress->mSocketAddress)) == 0) {
        if (::listen(mSocketId, backlog) == 0) {
            mIsBound = true;
            return true;
        } else {
            ::close(mSocketId);
            mSocketId = -1;
            return false;
        }
    } else {
        ::close(mSocketId);
        mSocketId = -1;
        return false;
    }
}

sp<Socket> ServerSocket::accept() {
    sp<Socket> socket = new Socket();
    socket->mSocketId = ::accept(mSocketId, 0, 0);
    if (socket->mSocketId < 0) {
        return nullptr;
    } else {
        socket->mIsConnected = true;
        return socket;
    }
}

void ServerSocket::close() {
    mIsClosed = true;
    mIsBound = false;
    if (mSocketId >= 0) {
        // Unblock calls like accept, etc. -> http://stackoverflow.com/questions/10619952/how-to-completely-destroy-a-socket-connection-in-c
        ::shutdown(mSocketId, SHUT_RDWR);
        ::close(mSocketId);
        mSocketId = -1;
    }
}

void ServerSocket::setReuseAddress(bool reuse) {
    mReuseAddress = reuse;
}

} /* namespace mindroid */
