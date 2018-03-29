/*
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

#ifndef MINDROID_UTIL_CONCURRENT_ATOMIC_ATOMICINTEGER_H_
#define MINDROID_UTIL_CONCURRENT_ATOMIC_ATOMICINTEGER_H_

#include <mindroid/lang/Object.h>
#include <atomic>

namespace mindroid {

class AtomicInteger :
        public Object {
public:
    AtomicInteger();
    AtomicInteger(int32_t initialValue);
    AtomicInteger(const AtomicInteger&) = delete;
    AtomicInteger& operator=(const AtomicInteger&) = delete;

    int32_t get();
    void set(int32_t newValue);
    bool compareAndSet(int32_t expect, int32_t update);

private:
    std::atomic<int32_t> mValue;
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_CONCURRENT_ATOMIC_ATOMICINTEGER_H_ */
