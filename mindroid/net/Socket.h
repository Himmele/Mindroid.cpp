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

#ifndef MINDROID_SOCKET_H_
#define MINDROID_SOCKET_H_

#include "mindroid/os/Ref.h"

namespace mindroid {

class ServerSocket;

class Socket :
		public Ref
{
public:
	Socket();
	Socket(const char* host, uint16_t port);
	virtual ~Socket();

	int connect(const char* host, uint16_t port);
	ssize_t read(uint8_t* data, size_t size);
	ssize_t readFully(uint8_t* data, size_t size);
	bool write(const void* data, size_t size);
	void close();
	bool setBlockingMode(bool blockingIO);
	bool isConnected() const { return mIsConnected; }
	bool isClosed() const { return mIsClosed; }
	int getId() const { return mSocketId; }

private:
	int mSocketId;
	bool mIsConnected;
	bool mIsClosed;

	friend class ServerSocket;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(Socket)
};

} /* namespace mindroid */

#endif /* MINDROID_SOCKET_H_ */
