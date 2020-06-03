/*
 * Copyright (C) 2019 E.S.R. Labs
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

#ifndef MINDROID_NET_NETWORKINTERFACE_H_
#define MINDROID_NET_NETWORKINTERFACE_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/net/InetAddress.h>
#include <mindroid/net/InterfaceAddress.h>
#include <mindroid/util/ArrayList.h>
#include <ifaddrs.h>
#include <netinet/in.h>

namespace mindroid {

/**
 * This class is used to represent a network interface of the local device.
 */
class NetworkInterface : public Object {
public:
    /**
     * Returns a list with all network interfaces on this machine.
     */
    static sp<ArrayList<sp<NetworkInterface>>> getNetworkInterfaces();

    /**
     * Returns an enumeration of the addresses bound to this network interface.
     */
    sp<ArrayList<sp<InetAddress>>> getInetAddresses();

    /**
     * Returns a list of the InterfaceAddresses of this network interface.
     */
    sp<ArrayList<sp<InterfaceAddress>>> getInterfaceAddresses();

    /**
     * Returns the name of this network interface (such as "eth0" or "lo").
     */
    sp<String> getName() const {
        return mName;
    }

    /**
     * Returns the index of this network interface.
     */
    int32_t getIndex() const;

    /**
     * Returns the hardware address of the interface, if it has one, or null otherwise.
     */
    sp<ByteArray> getHardwareAddress() const {
        return mHardwareAddress;
    }

    /**
     * Returns whether a network interface is up and running.
     */
    bool isUp() const;

    /**
     * Returns whether a network interface supports multicasting.
     */
    bool supportsMulticast() const;

    /**
     * Returns whether a network interface is a point to point interface.
     */
    bool isPointToPoint() const;

    /**
     * Returns whether a network interface is a loopback interface.
     */
    bool isLoopback() const;

private:
    static const size_t IPV6_ADDRESS_SIZE = 16;

    NetworkInterface(const sp<String>& name, uint32_t flags);
    void addInterfaceAddress(struct ifaddrs* ifAddr);
    static sp<InetAddress> toInet4Address(sockaddr_in* addr);
    static sp<InetAddress> toInet6Address(sockaddr_in6* addr);

    sp<String> mName;
    sp<ByteArray> mHardwareAddress;
    sp<ArrayList<sp<InterfaceAddress>>> mInterfaceAddresses;
    uint32_t mFlags;
};

} /* namespace mindroid */

#endif // MINDROID_NET_NETWORKINTERFACE_H_
