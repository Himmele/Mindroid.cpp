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

#ifndef MINDROID_NET_SOCKET_H_
#define MINDROID_NET_SOCKET_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/io/InputStream.h>
#include <mindroid/io/OutputStream.h>
#include <mindroid/net/SocketOption.h>

namespace mindroid {

class ServerSocket;
class InetAddress;
class InetSocketAddress;

class Socket :
        public Object {
public:
    /**
     * Creates a new unconnected socket. When a SocketImplFactory is defined it
     * creates the internal socket implementation, otherwise the default socket
     * implementation will be used for this socket.
     *
     * @see SocketImplFactory
     * @see SocketImpl
     */
    Socket() = default;

    /**
     * Creates a new streaming socket connected to the target host specified by
     * the parameters {@code host} and {@code port}. The socket is bound
     * to any available port on the local host.
     *
     * <p>This implementation tries each IP address for the given hostname (in
     * <a href="http://www.ietf.org/rfc/rfc3484.txt">RFC 3484</a> order)
     * until it either connects successfully or it exhausts the set.
     *
     * @param host
     *            the target host name or IP address to connect to.
     * @param port
     *            the port on the target host to connect to.
     * @throws UnknownHostException
     *             if the host name could not be resolved into an IP address.
     * @throws IOException
     *             if an error occurs while creating the socket.
     */
    Socket(const char* host, uint16_t port) :
            Socket(String::valueOf(host), port) {
    }
    Socket(const sp<String>& host, uint16_t port);

    virtual ~Socket();
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    /**
     * Closes the socket. It is not possible to reconnect or rebind to this
     * socket thereafter which means a new socket instance has to be created.
     *
     * @throws IOException
     *             if an error occurs while closing the socket.
     */
    void close();

    void close(bool shutdown);

    /**
     * Binds this socket to the given local host address and port specified by the SocketAddress {@code
     * socketAddress}. If {@code socketAddress} is set to {@code null}, this socket will be bound to an
     * available local address on any free port.
     *
     * @param socketAddress
     *            the specific address and port on the local machine to bind to.
     * @throws IOException
     *             if the socket is already bound or an error occurs while
     *             binding.
     */
    void bind(const sp<InetSocketAddress>& socketAddress);

    /**
     * Connects this socket to the given remote host address and port specified
     * by the SocketAddress {@code socketAddress}.
     *
     * @param socketAddress
     *            the address and port of the remote host to connect to.
     * @throws IllegalArgumentException
     *             if the given SocketAddress is invalid or not supported.
     * @throws IOException
     *             if the socket is already connected or an error occurs while
     *             connecting.
     */
    void connect(const sp<InetSocketAddress>& socketAddress);

    /**
     * Returns an input stream to read data from this socket. If the socket has an associated
     * {@link SocketChannel} and that channel is in non-blocking mode then reads from the
     * stream will throw a {@link java.nio.channels.IllegalBlockingModeException}.
     *
     * @return the byte-oriented input stream.
     * @throws IOException
     *             if an error occurs while creating the input stream or the
     *             socket is in an invalid state.
     */
    sp<InputStream> getInputStream();

    /**
     * Returns an output stream to write data into this socket. If the socket has an associated
     * {@link SocketChannel} and that channel is in non-blocking mode then writes to the
     * stream will throw a {@link java.nio.channels.IllegalBlockingModeException}.
     *
     * @return the byte-oriented output stream.
     * @throws IOException
     *             if an error occurs while creating the output stream or the
     *             socket is in an invalid state.
     */
    sp<OutputStream> getOutputStream();

    /**
     * Returns the local IP address this socket is bound to, or an address for which
     * {@link InetAddress#isAnyLocalAddress()} returns true if the socket is closed or unbound.
     */
    sp<InetAddress> getLocalAddress() const;

    /**
     * Returns the local port this socket is bound to, or -1 if the socket is unbound. If the socket
     * has been closed this method will still return the local port the socket was bound to.
     */
    int32_t getLocalPort() const;

    /**
     * Returns the IP address of the target host this socket is connected to, or null if this
     * socket is not yet connected.
     */
    sp<InetAddress> getInetAddress() const;

    /**
     * Returns the port number of the target host this socket is connected to, or 0 if this socket
     * is not yet connected.
     */
    int32_t getPort() const;

    /**
     * Returns the local address and port of this socket as a SocketAddress or null if the socket
     * has never been bound. If the socket is closed but has previously been bound then an address
     * for which {@link InetAddress#isAnyLocalAddress()} returns true will be returned with the
     * previously-bound port. This is useful on multihomed hosts.
     */
    sp<InetSocketAddress> getLocalSocketAddress() const;

    /**
     * Returns the remote address and port of this socket as a {@code
     * SocketAddress} or null if the socket is not connected.
     *
     * @return the remote socket address and port.
     */
    sp<InetSocketAddress> getRemoteSocketAddress() const;

    /**
     * Returns whether this socket is bound to a local address and port.
     *
     * @return {@code true} if the socket is bound to a local address, {@code
     *         false} otherwise.
     */
    bool isBound() const {
        return mIsBound;
    }

    /**
     * Returns whether this socket is connected to a remote host.
     *
     * @return {@code true} if the socket is connected, {@code false} otherwise.
     */
    bool isConnected() const { return mIsConnected; }

    /**
     * Returns whether this socket is closed.
     *
     * @return {@code true} if the socket is closed, {@code false} otherwise.
     */
    bool isClosed() const { return mIsClosed; }

    /**
     * Sets this socket's {@link SocketOptions#TCP_NODELAY} option.
     *
     * @param on
     *            the state whether this option is enabled or not.
     */
    void setTcpNoDelay(bool value);

    /**
     * Returns this socket's {@code SocketOptions#TCP_NODELAY} setting.
     *
     * @return {@code true} if the {@code SocketOptions.TCP_NODELAY} is enabled,
     *         {@code false} otherwise.
     */
    bool getTcpNoDelay() const;

    /**
     * Sets the state of the {@code SocketOptions.SO_LINGER} with the given
     * timeout in seconds. The timeout value for this option is silently limited
     * to the maximum of {@code 65535}.
     *
     * @param on
     *            the state whether this option is enabled or not.
     * @param timeout
     *            the linger timeout value in seconds.
     * @throws SocketException
     *             if an error occurs while setting the option.
     * @see SocketOptions#SO_LINGER
     */
    void setSoLinger(bool on, int32_t linger);

    /**
     * Returns this socket's {@link SocketOptions#SO_LINGER linger} timeout in seconds,
     * or -1 for no linger (i.e. {@code close} will return immediately).
     */
    int32_t getSoLinger() const;

    /**
     * Closes the input stream of this socket. Any further data sent to this
     * socket will be discarded. Reading from this socket after this method has
     * been called will return the value {@code EOF}.
     *
     * @throws IOException
     */
    void shutdownInput();

    /**
     * Closes the output stream of this socket. All buffered data will be sent
     * followed by the termination sequence. Writing to the closed output stream
     * will cause an {@code IOException}.
     *
     * @throws IOException
     */
    void shutdownOutput();

    /**
     * Returns whether the incoming channel of the socket has already been
     * closed.
     *
     * @return {@code true} if reading from this socket is not possible anymore,
     *         {@code false} otherwise.
     */
    bool isInputShutdown() const {
        return mIsInputShutdown;
    }

    /**
     * Returns whether the outgoing channel of the socket has already been
     * closed.
     *
     * @return {@code true} if writing to this socket is not possible anymore,
     *         {@code false} otherwise.
     */
    bool isOutputShutdown() const {
        return mIsOutputShutdown;
    }

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
    void setDefaultSocketOptions();

    int32_t mFd = -1;
    int32_t mLocalPort = -1;
    sp<InetAddress> mLocalAddress;
    sp<InetAddress> mInetAddress;
    int32_t mPort = -1;
    bool mIsBound = false;
    bool mIsConnected = false;
    bool mIsClosed = false;
    bool mIsInputShutdown = false;
    bool mIsOutputShutdown = false;
    bool mReuseAddress = false;
    bool mReusePort = false;

private:
    class SocketInputStream : public InputStream {
    public:
        virtual ~SocketInputStream() {
            close();
        }

        void close() override {
            InputStream::close();
            mSocket = nullptr;
            mFd = -1;
        }

        size_t available() override;
        int32_t read() override;
        ssize_t read(const sp<ByteArray>& buffer, size_t offset, size_t count) override;

    private:
        SocketInputStream(const sp<Socket>& socket) : mSocket(socket), mFd(socket->mFd) {
        }

        sp<Socket> mSocket;
        int32_t mFd;

        friend class Socket;
    };

    class SocketOutputStream : public OutputStream {
    public:
        virtual ~SocketOutputStream() {
            close();
        }

        void close() override  {
            OutputStream::close();
            mSocket = nullptr;
            mFd = -1;
        }

        void write(int32_t b) override;
        void write(const sp<ByteArray>& buffer, size_t offset, size_t count) override;

    private:
        SocketOutputStream(const sp<Socket>& socket) : mSocket(socket), mFd(socket->mFd) {
        }

        sp<Socket> mSocket;
        int32_t mFd;

        friend class Socket;
    };

    friend class ServerSocket;
};

} /* namespace mindroid */

#endif /* MINDROID_NET_SOCKET_H_ */
