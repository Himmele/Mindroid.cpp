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

#include <mindroid/lang/System.h>
#include <mindroid/lang/ByteArray.h>
#include <mindroid/lang/NullPointerException.h>
#include <mindroid/lang/IndexOutOfBoundsException.h>
#include <climits>
#include <unistd.h>
#include <sys/utsname.h>

namespace mindroid {

std::mutex System::sLock;
System* System::sInstance = nullptr;

System::System() {
    struct utsname info;
    if (::uname(&info) == 0) {
        mSystemProperties->put(String::valueOf("os.arch"), String::valueOf(info.machine));
        mSystemProperties->put(String::valueOf("os.name"), String::valueOf(info.sysname));
        mSystemProperties->put(String::valueOf("os.version"), String::valueOf(info.release));
    }
    char cwd[PATH_MAX];
    if (::getcwd(cwd, sizeof(cwd)) != nullptr) {
        mSystemProperties->put(String::valueOf("user.dir"), String::valueOf(cwd));
    }
}

System* System::getInstance() {
    sLock.lock();
    if (sInstance == nullptr) {
        sInstance = new System();
    }
    sLock.unlock();
    return sInstance;
}

bool System::arraycopy(const void* src, const size_t srcPos, const size_t srcLength,
        const void* dest, const size_t destPos, const size_t destLength, const size_t length) {
    if (srcPos + length > srcLength) {
        return false;
    }
    if (destPos + length > destLength) {
        return false;
    }
    if (src == nullptr || dest == nullptr) {
        return false;
    }
    std::memcpy(((uint8_t*) dest) + destPos, ((uint8_t*) src) + srcPos, length);
    return true;
}

void System::arraycopy(const sp<ByteArray>& src, const size_t srcPos,
        const sp<ByteArray>& dest, const size_t destPos, const size_t length) {
    if (src == nullptr || dest == nullptr) {
        throw NullPointerException();
    }
    if (srcPos + length > src->size() || destPos + length > dest->size()) {
        throw IndexOutOfBoundsException();
    }
    std::memcpy(dest->c_arr() + destPos, src->c_arr() + srcPos, length);
}

sp<String> System::getProperty(const sp<String>& name, const sp<String>& defaultValue) {
    System* self = getInstance();
    AutoLock autoLock(self->mLock);
    if (self->mSystemProperties->containsKey(name)) {
        return self->mSystemProperties->get(name);
    } else {
        return defaultValue;
    }
}

sp<String> System::setProperty(const sp<String>& name, const sp<String>& value) {
    System* self = getInstance();
    AutoLock autoLock(self->mLock);
    return self->mSystemProperties->put(name, value);
}

} /* namespace mindroid */
