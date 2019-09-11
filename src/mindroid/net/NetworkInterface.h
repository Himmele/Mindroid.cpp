/*
 * Copyright (c) 2019 E.S.R.Labs. All rights reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of E.S.R.Labs and its suppliers, if any.
 * The intellectual and technical concepts contained herein are
 * proprietary to E.S.R.Labs and its suppliers and may be covered
 * by German and Foreign Patents, patents in process, and are protected
 * by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from E.S.R.Labs.
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
