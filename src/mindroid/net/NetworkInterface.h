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
#include <mindroid/util/ArrayList.h>

namespace mindroid {

/**
 * This class is used to represent a network interface of the local device.
 */
class NetworkInterface : public Object {
public:
    /**
     * Returns an enumeration of the addresses bound to this network interface.
     */
    static sp<ArrayList<sp<NetworkInterface>>> getInetAddresses();

    /**
     * Returns the name of this network interface (such as "eth0" or "lo").
     */
    sp<String> getName() const {
        return mName;
    }

    /**
     * Returns the hardware address of the interface, if it has one, or null otherwise.
     */
    sp<ByteArray> getHardwareAddress() const {
        if (mHardwareAddress == nullptr) {
            mHardwareAddress = queryHardwareAddress();
        }
        return mHardwareAddress;
    }

private:
    NetworkInterface(const sp<String>& name);
    sp<ByteArray> queryHardwareAddress() const;

    sp<String> mName;
    mutable sp<ByteArray> mHardwareAddress;
};

} /* namespace mindroid */

#endif // MINDROID_NET_NETWORKINTERFACE_H_
