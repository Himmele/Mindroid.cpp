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

#ifndef MINDROID_NET_STANDARDSOCKETOPTIONS_H_
#define MINDROID_NET_STANDARDSOCKETOPTIONS_H_

#include <mindroid/net/NetworkInterface.h>
#include <mindroid/net/SocketOption.h>

namespace mindroid {

class StandardSocketOptions {
public:
    // See the corresponding SO_REUSEPORT, SO_REUSEADDR etc. in socket.h.

    static const SocketOption<bool> REUSE_ADDRESS;
    static const SocketOption<bool> REUSE_PORT;
    static const SocketOption<sp<NetworkInterface>> MULTICAST_INTERFACE;
};

} /* namespace mindroid */

#endif // MINDROID_NET_STANDARDSOCKETOPTIONS_H_
