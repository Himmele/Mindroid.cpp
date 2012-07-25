/*
 * Copyright (C) 2011 Daniel Himmelein
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

#ifndef MINDROID_MESSAGEQUEUE_H_
#define MINDROID_MESSAGEQUEUE_H_

#include <stdint.h>
#include <pthread.h>
#include "mindroid/util/Utils.h"
#include "mindroid/os/Ref.h"
#include "mindroid/os/Lock.h"
#include "mindroid/os/CondVar.h"

namespace mindroid {

class Message;
class Handler;
class Runnable;

class MessageQueue :
	public Ref
{
public:
	MessageQueue();
	virtual ~MessageQueue();
	bool enqueueMessage(const sp<Message>& message, uint64_t execTimestamp);
	sp<Message> dequeueMessage();
	bool removeMessages(const sp<Handler>& handler, int32_t what);
	bool removeCallbacks(const sp<Handler>& handler, const sp<Runnable>& runnable);
	bool removeCallbacksAndMessages(const sp<Handler>& handler);

private:
	sp<Message> getNextMessage(uint64_t now);

	sp<Message> mHeadMessage;
	Lock mCondVarLock;
	CondVar mCondVar;
	bool mLockMessageQueue;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(MessageQueue)
};

} /* namespace mindroid */

#endif /* MINDROID_MESSAGEQUEUE_H_ */
