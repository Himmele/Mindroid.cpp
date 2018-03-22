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

#include <mindroid/io/ByteArrayInputStream.h>
#include <mindroid/lang/System.h>
#include <mindroid/util/Assert.h>

namespace mindroid {

ByteArrayInputStream::ByteArrayInputStream(const sp<ByteArray>& buffer) :
        mBuffer(buffer),
        mPosition(0),
        mMark(0),
        mCount(0) {
    Assert::assertNotNull(buffer);
    mCount = buffer->size();
}

ByteArrayInputStream::ByteArrayInputStream(const sp<ByteArray>& buffer, size_t offset, size_t count) :
        mBuffer(buffer),
        mPosition(offset),
        mMark(offset),
        mCount(0) {
    Assert::assertNotNull(buffer);
    mCount = offset + count > buffer->size() ? buffer->size() : offset + count;
}

size_t ByteArrayInputStream::available() {
    return mCount - mPosition;
}

void ByteArrayInputStream::close() {
    InputStream::close();
}

void ByteArrayInputStream::mark(size_t limit) {
    mMark = mPosition;
}

bool ByteArrayInputStream::markSupported() {
    return true;
}

int32_t ByteArrayInputStream::read() {
    return mPosition < mCount ? mBuffer->get(mPosition++) & 0xFF : -1;
}

ssize_t ByteArrayInputStream::read(const sp<ByteArray>& buffer) {
    return read(buffer, 0, buffer->size());
}

ssize_t ByteArrayInputStream::read(const sp<ByteArray>& buffer, size_t offset, size_t count) {
    Assert::assertNotNull(buffer);
    Assert::assertFalse<IndexOutOfBoundsException>((offset < 0) || (count < 0) || ((offset + count) > buffer->size()));

    if (mPosition >= mCount) {
        return -1;
    }
    if (count == 0) {
        return 0;
    }

    count = mCount - mPosition < count ? mCount - mPosition : count;
    System::arraycopy(mBuffer, mPosition, buffer, offset, count);
    mPosition += count;
    return count;
}

void ByteArrayInputStream::reset() {
    mPosition = mMark;
}

size_t ByteArrayInputStream::skip(size_t count) {
    if (count <= 0) {
        return 0;
    }
    size_t position = mPosition;
    mPosition = mCount - mPosition < count ? mCount : (size_t) (mPosition + count);
    return mPosition - position;
}

} /* namespace mindroid */
