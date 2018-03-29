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

#include <mindroid/os/Handler.h>
#include <mindroid/os/Message.h>
#include <mindroid/os/MessageQueue.h>
#include <mindroid/os/Looper.h>
#include <mindroid/lang/NullPointerException.h>

namespace mindroid {

Handler::Handler() : Handler(Looper::myLooper()) {
}

Handler::Handler(const sp<Callback>& callback) : Handler(Looper::myLooper(), callback) {
}

Handler::Handler(const sp<Looper>& looper) {
    mLooper = looper;
    if (mLooper == nullptr) {
        throw NullPointerException("Can't create handler inside thread that has not called Looper.prepare()");
    }
    mMessageQueue = looper->mMessageQueue;
    mCallback = nullptr;
}

Handler::Handler(const sp<Looper>& looper, const sp<Callback>& callback) {
    mLooper = looper;
    if (mLooper == nullptr) {
        throw NullPointerException("Can't create handler inside thread that has not called Looper.prepare()");
    }
    mMessageQueue = looper->mMessageQueue;
    mCallback = callback;
}

void Handler::dispatchMessage(const sp<Message>& msg) {
    if (msg->callback != nullptr) {
        handleCallback(msg);
    } else {
        if (mCallback != nullptr) {
            if (mCallback->handleMessage(msg)) {
                return;
            }
        }
        handleMessage(msg);
    }
}

bool Handler::removeCallbacks(const sp<Runnable>& runnable) {
    return mMessageQueue->removeCallbacks(this, runnable, nullptr);
}

bool Handler::removeCallbacks(const sp<Runnable>& runnable, const sp<Object>& token) {
    return mMessageQueue->removeCallbacks(this, runnable, token);
}

bool Handler::sendMessageAtTime(const sp<Message>& message, uint64_t uptimeMillis) {
    message->target = this;
    return mMessageQueue->enqueueMessage(message, uptimeMillis);
}

bool Handler::removeMessages(int32_t what) {
    return mMessageQueue->removeMessages(this, what, nullptr);
}

bool Handler::removeMessages(int32_t what, const sp<Object>& object) {
    return mMessageQueue->removeMessages(this, what, object);
}

bool Handler::removeCallbacksAndMessages(const sp<Object>& token) {
    return mMessageQueue->removeCallbacksAndMessages(this, token);
}

bool Handler::hasMessages(int32_t what) {
    return mMessageQueue->hasMessages(this, what, nullptr);
}

bool Handler::hasMessages(int32_t what, const sp<Object>& object) {
    return mMessageQueue->hasMessages(this, what, object);
}

bool Handler::hasCallbacks(const sp<Runnable>& runnable) {
    return mMessageQueue->hasMessages(this, runnable, nullptr);
}

sp<Message> Handler::getPostMessage(const sp<Runnable>& runnable) {
    const sp<Message> message = Message::obtain();
    message->callback = runnable;
    return message;
}

sp<Message> Handler::getPostMessage(const sp<Runnable>& runnable, const sp<Object>& token) {
    sp<Message> message = Message::obtain();
    message->obj = token;
    message->callback = runnable;
    return message;
}

sp<Executor> Handler::asExecutor() {
    sp<Executor> executor;
    if (mExecutor != nullptr && (executor = mExecutor.get()) != nullptr) {
        return executor;
    }

    class HandlerExecutor : public Executor {
    public:
        HandlerExecutor(const sp<Handler>& handler) : mHandler(handler) {
        }

        void execute(const sp<Runnable>& command) override {
            mHandler->post(command);
        }

        virtual bool cancel(const sp<Runnable>& runnable) override {
            return false;
        }

    private:
        sp<Handler> mHandler;
    };

    executor = new HandlerExecutor(this);
    mExecutor = executor;
    return executor;
}

} /* namespace mindroid */
