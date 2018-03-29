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

#include <mindroid/util/concurrent/locks/ConditionImpl.h>

namespace mindroid {

ConditionImpl::ConditionImpl(std::recursive_timed_mutex& lock) :
        mLock(lock) {
}

void ConditionImpl::await() {
    mCondition.wait(mLock);
}

bool ConditionImpl::await(uint64_t timeoutMillis) {
    return mCondition.wait_for(mLock, std::chrono::milliseconds(timeoutMillis)) == std::cv_status::no_timeout;
}

void ConditionImpl::signal() {
    mCondition.notify_one();
}

void ConditionImpl::signalAll() {
    mCondition.notify_all();
}

} /* namespace mindroid */
