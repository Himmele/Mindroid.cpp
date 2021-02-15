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

#include <gtest/gtest.h>
#include <mindroid/io/IOException.h>
#include <mindroid/net/Socket.h>
#include <mindroid/net/ServerSocket.h>
#include <mindroid/net/SocketAddress.h>
#include <mindroid/net/DatagramSocket.h>
#include <mindroid/net/DatagramPacket.h>
#include <mindroid/net/InetAddress.h>
#include <mindroid/net/Inet4Address.h>
#include <mindroid/net/Inet6Address.h>
#include <mindroid/net/InetSocketAddress.h>
#include <mindroid/lang/Thread.h>
#include <mindroid/util/concurrent/Promise.h>

using namespace mindroid;

TEST(Mindroid, InetAddress) {
    sp<InetAddress> inet6Address = InetAddress::getByName("ip6-localhost");
    ASSERT_STREQ(inet6Address->toString()->c_str(), "ip6-localhost/::1");
    ASSERT_STREQ(inet6Address->getHostAddress()->c_str(), "::1");

    sp<InetAddress> inet4Address = InetAddress::getByName("localhost");
    ASSERT_STREQ(inet4Address->toString()->c_str(), "localhost/127.0.0.1");
    ASSERT_STREQ(inet4Address->getHostAddress()->c_str(), "127.0.0.1");
}

TEST(Mindroid, TcpIpServerSocketClosing) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<InetAddress> inetAddress = InetAddress::getByName("localhost");
    sp<ServerSocket> serverSocket = new ServerSocket();
    sp<Thread> thread = new Thread([=] {
        serverSocket->setReuseAddress(true);
        serverSocket->bind(new InetSocketAddress((sp<InetAddress>) inetAddress, 1234));
        promise->complete(true);
        try {
            sp<Socket> socket = serverSocket->accept();
            ASSERT_FALSE(true);
        } catch (const IOException& e) {
            ASSERT_STREQ(e.getMessage()->c_str(), "ServerSocket on port 1234 closed: Invalid argument (errno=22)");
        }
    });
    thread->start();
    promise->get();
    Thread::sleep(100);
    serverSocket->close();
    thread->join();
}

TEST(Mindroid, TcpIpSocketClosing) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<InetAddress> inetAddress = InetAddress::getByName("localhost");
    sp<ServerSocket> serverSocket = new ServerSocket();
    sp<Thread> thread = new Thread([=] {
        serverSocket->setReuseAddress(true);
        serverSocket->bind(new InetSocketAddress((sp<InetAddress>) inetAddress, 1234));
        promise->complete(true);
        sp<Socket> socket = serverSocket->accept();
        ASSERT_EQ(socket->isConnected(), 1);
        ASSERT_EQ(socket->getLocalPort(), 1234);
        sp<ByteArray> buffer = new ByteArray(16);
        ssize_t rc = socket->getInputStream()->read(buffer);
        ASSERT_EQ(rc, 6);
        sp<String> s = new String(buffer);
        ASSERT_STREQ(s->c_str(), "Hello");
        rc = socket->getInputStream()->read(buffer);
        ASSERT_EQ(rc, -1);
        socket->close();
    });
    thread->start();
    promise->get();
    sp<Socket> socket = new Socket();
    socket->connect(new InetSocketAddress(inetAddress, 1234));
    sp<ByteArray> buffer = new ByteArray(16);
    std::memcpy(buffer->c_arr(), "Hello", 6);
    socket->getOutputStream()->write(buffer, 0, 6);
    Thread::sleep(100);
    socket->close();
    serverSocket->close();
    thread->join();
}

TEST(Mindroid, TcpIpV6Localhost) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<InetAddress> inetAddress = InetAddress::getByName("ip6-localhost");
    sp<Thread> thread = new Thread([=] {
        sp<ServerSocket> serverSocket = new ServerSocket();
        serverSocket->setReuseAddress(true);
        serverSocket->bind(new InetSocketAddress((sp<InetAddress>) inetAddress, 1234));
        ASSERT_EQ(serverSocket->getLocalPort(), 1234);
        ASSERT_STREQ(serverSocket->getInetAddress()->toString()->c_str(), "ip6-localhost/::1");
        ASSERT_STREQ(serverSocket->getLocalSocketAddress()->toString()->c_str(), "ip6-localhost/::1:1234");
        promise->complete(true);
        sp<Socket> socket = serverSocket->accept();
        ASSERT_EQ(socket->isConnected(), 1);
        ASSERT_EQ(socket->getLocalPort(), 1234);
        ASSERT_STREQ(socket->getLocalSocketAddress()->toString()->c_str(), "ip6-localhost/::1:1234");
        ASSERT_NE(socket->getPort(), -1);
        ASSERT_STREQ(socket->getRemoteSocketAddress()->toString()->c_str(), String::format("/::1:%u", socket->getPort())->c_str());
        sp<ByteArray> buffer = new ByteArray(16);
        ssize_t rc = socket->getInputStream()->read(buffer);
        ASSERT_EQ(rc, 6);
        sp<String> s = new String(buffer);
        ASSERT_STREQ(s->c_str(), "Hello");
        socket->close();
        serverSocket->close();
    });
    thread->start();
    promise->get();
    sp<Socket> socket = new Socket();
    socket->connect(new InetSocketAddress(inetAddress, 1234));
    ASSERT_NE(socket->getLocalPort(), -1);
    ASSERT_STREQ(socket->getLocalAddress()->toString()->c_str(), "/::1");
    ASSERT_STREQ(socket->getRemoteSocketAddress()->toString()->c_str(), "ip6-localhost/::1:1234");
    sp<ByteArray> buffer = new ByteArray(16);
    std::memcpy(buffer->c_arr(), "Hello", 6);
    socket->getOutputStream()->write(buffer, 0, 6);
    thread->join();
    socket->close();
}

TEST(Mindroid, TcpIpV6AnyHost) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<InetAddress> inetAddress = InetAddress::getByName("ip6-localhost");
    sp<Thread> thread = new Thread([=] {
        sp<ServerSocket> serverSocket = new ServerSocket();
        serverSocket->setReuseAddress(true);
        serverSocket->bind(new InetSocketAddress(Inet6Address::ANY, 1234));
        promise->complete(true);
        sp<Socket> socket = serverSocket->accept();
        ASSERT_EQ(socket->isConnected(), 1);
        sp<ByteArray> buffer = new ByteArray(16);
        ssize_t rc = socket->getInputStream()->read(buffer);
        ASSERT_EQ(rc, 6);
        sp<String> s = new String(buffer);
        ASSERT_STREQ(s->c_str(), "Hello");
        socket->close();
        serverSocket->close();
    });
    thread->start();
    promise->get();
    sp<Socket> socket = new Socket();
    socket->connect(new InetSocketAddress(inetAddress, 1234));
    sp<ByteArray> buffer = new ByteArray(16);
    std::memcpy(buffer->c_arr(), "Hello", 6);
    socket->getOutputStream()->write(buffer, 0, 6);
    thread->join();
    socket->close();
}

TEST(Mindroid, TcpIpV4Localhost) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<InetAddress> inetAddress = InetAddress::getByName("localhost");
    sp<Thread> thread = new Thread([=] {
        sp<ServerSocket> serverSocket = new ServerSocket();
        serverSocket->setReuseAddress(true);
        serverSocket->bind(new InetSocketAddress((sp<InetAddress>) inetAddress, 1234));
        ASSERT_EQ(serverSocket->getLocalPort(), 1234);
        ASSERT_STREQ(serverSocket->getInetAddress()->toString()->c_str(), "localhost/127.0.0.1");
        ASSERT_STREQ(serverSocket->getLocalSocketAddress()->toString()->c_str(), "localhost/127.0.0.1:1234");
        promise->complete(true);
        sp<Socket> socket = serverSocket->accept();
        ASSERT_EQ(socket->isConnected(), 1);
        ASSERT_EQ(socket->getLocalPort(), 1234);
        ASSERT_STREQ(socket->getLocalSocketAddress()->toString()->c_str(), "localhost/127.0.0.1:1234");
        ASSERT_NE(socket->getPort(), -1);
        ASSERT_STREQ(socket->getRemoteSocketAddress()->toString()->c_str(), String::format("/127.0.0.1:%u", socket->getPort())->c_str());
        sp<ByteArray> buffer = new ByteArray(16);
        ssize_t rc = socket->getInputStream()->read(buffer);
        ASSERT_EQ(rc, 6);
        sp<String> s = new String(buffer);
        ASSERT_STREQ(s->c_str(), "Hello");
        socket->close();
        serverSocket->close();
    });
    thread->start();
    promise->get();
    sp<Socket> socket = new Socket();
    socket->connect(new InetSocketAddress(inetAddress, 1234));
    ASSERT_NE(socket->getLocalPort(), -1);
    ASSERT_STREQ(socket->getLocalAddress()->toString()->c_str(), "/127.0.0.1");
    ASSERT_STREQ(socket->getRemoteSocketAddress()->toString()->c_str(), "localhost/127.0.0.1:1234");
    sp<ByteArray> buffer = new ByteArray(16);
    std::memcpy(buffer->c_arr(), "Hello", 6);
    socket->getOutputStream()->write(buffer, 0, 6);
    thread->join();
    socket->close();
}

TEST(Mindroid, TcpIpV4AnyHost) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<InetAddress> inetAddress = InetAddress::getByName("localhost");
    sp<Thread> thread = new Thread([=] {
        sp<ServerSocket> serverSocket = new ServerSocket();
        serverSocket->setReuseAddress(true);
        serverSocket->bind(new InetSocketAddress(Inet4Address::ANY, 1234));
        promise->complete(true);
        sp<Socket> socket = serverSocket->accept();
        ASSERT_EQ(socket->isConnected(), 1);
        sp<ByteArray> buffer = new ByteArray(16);
        ssize_t rc = socket->getInputStream()->read(buffer);
        ASSERT_EQ(rc, 6);
        sp<String> s = new String(buffer);
        ASSERT_STREQ(s->c_str(), "Hello");
        socket->close();
        serverSocket->close();
    });
    thread->start();
    promise->get();
    sp<Socket> socket = new Socket();
    socket->connect(new InetSocketAddress(inetAddress, 1234));
    sp<ByteArray> buffer = new ByteArray(16);
    std::memcpy(buffer->c_arr(), "Hello", 6);
    socket->getOutputStream()->write(buffer, 0, 6);
    thread->join();
    socket->close();
}

TEST(Mindroid, UdpIpV6Localhost) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<InetAddress> inetAddress = InetAddress::getByName("ip6-localhost");
    sp<Thread> thread = new Thread([=] {
        sp<DatagramSocket> datagramSocket = new DatagramSocket(1234, inetAddress);
        promise->complete(true);
        sp<ByteArray> buffer = new ByteArray(16);
        sp<DatagramPacket> datagramPacket = new DatagramPacket(buffer, buffer->size());
        datagramSocket->receive(datagramPacket);
        ASSERT_EQ(datagramPacket->getLength(), 6);
        sp<String> s = new String(datagramPacket->getData());
        ASSERT_STREQ(s->c_str(), "Hello");
        ASSERT_STREQ(datagramPacket->getAddress()->toString()->c_str(), "/::1");
        datagramSocket->close();
    });
    thread->start();
    promise->get();
    sp<DatagramSocket> datagramSocket = new DatagramSocket();
    sp<ByteArray> buffer = new ByteArray(16);
    std::memcpy(buffer->c_arr(), "Hello", 6);
    sp<DatagramPacket> datagramPacket = new DatagramPacket(buffer, 6);
    datagramPacket->setAddress(inetAddress);
    datagramPacket->setPort(1234);
    datagramSocket->send(datagramPacket);
    thread->join();
    datagramSocket->close();
}

TEST(Mindroid, UdpIpV6AnyHost) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<InetAddress> inetAddress = InetAddress::getByName("ip6-localhost");
    sp<Thread> thread = new Thread([=] {
        sp<DatagramSocket> datagramSocket = new DatagramSocket(1234, Inet6Address::ANY);
        promise->complete(true);
        sp<ByteArray> buffer = new ByteArray(16);
        sp<DatagramPacket> datagramPacket = new DatagramPacket(buffer, buffer->size());
        datagramSocket->receive(datagramPacket);
        ASSERT_EQ(datagramPacket->getLength(), 6);
        sp<String> s = new String(datagramPacket->getData());
        ASSERT_STREQ(s->c_str(), "Hello");
        ASSERT_STREQ(datagramPacket->getAddress()->toString()->c_str(), "/::1");
        datagramSocket->close();
    });
    thread->start();
    promise->get();
    sp<DatagramSocket> datagramSocket = new DatagramSocket();
    sp<ByteArray> buffer = new ByteArray(16);
    std::memcpy(buffer->c_arr(), "Hello", 6);
    sp<DatagramPacket> datagramPacket = new DatagramPacket(buffer, 6);
    datagramPacket->setAddress(inetAddress);
    datagramPacket->setPort(1234);
    datagramSocket->send(datagramPacket);
    thread->join();
    datagramSocket->close();
}

TEST(Mindroid, UdpIpV4Localhost) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<InetAddress> inetAddress = InetAddress::getByName("localhost");
    sp<Thread> thread = new Thread([=] {
        sp<DatagramSocket> datagramSocket = new DatagramSocket(1234, inetAddress);
        promise->complete(true);
        sp<ByteArray> buffer = new ByteArray(16);
        sp<DatagramPacket> datagramPacket = new DatagramPacket(buffer, buffer->size());
        datagramSocket->receive(datagramPacket);
        ASSERT_EQ(datagramPacket->getLength(), 6);
        sp<String> s = new String(datagramPacket->getData());
        ASSERT_STREQ(s->c_str(), "Hello");
        ASSERT_STREQ(datagramPacket->getAddress()->toString()->c_str(), "/127.0.0.1");
        datagramSocket->close();
    });
    thread->start();
    promise->get();
    sp<DatagramSocket> datagramSocket = new DatagramSocket();
    sp<ByteArray> buffer = new ByteArray(16);
    std::memcpy(buffer->c_arr(), "Hello", 6);
    sp<DatagramPacket> datagramPacket = new DatagramPacket(buffer, 6);
    datagramPacket->setAddress(inetAddress);
    datagramPacket->setPort(1234);
    datagramSocket->send(datagramPacket);
    thread->join();
    datagramSocket->close();
}

TEST(Mindroid, UdpIpV4AnyHost) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<InetAddress> inetAddress = InetAddress::getByName("localhost");
    sp<Thread> thread = new Thread([=] {
        sp<DatagramSocket> datagramSocket = new DatagramSocket(1234, Inet4Address::ANY);
        promise->complete(true);
        sp<ByteArray> buffer = new ByteArray(16);
        sp<DatagramPacket> datagramPacket = new DatagramPacket(buffer, buffer->size());
        datagramSocket->receive(datagramPacket);
        ASSERT_EQ(datagramPacket->getLength(), 6);
        sp<String> s = new String(datagramPacket->getData());
        ASSERT_STREQ(s->c_str(), "Hello");
        ASSERT_STREQ(datagramPacket->getAddress()->toString()->c_str(), "/127.0.0.1");
        datagramSocket->close();
    });
    thread->start();
    promise->get();
    sp<DatagramSocket> datagramSocket = new DatagramSocket();
    sp<ByteArray> buffer = new ByteArray(16);
    std::memcpy(buffer->c_arr(), "Hello", 6);
    sp<DatagramPacket> datagramPacket = new DatagramPacket(buffer, 6);
    datagramPacket->setAddress(inetAddress);
    datagramPacket->setPort(1234);
    datagramSocket->send(datagramPacket);
    thread->join();
    datagramSocket->close();
}
