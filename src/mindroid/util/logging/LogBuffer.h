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

#ifndef MINDROID_UTIL_LOGGING_LOGBUFFER_H_
#define MINDROID_UTIL_LOGGING_LOGBUFFER_H_

#include <mindroid/lang/String.h>
#include <mindroid/util/concurrent/Promise.h>
#include <mindroid/util/concurrent/locks/ReentrantLock.h>
#include <mindroid/util/concurrent/locks/Condition.h>

namespace mindroid {

class LogBuffer : public Object {
public:
    class LogRecord : public Object {
    public:
        LogRecord(const uint64_t timestamp, const uint64_t threadId, const int32_t priority, const sp<String>& tag, const sp<String>& message);

        sp<String> toString();
        sp<String> toShortString();

        uint64_t getTimestamp() {
            return mTimestamp;
        }

        uint64_t getThreadId() {
            return mThreadId;
        }

        int32_t getPriority() {
            return mPriority;
        }

        sp<String> getTag() {
            return mTag;
        }

        sp<String> getMessage() {
            return mMessage;
        }

    private:
        uint64_t mTimestamp;
        uint64_t mThreadId;
        int32_t mPriority;
        sp<String> mTag;
        sp<String> mMessage;
    };

    LogBuffer(const int32_t id, const size_t size);

    virtual ~LogBuffer() = default;

    int32_t getId() {
        return mId;
    }

    void reset();
    bool put(const int32_t priority, const sp<String>& tag, const sp<String>& message);
    bool put(const uint64_t timestamp, const uint64_t threadId, const int32_t priority, const sp<String>& tag, const sp<String>& message);
    sp<Promise<sp<LogRecord>>> get();

private:
    bool isEmpty();
    bool isFull();
    size_t remainingCapacity();
    void writeByteArray(const sp<ByteArray>& data);
    sp<ByteArray> readByteArray(const size_t size);
    static int32_t intFromByteArray(const sp<ByteArray>& data);

    static const uint32_t TIMESTAMP_SIZE = 8;
    static const uint32_t PRIORITY_SIZE = 4;
    static const uint32_t THREAD_ID_SIZE = 8;
    static const uint32_t TAG_SIZE = 4;
    static const uint32_t MESSAGE_SIZE = 4;

    const int32_t mId;
    const size_t mSize;
    sp<ByteArray> mBuffer;
    size_t mReadIndex;
    size_t mWriteIndex;
    sp<ReentrantLock> mLock;
    sp<Condition> mCondition;
    sp<Promise<sp<LogRecord>>> mPromise;
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_LOGGING_LOGBUFFER_H_ */
