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

#ifndef MINDROID_UTIL_CONCURRENT_LOCKS_REENTRANTLOCK_H_
#define MINDROID_UTIL_CONCURRENT_LOCKS_REENTRANTLOCK_H_

#include <mindroid/util/concurrent/locks/Lock.h>
#include <mindroid/util/concurrent/locks/Condition.h>
#include <mutex>

namespace mindroid {

class ReentrantLock :
        public Lock {
public:
    ReentrantLock() = default;
    virtual ~ReentrantLock() = default;
    ReentrantLock(const ReentrantLock&) = delete;
    ReentrantLock& operator=(const ReentrantLock&) = delete;
    void lock() override;
    sp<Condition> newCondition() override;
    bool tryLock(uint64_t timeoutMillis) override;
    void unlock() override;

private:
    std::recursive_timed_mutex mLock;
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_CONCURRENT_LOCKS_REENTRANTLOCK_H_ */
