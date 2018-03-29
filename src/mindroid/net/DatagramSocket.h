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

#ifndef MINDROID_NET_DATAGRAMSOCKET_H_
#define MINDROID_NET_DATAGRAMSOCKET_H_

#include <mindroid/lang/Object.h>

namespace mindroid {

class InetAddress;
class InetSocketAddress;
class DatagramPacket;

class DatagramSocket :
        public Object {
public:
    /**
     * Constructs a UDP datagram socket which is bound to any available port on
     * the local host using a wildcard address.
     *
     * @throws SocketException
     *             if an error occurs while creating or binding the socket.
     */
    DatagramSocket();

    /**
     * Constructs a UDP datagram socket which is bound to the specific port
     * {@code port} on the local host using a wildcard address. Valid values for {@code port} are
     * between 0 and 65535 inclusive.
     *
     * @param port
     *            the port to bind on the local host.
     * @throws SocketException
     *             if an error occurs while creating or binding the socket.
     */
    DatagramSocket(uint16_t port);

    /**
     * Constructs a UDP datagram socket which is bound to the specific local address {@code inetAddress} on
     * port {@code port}. Valid values for {@code port} are between 0 and 65535 inclusive. If
     * {@code inetAddress} is {@code null} the socket will be bound to a wildcard address.
     *
     * @param port
     *            the port to bind on the local host.
     * @param inetAddress
     *            the address to bind on the local host.
     * @throws SocketException
     *             if an error occurs while creating or binding the socket.
     */
    DatagramSocket(uint16_t port, const sp<InetAddress>& inetAddress);

    virtual ~DatagramSocket();
    DatagramSocket(const DatagramSocket&) = delete;
    DatagramSocket& operator=(const DatagramSocket&) = delete;

    /**
     * Closes this UDP datagram socket and all possibly associated channels.
     */
    void close();

    /**
     * Binds this socket to the local address and port specified by {@code
     * localAddress}. If this value is {@code null} any free port on a valid local
     * address is used.
     *
     * @param localAddress
     *            the local machine address and port to bind on.
     * @throws IllegalArgumentException
     *             if the SocketAddress is not supported
     * @throws SocketException
     *             if the socket is already bound or a problem occurs during
     *             binding.
     */
    void bind(const sp<InetSocketAddress>& localAddress);

    /**
     * Returns true if this socket is bound to a local address. See {@link #bind}.
     */
    bool isBound() const { return mIsBound; }

    /**
     * Gets the state of this socket.
     *
     * @return {@code true} if the socket is closed, {@code false} otherwise.
     */
    bool isClosed() const { return mIsClosed; }

    /**
     * Receives a packet from this socket and stores it in the argument {@code
     * datagramPacket}. All fields of {@code datagramPacket} must be set according
     * to the data received. If the received data is longer than the packet buffer
     * size it is truncated. This method blocks until a packet is received or a
     * timeout has expired.
     *
     * @param datagramPacket
     *            the {@code DatagramPacket} to store the received data.
     * @throws IOException
     *                if an error occurs while receiving the packet.
     */
    void receive(const sp<DatagramPacket>& datagramPacket);

    /**
     * Sends a packet over this socket.
     *
     * @param datagramPacket
     *            the {@code DatagramPacket} which has to be sent.
     * @throws IOException
     *                if an error occurs while sending the packet.
     */
    void send(const sp<DatagramPacket>& datagramPacket);

private:
    void bind(uint16_t port, const sp<InetAddress>& localAddress);

    int32_t mFd = -1;
    int32_t mPort = -1;
    bool mIsBound = false;
    bool mIsClosed = false;
};

} /* namespace mindroid */

#endif /*MINDROID_NET_DATAGRAMSOCKET_H_*/
