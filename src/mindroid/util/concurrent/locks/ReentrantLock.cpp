/*
 * Copyright (C) 2011 Daniel Himmelein
 * Copyright (C) 2016 E.S.R.Labs
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

#include <mindroid/os/SystemClock.h>
#include <mindroid/util/concurrent/locks/ReentrantLock.h>
#include <mindroid/util/concurrent/locks/ConditionImpl.h>

namespace mindroid {

void ReentrantLock::lock() {
    mLock.lock();
}

sp<Condition> ReentrantLock::newCondition() {
    return new ConditionImpl(mLock);
}

bool ReentrantLock::tryLock(uint64_t timeoutMillis) {
    return mLock.try_lock_for(std::chrono::milliseconds(timeoutMillis));
}

void ReentrantLock::unlock() {
    mLock.unlock();
}

} /* namespace mindroid */
