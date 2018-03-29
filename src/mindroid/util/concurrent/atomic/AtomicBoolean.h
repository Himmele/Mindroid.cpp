/*
 * Copyright (C) 2017 E.S.R.Labs
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

#ifndef MINDROID_UTIL_CONCURRENT_ATOMIC_ATOMICBOOLEAN_H_
#define MINDROID_UTIL_CONCURRENT_ATOMIC_ATOMICBOOLEAN_H_

#include <mindroid/lang/Object.h>
#include <atomic>

namespace mindroid {

class AtomicBoolean :
        public Object {
public:
    AtomicBoolean();
    AtomicBoolean(bool initialValue);
    AtomicBoolean(const AtomicBoolean&) = delete;
    AtomicBoolean& operator=(const AtomicBoolean&) = delete;

    bool get();
    void set(bool newValue);
    bool compareAndSet(bool expect, bool update);

private:
    std::atomic<bool> mValue;
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_CONCURRENT_ATOMIC_ATOMICBOOLEAN_H_ */
