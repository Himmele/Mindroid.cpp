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

#include <mindroid/lang/ByteArray.h>
#include <mindroid/lang/Class.h>
#include <mindroid/lang/String.h>
#include <mindroid/lang/StringBuilder.h>
#include <mindroid/lang/IndexOutOfBoundsException.h>

namespace mindroid {

ByteArray::ByteArray(size_t size) : mBuffer(size, 0) {
}

ByteArray::ByteArray(const sp<ByteArray>& other) : ByteArray(other->mBuffer) {
}

ByteArray::ByteArray(const std::vector<uint8_t>& other) : mBuffer(other) {
}

ByteArray::ByteArray(const uint8_t* buffer, size_t size) : mBuffer(buffer, buffer + size) {
}

sp<ByteArray> ByteArray::valueOf(const std::initializer_list<uint8_t>& list) {
    return new ByteArray(std::vector<uint8_t>(std::move(list)));
}

size_t ByteArray::size() const {
    return mBuffer.size();
}

const uint8_t* ByteArray::c_arr() const {
    return mBuffer.data();
}

uint8_t* ByteArray::c_arr() {
    return mBuffer.data();
}

sp<String> ByteArray::toString() const {
    sp<StringBuilder> sb = new StringBuilder{"{"};
    const uint8_t* arr = c_arr();
    for (size_t i = 0; i < size(); ++i) {
        if (i != 0) {
            sb->append(", ");
        }
        sb->append(arr[i]);
    }
    return sb->append("}")->toString();
}

sp<ByteArray> ByteArray::clone() const {
    return new ByteArray(mBuffer);
}

const uint8_t& ByteArray::get(size_t position) const {
    if (position >= mBuffer.size()) {
        throw IndexOutOfBoundsException();
    }
    return mBuffer[position];
}

const uint8_t& ByteArray::operator[](size_t position) const {
    if (position >= mBuffer.size()) {
        throw IndexOutOfBoundsException();
    }
    return mBuffer[position];
}

uint8_t& ByteArray::get(size_t position) {
    if (position >= mBuffer.size()) {
        throw IndexOutOfBoundsException();
    }
    return mBuffer[position];
}

void ByteArray::set(size_t position, uint8_t value) {
    if (position >= mBuffer.size()) {
        throw IndexOutOfBoundsException();
    }
    mBuffer[position] = value;
}

uint8_t& ByteArray::operator[](size_t position) {
    if (position >= mBuffer.size()) {
        throw IndexOutOfBoundsException();
    }
    return mBuffer[position];
}

bool ByteArray::operator==(const ByteArray& other) const {
    return size() == other.size() && std::memcmp(c_arr(), other.c_arr(), size()) == 0;
}

bool ByteArray::operator==(const sp<ByteArray>& other) const {
    if (other != nullptr) {
        return operator==(*other);
    }
    return false;
}

bool ByteArray::equals(const sp<Object>& object) const {
    if (Class<ByteArray>::isInstance(object)) {
        sp<ByteArray> o = object_cast<ByteArray>(object);
        return *this == *o;
    }
    return false;
}

} /* namespace mindroid */
