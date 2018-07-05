/*
 * Copyright (C) 2013 Daniel Himmelein
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

#include <mindroid/util/logging/LogBuffer.h>
#include <mindroid/util/logging/Logger.h>
#include <mindroid/lang/System.h>
#include <mindroid/lang/Thread.h>
#include <mindroid/nio/ByteBuffer.h>
#include <cstdio>
#include <cstring>

namespace mindroid {

LogBuffer::LogRecord::LogRecord(const uint64_t timestamp, const uint64_t threadId, const int32_t priority, const sp<String>& tag, const sp<String>& message) :
        mTimestamp(timestamp),
        mThreadId(threadId),
        mPriority(priority),
        mTag(tag),
        mMessage(message) {
}

sp<String> LogBuffer::LogRecord::toString() {
    struct timespec ts;
    ts.tv_nsec = (mTimestamp % 1000) * 1000000;
    ts.tv_sec = mTimestamp / 1000;

    char time[64];
    struct tm t;
    ::tzset();
    if (::localtime_r(&(ts.tv_sec), &t) != NULL) {
        size_t s = strftime(time, sizeof(time), "%F %T.000", &t);
        if (s > 0) {
            ::snprintf(&time[strlen(time) - 4], 5, ".%03ld", ts.tv_nsec / 1000000);
        } else {
            time[0] = '\0';
        }
    } else {
        time[0] = '\0';
    }

    return String::format("%s  0x%" PRIx64 " %c %s: %s", time, mThreadId, Logger::LOG_LEVELS[mPriority], mTag->c_str(), mMessage->c_str());
}

sp<String> LogBuffer::LogRecord::toShortString() {
    return String::format("%c/%s(0x%" PRIx64 "): %s", Logger::LOG_LEVELS[mPriority], mTag->c_str(), mThreadId, mMessage->c_str());
}

LogBuffer::LogBuffer(const int32_t id, const size_t size) :
        mId(id),
        mSize(size),
        mBuffer(new ByteArray(size)),
        mReadIndex(0),
        mWriteIndex(0),
        mLock(new ReentrantLock()) {
}

void LogBuffer::reset() {
    AutoLock autoLock(mLock);
    mReadIndex = 0;
    mWriteIndex = 0;
    if (mPromise != nullptr) {
        mPromise->complete(sp<LogRecord>(nullptr));
        mPromise = nullptr;
    }
}

bool LogBuffer::put(const int32_t priority, const sp<String>& tag, const sp<String>& message) {
    return put(System::currentTimeMillis(), Thread::currentThread()->getId(), priority, tag, message);
}

bool LogBuffer::put(const uint64_t timestamp, const uint64_t threadId, const int32_t priority, const sp<String>& tag, const sp<String>& message) {
    if (tag == nullptr) {
        return false;
    }
    if (message == nullptr) {
        return false;
    }

    sp<ByteArray> tagBuffer = tag->getBytes();
    sp<ByteArray> messageBuffer = message->getBytes();
    const uint32_t size = TIMESTAMP_SIZE + THREAD_ID_SIZE + PRIORITY_SIZE + TAG_SIZE + tagBuffer->size() + MESSAGE_SIZE + messageBuffer->size();
    if ((size + 4) > mSize) {
        return false;
    }

    sp<ByteBuffer> byteBuffer = ByteBuffer::allocate(size + 4);
    byteBuffer->putInt(size);
    byteBuffer->putLong(timestamp);
    byteBuffer->putLong(threadId);
    byteBuffer->putInt(priority);
    byteBuffer->putInt(tagBuffer->size());
    byteBuffer->put(tagBuffer);
    byteBuffer->putInt(messageBuffer->size());
    byteBuffer->put(messageBuffer);

    {
        AutoLock autoLock(mLock);
        if (mPromise != nullptr) {
            mPromise->complete(sp<LogRecord>(new LogRecord(timestamp, threadId, priority, tag, message)));
            mPromise = nullptr;
        } else {
            writeByteArray(byteBuffer->array());
        }
    }

    return true;
}

sp<Promise<sp<LogBuffer::LogRecord>>> LogBuffer::get() {
    sp<ByteArray> buffer;
    {
        AutoLock autoLock(mLock);
        if (mPromise == nullptr) {
            if (!isEmpty()) {
                int32_t size = intFromByteArray(readByteArray(4));
                buffer = readByteArray(size);
            } else {
                mPromise = new Promise<sp<LogRecord>>();
                return mPromise;
            }
        } else {
            return mPromise;
        }
    }

    sp<ByteBuffer> byteBuffer = ByteBuffer::wrap(buffer);
    uint64_t timestamp = (uint64_t) byteBuffer->getLong();
    uint64_t threadId = (uint64_t) byteBuffer->getLong();
    int32_t priority = byteBuffer->getInt();
    int32_t tagSize = byteBuffer->getInt();
    sp<ByteArray> tagData = new ByteArray(tagSize);
    byteBuffer->get(tagData);
    sp<String> tag = new String(tagData);
    int32_t messageSize = byteBuffer->getInt();
    sp<ByteArray> messageData = new ByteArray(messageSize);
    byteBuffer->get(messageData);
    sp<String> message = new String(messageData);
    return new Promise<sp<LogRecord>>(sp<LogRecord>(new LogRecord(timestamp, threadId, priority, tag, message)));
}

bool LogBuffer::isEmpty() {
    return mReadIndex == mWriteIndex;
}

bool LogBuffer::isFull() {
    return (mWriteIndex + 1) % mSize == mReadIndex;
}

size_t LogBuffer::remainingCapacity() {
    if (mWriteIndex >= mReadIndex) {
        return (mSize - (mWriteIndex - mReadIndex));
    } else {
        return (mReadIndex - mWriteIndex);
    }
}

void LogBuffer::writeByteArray(const sp<ByteArray>& data) {
    const size_t size = data->size();
    size_t freeCapacity = remainingCapacity();
    while (freeCapacity < size) {
        int32_t s = intFromByteArray(readByteArray(4));
        mReadIndex = (mReadIndex + s) % mSize;
        freeCapacity += (s + 4);
    }

    if (mWriteIndex + size < mSize) {
        System::arraycopy(data, 0, mBuffer, mWriteIndex, size);
        mWriteIndex = (mWriteIndex + size) % mSize;
    } else {
        size_t partialSize = (mSize - mWriteIndex);
        System::arraycopy(data, 0, mBuffer, mWriteIndex, partialSize);
        System::arraycopy(data, partialSize, mBuffer, 0, size - partialSize);
        mWriteIndex = (mWriteIndex + size) % mSize;
    }
}

sp<ByteArray> LogBuffer::readByteArray(const size_t size) {
    sp<ByteArray> data = new ByteArray(size);
    if (mReadIndex + size < mSize) {
        System::arraycopy(mBuffer, mReadIndex, data, 0, size);
        mReadIndex = (mReadIndex + size) % mSize;
    } else {
        size_t partialSize = (mSize - mReadIndex);
        System::arraycopy(mBuffer, mReadIndex, data, 0, partialSize);
        System::arraycopy(mBuffer, 0, data, partialSize, size - partialSize);
        mReadIndex = (mReadIndex + size) % mSize;
    }
    return data;
}

int32_t LogBuffer::intFromByteArray(const sp<ByteArray>& data) {
    return (*reinterpret_cast<int32_t*>(data->c_arr()));
}

} /* namespace mindroid */
