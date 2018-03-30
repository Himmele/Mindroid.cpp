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

#ifndef MINDROID_NET_INETSOCKETADDRESS_H_
#define MINDROID_NET_INETSOCKETADDRESS_H_

#include <mindroid/net/SocketAddress.h>
#include <mindroid/net/InetAddress.h>

namespace mindroid {

class InetSocketAddress :
        public SocketAddress {
public:
    virtual ~InetSocketAddress() = default;

    /**
     * @hide
     */
    InetSocketAddress() {
    }

    /**
     * Creates a socket endpoint with the given port number {@code port} and
     * no specified address. The range for valid port numbers is between 0 and
     * 65535 inclusive.
     *
     * @param port the port number of the socket endpoint.
     */
    InetSocketAddress(uint16_t port) : InetSocketAddress((sp<InetAddress>) nullptr, port) {
    }

    /**
     * Creates a socket endpoint with the given port number {@code port} and
     * {@code address}. The range for valid port numbers is between 0 and 65535
     * inclusive. If {@code address} is {@code null} the address is set to a
     * wildcard address.
     *
     * @param address the address of the socket endpoint.
     * @param port the port number of the socket endpoint.
     */
    InetSocketAddress(const sp<InetAddress>& address, uint16_t port);

    /**
     * Creates a socket endpoint with the given port number {@code port} and the
     * hostname {@code host}. The hostname is tried to be resolved and cannot be
     * {@code null}. The range for valid port numbers is between 0 and 65535
     * inclusive.
     *
     * @param host the hostname of the socket endpoint.
     * @param port the port number of the socket endpoint.
     */
    InetSocketAddress(const char* host, uint16_t port) : InetSocketAddress(String::valueOf(host), port, true) {
    }

    InetSocketAddress(const sp<String>& host, uint16_t port) : InetSocketAddress(host, port, true) {
    }

    /**
     * Creates an {@code InetSocketAddress} without trying to resolve the
     * hostname into an {@code InetAddress}. The address field is marked as
     * unresolved.
     *
     * @param host the hostname of the socket endpoint.
     * @param port the port number of the socket endpoint.
     * @return the created InetSocketAddress instance.
     * @throws IllegalArgumentException if the hostname {@code host} is {@code null} or the port is
     *         not in the range between 0 and 65535.
     */
    static sp<InetSocketAddress> createUnresolved(const sp<String>& host, uint16_t port) {
        return new InetSocketAddress(host, port, false);
    }

    /**
     * Returns the socket endpoint's port.
     */
    int32_t getPort() const {
        return mPort;
    }

    /**
     * Returns the socket endpoint's address.
     */
    sp<InetAddress> getAddress() const {
        return mAddress;
    }

    /**
     * Returns the hostname, doing a reverse DNS lookup on the {@code InetAddress} if no
     * hostname string was provided at construction time. Use {@link #getHostString} to
     * avoid the reverse DNS lookup.
     */
    sp<String> getHostName() const {
        return (mAddress != nullptr) ? mAddress->getHostName() : mHostname;
    }

    /**
     * Returns whether this socket address is unresolved or not.
     *
     * @return {@code true} if this socket address is unresolved, {@code false}
     *         otherwise.
     */
    bool isUnresolved() {
        return mAddress == nullptr;
    }

    /**
     * Returns a string containing the address (or the hostname for an
     * unresolved {@code InetSocketAddress}) and port number.
     * For example: {@code "www.google.com/74.125.224.115:80"} or {@code "/127.0.0.1:80"}.
     */
    sp<String> toString() {
        return String::format("%s:%d", (mAddress != nullptr) ? mAddress->toString()->c_str() : mHostname->c_str(), mPort);
    }

private:
    InetSocketAddress(sp<String> hostname, uint16_t port, bool hostnameResultion);

    sp<InetAddress> mAddress;
    sp<String> mHostname;
    int32_t mPort = -1;
};

} /* namespace mindroid */

#endif /* MINDROID_NET_INETSOCKETADDRESS_H_ */
