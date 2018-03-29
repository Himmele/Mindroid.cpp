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

#include <mindroid/net/ServerSocket.h>
#include <mindroid/net/Socket.h>
#include <mindroid/net/Inet4Address.h>
#include <mindroid/net/Inet6Address.h>
#include <mindroid/net/InetSocketAddress.h>
#include <mindroid/net/SocketException.h>
#include <mindroid/io/IOException.h>
#include <mindroid/lang/Class.h>
#include <mindroid/lang/NullPointerException.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace mindroid {

ServerSocket::ServerSocket() {
}

ServerSocket::ServerSocket(uint16_t port) {
    bind(port, DEFAULT_BACKLOG, nullptr);
}

ServerSocket::ServerSocket(uint16_t port, int32_t backlog) {
    bind(port, backlog, nullptr);
}

ServerSocket::ServerSocket(uint16_t port, int32_t backlog, const sp<InetAddress>& localAddress) {
    bind(port, backlog, localAddress);
}

ServerSocket::~ServerSocket() {
    close();
}

void ServerSocket::close() {
    mIsClosed = true;
    mIsBound = false;
    if (mFd != -1) {
        // Unblock calls like accept, etc. -> http://stackoverflow.com/questions/10619952/how-to-completely-destroy-a-socket-connection-in-c
        ::shutdown(mFd, SHUT_RDWR);
        ::close(mFd);
        mFd = -1;
    }
}

void ServerSocket::bind(const sp<InetSocketAddress>& localAddress, int32_t backlog) {
    if (localAddress == nullptr) {
        throw NullPointerException();
    }
    if (localAddress->getAddress() == nullptr) {
        throw SocketException(String::format("Host is unresolved: %s", localAddress->getHostName()->c_str()));
    }
    bind((uint16_t) localAddress->getPort(), backlog, localAddress->getAddress());
}

void ServerSocket::bind(uint16_t port, int32_t backlog, const sp<InetAddress>& localAddress) {
    if (mIsBound) {
        throw SocketException("Socket is already bound");
    }
    if (mIsClosed) {
        throw SocketException("Socket is already closed");
    }

    sp<InetAddress> address;
    if (localAddress == nullptr) {
        address = Inet6Address::ANY;
    } else {
        address = localAddress;
    }

    sockaddr_storage ss;
    socklen_t saSize = 0;
    std::memset(&ss, 0, sizeof(ss));
    if (Class<Inet6Address>::isInstance(address)) {
        mFd = ::socket(AF_INET6, SOCK_STREAM, 0);
        int32_t value = 0;
        ::setsockopt(mFd, SOL_SOCKET, IPV6_V6ONLY, &value, sizeof(value));
        value = mReuseAddress;
        ::setsockopt(mFd, SOL_SOCKET, SO_REUSEADDR, (char*) &value, sizeof(value));

        sockaddr_in6& sin6 = reinterpret_cast<sockaddr_in6&>(ss);
        sin6.sin6_family = AF_INET6;
        std::memcpy(&sin6.sin6_addr.s6_addr, address->getAddress()->c_arr(), 16);
        sin6.sin6_port = htons(port);
        saSize = sizeof(sockaddr_in6);
    } else {
        mFd = ::socket(AF_INET, SOCK_STREAM, 0);
        int32_t value = mReuseAddress;
        ::setsockopt(mFd, SOL_SOCKET, SO_REUSEADDR, (char*) &value, sizeof(value));

        sockaddr_in& sin = reinterpret_cast<sockaddr_in&>(ss);
        sin.sin_family = AF_INET;
        std::memcpy(&sin.sin_addr.s_addr, address->getAddress()->c_arr(), 4);
        sin.sin_port = htons(port);
        saSize = sizeof(sockaddr_in);
    }
    if (::bind(mFd, (struct sockaddr*) &ss, saSize) != 0) {
        close();
    }
    if (mFd != -1 && ::listen(mFd, backlog) == 0) {
        mIsBound = true;
    } else {
        close();
        throw SocketException(String::format("Failed to bind to port %u (errno=%d)", port, errno));
    }
}

sp<Socket> ServerSocket::accept() {
    if (!isBound()) {
        throw SocketException("Socket is not bound");
    }

    sp<Socket> socket = new Socket();
    int32_t rc = ::accept(mFd, 0, 0);
    if (rc < 0) {
        throw IOException();
    } else {
        socket->mFd = rc;
        socket->mIsConnected = true;
        return socket;
    }
}

void ServerSocket::setReuseAddress(bool reuse) {
    mReuseAddress = reuse;
}

} /* namespace mindroid */
