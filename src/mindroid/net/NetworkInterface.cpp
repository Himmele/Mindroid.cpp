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
#include <mindroid/net/SocketException.h>
#include <cerrno>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

namespace mindroid {

NetworkInterface::NetworkInterface(const sp<String>& name) : mName(name) {
}

sp<ArrayList<sp<NetworkInterface>>> NetworkInterface::getInetAddresses() {
    sp<ArrayList<sp<NetworkInterface>>> networkInterfaceList = new ArrayList<sp<NetworkInterface>>;

    ifaddrs* networkInterfaces;

    if (getifaddrs(&networkInterfaces) == -1) {
        throw SocketException(String::format("Failed to get information on network interfaces: %s (errno=%d)",
                    strerror(errno), errno));
    }

    ifaddrs* curNetworkInterface;
    for (curNetworkInterface = networkInterfaces; curNetworkInterface != nullptr; curNetworkInterface = curNetworkInterface->ifa_next) {
        if (curNetworkInterface->ifa_addr == nullptr) {
            continue;
        }
        sp<NetworkInterface> networkInterface = new NetworkInterface(new String(curNetworkInterface->ifa_name));
        networkInterfaceList->add(networkInterface);
    }

    freeifaddrs(networkInterfaces);

    return networkInterfaceList;
}

sp<ByteArray> NetworkInterface::queryHardwareAddress() const {
    int socketFd;
    struct ifreq buffer;
    const size_t MAC_ADDRESS_SIZE = 6;

    socketFd = socket(PF_INET, SOCK_DGRAM, 0);
    if (socketFd == -1) {
        throw SocketException(String::format("Failed create a generic socket: %s (errno=%d)",
                    strerror(errno), errno));
    }

    memset(&buffer, 0x00, sizeof(buffer));
    strcpy(buffer.ifr_name, mName->c_str());

    if (ioctl(socketFd, SIOCGIFHWADDR, &buffer) == -1) {
        throw SocketException(String::format("Failed to query hardware address from socket: %s (errno=%d)",
                    strerror(errno), errno));
    }

    close(socketFd);

    sp<ByteArray> byteArray = new ByteArray(MAC_ADDRESS_SIZE);
    for (size_t i = 0; i < MAC_ADDRESS_SIZE; ++i) {
        byteArray->set(i, buffer.ifr_hwaddr.sa_data[i]);
    }
    return byteArray;
}

} /* namespace mindroid */
