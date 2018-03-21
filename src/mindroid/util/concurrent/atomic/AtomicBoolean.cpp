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

#include <mindroid/util/concurrent/atomic/AtomicBoolean.h>

namespace mindroid {

AtomicBoolean::AtomicBoolean() {
    set(false);
}

AtomicBoolean::AtomicBoolean(bool initialValue) {
    set(initialValue);
}

bool AtomicBoolean::get() {
    return mValue.load();
}

void AtomicBoolean::set(bool newValue) {
    mValue.store(newValue);
}

bool AtomicBoolean::compareAndSet(bool expect, bool update) {
    return mValue.compare_exchange_strong(expect, update);
}

} /* namespace mindroid */
