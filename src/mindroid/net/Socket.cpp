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
#include <mindroid/net/StandardSocketOptions.h>
#include <mindroid/io/IOException.h>
#include <mindroid/lang/Class.h>
#include <mindroid/lang/NullPointerException.h>
#include <mindroid/lang/IndexOutOfBoundsException.h>
#include <mindroid/lang/UnsupportedOperationException.h>
#include <mindroid/util/Assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

namespace mindroid {

Socket::Socket(const sp<String>& host, uint16_t port) :
        mLocalAddress(Inet6Address::ANY) {
    sp<InetAddress> inetAddress = InetAddress::getByName(host);
    connect(new InetSocketAddress(inetAddress, port));
}

Socket::~Socket() {
    close();
}

void Socket::close() {
    close(true);
}

void Socket::close(bool shutdown) {
    mIsClosed = true;
    mIsConnected = false;
    mLocalAddress = Inet6Address::ANY;
    if (mFd != -1) {
        if (shutdown) {
            ::shutdown(mFd, SHUT_RDWR);
        }
        ::close(mFd);
        mFd = -1;
    }
}

void Socket::bind(const sp<InetSocketAddress>& socketAddress) {
    if (mIsBound) {
        throw SocketException("Socket is already bound");
    }
    if (mIsClosed) {
        throw SocketException("Socket is already closed");
    }

    sp<InetSocketAddress> bindAddress;
    if (socketAddress == nullptr) {
        bindAddress = new InetSocketAddress(Inet6Address::ANY, 0);
    } else {
        bindAddress = socketAddress;
    }

    sp<InetAddress> address = bindAddress->getAddress();

    sockaddr_storage ss;
    socklen_t saSize = 0;
    std::memset(&ss, 0, sizeof(ss));
    if (Class<Inet6Address>::isInstance(address)) {
        mFd = ::socket(AF_INET6, SOCK_STREAM | SOCK_CLOEXEC, 0);
        int32_t value = 0;
        ::setsockopt(mFd, SOL_SOCKET, IPV6_V6ONLY, &value, sizeof(value));

        sockaddr_in6& sin6 = reinterpret_cast<sockaddr_in6&>(ss);
        sin6.sin6_family = AF_INET6;
        std::memcpy(&sin6.sin6_addr.s6_addr, address->getAddress()->c_arr(), 16);
        sin6.sin6_port = htons(bindAddress->getPort());
        saSize = sizeof(sockaddr_in6);
    } else {
        mFd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);

        sockaddr_in& sin = reinterpret_cast<sockaddr_in&>(ss);
        sin.sin_family = AF_INET;
        std::memcpy(&sin.sin_addr.s_addr, address->getAddress()->c_arr(), 4);
        sin.sin_port = htons(bindAddress->getPort());
        saSize = sizeof(sockaddr_in);
    }

    setDefaultSocketOptions();

    if (::bind(mFd, (struct sockaddr*) &ss, saSize) != 0) {
        int bindErrno = errno; // Save errno before closing.
        close();
        throw SocketException(String::format("Failed to bind to port %u: %s (errno=%d)",
                bindAddress->getPort(), strerror(bindErrno), bindErrno));
    }
    mIsBound = true;
}

void Socket::connect(const sp<InetSocketAddress>& socketAddress) {
    if (mIsConnected) {
        throw SocketException("Already connected");
    }
    if (mIsClosed) {
        throw SocketException("Already closed");
    }

    sp<InetAddress> inetAddress = socketAddress->getAddress();

    if (mFd == -1) {
        Assert::assertFalse(mIsBound);
        if (Class<Inet6Address>::isInstance(inetAddress)) {
            mFd = ::socket(AF_INET6, SOCK_STREAM | SOCK_CLOEXEC, 0);
        } else {
            mFd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
        }
    }

    sockaddr_storage ss;
    socklen_t saSize = 0;
    std::memset(&ss, 0, sizeof(ss));
    if (Class<Inet6Address>::isInstance(inetAddress)) {
        sp<Inet6Address> inet6Address = Class<Inet6Address>::cast(inetAddress);
        sockaddr_in6& sin6 = reinterpret_cast<sockaddr_in6&>(ss);
        sin6.sin6_family = AF_INET6;
        std::memcpy(&sin6.sin6_addr.s6_addr, inet6Address->getAddress()->c_arr(), 16);
        sin6.sin6_port = htons(socketAddress->getPort());
        sin6.sin6_scope_id = inet6Address->getScopeId();
        saSize = sizeof(sockaddr_in6);
    } else {
        sockaddr_in& sin = reinterpret_cast<sockaddr_in&>(ss);
        sin.sin_family = AF_INET;
        std::memcpy(&sin.sin_addr.s_addr, inetAddress->getAddress()->c_arr(), 4);
        sin.sin_port = htons(socketAddress->getPort());
        saSize = sizeof(sockaddr_in);
    }

#ifdef __APPLE__
    const int32_t value = 1;
    const int32_t rc = setsockopt(mFd, SOL_SOCKET, SO_NOSIGPIPE, (void*) &value, sizeof(int32_t));
    if (rc < 0) {
        close();
        throw SocketException(String::format("Failed to set socket option SO_NOSIGPIPE (errno=%d)", errno));
    }
#endif

    if (::connect(mFd, (struct sockaddr*) &ss, saSize) == 0) {
        mInetAddress = socketAddress->getAddress();
        mPort = socketAddress->getPort();

        sockaddr_storage ss;
        sockaddr* sa = reinterpret_cast<sockaddr*>(&ss);
        socklen_t saSize = sizeof(ss);
        std::memset(&ss, 0, saSize);
        int32_t rc = ::getsockname(mFd, sa, &saSize);
        if (rc == 0) {
            switch (ss.ss_family) {
            case AF_INET6: {
                const sockaddr_in6& sin6 = *reinterpret_cast<const sockaddr_in6*>(&ss);
                const void* ipAddress = &sin6.sin6_addr.s6_addr;
                size_t ipAddressSize = 16;
                int32_t scope_id = sin6.sin6_scope_id;
                sp<ByteArray> ba = new ByteArray((const uint8_t*) ipAddress, ipAddressSize);
                mLocalAddress = new Inet6Address(ba, nullptr, scope_id);
                mLocalPort = ntohs(sin6.sin6_port);
                break;
            }
            case AF_INET: {
                const sockaddr_in& sin = *reinterpret_cast<const sockaddr_in*>(&ss);
                const void* ipAddress = &sin.sin_addr.s_addr;
                size_t ipAddressSize = 4;
                sp<ByteArray> ba = new ByteArray((const uint8_t*) ipAddress, ipAddressSize);
                mLocalAddress = new Inet4Address(ba, nullptr);
                mLocalPort = ntohs(sin.sin_port);
                break;
            }
            default:
                break;
            }
        }
        mIsBound = true;
        mIsConnected = true;
    } else {
        int connectErrno = errno; // Save errno before closing.
        close();
        throw SocketException(String::format("Failed to connect to %s: %s (errno=%d)", inetAddress->toString()->c_str(),
                    strerror(connectErrno), connectErrno));
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

sp<InetAddress> Socket::getLocalAddress() const {
    return mLocalAddress;
}

int32_t Socket::getLocalPort() const {
    if (!isBound()) {
        return -1;
    }
    return mLocalPort;
}

sp<InetAddress> Socket::getInetAddress() const {
    if (!isConnected()) {
        return nullptr;
    }
    return mInetAddress;
}

int32_t Socket::getPort() const {
    if (!isConnected()) {
        return 0;
    }
    return mPort;
}

sp<InetSocketAddress> Socket::getLocalSocketAddress() const {
    if (!isBound()) {
        return nullptr;
    }
    return new InetSocketAddress(getLocalAddress(), getLocalPort());
}

sp<InetSocketAddress> Socket::getRemoteSocketAddress() const {
    if (!isConnected()) {
        return nullptr;
    }
    return new InetSocketAddress(getInetAddress(), getPort());
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
    ssize_t rc;
    do {
#ifndef __APPLE__
        rc = ::send(mFd, reinterpret_cast<const char*>(&data), 1, MSG_NOSIGNAL);
#else
        rc = ::send(mFd, reinterpret_cast<const char*>(&data), 1, 0);
#endif
        if (rc < 0) {
            throw IOException(String::format("Failed to write to socket (errno=%d)", (rc < 0) ? errno : -1));
        }
    } while (rc == 0);
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

    ssize_t rc;
    do {
#ifndef __APPLE__
        rc = ::send(mFd, reinterpret_cast<const char*>(buffer->c_arr() + offset), count, MSG_NOSIGNAL);
#else
        rc = ::send(mFd, reinterpret_cast<const char*>(buffer->c_arr() + offset), count, 0);
#endif
        if (rc >= 0) {
            offset += (size_t) rc;
            count -= (size_t) rc;
        } else {
            throw IOException(String::format("Failed to write to socket (errno=%d)", (rc < 0) ? errno : -1));
        }
    } while (count > 0);
}

void Socket::setTcpNoDelay(bool enabled) {
    if (mFd == -1) {
        throw SocketException("Socket is closed");
    }

    const int32_t value = enabled ? 1 : 0;
    const int32_t rc = setsockopt(mFd, IPPROTO_TCP, TCP_NODELAY, (void*) &value, sizeof(value));
    if (rc != 0) {
        throw SocketException();
    }
}

bool Socket::getTcpNoDelay() const {
    if (mFd == -1) {
        throw SocketException("Socket is closed");
    }

    int32_t value;
    socklen_t size = sizeof(int32_t);
    const int32_t rc = getsockopt(mFd, IPPROTO_TCP, TCP_NODELAY, (void*) &value, &size);
    if (rc != 0) {
        throw SocketException();
    }
    return value != 0;
}

void Socket::setSoLinger(bool on, int32_t linger) {
    if (mFd == -1) {
        throw SocketException("Socket is closed");
    }

    struct linger sl;
    sl.l_onoff = on ? 1 : 0;
    sl.l_linger = linger; // Timeout period in seconds.
    const int32_t rc = setsockopt(mFd, SOL_SOCKET, SO_LINGER, &sl, sizeof(sl));
    if (rc != 0) {
        throw SocketException();
    }
}

int32_t Socket::getSoLinger() const {
    if (mFd == -1) {
        throw SocketException("Socket is closed");
    }

    struct linger sl;
    socklen_t size = sizeof(sl);
    const int32_t rc = getsockopt(mFd, SOL_SOCKET, SO_LINGER, (void*) &sl, &size);
    if (rc != 0) {
        throw SocketException();
    }
    return (sl.l_onoff != 0) ? sl.l_linger : -1;
}

void Socket::shutdownInput() {
    if (isInputShutdown()) {
        throw SocketException("Input has already been shutdown");
    }
    if (mFd == -1) {
        throw SocketException("Socket is closed");
    }

    const int32_t rc = ::shutdown(mFd, SHUT_RD);
    if (rc != 0) {
        throw SocketException(String::format("Failed to shutdown input: %s (errno=%d)", strerror(errno), errno));
    }

    mIsInputShutdown = true;
}

void Socket::shutdownOutput() {
    if (isOutputShutdown()) {
        throw SocketException("Output has already been shutdown");
    }
    if (mFd == -1) {
        throw SocketException("Socket is closed");
    }

    const int32_t rc = ::shutdown(mFd, SHUT_WR);
    if (rc != 0) {
        throw SocketException(String::format("Failed to shutdown output: %s (errno=%d)", strerror(errno), errno));
    }

    mIsOutputShutdown = true;
}

void Socket::setDefaultSocketOptions() {
    int32_t value = mReuseAddress;
    ::setsockopt(mFd, SOL_SOCKET, SO_REUSEADDR, (char*) &value, sizeof(value));

    value = mReusePort;
    ::setsockopt(mFd, SOL_SOCKET, SO_REUSEPORT, (char*) &value, sizeof(value));
}

template<>
void Socket::setOption<bool>(const SocketOption<bool>& name, bool value) {
    if (name == StandardSocketOptions::REUSE_PORT) {
        mReusePort = value;
    } else if (name == StandardSocketOptions::REUSE_ADDRESS) {
        mReuseAddress = value;
    } else {
        throw UnsupportedOperationException("Unknown socket option");
    }
}

template<>
bool Socket::getOption<bool>(const SocketOption<bool>& name) {
    if (name == StandardSocketOptions::REUSE_PORT) {
        return mReusePort;
    } else if (name == StandardSocketOptions::REUSE_ADDRESS) {
        return mReuseAddress;
    } else {
        throw UnsupportedOperationException("Unknown socket option");
    }
}

} /* namespace mindroid */
