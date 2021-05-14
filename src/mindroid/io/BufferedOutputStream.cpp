/*
 * Copyright (C) 2021 E.S.R.Labs
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

#include <mindroid/io/BufferedOutputStream.h>
#include <mindroid/lang/IllegalArgumentException.h>
#include <mindroid/lang/NullPointerException.h>
#include <mindroid/lang/System.h>
#include <mindroid/io/IOException.h>

namespace mindroid {

BufferedOutputStream::BufferedOutputStream(const sp<OutputStream>& outputStream, size_t size) :
        mOutputStream(outputStream) {
    if (size == 0) {
        throw IllegalArgumentException("size = 0");
    }
    mBuffer = new ByteArray(size);
}

void BufferedOutputStream::close() {
    mBuffer = nullptr;
    sp<OutputStream> localOutputStream = mOutputStream;
    mOutputStream = nullptr;
    if (localOutputStream != nullptr) {
        localOutputStream->close();
    }
}

void BufferedOutputStream::flush() {
    checkValidity();
    flushBuffer();
    mOutputStream->flush();
}

void BufferedOutputStream::write(const sp<ByteArray>& buffer, size_t offset, size_t count) {
    sp<ByteArray> localBuffer = mBuffer;
    checkValidity();

    if (buffer == nullptr) {
        throw NullPointerException("buffer == null");
    }

    if (count >= localBuffer->size()) {
        flushBuffer();
        mOutputStream->write(buffer, offset, count);
        return;
    }

    if (offset > buffer->size() || buffer->size() - offset < count) {
        throw IndexOutOfBoundsException();
    }

    if (count > (localBuffer->size() - mCount)) {
        flushBuffer();
    }

    System::arraycopy(buffer, offset, localBuffer, mCount, count);
    mCount += count;
}

void BufferedOutputStream::write(int32_t b) {
    checkValidity();

    if (mCount == mBuffer->size()) {
        mOutputStream->write(mBuffer, 0, mCount);
        mCount = 0;
    }

    mBuffer->set(mCount++, b);
}

void BufferedOutputStream::checkValidity() const {
    if (mBuffer == nullptr || mOutputStream == nullptr) {
        throw IOException("Stream closed");
    }
}

void BufferedOutputStream::flushBuffer() {
    if (mCount > 0) {
        mOutputStream->write(mBuffer, 0, mCount);
        mCount = 0;
    }
}

} /* namespace mindroid */
