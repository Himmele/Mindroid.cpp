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

#include <mindroid/util/concurrent/atomic/AtomicInteger.h>

namespace mindroid {

AtomicInteger::AtomicInteger() {
    set(0);
}

AtomicInteger::AtomicInteger(int32_t initialValue) {
    set(initialValue);
}

int32_t AtomicInteger::get() {
    return mValue.load();
}

void AtomicInteger::set(int32_t newValue) {
    mValue.store(newValue);
}

bool AtomicInteger::compareAndSet(int32_t expect, int32_t update) {
    return mValue.compare_exchange_strong(expect, update);
}

int32_t AtomicInteger::getAndIncrement() {
    return mValue.fetch_add(1, std::memory_order_relaxed);
}

} /* namespace mindroid */
