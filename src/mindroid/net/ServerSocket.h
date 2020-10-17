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

#ifndef MINDROID_NET_SERVERSOCKET_H_
#define MINDROID_NET_SERVERSOCKET_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/lang/UnsupportedOperationException.h>
#include <mindroid/net/SocketOption.h>
#include <mindroid/net/StandardSocketOptions.h>

namespace mindroid {

class Socket;
class InetAddress;
class InetSocketAddress;

class ServerSocket :
        public Object {
public:
    static const int DEFAULT_BACKLOG = 10;

    /**
     * Constructs a new unbound {@code ServerSocket}.
     *
     * @throws IOException if an error occurs while creating the socket.
     */
    ServerSocket();

    /**
     * Constructs a new {@code ServerSocket} instance bound to the given {@code port} using a
     * wildcard address. The backlog is set to 10. If {@code port == 0}, a port will be assigned by
     * the OS.
     *
     * @throws IOException if an error occurs while creating the socket.
     */
    ServerSocket(uint16_t port);

    /**
     * Constructs a new {@code ServerSocket} instance bound to the given {@code port} using a
     * wildcard address. The backlog is set to {@code backlog}.
     * If {@code port == 0}, a port will be assigned by the OS.
     *
     * @throws IOException if an error occurs while creating the socket.
     */
    ServerSocket(uint16_t port, int32_t backlog);

    /**
     * Constructs a new {@code ServerSocket} instance bound to the given {@code localAddress}
     * and {@code port}. The backlog is set to {@code backlog}.
     * If {@code localAddress == null}, the ANY address is used.
     * If {@code port == 0}, a port will be assigned by the OS.
     *
     * @throws IOException if an error occurs while creating the socket.
     */
    ServerSocket(uint16_t port, int32_t backlog, const sp<InetAddress>& localAddress);

    virtual ~ServerSocket();
    ServerSocket(const ServerSocket&) = delete;
    ServerSocket& operator=(const ServerSocket&) = delete;

    /**
     * Closes this server socket and its implementation. Any attempt to connect
     * to this socket thereafter will fail.
     *
     * @throws IOException
     *             if an error occurs while closing this socket.
     */
    void close();

    /**
     * Binds this server socket to the given local socket address with a maximum
     * backlog of 50 unaccepted connections. If the {@code localAddr} is set to
     * {@code null} the socket will be bound to an available local address on
     * any free port of the system.
     *
     * @param localAddress
     *            the local address and port to bind on.
     * @throws IllegalArgumentException
     *             if the {@code SocketAddress} is not supported.
     * @throws IOException
     *             if the socket is already bound or a problem occurs during
     *             binding.
     */
    void bind(const sp<InetSocketAddress>& localAddress) {
        bind(localAddress, DEFAULT_BACKLOG);
    }

    /**
     * Binds this server socket to the given local socket address with a maximum
     * backlog of 50 unaccepted connections. If the {@code localAddr} is set to
     * {@code null} the socket will be bound to an available local address on
     * any free port of the system.
     *
     * @param localAddress
     *            the local address and port to bind on.
     * @throws IllegalArgumentException
     *             if the {@code SocketAddress} is not supported.
     * @throws IOException
     *             if the socket is already bound or a problem occurs during
     *             binding.
     */
    void bind(const sp<InetSocketAddress>& localAddress, int32_t backlog);

    /**
     * Waits for an incoming request and blocks until the connection is opened.
     * This method returns a socket object representing the just opened
     * connection.
     *
     * @return the connection representing socket.
     * @throws IOException
     *             if an error occurs while accepting a new connection.
     */
    sp<Socket> accept();

    /**
     * Gets the local IP address of this server socket if this socket has ever been bound,
     * {@code null} otherwise. This is useful for multihomed hosts.
     *
     * @return the local address of this server socket.
     */
    sp<InetAddress> getInetAddress() const;

    /**
     * Gets the local port of this server socket or {@code -1} if the socket is not bound.
     * If the socket has ever been bound this method will return the local port it was bound to,
     * even after it has been closed.
     *
     * @return the local port this server is listening on.
     */
    int32_t getLocalPort() const;

    /**
     * Gets the local socket address of this server socket or {@code null} if the socket is unbound.
     * This is useful on multihomed hosts. If the socket has ever been bound this method will return
     * the local address it was bound to, even after it has been closed.
     *
     * @return the local socket address and port this socket is bound to.
     */
    sp<InetSocketAddress> getLocalSocketAddress() const;

    /**
     * Returns whether this server socket is bound to a local address and port
     * or not.
     *
     * @return {@code true} if this socket is bound, {@code false} otherwise.
     */
    bool isBound() const { return mIsBound; }

    /**
     * Returns whether this server socket is closed or not.
     *
     * @return {@code true} if this socket is closed, {@code false} otherwise.
     */
    bool isClosed() const { return mIsClosed; }

    /**
    * Sets the value for the socket option {@code SocketOptions.SO_REUSEADDR}.
    *
    * @param reuse
    *            the socket option setting.
    * @throws SocketException
    *             if an error occurs while setting the option value.
    */
    void setReuseAddress(bool reuse);

    /**
     * Sets the value of a socket option.
     */
    template<typename T>
    void setOption(const SocketOption<T>& name, T value);

    /**
     * Returns the value of a socket option.
     */
    template<typename T>
    T getOption(const SocketOption<T>& name);

protected:
    void bind(uint16_t port, int32_t backlog, const sp<InetAddress>& localAddress);
    void setCommonSocketOptions();

    int32_t mFd = -1;
    sp<InetAddress> mLocalAddress;
    int32_t mLocalPort = -1;
    bool mIsBound = false;
    bool mIsClosed = false;
    bool mReuseAddress = false;
    bool mReusePort = false;
};

} /* namespace mindroid */

#endif /* MINDROID_NET_SERVERSOCKET_H_ */
