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

#include <mindroid/net/InetAddress.h>
#include <mindroid/net/Inet4Address.h>
#include <mindroid/net/Inet6Address.h>
#include <mindroid/net/UnknownHostException.h>
#include <mindroid/lang/NullPointerException.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

namespace mindroid {

InetAddress::InetAddress(int32_t family, const sp<ByteArray>& ipAddress, const char* hostName) :
    InetAddress(family, ipAddress, String::valueOf(hostName)) {
}

InetAddress::InetAddress(int32_t family, const sp<ByteArray>& ipAddress, const sp<String>& hostName) :
        mFamily(family),
        mIpAddress(ipAddress),
        mHostName(hostName) {
    if (ipAddress == nullptr) {
        throw NullPointerException();
    }
}

sp<InetAddress> InetAddress::getByName(const sp<String>& host) {
    if (host == nullptr || host->isEmpty()) {
        return Inet6Address::LOOPBACK;
    }

    sp<InetAddress> inetAddress = parseNumericIpAddress(host);
    if (inetAddress != nullptr) {
        return inetAddress;
    } else {
        struct addrinfo hints, *address;
        std::memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        int32_t status = ::getaddrinfo(host->c_str(), nullptr, &hints, &address);
        if (status == 0) {
            if (address->ai_family == AF_INET6) {
                const sockaddr_in6& sin6 = *reinterpret_cast<const sockaddr_in6*>(address->ai_addr);
                const void* ipAddress = &sin6.sin6_addr.s6_addr;
                size_t ipAddressSize = 16;
                int32_t scope_id = sin6.sin6_scope_id;
                sp<ByteArray> ba = new ByteArray((const uint8_t*) ipAddress, ipAddressSize);
                inetAddress = new Inet6Address(ba, host, scope_id);
            } else if (address->ai_family == AF_INET) {
                const sockaddr_in& sin = *reinterpret_cast<const sockaddr_in*>(address->ai_addr);
                const void* ipAddress = &sin.sin_addr.s_addr;
                size_t ipAddressSize = 4;
                sp<ByteArray> ba = new ByteArray((const uint8_t*) ipAddress, ipAddressSize);
                inetAddress = new Inet4Address(ba, host);
            } else {
                inetAddress = nullptr;
            }
            ::freeaddrinfo(address);
        } else {
            inetAddress = nullptr;
        }
    }
    if (inetAddress == nullptr) {
        throw UnknownHostException();
    }
    return inetAddress;
}

sp<InetAddress> InetAddress::parseNumericIpAddress(sp<String> numericAddress) {
    if (numericAddress->startsWith("[") && numericAddress->endsWith("]") && numericAddress->indexOf(':') != -1) {
        numericAddress = numericAddress->substring(1, numericAddress->length() - 1);
    }

    sp<InetAddress> inetAddress;
    struct addrinfo hints, *address;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int32_t status = ::getaddrinfo(numericAddress->c_str(), nullptr, &hints, &address);
    if (status == 0) {
        if (address->ai_family == AF_INET6) {
            const sockaddr_in6& sin6 = *reinterpret_cast<const sockaddr_in6*>(address->ai_addr);
            const void* ipAddress = &sin6.sin6_addr.s6_addr;
            size_t ipAddressSize = 16;
            int32_t scope_id = sin6.sin6_scope_id;
            sp<ByteArray> ba = new ByteArray((const uint8_t*) ipAddress, ipAddressSize);
            inetAddress = new Inet6Address(ba, numericAddress, scope_id);
        } else if (address->ai_family == AF_INET) {
            const sockaddr_in& sin = *reinterpret_cast<const sockaddr_in*>(address->ai_addr);
            const void* ipAddress = &sin.sin_addr.s_addr;
            size_t ipAddressSize = 4;
            sp<ByteArray> ba = new ByteArray((const uint8_t*) ipAddress, ipAddressSize);
            inetAddress = new Inet4Address(ba, numericAddress);
        } else {
            inetAddress = nullptr;
        }
        ::freeaddrinfo(address);
    } else {
        inetAddress = nullptr;
    }
    return inetAddress;
}

sp<String> InetAddress::getHostAddress() const {
    sockaddr_storage ss;
    socklen_t saSize = 0;
    std::memset(&ss, 0, sizeof(ss));
    ss.ss_family = mFamily;

    if (ss.ss_family == AF_INET6) {
        sockaddr_in6& sin6 = reinterpret_cast<sockaddr_in6&>(ss);
        std::memcpy(&sin6.sin6_addr.s6_addr, mIpAddress->c_arr(), 16);
        saSize = sizeof(sockaddr_in6);
    } else {
        sockaddr_in& sin = reinterpret_cast<sockaddr_in&>(ss);
        std::memcpy(&sin.sin_addr.s_addr, mIpAddress->c_arr(), 4);
        saSize = sizeof(sockaddr_in);
    }
    char host[NI_MAXHOST]; // NI_MAXHOST > INET6_ADDRSTRLEN.
    int32_t rc = ::getnameinfo(reinterpret_cast<const sockaddr*>(&ss), saSize, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
    if (rc != 0) {
        return String::EMPTY_STRING;
    }
    return new String(host);
}

sp<String> InetAddress::getHostName() {
    if (mHostName == nullptr) {
        mHostName = getHostAddress();
    }
    return mHostName;
}

} /* namespace mindroid */
