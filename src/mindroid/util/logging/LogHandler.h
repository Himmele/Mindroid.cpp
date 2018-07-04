/*
 * Copyright (C) 2017 Daniel Himmelein
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

#ifndef MINDROID_UTIL_LOGGING_LOGHANDLER_H_
#define MINDROID_UTIL_LOGGING_LOGHANDLER_H_

#include <mindroid/lang/Object.h>
#include <mindroid/util/Log.h>
#include <mindroid/util/logging/LogBuffer.h>

namespace mindroid {

class LogHandler : public Object {
public:
    int32_t getPriority() {
        return mPriority;
    }

    void setPriority(int32_t priority) {
        mPriority = priority;
    }

    /**
     * Closes this handler. A flush operation will be performed and all the
     * associated resources will be freed. Client applications should not use
     * this handler after closing it.
     */
    virtual void close() = 0;

    /**
     * Flushes any buffered output.
     */
    virtual void flush() = 0;

    /**
     * Accepts a logging request and sends it to the the target.
     *
     * @param record
     *            the log record to be logged; {@code null} records are ignored.
     */
    virtual void publish(const sp<LogBuffer::LogRecord>& record) = 0;

private:
    int32_t mPriority = Log::INFO;
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_LOGGING_LOGHANDLER_H_ */
