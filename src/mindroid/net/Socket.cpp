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

#include "mindroid/net/Socket.h"
#include "mindroid/net/SocketAddress.h"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

namespace mindroid {

Socket::Socket() :
        mIsConnected(false),
        mIsClosed(false) {
    mSocketId = ::socket(AF_INET, SOCK_STREAM, 0);
}

Socket::Socket(const sp<String>& host, uint16_t port) :
        mIsConnected(false),
        mIsClosed(false) {
    mSocketId = ::socket(AF_INET, SOCK_STREAM, 0);
    connect(host, port);
}

Socket::~Socket() {
    close();
}

int Socket::connect(const sp<String>& host, uint16_t port) {
    sp<SocketAddress> socketAddress = new SocketAddress(host, port);
    int rc = -1;
    if (mSocketId >= 0 && !socketAddress->isUnresolved()) {
        if ((rc = ::connect(mSocketId, (struct sockaddr*) &socketAddress->mSocketAddress, sizeof(socketAddress->mSocketAddress))) == 0) {
            mIsConnected = true;
            return rc;
        } else {
            return rc;
        }
    } else {
        return rc;
    }
}

ssize_t Socket::read(uint8_t* data, size_t size) {
    return ::recv(mSocketId, reinterpret_cast<char*>(data), size, 0);
}

ssize_t Socket::readFully(uint8_t* data, size_t size) {
    size_t curSize = 0;
    ssize_t result = 0;
    while (curSize < size) {
        result = ::recv(mSocketId, reinterpret_cast<char*>(data + curSize), size - curSize, 0);
        if (result > 0) {
            curSize += result;
        } else {
            break;
        }
    }
    return (result > 0) ? size : result;
}

bool Socket::write(const void* data, size_t size) {
    return (size_t) ::send(mSocketId, reinterpret_cast<const char*>(data), size, 0) == size;
}

void Socket::close() {
    mIsClosed = true;
    mIsConnected = false;
    if (mSocketId >= 0) {
        ::shutdown(mSocketId, SHUT_RDWR);
        ::close(mSocketId);
        mSocketId = -1;
    }
}

} /* namespace mindroid */
