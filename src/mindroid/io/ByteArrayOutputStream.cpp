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

#include <mindroid/io/ByteArrayOutputStream.h>
#include <mindroid/lang/System.h>
#include <mindroid/lang/NullPointerException.h>
#include <mindroid/lang/IndexOutOfBoundsException.h>

namespace mindroid {

ByteArrayOutputStream::ByteArrayOutputStream() :
        mBuffer(new ByteArray(64)),
        mCount(0) {
}

ByteArrayOutputStream::ByteArrayOutputStream(size_t size) :
        mBuffer(new ByteArray(size)),
        mCount(0) {
}

ByteArrayOutputStream::ByteArrayOutputStream(const sp<ByteArray>& buffer) :
        mBuffer(buffer),
        mCount(buffer->size()) {
}

void ByteArrayOutputStream::close() {
    OutputStream::close();
}

void ByteArrayOutputStream::flush() {
}

void ByteArrayOutputStream::reset() {
    mCount = 0;
}

size_t ByteArrayOutputStream::size() const {
    return mCount;
}

sp<ByteArray> ByteArrayOutputStream::toByteArray() const {
    return new ByteArray(mBuffer->c_arr(), mCount);
}

void ByteArrayOutputStream::write(const sp<ByteArray>& buffer)  {
    write(buffer, 0, buffer->size());
}

void ByteArrayOutputStream::write(const sp<ByteArray>& buffer, size_t offset, size_t count) {
    if (buffer == nullptr) {
        throw NullPointerException();
    }
    if ((offset + count) > buffer->size()) {
        throw IndexOutOfBoundsException();
    }

    if (count == 0) {
        return;
    }
    expand(count);
    System::arraycopy(buffer, offset, mBuffer, mCount, count);
    mCount += count;
}

void ByteArrayOutputStream::write(int32_t b) {
    if (mCount == mBuffer->size()) {
        expand(1);
    }
    mBuffer->set(mCount++, (uint8_t) b);
}

void ByteArrayOutputStream::expand(size_t i) {
    if (mCount + i <= mBuffer->size()) {
        return;
    }

    sp<ByteArray> buffer = new ByteArray((mCount + i) * 2);
    System::arraycopy(mBuffer, 0, buffer, 0, mCount);
    mBuffer = buffer;
}

} /* namespace mindroid */
