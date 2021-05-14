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

#include <mindroid/io/BufferedInputStream.h>
#include <mindroid/lang/System.h>
#include <mindroid/lang/IllegalArgumentException.h>
#include <mindroid/io/IOException.h>
#include <mindroid/lang/Math.h>

namespace mindroid {

BufferedInputStream::BufferedInputStream(const sp<InputStream>& inputStream, size_t size) :
        mInputStream(inputStream) {
    if (size == 0) {
        throw IllegalArgumentException("size = 0");
    }
    mBuffer = new ByteArray(size);
}

size_t BufferedInputStream::available() {
    // Use local references since mBuffer and mInputStream may be invalidated by unsynchronized close() calls.
    sp<InputStream> localInputStream = mInputStream;
    if (mBuffer == nullptr || localInputStream == nullptr) {
        failWithIoException();
    }
    return mCount - mPosition + localInputStream->available();
}

void BufferedInputStream::close() {
    mBuffer = nullptr;
    sp<InputStream> localInputStream = mInputStream;
    mInputStream = nullptr;
    if (localInputStream != nullptr) {
        localInputStream->close();
    }
}

void BufferedInputStream::mark(size_t limit) {
    mMarkLimit = limit;
    mMarkPosition = mPosition;
}

bool BufferedInputStream::markSupported() {
    return true;
}

int32_t BufferedInputStream::read() {
    // Use local references since mBuffer and mInputStream may be invalidated by unsynchronized close() calls.
    sp<ByteArray> localBuffer = mBuffer;
    sp<InputStream> localInputStream = mInputStream;
    if (localBuffer == nullptr || localInputStream == nullptr) {
        failWithIoException();
    }

    if (mPosition >= mCount && fillBuffer(localInputStream, localBuffer) == -1) {
        return -1;
    }
    // localBuffer may have been invalidated by fillBuffer.
    if (localBuffer != mBuffer) {
        localBuffer = mBuffer;
        if (localBuffer == nullptr) {
            failWithIoException();
        }
    }

    // Did filling the buffer fail with -1 (EOF)?
    if (mCount - mPosition > 0) {
        return localBuffer->get(mPosition++);
    }
    return -1;
}

ssize_t BufferedInputStream::read(const sp<ByteArray>& buffer, size_t offset, size_t count) {
    // Use local references since mBuffer and mInputStream may be invalidated by unsynchronized close() calls.
    sp<ByteArray> localBuffer = mBuffer;
    if (localBuffer == nullptr) {
        failWithIoException();
    }
    if (offset > buffer->size() || buffer->size() - offset < count) {
        throw IndexOutOfBoundsException();
    }
    if (count == 0) {
        return 0;
    }
    sp<InputStream> localInputStream = mInputStream;
    if (localInputStream == nullptr) {
        failWithIoException();
    }

    size_t requiredStreamCount;
    if (mPosition < mCount) {
        // Some data available in buffer.
        size_t size = mCount - mPosition >= count ? count : mCount - mPosition;
        System::arraycopy(localBuffer, mPosition, buffer, offset, size);
        mPosition += size;
        if (size == count || localInputStream->available() == 0) {
            return size;
        }
        offset += size;
        requiredStreamCount = count - size;
    } else {
        requiredStreamCount = count;
    }

    while (true) {
        ssize_t size;
        // If not marked and the stream data size is greater than the buffer,
        // simply read the data directly into buffer.
        if (mMarkPosition == -1 && requiredStreamCount >= localBuffer->size()) {
            size = localInputStream->read(buffer, offset, requiredStreamCount);
            if (size == -1) {
                return (requiredStreamCount == count) ? -1 : count - requiredStreamCount;
            }
        } else {
            if (fillBuffer(localInputStream, localBuffer) == -1) {
                return requiredStreamCount == count ? -1 : count - requiredStreamCount;
            }
            // localBuffer may have been invalidated by fillBuffer.
            if (localBuffer != mBuffer) {
                localBuffer = mBuffer;
                if (localBuffer == nullptr) {
                    failWithIoException();
                }
            }

            size = mCount - mPosition >= requiredStreamCount ? requiredStreamCount : mCount - mPosition;
            System::arraycopy(localBuffer, mPosition, buffer, offset, size);
            mPosition += size;
        }
        requiredStreamCount -= size;
        if (requiredStreamCount == 0) {
            return count;
        }
        if (localInputStream->available() == 0) {
            return count - requiredStreamCount;
        }
        offset += size;
    }
}

void BufferedInputStream::reset() {
    if (mBuffer == nullptr) {
        throw IOException("Stream is closed");
    }
    if (mMarkPosition == -1) {
        throw IOException("Mark has been invalidated.");
    }
    mPosition = mMarkPosition;
}

size_t BufferedInputStream::skip(size_t count) {
    // Use local references since mBuffer and mInputStream may be invalidated by unsynchronized close() calls.
    sp<ByteArray> localBuffer = mBuffer;
    sp<InputStream> localInputStream = mInputStream;
    if (localBuffer == nullptr) {
        failWithIoException();
    }
    if (count < 1) {
        return 0;
    }
    if (localInputStream == nullptr) {
        failWithIoException();
    }

    if (mCount - mPosition >= count) {
        mPosition += count;
        return count;
    }
    size_t size = mCount - mPosition;
    mPosition = mCount;

    if (mMarkPosition != -1) {
        if (count <= mMarkLimit) {
            if (fillBuffer(localInputStream, localBuffer) == -1) {
                return size;
            }
            if (mCount - mPosition >= count - size) {
                mPosition += (count - size);
                return count;
            }
            // No possibility to get all the data, so skip all available data.
            size += (mCount - mPosition);
            mPosition = mCount;
            return size;
        }
    }
    return size + localInputStream->skip(count - size);
}

ssize_t BufferedInputStream::fillBuffer(const sp<InputStream>& inputStream, sp<ByteArray>& buffer) {
    if (mMarkPosition == -1 || (mPosition - mMarkPosition >= mMarkLimit)) {
        // Mark position is not set or has exceeded mMarkLimit.
        ssize_t result = inputStream->read(buffer);
        if (result > 0) {
            mMarkPosition = -1;
            mPosition = 0;
            mCount = (result == -1) ? 0 : result;
        }
        return result;
    }
    if ((mMarkPosition) == 0 && (mMarkLimit > buffer->size())) {
        // Increase buffer size.
        size_t newSize = buffer->size() * 2;
        if (newSize > mMarkLimit) {
            newSize = mMarkLimit;
        }
        sp<ByteArray> newBuffer = new ByteArray(newSize);
        System::arraycopy(buffer, 0, newBuffer, 0, buffer->size());
        buffer = mBuffer = newBuffer;
    } else if (mMarkPosition > 0) {
        System::arraycopy(buffer, mMarkPosition, buffer, 0, buffer->size() - mMarkPosition);
    }
    // Set the new position and mark position.
    mPosition -= mMarkPosition;
    mCount = mMarkPosition = 0;
    int size = inputStream->read(buffer, mPosition, buffer->size() - mPosition);
    mCount = (size <= 0) ? mPosition : mPosition + size;
    return size;
}

void BufferedInputStream::failWithIoException() const {
    throw IOException("BufferedInputStream is closed");
}

} /* namespace mindroid */
