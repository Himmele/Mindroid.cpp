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

#ifndef MINDROID_NET_INET4ADDRESS_H_
#define MINDROID_NET_INET4ADDRESS_H_

#include <mindroid/net/InetAddress.h>
#include <mindroid/lang/String.h>
#include <arpa/inet.h>

namespace mindroid {

class ByteArray;
class DatagramSocket;

class Inet4Address :
        public InetAddress {
public:
    static sp<InetAddress> ANY;
    static sp<InetAddress> ALL;
    static sp<InetAddress> LOOPBACK;

    virtual ~Inet4Address() = default;

private:
    Inet4Address() = default;

    Inet4Address(const sp<ByteArray>& ipAddress, const char* hostName) :
        InetAddress(AF_INET, ipAddress, hostName) {
    }

    Inet4Address(const sp<ByteArray>& ipAddress, const sp<String>& hostName) :
        InetAddress(AF_INET, ipAddress, hostName) {
    }

    friend class InetAddress;
    friend class ServerSocket;
    friend class Socket;
    friend class DatagramSocket;
};

} /* namespace mindroid */

#endif /* MINDROID_NET_INET4ADDRESS_H_ */
