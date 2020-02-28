/*
 * Copyright (C) 2017 E.S.R. Labs
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

#ifndef MINDROID_NET_INET6ADDRESS_H_
#define MINDROID_NET_INET6ADDRESS_H_

#include <mindroid/net/InetAddress.h>
#include <mindroid/lang/String.h>
#include <arpa/inet.h>

namespace mindroid {

class DatagramSocket;

class Inet6Address :
        public InetAddress {
public:
    virtual ~Inet6Address() = default;

    static sp<InetAddress> ANY;
    static sp<InetAddress> LOOPBACK;

    /**
     * Gets the scope id as a number if this address is linked to an interface.
     * Otherwise returns {@code 0}.
     *
     * @return the scope_id of this address or 0 when not linked with an
     *         interface.
     */
    int32_t getScopeId() const { return mScopeId; }

    bool isMulticastAddress() const override {
        return mIpAddress->get(0) == 0xFFu;
    }

private:
    /**
     * Constructs an {@code InetAddress} representing the {@code address} and
     * {@code name} and {@code scope_id}.
     *
     * @param address
     *            the network address.
     * @param name
     *            the name associated with the address.
     * @param scope_id
     *            the scope id for link- or site-local addresses.
     *
     * @hide
     */
    Inet6Address(const sp<ByteArray>& ipAddress, const char* hostName, int32_t scope_id) :
        InetAddress(AF_INET6, ipAddress, hostName), mScopeId(scope_id) {
    }

    Inet6Address(const sp<ByteArray>& ipAddress, const sp<String>& hostName, int32_t scope_id) :
        InetAddress(AF_INET6, ipAddress, hostName), mScopeId(scope_id) {
    }

    friend class InetAddress;
    friend class ServerSocket;
    friend class Socket;
    friend class DatagramSocket;
    friend class NetworkInterface;

    int32_t mScopeId;
};

} /* namespace mindroid */

#endif /* MINDROID_NET_INET6ADDRESS_H_ */
