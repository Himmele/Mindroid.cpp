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

#include "HandlerExample.h"
#include <mindroid/lang/System.h>
#include <mindroid/util/Log.h>
#include <cstdio>

using namespace mindroid;

namespace examples {
namespace concurrency {

const char* const HandlerExample::TAG = "HandlerExample";

HandlerExample::HandlerExample() {
    Log::d(TAG, "HandlerExample::ctor");
}

HandlerExample::~HandlerExample() {
    Log::d(TAG, "HandlerExample::dtor");
}

void HandlerExample::onCreate() {
    Log::d(TAG, "HandlerExample::onCreate");
    mWorldHandler = new WorldHandler();
    mHelloHandler = new HelloHandler(mWorldHandler);
    mHelloHandler->obtainMessage(SAY_HELLO)->sendToTarget();
}

void HandlerExample::onDestroy() {
    Log::d(TAG, "HandlerExample::onDestroy");
    mHelloHandler->removeMessages(SAY_HELLO);
    mWorldHandler->removeMessages(SAY_WORLD);
}

sp<IBinder> HandlerExample::onBind(const sp<Intent>& intent) {
    return nullptr;
}

void HandlerExample::HelloHandler::handleMessage(const sp<Message>& msg) {
    switch (msg->what) {
    case SAY_HELLO:
        printf("Hello ");
        sp<Message> message = mWorldHandler->obtainMessage(SAY_WORLD);
        message->getData()->putObject("Handler", this);
        message->sendToTarget();
        break;
    }
}

void HandlerExample::WorldHandler::handleMessage(const sp<Message>& msg) {
    switch (msg->what) {
    case SAY_WORLD:
        printf("World!\n");
        sp<Handler> helloHandler = object_cast<Handler>(msg->getData()->getObject("Handler"));
        sp<Message> message = helloHandler->obtainMessage(SAY_HELLO);
        helloHandler->sendMessageDelayed(message, 1000);
        break;
    }
}

} /* namespace concurrency */
} /* namespace examples */
