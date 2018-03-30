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

#include <mindroid/lang/Class.h>
#include <mindroid/nio/ByteBuffer.h>
#include <mindroid/nio/ByteArrayBuffer.h>
#include <mindroid/nio/BufferOverflowException.h>

namespace mindroid {

ByteBuffer::ByteBuffer(const sp<ByteArray>& buffer, bool readOnly) :
        Buffer(buffer->size(), readOnly), mBuffer(buffer) {
}

ByteBuffer::ByteBuffer(const sp<ByteArray>& buffer, size_t offset, size_t count, bool readOnly) :
        Buffer(count, readOnly), mBuffer(new ByteArray(buffer->c_arr() + offset, count)) {
}

sp<ByteBuffer> ByteBuffer::allocate(size_t capacity) {
    return new ByteArrayBuffer(capacity);
}

size_t ByteBuffer::arrayOffset() {
    return mOffset;
}

int32_t ByteBuffer::compareTo(const sp<ByteBuffer>& other) const {
    if (other == nullptr) {
        return false;
    }
    return compareTo(*other);
}

int32_t ByteBuffer::compareTo(const ByteBuffer& other) const {
    return std::memcmp(mBuffer->c_arr(), other.mBuffer->c_arr(), std::min(mCapacity, other.mCapacity));
}

bool ByteBuffer::equals(const sp<Object>& other) const{
    if (other == this) {
        return true;
    }
    if (Class<ByteBuffer>::isInstance(other)) {
        sp<ByteBuffer> o = object_cast<ByteBuffer>(other);
        return *this == *o;
    }
    return false;
}

bool ByteBuffer::operator==(const ByteBuffer& other) const {
    return mCapacity == other.mCapacity && std::memcmp(mBuffer->c_arr(), other.mBuffer->c_arr(), mCapacity) == 0;
}

bool ByteBuffer::operator!=(const ByteBuffer& other) const {
    return !(*this == other);
}

uint8_t ByteBuffer::get() {
    return get(mPosition++);
}

uint8_t ByteBuffer::get(size_t index) {
    checkBufferOverflow(index, sizeof(char));
    return mBuffer->get(index);
}

char ByteBuffer::getChar() {
    auto value = getChar(mPosition);
    mPosition += sizeof(value);
    return value;
}

char ByteBuffer::getChar(size_t index) {
    checkBufferOverflow(index, sizeof(char));
    return getChar(index);
}

double ByteBuffer::getDouble() {
    auto value = getDouble(mPosition);
    mPosition += sizeof(value);
    return value;
}

double ByteBuffer::getDouble(size_t index) {
    checkBufferOverflow(index, sizeof(double));
    return *reinterpret_cast<double*>(mBuffer->c_arr() + index);
}

float ByteBuffer::getFloat() {
    auto value = getFloat(mPosition);
    mPosition += sizeof(value);
    return value;
}

float ByteBuffer::getFloat(size_t index) {
    checkBufferOverflow(index, sizeof(float));
    return *reinterpret_cast<float*>(mBuffer->c_arr() + index);
}

int32_t ByteBuffer::getInt() {
    auto value = getInt(mPosition);
    mPosition += sizeof(value);
    return value;
}

int32_t ByteBuffer::getInt(size_t index) {
    checkBufferOverflow(index, sizeof(int32_t));
    return (*reinterpret_cast<int32_t*>(mBuffer->c_arr() + index));
}

int64_t ByteBuffer::getLong() {
    auto value = getLong(mPosition);
    mPosition += sizeof(value);
    return value;
}

int64_t ByteBuffer::getLong(size_t index) {
    checkBufferOverflow(index, sizeof(int64_t));
    return (*reinterpret_cast<int64_t*>(mBuffer->c_arr() + index));
}

int16_t ByteBuffer::getShort() {
    auto value = getShort(mPosition);
    mPosition += sizeof(value);
    return value;
}

int16_t ByteBuffer::getShort(size_t index) {
    checkBufferOverflow(index, sizeof(int16_t));
    return (*reinterpret_cast<int16_t*>(mBuffer->c_arr() + index));
}

sp<ByteBuffer> ByteBuffer::put(uint8_t value) {
    return put(mPosition++, value);
}

sp<ByteBuffer> ByteBuffer::put(size_t index, uint8_t value) {
    checkBufferOverflow(index, sizeof(uint8_t));
    mBuffer->set(index, value);
    return this;
}

sp<ByteBuffer> ByteBuffer::putChar(char value) {
    return put(mPosition++, value);
}

sp<ByteBuffer> ByteBuffer::putChar(size_t index, char value) {
    checkBufferOverflow(index, sizeof(value));
    mBuffer->set(index, value);
    return this;
}

sp<ByteBuffer> ByteBuffer::putDouble(double value) {
    putDouble(mPosition, value);
    mPosition += sizeof(value);
    return this;
}

sp<ByteBuffer> ByteBuffer::putDouble(size_t index, double value) {
    checkBufferOverflow(index, sizeof(value));
    *reinterpret_cast<double*>(mBuffer->c_arr() + index) = value;
    return this;
}

sp<ByteBuffer> ByteBuffer::putFloat(float value) {
    putFloat(mPosition, value);
    mPosition += sizeof(value);
    return this;
}

sp<ByteBuffer> ByteBuffer::putFloat(size_t index, float value) {
    checkBufferOverflow(index, sizeof(value));
    *reinterpret_cast<float*>(mBuffer->c_arr() + index) = value;
    return this;
}

sp<ByteBuffer> ByteBuffer::putInt(int32_t value) {
    putInt(mPosition, value);
    mPosition += sizeof(value);
    return this;
}

sp<ByteBuffer> ByteBuffer::putInt(size_t index, int32_t value) {
    checkBufferOverflow(index, sizeof(value));
    *reinterpret_cast<int32_t*>(mBuffer->c_arr() + index) = value;
    return this;
}

sp<ByteBuffer> ByteBuffer::putLong(size_t index, int64_t value) {
    checkBufferOverflow(index, sizeof(value));
    *reinterpret_cast<int64_t*>(mBuffer->c_arr() + index) = value;
    return this;
}

sp<ByteBuffer> ByteBuffer::putLong(int64_t value) {
    putLong(mPosition, value);
    mPosition += sizeof(value);
    return this;
}

sp<ByteBuffer> ByteBuffer::putShort(size_t index, int16_t value) {
    checkBufferOverflow(index, sizeof(value));
    *reinterpret_cast<short*>(mBuffer->c_arr() + index) = value;
    return this;
}

sp<ByteBuffer> ByteBuffer::putShort(int16_t value) {
    putShort(mPosition, value);
    mPosition += sizeof(value);
    return this;
}

sp<String> ByteBuffer::toString() const {
    return Buffer::toString();
}

sp<ByteBuffer> ByteBuffer::wrap(const sp<ByteArray>& array) {
    return new ByteArrayBuffer(array);
}

sp<ByteBuffer> ByteBuffer::wrap(const sp<ByteArray>& array, size_t offset, size_t count) {
    return new ByteArrayBuffer(array, offset, count);
}

void ByteBuffer::checkBufferOverflow(size_t index, size_t amount) {
    if (index + amount > mCapacity) {
        throw BufferOverflowException();
    }
}

} /* namespace mindroid */
