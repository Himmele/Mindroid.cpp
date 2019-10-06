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

NetworkInterface::NetworkInterface(const sp<String>& name) : mName(name), mInetAddresses(new ArrayList<sp<InetAddress>>()) {
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
            networkInterface = new NetworkInterface(name);
            networkInterfaceMap->put(name, networkInterface);
        }

        networkInterface->addAddress(curNetworkInterface->ifa_addr);
    }
    freeifaddrs(networkInterfaces);

    return networkInterfaceMap->values();
}

void NetworkInterface::addAddress(struct sockaddr* interfaceAddress) {
    const size_t IPV6_ADDRESS_SIZE = 16;

    switch (interfaceAddress->sa_family) {
    case AF_INET6: {
        auto ipv6InterfaceAddress = reinterpret_cast<struct sockaddr_in6*>(interfaceAddress);
        sp<ByteArray> ipv6Address = new ByteArray(IPV6_ADDRESS_SIZE);
        memcpy(ipv6Address->c_arr(), ipv6InterfaceAddress->sin6_addr.s6_addr, IPV6_ADDRESS_SIZE);
        mInetAddresses->add(new Inet6Address(ipv6Address, nullptr, ipv6InterfaceAddress->sin6_scope_id));
        break;
    }
    case AF_INET: {
        auto ipv4InterfaceAddress = reinterpret_cast<struct sockaddr_in*>(interfaceAddress);
        sp<ByteArray> ipv4Address = new ByteArray(sizeof(uint32_t));
        memcpy(ipv4Address->c_arr(), &ipv4InterfaceAddress->sin_addr.s_addr, sizeof(uint32_t));
        mInetAddresses->add(new Inet4Address(ipv4Address, nullptr));
        break;
    }
#ifndef __APPLE__
    case AF_PACKET: {
        auto packetInterfaceAddress = reinterpret_cast<struct sockaddr_ll*>(interfaceAddress);
        mHardwareAddress = new ByteArray(packetInterfaceAddress->sll_halen);
        memcpy(mHardwareAddress->c_arr(), packetInterfaceAddress->sll_addr, packetInterfaceAddress->sll_halen);
        break;
    }
#else
    case AF_LINK: {
        sockaddr_dl* linkLevelSocketAddress = (sockaddr_dl*) interfaceAddress->sa_data;
        mHardwareAddress = new ByteArray(linkLevelSocketAddress->sdl_alen);
        memcpy(mHardwareAddress->c_arr(), LLADDR(linkLevelSocketAddress), linkLevelSocketAddress->sdl_alen);
        break;
    }
#endif // __APPLE__
    default:
        break;
    }
}

sp<ArrayList<sp<InetAddress>>> NetworkInterface::getInetAddresses() {
    return mInetAddresses;
}

bool NetworkInterface::isUp() const {
    int32_t fd = ::socket(AF_INET6, SOCK_STREAM, 0);
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

} /* namespace mindroid */
