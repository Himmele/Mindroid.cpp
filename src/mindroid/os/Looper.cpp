/*
 * Copyright (C) 2006 The Android Open Source Project
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

#include <mindroid/os/Looper.h>
#include <mindroid/os/Handler.h>
#include <mindroid/lang/RuntimeException.h>

namespace mindroid {

thread_local sp<Looper> tlsLooper;

Looper::Looper(bool quitAllowed) {
    mMessageQueue = new MessageQueue(quitAllowed);
    mThread = Thread::currentThread();
}

void Looper::prepare(bool quitAllowed) {
    if (tlsLooper != nullptr) {
        throw RuntimeException("Only one Looper may be created per thread");
    }
    tlsLooper = new Looper(quitAllowed);
}

void Looper::loop() {
    sp<Looper> me = myLooper();
    if (me == nullptr) {
        throw RuntimeException("No Looper; Looper.prepare() wasn't called on this thread");
    }
    sp<MessageQueue> mq = me->mMessageQueue;

    for (;;) {
        sp<Message> message = mq->dequeueMessage();
        if (message == nullptr) {
            return;
        }
        message->target->dispatchMessage(message);
        message->recycle();
    }
}

sp<Looper> Looper::myLooper() {
    return tlsLooper;
}

void Looper::quit() {
    mMessageQueue->quit();
}

} /* namespace mindroid */
