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
#include <mindroid/lang/System.h>
#include <mindroid/lang/NullPointerException.h>
#include <mindroid/nio/ByteBuffer.h>
#include <mindroid/nio/ByteArrayBuffer.h>
#include <mindroid/nio/BufferOverflowException.h>
#include <mindroid/nio/BufferUnderflowException.h>

namespace mindroid {

ByteBuffer::ByteBuffer(const sp<ByteArray>& buffer, bool readOnly) :
        Buffer(0, buffer->size(), buffer->size(), readOnly), mBuffer(buffer), mOffset(0) {
}

ByteBuffer::ByteBuffer(const sp<ByteArray>& buffer, size_t position, size_t limit, size_t capacity, bool readOnly, size_t offset) :
        Buffer(position, limit, capacity, readOnly), mBuffer(buffer), mOffset(offset) {
    if (position > limit) {
        throw IndexOutOfBoundsException("position > limit");
    }
    if (limit > capacity) {
        throw IndexOutOfBoundsException("limit > capacity");
    }
    if (capacity > buffer->size()) {
        throw IndexOutOfBoundsException("capacity > buffer size");
    }
    if (offset + limit > buffer->size()) {
        throw IndexOutOfBoundsException("buffer overflow");
    }
}

sp<ByteBuffer> ByteBuffer::allocate(size_t capacity) {
    return new ByteArrayBuffer(capacity);
}

size_t ByteBuffer::arrayOffset() const {
    return mOffset;
}

int32_t ByteBuffer::compareTo(const sp<ByteBuffer>& other) const {
    if (other == nullptr) {
        return false;
    }
    return compareTo(*other);
}

int32_t ByteBuffer::compareTo(const ByteBuffer& other) const {
    size_t count = (remaining() < other.remaining()) ? remaining() : other.remaining();
    int32_t comparison = std::memcmp(mBuffer->c_arr() + mPosition, other.mBuffer->c_arr() + other.mPosition, count);
    if (comparison != 0) {
        return comparison;
    } else {
        return remaining() - other.remaining();
    }
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
    return compareTo(other) == 0;
}

bool ByteBuffer::operator!=(const ByteBuffer& other) const {
    return !(*this == other);
}

uint8_t ByteBuffer::get() {
    return get(mPosition++);
}

uint8_t ByteBuffer::get(size_t index) {
    checkBufferUnderflow(index, sizeof(char));
    return mBuffer->get(mOffset + index);
}

char ByteBuffer::getChar() {
    auto value = getChar(mPosition);
    mPosition += sizeof(value);
    return value;
}

char ByteBuffer::getChar(size_t index) {
    checkBufferUnderflow(index, sizeof(char));
    return (char) mBuffer->get(mOffset + index);
}

double ByteBuffer::getDouble() {
    auto value = getDouble(mPosition);
    mPosition += sizeof(value);
    return value;
}

double ByteBuffer::getDouble(size_t index) {
    checkBufferUnderflow(index, sizeof(double));
    return *reinterpret_cast<double*>(mBuffer->c_arr() + mOffset + index);
}

float ByteBuffer::getFloat() {
    auto value = getFloat(mPosition);
    mPosition += sizeof(value);
    return value;
}

float ByteBuffer::getFloat(size_t index) {
    checkBufferUnderflow(index, sizeof(float));
    return *reinterpret_cast<float*>(mBuffer->c_arr() + mOffset + index);
}

int32_t ByteBuffer::getInt() {
    auto value = getInt(mPosition);
    mPosition += sizeof(value);
    return value;
}

int32_t ByteBuffer::getInt(size_t index) {
    checkBufferUnderflow(index, sizeof(int32_t));
    return (*reinterpret_cast<int32_t*>(mBuffer->c_arr() + mOffset + index));
}

int64_t ByteBuffer::getLong() {
    auto value = getLong(mPosition);
    mPosition += sizeof(value);
    return value;
}

int64_t ByteBuffer::getLong(size_t index) {
    checkBufferUnderflow(index, sizeof(int64_t));
    return (*reinterpret_cast<int64_t*>(mBuffer->c_arr() + mOffset + index));
}

int16_t ByteBuffer::getShort() {
    auto value = getShort(mPosition);
    mPosition += sizeof(value);
    return value;
}

int16_t ByteBuffer::getShort(size_t index) {
    checkBufferUnderflow(index, sizeof(int16_t));
    return (*reinterpret_cast<int16_t*>(mBuffer->c_arr() + mOffset + index));
}

sp<ByteBuffer> ByteBuffer::get(const sp<ByteArray>& dst) {
    get(dst, 0, dst->size());
    return this;
}

sp<ByteBuffer> ByteBuffer::get(const sp<ByteArray>& dst, size_t offset, size_t count) {
    if (dst == nullptr) {
        throw NullPointerException();
    }
    if ((offset + count) > dst->size()) {
        throw IndexOutOfBoundsException();
    }
    checkBufferUnderflow(mPosition, count);
    System::arraycopy(mBuffer, mOffset + mPosition, dst, offset, count);
    mPosition += count;
    return this;
}

sp<ByteBuffer> ByteBuffer::put(uint8_t value) {
    return put(mPosition++, value);
}

sp<ByteBuffer> ByteBuffer::put(size_t index, uint8_t value) {
    checkBufferOverflow(index, sizeof(uint8_t));
    mBuffer->set(mOffset + index, value);
    return this;
}

sp<ByteBuffer> ByteBuffer::putChar(char value) {
    return put(mPosition++, value);
}

sp<ByteBuffer> ByteBuffer::putChar(size_t index, char value) {
    checkBufferOverflow(index, sizeof(value));
    mBuffer->set(mOffset + index, value);
    return this;
}

sp<ByteBuffer> ByteBuffer::putDouble(double value) {
    putDouble(mPosition, value);
    mPosition += sizeof(value);
    return this;
}

sp<ByteBuffer> ByteBuffer::putDouble(size_t index, double value) {
    checkBufferOverflow(index, sizeof(value));
    *reinterpret_cast<double*>(mBuffer->c_arr() + mOffset + index) = value;
    return this;
}

sp<ByteBuffer> ByteBuffer::putFloat(float value) {
    putFloat(mPosition, value);
    mPosition += sizeof(value);
    return this;
}

sp<ByteBuffer> ByteBuffer::putFloat(size_t index, float value) {
    checkBufferOverflow(index, sizeof(value));
    *reinterpret_cast<float*>(mBuffer->c_arr() + mOffset + index) = value;
    return this;
}

sp<ByteBuffer> ByteBuffer::putInt(int32_t value) {
    putInt(mPosition, value);
    mPosition += sizeof(value);
    return this;
}

sp<ByteBuffer> ByteBuffer::putInt(size_t index, int32_t value) {
    checkBufferOverflow(index, sizeof(value));
    *reinterpret_cast<int32_t*>(mBuffer->c_arr() + mOffset + index) = value;
    return this;
}

sp<ByteBuffer> ByteBuffer::putLong(int64_t value) {
    putLong(mPosition, value);
    mPosition += sizeof(value);
    return this;
}

sp<ByteBuffer> ByteBuffer::putLong(size_t index, int64_t value) {
    checkBufferOverflow(index, sizeof(value));
    *reinterpret_cast<int64_t*>(mBuffer->c_arr() + mOffset + index) = value;
    return this;
}

sp<ByteBuffer> ByteBuffer::putShort(int16_t value) {
    putShort(mPosition, value);
    mPosition += sizeof(value);
    return this;
}

sp<ByteBuffer> ByteBuffer::putShort(size_t index, int16_t value) {
    checkBufferOverflow(index, sizeof(value));
    *reinterpret_cast<short*>(mBuffer->c_arr() + mOffset + index) = value;
    return this;
}

sp<ByteBuffer> ByteBuffer::put(const sp<ByteArray>& byteArray) {
    return put(byteArray, 0, byteArray->size());
}

sp<ByteBuffer> ByteBuffer::put(const sp<ByteArray>& src, size_t offset, size_t size) {
    if (src == nullptr) {
        throw NullPointerException();
    }
    if ((offset + size) > src->size()) {
        throw IndexOutOfBoundsException();
    }
    checkBufferOverflow(mPosition, size);
    System::arraycopy(src, offset, mBuffer, mOffset + mPosition, size);
    mPosition += size;
    return this;
}

sp<String> ByteBuffer::toString() const {
    return Buffer::toString();
}

sp<ByteBuffer> ByteBuffer::wrap(const sp<ByteArray>& array) {
    return new ByteArrayBuffer(array);
}

sp<ByteBuffer> ByteBuffer::wrap(const sp<ByteArray>& array, size_t offset, size_t size) {
    return new ByteArrayBuffer(array, offset, size);
}

void ByteBuffer::checkBufferOverflow(size_t index, size_t amount) {
    if (index + amount > mLimit) {
        throw BufferOverflowException();
    }
}

void ByteBuffer::checkBufferUnderflow(size_t index, size_t amount) {
    if (index + amount > mLimit) {
        throw BufferUnderflowException();
    }
}

} /* namespace mindroid */
