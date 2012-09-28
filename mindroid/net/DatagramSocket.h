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

#ifndef MINDROID_DATAGRAMSOCKET_H_
#define MINDROID_DATAGRAMSOCKET_H_

#include "mindroid/os/Ref.h"

namespace mindroid {

class SocketAddress;

class DatagramSocket :
		public Ref
{
public:
	DatagramSocket();
	DatagramSocket(uint16_t port);
	DatagramSocket(const char* host, uint16_t port);
	virtual ~DatagramSocket();

	bool bind(uint16_t port);
	bool bind(const char* host, uint16_t port);
	ssize_t recv(uint8_t* data, size_t size);
	ssize_t recv(uint8_t* data, size_t size, sp<SocketAddress>& sender);
	bool send(const void* data, size_t size, const sp<SocketAddress>& receiver);
	void close();
	bool setBlockingMode(bool blockingIO);
	bool isBound() const { return mIsBound; }
	bool isClosed() const { return mIsClosed; }
	int getId() const { return mSocketId; }

private:
	int mSocketId;
	bool mIsBound;
	bool mIsClosed;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(DatagramSocket)
};

} /* namespace mindroid */

#endif /*MINDROID_DATAGRAMSOCKET_H_*/
