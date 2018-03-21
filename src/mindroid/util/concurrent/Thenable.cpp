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
#include <mindroid/util/Assert.h>
#include <mindroid/os/Handler.h>

namespace mindroid {

Thenable::Thenable() :
        mLock(new ReentrantLock()),
        mCondition(mLock->newCondition()),
        mIsDone(false) {
    if (Looper::myLooper() != nullptr) {
        sp<Handler> handler = new Handler();
        mExecutor = handler->asExecutor();
    } else {
        mExecutor = nullptr;
    }
}

Thenable::Thenable(const sp<Handler>& handler) :
        mLock(new ReentrantLock()),
        mCondition(mLock->newCondition()),
        mIsDone(false) {
    Assert::assertNotNull("Handler must not be null", handler);
    mExecutor = handler->asExecutor();
}

Thenable::Thenable(const sp<Executor>& executor) :
        mLock(new ReentrantLock()),
        mCondition(mLock->newCondition()),
        mIsDone(false) {
    Assert::assertNotNull("Executor must not be null", executor);
    mExecutor = executor;
}

} /* namespace mindroid */
