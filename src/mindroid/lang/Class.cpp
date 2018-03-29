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

#include <mindroid/lang/Class.h>

namespace mindroid {

std::mutex Classes::sLock;
Classes* Classes::sInstance = nullptr;

void Classes::put(const sp<String>& name, Factory* factory) {
    sLock.lock();
    mClasses->put(name, factory);
    sLock.unlock();
}

Factory* Classes::get(const sp<String>& name) {
    sLock.lock();
    Factory* factory = mClasses->get(name);
    sLock.unlock();
    return factory;
}

Classes* Classes::getInstance() {
    sLock.lock();
    if (sInstance == nullptr) {
        sInstance = new Classes();
    }
    sLock.unlock();
    return sInstance;
}

} /* namespace mindroid */
