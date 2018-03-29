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

#ifndef MINDROID_UTIL_CONCURRENT_LOCKS_CONDITIONIMPL_H_
#define MINDROID_UTIL_CONCURRENT_LOCKS_CONDITIONIMPL_H_

#include <mindroid/util/concurrent/locks/Condition.h>
#include <condition_variable>
#include <mutex>

namespace mindroid {

class ConditionImpl :
        public Condition {
public:
    virtual ~ConditionImpl() = default;
    ConditionImpl(const ConditionImpl&) = delete;
    ConditionImpl& operator=(const ConditionImpl&) = delete;
    virtual void await();
    virtual bool await(uint64_t timeoutMillis);
    virtual void signal();
    virtual void signalAll();

private:
    ConditionImpl(std::recursive_timed_mutex& lock);

    std::recursive_timed_mutex& mLock;
    std::condition_variable_any mCondition;

    friend class ReentrantLock;
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_CONCURRENT_LOCKS_CONDITIONIMPL_H_ */
