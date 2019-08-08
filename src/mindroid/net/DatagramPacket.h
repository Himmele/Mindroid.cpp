/*
 * Copyright (C) 2018 Daniel Himmelein
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

#ifndef MINDROID_NET_DATAGRAMPACKET_H_
#define MINDROID_NET_DATAGRAMPACKET_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/IllegalArgumentException.h>
#include <mindroid/lang/IndexOutOfBoundsException.h>
#include <mindroid/net/InetAddress.h>

namespace mindroid {


class DatagramPacket :
        public Object {
public:
    /**
     * Constructs a new {@code DatagramPacket} object to receive data up to
     * {@code length} bytes.
     *
     * @param data
     *            a byte array to store the read characters.
     * @param size
     *            the size of the data buffer.
     */
    DatagramPacket(const sp<ByteArray>& data, const size_t size) :
            DatagramPacket(data, 0, size) {
    }

    /**
     * Constructs a new {@code DatagramPacket} object to receive data up to
     * {@code length} bytes with a specified buffer offset.
     *
     * @param data
     *            a byte array to store the read characters.
     * @param offset
     *            the offset of the byte array where the bytes is written.
     * @param size
     *            the size of the data.
     */
    DatagramPacket(const sp<ByteArray>& data, const size_t offset, const size_t size) {
        setData(data, offset, size);
    }

    /**
     * Gets the sender or destination IP address of this datagram packet.
     *
     * @return the address from where the datagram was received or to which it
     *         is sent.
     */
    sp<InetAddress> getAddress() {
        return mInetAddress;
    }

    /**
     * Gets the data of this datagram packet.
     *
     * @return the received data or the data to be sent.
     */
    sp<ByteArray> getData() {
        return mData;
    }

    /**
     * Gets the length of the data stored in this datagram packet.
     *
     * @return the length of the received data or the data to be sent.
     */
    size_t getLength() {
        return mSize;
    }

    /**
     * Gets the offset of the data stored in this datagram packet.
     *
     * @return the position of the received data or the data to be sent.
     */
    size_t getOffset() {
        return mOffset;
    }

    /**
     * Gets the port number of the target or sender host of this datagram
     * packet.
     *
     * @return the port number of the origin or target host.
     */
    uint16_t getPort() {
        return mPort;
    }

    /**
     * Sets the IP address of the target host.
     *
     * @param addr
     *            the target host address.
     */
    void setAddress(const sp<InetAddress>& inetAddress) {
        mInetAddress = inetAddress;
    }

    /**
     * Sets the data buffer for this datagram packet.
     */
    void setData(const sp<ByteArray>& data, size_t offset, size_t size) {
        if ((offset + size) > data->size()) {
            throw IllegalArgumentException();
        }
        mData = data;
        mOffset = offset;
        mSize = size;
    }

    /**
     * Sets the data buffer for this datagram packet. The length of the datagram
     * packet is set to the buffer length.
     *
     * @param buf
     *            the buffer to store the data.
     */
    void setData(const sp<ByteArray>& data) {
        mData = data;
        mOffset = 0;
        mSize = data->size();
    }

    /**
     * Sets the length of the datagram packet. This length plus the offset must
     * be lesser than or equal to the buffer size.
     *
     * @param length
     *            the length of this datagram packet.
     */
    void setLength(size_t size) {
        if (mOffset + size > mData->size()) {
            throw IndexOutOfBoundsException();
        }
        mSize = size;
    }

    /**
     * Sets the port number of the target host of this datagram packet.
     *
     * @param port
     *            the target host port number.
     */
    void setPort(uint16_t port) {
        mPort = port;
    }

private:
    sp<ByteArray> mData;
    size_t mOffset = 0;
    size_t mSize = 0;
    uint16_t mPort = 0;
    sp<InetAddress> mInetAddress;
};

} /* namespace mindroid */

#endif /*MINDROID_NET_DATAGRAMPACKET_H_*/
