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

#include <mindroid/net/Socket.h>
#include <mindroid/net/SocketAddress.h>
#include <mindroid/net/InetAddress.h>
#include <mindroid/net/Inet4Address.h>
#include <mindroid/net/Inet6Address.h>
#include <mindroid/net/InetSocketAddress.h>
#include <mindroid/net/SocketException.h>
#include <mindroid/io/IOException.h>
#include <mindroid/lang/Class.h>
#include <mindroid/lang/NullPointerException.h>
#include <mindroid/lang/IndexOutOfBoundsException.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace mindroid {

Socket::Socket(const sp<String>& host, uint16_t port) {
    sp<InetAddress> inetAddress = InetAddress::getByName(host);
    connect(new InetSocketAddress(inetAddress, port));
}

Socket::~Socket() {
    close();
}

void Socket::close() {
    mIsClosed = true;
    mIsConnected = false;
    if (mFd != -1) {
        ::shutdown(mFd, SHUT_RDWR);
        ::close(mFd);
        mFd = -1;
    }
}

void Socket::connect(const sp<InetSocketAddress>& socketAddress) {
    if (mIsConnected) {
        throw SocketException("Already connected");
    }
    if (mIsClosed) {
        throw SocketException("Already closed");
    }

    sockaddr_storage ss;
    socklen_t saSize = 0;
    std::memset(&ss, 0, sizeof(ss));
    sp<InetAddress> inetAddress = socketAddress->getAddress();
    if (Class<Inet6Address>::isInstance(inetAddress)) {
        mFd = ::socket(AF_INET6, SOCK_STREAM, 0);
        sockaddr_in6& sin6 = reinterpret_cast<sockaddr_in6&>(ss);
        sin6.sin6_family = AF_INET6;
        std::memcpy(&sin6.sin6_addr.s6_addr, inetAddress->getAddress()->c_arr(), 16);
        sin6.sin6_port = htons(socketAddress->getPort());
        saSize = sizeof(sockaddr_in6);
    } else {
        mFd = ::socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in& sin = reinterpret_cast<sockaddr_in&>(ss);
        sin.sin_family = AF_INET;
        std::memcpy(&sin.sin_addr.s_addr, inetAddress->getAddress()->c_arr(), 4);
        sin.sin_port = htons(socketAddress->getPort());
        saSize = sizeof(sockaddr_in);
    }
    if (::connect(mFd, (struct sockaddr*) &ss, saSize) == 0) {
        mIsConnected = true;
    } else {
        close();
        throw SocketException(String::format("Failed to connect to %s (errno=%d)", inetAddress->toString()->c_str(), errno));
    }
}

sp<InputStream> Socket::getInputStream() {
    if (!mIsConnected) {
        throw IOException("Socket is not connected");
    }
    return new SocketInputStream(this);
}

sp<OutputStream> Socket::getOutputStream() {
    if (!mIsConnected) {
        throw IOException("Socket is not connected");
    }
    return new SocketOutputStream(this);
}

size_t Socket::SocketInputStream::available() {
    return 0;
}

int32_t Socket::SocketInputStream::read() {
    if (mFd == -1) {
        throw IOException("Socket already closed");
    }

    uint8_t data;
    ssize_t rc = ::recv(mFd, reinterpret_cast<char*>(&data), sizeof(data), 0);
    if (rc < 0) {
        throw IOException(String::format("Failed to read from socket (errno=%d)", errno));
    } else {
        return rc != 0 ? rc : -1;
    }
}

ssize_t Socket::SocketInputStream::read(const sp<ByteArray>& buffer, size_t offset, size_t count) {
    if (buffer == nullptr) {
        throw NullPointerException();
    }
    if ((offset + count) > buffer->size()) {
        throw IndexOutOfBoundsException();
    }
    if (mFd == -1) {
        throw IOException("Socket already closed");
    }

    ssize_t rc = ::recv(mFd, reinterpret_cast<char*>(buffer->c_arr() + offset), count, 0);
    if (rc < 0) {
        throw IOException(String::format("Failed to read from socket (errno=%d)", errno));
    } else {
        return rc != 0 ? rc : -1;
    }
}

void Socket::SocketOutputStream::write(int32_t b) {
    if (mFd == -1) {
        throw IOException("Socket already closed");
    }

    uint8_t data = (uint8_t) b;
    ssize_t rc = ::send(mFd, reinterpret_cast<const char*>(&data), sizeof(data), 0);
    if (rc < 0 || ((size_t) rc) != sizeof(data)) {
        throw IOException(String::format("Failed to write to socket (errno=%d)", (rc < 0) ? errno : -1));
    }
}

void Socket::SocketOutputStream::write(const sp<ByteArray>& buffer, size_t offset, size_t count) {
    if (buffer == nullptr) {
        throw NullPointerException();
    }
    if ((offset + count) > buffer->size()) {
        throw IndexOutOfBoundsException();
    }
    if (mFd == -1) {
        throw IOException("Socket already closed");
    }

    if (count == 0) {
        return;
    }
    ssize_t rc = ::send(mFd, reinterpret_cast<const char*>(buffer->c_arr() + offset), count, 0);
    if (rc < 0 || ((size_t) rc) != count) {
        throw IOException(String::format("Failed to write to socket (errno=%d)", (rc < 0) ? errno : -1));
    }
}

} /* namespace mindroid */
