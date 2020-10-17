/*
 * Copyright (C) 2018 E.S.R.Labs
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

#include <mindroid/util/concurrent/Thenable.h>
#include <mindroid/util/Log.h>
#include <mindroid/lang/NullPointerException.h>
#include <mindroid/os/Handler.h>

namespace mindroid {

Thenable::Thenable() :
        mLock(new ReentrantLock()),
        mCondition(mLock->newCondition()) {
    if (Looper::myLooper() != nullptr) {
        sp<Handler> handler = new Handler();
        mExecutor = handler->asExecutor();
    } else {
        mExecutor = nullptr;
    }
}

Thenable::Thenable(const sp<Handler>& handler) :
        mLock(new ReentrantLock()),
        mCondition(mLock->newCondition()) {
    if (handler == nullptr) {
        throw NullPointerException("Handler must not be null");
    }
    mExecutor = handler->asExecutor();
}

Thenable::Thenable(const sp<Executor>& executor) :
        mLock(new ReentrantLock()),
        mCondition(mLock->newCondition()) {
    mExecutor = executor;
}

void Thenable::logException(const sp<Exception>& exception) {
    Log::e("Promise", "Uncaught exception: %s", (exception != nullptr && exception->getMessage() != nullptr) ? exception->getMessage()->c_str() : "nullptr");
}

} /* namespace mindroid */
