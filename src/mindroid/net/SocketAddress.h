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

#ifndef MINDROID_NET_SOCKETADDRESS_H_
#define MINDROID_NET_SOCKETADDRESS_H_

#include <mindroid/lang/Object.h>

namespace mindroid {

/**
 * This abstract class represents a protocol-independent base for
 * socket-endpoint representing classes. The class has to be implemented
 * according to a specific protocol.
 */
class SocketAddress :
        public Object {
public:
    virtual ~SocketAddress() = default;
};

} /* namespace mindroid */

#endif /* MINDROID_NET_SOCKETADDRESS_H_ */
