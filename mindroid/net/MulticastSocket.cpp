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

#include "mindroid/net/MulticastSocket.h"
#include "mindroid/lang/String.h"

#include <stdio.h>

namespace mindroid {

MulticastSocket::MulticastSocket(uint16_t port) :
	DatagramSocket(port),
	mJoinedAddresses(new List<struct ip_mreq>()) {
}

MulticastSocket::~MulticastSocket() {
	RequestList::iterator i = mJoinedAddresses->begin();
	while (i != mJoinedAddresses->end()) {
		struct ip_mreq mreq = *i;
		int res = ::setsockopt(getId(), IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
		if (-1 == res) {
			perror("could not drop multicast membership\n");
		}
		i++;
	}
}

bool MulticastSocket::join(const String& addr) {
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(addr.c_str());
	mreq.imr_interface.s_addr = INADDR_ANY;
	int res = ::setsockopt(getId(), IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	if (-1 == res) {
		perror("could not add multicast membership\n");
		return false;
	}
	mJoinedAddresses->push_back(mreq);
	return true;
}

}
