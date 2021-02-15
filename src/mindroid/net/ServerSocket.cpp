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
        // Unblock accept calls with shutdown: https://stackoverflow.com/questions/11191028/boostasio-how-to-interrupt-a-blocked-tcp-server-thread/62356967
        ::shutdown(mFd, SHUT_RD);
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

    if (localAddress == nullptr) {
        mLocalAddress = Inet6Address::ANY;
    } else {
        mLocalAddress = localAddress;
    }

    sockaddr_storage ss;
    socklen_t saSize = 0;
    std::memset(&ss, 0, sizeof(ss));
    if (Class<Inet6Address>::isInstance(mLocalAddress)) {
        if ((mFd = ::socket(AF_INET6, SOCK_STREAM | SOCK_CLOEXEC, 0)) == -1) {
            throw SocketException(String::format("Failed to open server socket: %s (errno=%d)",
                    strerror(errno), errno));
        }
        int32_t value = 0;
        if (::setsockopt(mFd, IPPROTO_IPV6, IPV6_V6ONLY, &value, sizeof(value)) != 0) {
            throw SocketException(String::format("Failed to set IPV6_V6ONLY socket option: %s (errno=%d)",
                    strerror(errno), errno));
        }
        setCommonSocketOptions();

        sockaddr_in6& sin6 = reinterpret_cast<sockaddr_in6&>(ss);
        sin6.sin6_family = AF_INET6;
        std::memcpy(&sin6.sin6_addr.s6_addr, mLocalAddress->getAddress()->c_arr(), 16);
        sin6.sin6_port = htons(port);
        saSize = sizeof(sockaddr_in6);
    } else {
        if ((mFd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0)) == -1) {
            throw SocketException(String::format("Failed to open server socket: %s (errno=%d)",
                    strerror(errno), errno));
        }
        setCommonSocketOptions();

        sockaddr_in& sin = reinterpret_cast<sockaddr_in&>(ss);
        sin.sin_family = AF_INET;
        std::memcpy(&sin.sin_addr.s_addr, mLocalAddress->getAddress()->c_arr(), 4);
        sin.sin_port = htons(port);
        saSize = sizeof(sockaddr_in);
    }
    if (::bind(mFd, (struct sockaddr*) &ss, saSize) != 0) {
        int bindErrno = errno; // Save errno before closing.
        close();
        throw SocketException(String::format("Failed to bind to port %u: %s (errno=%d)", port,
                strerror(bindErrno), bindErrno));
    }
    if (::listen(mFd, backlog) != 0) {
        int listenErrno = errno; // Save errno before closing.
        close();
        throw SocketException(String::format("Failed to listen on port %u: %s (errno=%d)", port,
                strerror(listenErrno), listenErrno));
    }
    mIsBound = true;
    if (port != 0) {
        mLocalPort = port;
    } else {
        sockaddr_storage ss;
        sockaddr* sa = reinterpret_cast<sockaddr*>(&ss);
        socklen_t saSize = sizeof(ss);
        std::memset(&ss, 0, saSize);
        int32_t rc = ::getsockname(mFd, sa, &saSize);
        if (rc == 0) {
            switch (ss.ss_family) {
            case AF_INET6: {
                const sockaddr_in6& sin6 = *reinterpret_cast<const sockaddr_in6*>(&ss);
                mLocalPort = ntohs(sin6.sin6_port);
                break;
            }
            case AF_INET: {
                const sockaddr_in& sin = *reinterpret_cast<const sockaddr_in*>(&ss);
                mLocalPort = ntohs(sin.sin_port);
                break;
            }
            default:
                mLocalPort = -1;
                break;
            }
        }
    }
}

void ServerSocket::setCommonSocketOptions() {
    int32_t value = mReuseAddress;
    if (::setsockopt(mFd, SOL_SOCKET, SO_REUSEADDR, (char*) &value, sizeof(value)) != 0) {
        throw SocketException(String::format("Failed to set SO_REUSEADDR socket option: %s (errno=%d)",
                strerror(errno), errno));
    }

    value = mReusePort;
    if (::setsockopt(mFd, SOL_SOCKET, SO_REUSEPORT, (char*) &value, sizeof(value)) != 0) {
        throw SocketException(String::format("Failed to set SO_REUSEPORT socket option: %s (errno=%d)",
                strerror(errno), errno));
    }
}

sp<Socket> ServerSocket::accept() {
    if (!isBound()) {
        throw SocketException("Socket is not bound");
    }

    sp<Socket> socket = new Socket();
    int32_t rc = ::accept4(mFd, 0, 0, SOCK_CLOEXEC);
    if (rc < 0) {
        int acceptErrno = errno;
        throw IOException(String::format("ServerSocket on port %u closed: %s (errno=%d)", mLocalPort,
                strerror(acceptErrno), acceptErrno));
    } else {
        socket->mFd = rc;
        socket->mLocalAddress = mLocalAddress;
        socket->mLocalPort = mLocalPort;
        socket->mIsBound = true;
        socket->mIsConnected = true;

        sockaddr_storage ss;
        sockaddr* sa = reinterpret_cast<sockaddr*>(&ss);
        socklen_t saSize = sizeof(ss);
        std::memset(&ss, 0, saSize);
        int32_t rc = ::getpeername(socket->mFd, sa, &saSize);
        if (rc == 0) {
            switch (ss.ss_family) {
            case AF_INET6: {
                const sockaddr_in6& sin6 = *reinterpret_cast<const sockaddr_in6*>(&ss);
                const void* ipAddress = &sin6.sin6_addr.s6_addr;
                size_t ipAddressSize = 16;
                int32_t scope_id = sin6.sin6_scope_id;
                sp<ByteArray> ba = new ByteArray((const uint8_t*) ipAddress, ipAddressSize);
                socket->mInetAddress = new Inet6Address(ba, nullptr, scope_id);
                socket->mPort = ntohs(sin6.sin6_port);
                break;
            }
            case AF_INET: {
                const sockaddr_in& sin = *reinterpret_cast<const sockaddr_in*>(&ss);
                const void* ipAddress = &sin.sin_addr.s_addr;
                size_t ipAddressSize = 4;
                sp<ByteArray> ba = new ByteArray((const uint8_t*) ipAddress, ipAddressSize);
                socket->mInetAddress = new Inet4Address(ba, nullptr);
                socket->mPort = ntohs(sin.sin_port);
                break;
            }
            default:
                break;
            }
        }
        return socket;
    }
}

sp<InetAddress> ServerSocket::getInetAddress() const {
    if (!isBound()) {
        return nullptr;
    }
    return mLocalAddress;
}

int32_t ServerSocket::getLocalPort() const {
    if (!isBound()) {
        return -1;
    }
    return mLocalPort;
}

sp<InetSocketAddress> ServerSocket::getLocalSocketAddress() const {
    if (!isBound()) {
        return nullptr;
    }
    return new InetSocketAddress(mLocalAddress, getLocalPort());
}

void ServerSocket::setReuseAddress(bool reuse) {
    mReuseAddress = reuse;
}

template<>
void ServerSocket::setOption<bool>(const SocketOption<bool>& name, bool value) {
    if (name == StandardSocketOptions::REUSE_PORT) {
        mReusePort = value;
    } else if (name == StandardSocketOptions::REUSE_ADDRESS) {
        mReuseAddress = value;
    } else {
        throw UnsupportedOperationException("Unknown socket option");
    }
}

template<>
bool ServerSocket::getOption<bool>(const SocketOption<bool>& name) {
    if (name == StandardSocketOptions::REUSE_PORT) {
        return mReusePort;
    } else if (name == StandardSocketOptions::REUSE_ADDRESS) {
        return mReuseAddress;
    } else {
        throw UnsupportedOperationException("Unknown socket option");
    }
}

} /* namespace mindroid */
