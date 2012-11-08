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

#ifndef MINDROID_MULTICASTSOCKET_H_
#define MINDROID_MULTICASTSOCKET_H_

#include "mindroid/util/List.h"
#include "mindroid/net/DatagramSocket.h"
#include <arpa/inet.h>

namespace mindroid {

class String;

class MulticastSocket : public DatagramSocket {
public:
	MulticastSocket(uint16_t port);
	~MulticastSocket();
	bool join(const String& addr);

private:
	typedef List< struct ip_mreq > RequestList;
	sp< RequestList > mJoinedAddresses;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(MulticastSocket)
};

}

#endif // MINDROID_MULTICASTSOCKET_H_
