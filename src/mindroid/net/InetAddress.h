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

#ifndef MINDROID_NET_INETADDRESS_H_
#define MINDROID_NET_INETADDRESS_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/lang/ByteArray.h>

namespace mindroid {

class InetAddress :
        public Object {
public:
    /**
     * Returns the address of a host according to the given host string name
     * {@code host}. The host string may be either a machine name or a dotted
     * string IP address. If the latter, the {@code hostName} field is
     * determined upon demand. {@code host} can be {@code null} which means that
     * an address of the loopback interface is returned.
     *
     * @param host
     *            the hostName to be resolved to an address or {@code null}.
     * @return the {@code InetAddress} instance representing the host.
     * @throws UnknownHostException
     *             if the address lookup fails.
     */
    static sp<InetAddress> getByName(const char* host) {
        return getByName(String::valueOf(host));
    }
    static sp<InetAddress> getByName(const sp<String>& host);

    /**
     * Returns the numeric representation of this IP address (such as "127.0.0.1").
     */
    sp<String> getHostAddress() const;

    /**
     * Returns the host name corresponding to this IP address. This may or may not be a
     * fully-qualified name. If the IP address could not be resolved, the numeric representation
     * is returned instead (see {@link #getHostAddress}).
     */
    sp<String> getHostName();

    /**
     * Returns a string containing the host name (if available) and host address.
     * For example: {@code "www.google.com/74.125.224.115"} or {@code "/127.0.0.1"}.
     *
     * <p>IPv6 addresses may additionally include an interface name or scope id.
     * For example: {@code "www.google.com/2001:4860:4001:803::1013%eth0"} or
     * {@code "/2001:4860:4001:803::1013%2"}.
     */
    sp<String> toString() const {
         return String::format("%s/%s", (mHostName == nullptr) ? "" : mHostName->c_str(), getHostAddress()->c_str());
    }

    /**
     * Returns the IP address represented by this {@code InetAddress} instance
     * as a byte array. The elements are in network order (the highest order
     * address byte is in the zeroth element).
     *
     * @return the address in form of a byte array.
     */
    sp<ByteArray> getAddress() const {
        return mIpAddress->clone();
    }

    /**
     * Returns whether this address is a multicast address or not.
     *
     * <p>Valid IPv6 multicast addresses have the prefix {@code ff::/8}.
     *
     * <p>Valid IPv4 multicast addresses have the prefix {@code 224/4}.
     */
    virtual bool isMulticastAddress() const = 0;

private:
    /**
     * Constructs an {@code InetAddress}.
     *
     * Note: this constructor is for subclasses only.
     */
    InetAddress(int32_t family, const sp<ByteArray>& ipAddress, const char* hostName);
    InetAddress(int32_t family, const sp<ByteArray>& ipAddress, const sp<String>& hostName);

    static sp<InetAddress> parseNumericIpAddress(sp<String> address);

    int32_t mFamily = -1;
    sp<ByteArray> mIpAddress;
    sp<String> mHostName;

    friend class Inet4Address;
    friend class Inet6Address;
};

} /* namespace mindroid */

#endif /* MINDROID_NET_INETADDRESS_H_ */
