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

#include <mindroid/net/NetworkInterface.h>
#include <mindroid/net/Inet4Address.h>
#include <mindroid/net/Inet6Address.h>
#include <mindroid/net/SocketException.h>
#include <mindroid/util/HashMap.h>
#include <cerrno>
#include <map>
#include <net/if.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#ifndef __APPLE__
#include <linux/if_packet.h>
#else
#include <net/if_dl.h>
#endif
#include <sys/socket.h>

namespace mindroid {

NetworkInterface::NetworkInterface(const sp<String>& name, uint32_t flags) :
        mName(name), mInterfaceAddresses(new ArrayList<sp<InterfaceAddress>>()), mFlags(flags) {
}

sp<ArrayList<sp<NetworkInterface>>> NetworkInterface::getNetworkInterfaces() {
    sp<HashMap<sp<String>, sp<NetworkInterface>>> networkInterfaceMap = new HashMap<sp<String>, sp<NetworkInterface>>();

    ifaddrs* networkInterfaces = nullptr;
    if (getifaddrs(&networkInterfaces) == -1) {
        throw SocketException(String::format("Failed to get information on network interfaces: %s (errno=%d)",
                    strerror(errno), errno));
    }

    ifaddrs* curNetworkInterface;
    for (curNetworkInterface = networkInterfaces; curNetworkInterface != nullptr; curNetworkInterface = curNetworkInterface->ifa_next) {
        sp<String> name = String::valueOf(curNetworkInterface->ifa_name);

        sp<NetworkInterface> networkInterface;
        if (networkInterfaceMap->containsKey(name)) {
            networkInterface = networkInterfaceMap->get(name);
        } else {
            networkInterface = new NetworkInterface(name, curNetworkInterface->ifa_flags);
            networkInterfaceMap->put(name, networkInterface);
        }

        if (curNetworkInterface->ifa_addr != nullptr) {
            networkInterface->addInterfaceAddress(curNetworkInterface);
        }
    }
    freeifaddrs(networkInterfaces);

    return networkInterfaceMap->values();
}

void NetworkInterface::addInterfaceAddress(struct ifaddrs* ifAddress) {
    switch (ifAddress->ifa_addr->sa_family) {
    case AF_INET6: {
        sp<InterfaceAddress> interfaceAddress =
            new InterfaceAddress(toInet6Address(reinterpret_cast<sockaddr_in6*>(ifAddress->ifa_addr)));
        mInterfaceAddresses->add(interfaceAddress);
        break;
    }
    case AF_INET: {
        sp<InterfaceAddress> interfaceAddress =
            new InterfaceAddress(toInet4Address(reinterpret_cast<sockaddr_in*>(ifAddress->ifa_addr)));
        if (ifAddress->ifa_flags & IFF_BROADCAST) {
            interfaceAddress->mBroadcast = toInet4Address(reinterpret_cast<sockaddr_in*>(ifAddress->ifa_broadaddr));
        }
        mInterfaceAddresses->add(interfaceAddress);
        break;
    }
#ifndef __APPLE__
    case AF_PACKET: {
        auto packetInterfaceAddress = reinterpret_cast<struct sockaddr_ll*>(ifAddress->ifa_addr);
        mHardwareAddress = new ByteArray(packetInterfaceAddress->sll_halen);
        memcpy(mHardwareAddress->c_arr(), packetInterfaceAddress->sll_addr, packetInterfaceAddress->sll_halen);
        break;
    }
#else
    case AF_LINK: {
        sockaddr_dl* linkLevelSocketAddress = (sockaddr_dl*) sockAddress->sa_data;
        mHardwareAddress = new ByteArray(linkLevelSocketAddress->sdl_alen);
        memcpy(mHardwareAddress->c_arr(), LLADDR(linkLevelSocketAddress), linkLevelSocketAddress->sdl_alen);
        break;
    }
#endif // __APPLE__
    default:
        break;
    }
}

sp<InetAddress> NetworkInterface::toInet4Address(struct sockaddr_in* socketAddress) {
    sp<ByteArray> ipv4Address = new ByteArray(sizeof(uint32_t));
    memcpy(ipv4Address->c_arr(), &socketAddress->sin_addr.s_addr, sizeof(uint32_t));
    return new Inet4Address(ipv4Address, nullptr);
}

sp<InetAddress> NetworkInterface::toInet6Address(struct sockaddr_in6* socketAddress) {
    sp<ByteArray> ipv6Address = new ByteArray(IPV6_ADDRESS_SIZE);
    memcpy(ipv6Address->c_arr(), socketAddress->sin6_addr.s6_addr, IPV6_ADDRESS_SIZE);
    return new Inet6Address(ipv6Address, nullptr, socketAddress->sin6_scope_id);
}

sp<ArrayList<sp<InetAddress>>> NetworkInterface::getInetAddresses() {
    sp<ArrayList<sp<InetAddress>>> inetAddresses = new ArrayList<sp<InetAddress>>(mInterfaceAddresses->size());
    for (const auto& interfaceAddress : mInterfaceAddresses->arr()) {
        inetAddresses->add(interfaceAddress->getAddress());
    }
    return inetAddresses;
}

sp<ArrayList<sp<InterfaceAddress>>> NetworkInterface::getInterfaceAddresses() {
    return mInterfaceAddresses;
}

bool NetworkInterface::isUp() const {
    int32_t fd = ::socket(AF_INET6, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (fd < 0) {
        throw SocketException(String::format("Failed to open socket: %s (errno=%d)", strerror(errno), errno));
    }

    struct ifreq configuration;
    memset(&configuration, 0, sizeof(configuration));
    strcpy(configuration.ifr_name, mName->c_str());
    if (ioctl(fd, SIOCGIFFLAGS, &configuration) < 0) {
        throw SocketException(String::format("Failed to get network interface flags: %s (errno=%d)", strerror(errno), errno));
    }

    if (close(fd) < 0) {
        throw SocketException(String::format("Failed to close socket: %s (errno=%d)", strerror(errno), errno));
    }

    return (configuration.ifr_flags & IFF_RUNNING);
}

int32_t NetworkInterface::getIndex() const {
    int rc = if_nametoindex(mName->c_str());
    return rc == 0 ? -1 : rc;
}

bool NetworkInterface::supportsMulticast() const {
    return (mFlags & IFF_MULTICAST) == IFF_MULTICAST;
}

bool NetworkInterface::isPointToPoint() const {
    return (mFlags & IFF_POINTOPOINT) == IFF_POINTOPOINT;
}

bool NetworkInterface::isLoopback() const {
    return (mFlags & IFF_LOOPBACK) == IFF_LOOPBACK;
}

} /* namespace mindroid */
