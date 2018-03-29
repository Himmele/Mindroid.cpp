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

#include <mindroid/net/InetSocketAddress.h>
#include <mindroid/net/Inet6Address.h>
#include <mindroid/net/UnknownHostException.h>
#include <mindroid/lang/IllegalArgumentException.h>

namespace mindroid {

InetSocketAddress::InetSocketAddress(const sp<InetAddress>& address, uint16_t port) {
    mAddress = (address == nullptr) ? Inet6Address::ANY : address;
    mHostname = nullptr;
    mPort = port;
}

InetSocketAddress::InetSocketAddress(sp<String> hostname, uint16_t port, bool hostnameResultion) {
    if (hostname == nullptr) {
        throw IllegalArgumentException(String::format("host=nullptr, port=%d", port));
    }

    sp<InetAddress> address = nullptr;
    if (hostnameResultion) {
        try {
            address = InetAddress::getByName(hostname);
            hostname = nullptr;
        } catch (const UnknownHostException& ignore) {
        }
    }
    mAddress = address;
    mHostname = hostname;
    mPort = port;
}

} /* namespace mindroid */
