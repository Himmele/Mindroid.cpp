/*
 * Copyright (C) 2006 The Android Open Source Project
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

#ifndef MINDROID_UTIL_EVENTLOG_H_
#define MINDROID_UTIL_EVENTLOG_H_

#include <mindroid/util/Log.h>

namespace mindroid {

/**
 * Mindroid event logger.
 * 
 * <p>
 * Generally, use the EventLog.v() EventLog.d() EventLog.i() EventLog.w() and EventLog.e() methods.
 * 
 * <p>
 * The order in terms of verbosity, from least to most is ERROR, WARN, INFO, DEBUG, VERBOSE. Verbose
 * should never be compiled into an application except during development. Debug logs are compiled
 * in but stripped at runtime. Error, warning and info logs are always kept.
 * 
 * <p>
 * <b>Tip:</b> A good convention is to declare a <code>LOG_TAG</code> constant in your class:
 * 
 * <pre>
 * private static final String LOG_TAG = &quot;MyService&quot;;
 * </pre>
 * 
 * and use that in subsequent calls to the log methods.
 * </p>
 * 
 * <p>
 * <b>Tip:</b> Don't forget that when you make a call like
 * 
 * <pre>
 * EventLog.v(LOG_TAG, &quot;index=&quot; + i);
 * </pre>
 * 
 * that when you're building the string to pass into EventLog.d, the compiler uses a StringBuilder and at
 * least three allocations occur: the StringBuilder itself, the buffer, and the String object.
 * Realistically, there is also another buffer allocation and copy, and even more pressure on the
 * gc. That means that if your log message is filtered out, you might be doing significant work and
 * incurring significant overhead.
 */
class EventLog final {
public:
    EventLog() noexcept = delete;
    ~EventLog() noexcept = delete;
    EventLog(const Log&) = delete;
    EventLog& operator=(const Log&) = delete;

    /**
     * Send a {@link #VERBOSE} log message.
     * @param tag Used to identify the source of a log message.  It usually identifies
     *        the class or activity where the log call occurs.
     * @param msg The message you would like logged.
     */
    static int v(const char* tag, const char* format, ...);

    /**
     * Send a {@link #DEBUG} log message.
     * @param tag Used to identify the source of a log message.  It usually identifies
     *        the class or activity where the log call occurs.
     * @param msg The message you would like logged.
     */
    static int d(const char* tag, const char* format, ...);

    /**
     * Send an {@link #INFO} log message.
     * @param tag Used to identify the source of a log message.  It usually identifies
     *        the class or activity where the log call occurs.
     * @param msg The message you would like logged.
     */
    static int i(const char* tag, const char* format, ...);

    /**
     * Send a {@link #WARN} log message.
     * @param tag Used to identify the source of a log message.  It usually identifies
     *        the class or activity where the log call occurs.
     * @param msg The message you would like logged.
     */
    static int w(const char* tag, const char* format, ...);

    /**
     * Send an {@link #ERROR} log message.
     * @param tag Used to identify the source of a log message.  It usually identifies
     *        the class or activity where the log call occurs.
     * @param msg The message you would like logged.
     */
    static int e(const char* tag, const char* format, ...);

    /**
     * What a Terrible Failure: Report a condition that should never happen.
     * The error will always be logged at level ASSERT with the call stack.
     * Depending on system configuration, a report may be added to the
     * {@link android.os.DropBoxManager} and/or the process may be terminated
     * immediately with an error dialog.
     * @param tag Used to identify the source of a log message.
     * @param msg The message you would like logged.
     */
    static int wtf(const char* tag, const char* format, ...);

private:
    static const size_t LOG_MESSAGE_SIZE = 256;
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_EVENTLOG_H_ */
