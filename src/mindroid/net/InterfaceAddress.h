/*
 * Copyright (C) 2020 E.S.R. Labs
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

#ifndef MINDROID_NET_INTERFACEADDRESS_H_
#define MINDROID_NET_INTERFACEADDRESS_H_

#include <mindroid/net/InetAddress.h>

namespace mindroid {

class NetworkInterface;

class InterfaceAddress : public Object {
public:

    /**
     * Returns the address of this network interface.
     */
    sp<InetAddress> getAddress() const { return mAddress; }

    /**
     * Returns the broadcast address of this network interface.
     *
     * Will be null for an IPv6 address.
     */
    sp<InetAddress> getBroadcast() const { return mBroadcast; }

private:
    InterfaceAddress(const sp<InetAddress>& address) : mAddress(address) {}

    sp<InetAddress> mAddress;
    sp<InetAddress> mBroadcast;

    friend class NetworkInterface;
};

} /* namespace mindroid */

#endif // MINDROID_NET_INTERFACEADDRESS_H_
