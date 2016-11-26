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

#ifndef MINDROID_SOCKETADDRESS_H_
#define MINDROID_SOCKETADDRESS_H_

#include "mindroid/lang/Object.h"
#include "mindroid/lang/String.h"
#include <netinet/in.h>

namespace mindroid {

class SocketAddress :
        public Object
{
public:
    SocketAddress();
    SocketAddress(uint16_t port);
    SocketAddress(const char* host, uint16_t port) :
            SocketAddress(String::valueOf(host), port) {
    }
    SocketAddress(const sp<String>& host, uint16_t port);
    virtual ~SocketAddress() = default;

    SocketAddress(const SocketAddress&) = delete;
    SocketAddress& operator=(const SocketAddress&) = delete;

    sp<String> getHostName() const;
    uint16_t getPort() const { return ntohs(mSocketAddress.sin_port); }
    bool isUnresolved() const { return mIsUnresolved; }

private:
    struct sockaddr_in mSocketAddress;
    bool mIsUnresolved;

    friend class ServerSocket;
    friend class Socket;
    friend class DatagramSocket;
};

} /* namespace mindroid */

#endif /* MINDROID_SOCKETADDRESS_H_ */
