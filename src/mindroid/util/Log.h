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

#ifndef MINDROID_UTIL_LOG_H_
#define MINDROID_UTIL_LOG_H_

#include <mindroid/util/logging/LogBuffer.h>
#include <mutex>
#if defined(ANDROID)
#include <android/log.h>
#endif

namespace mindroid {

/**
 * Mindroid logger.
 *
 * <p>
 * Generally, use the Log.v() Log.d() Log.i() Log.w() and Log.e() methods.
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
 * Log.v(LOG_TAG, &quot;index=&quot; + i);
 * </pre>
 *
 * that when you're building the string to pass into Log.d, the compiler uses a StringBuilder and at
 * least three allocations occur: the StringBuilder itself, the buffer, and the String object.
 * Realistically, there is also another buffer allocation and copy, and even more pressure on the
 * gc. That means that if your log message is filtered out, you might be doing significant work and
 * incurring significant overhead.
 */
class Log final {
public:
    static const int32_t VERBOSE = 0;
    static const int32_t DEBUG = 1;
    static const int32_t INFO = 2;
    static const int32_t WARN = 3;
    static const int32_t ERROR = 4;
    static const int32_t WTF = 5;

    Log() noexcept = delete;
    ~Log() noexcept = delete;
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

    /**
     * Send a {@link #VERBOSE} log message.
     * @param tag Used to identify the source of a log message.  It usually identifies
     *        the class or activity where the log call occurs.
     * @param msg The message you would like logged.
     */
    template<typename... Args>
    static int v(const char* tag, const char* format, Args... args) {
        sp<String> message = String::format(format, args...);
        return println(LOG_ID_MAIN, VERBOSE, String::valueOf(tag), message);
    }

    template<typename... Args>
    static int v(const sp<String>& tag, const sp<String>& format, Args... args) {
        sp<String> message = String::format(format, args...);
        return println(LOG_ID_MAIN, VERBOSE, tag, message);
    }

    /**
     * Send a {@link #DEBUG} log message.
     * @param tag Used to identify the source of a log message.  It usually identifies
     *        the class or activity where the log call occurs.
     * @param msg The message you would like logged.
     */
    template<typename... Args>
    static int d(const char* tag, const char* format, Args... args) {
        sp<String> message = String::format(format, args...);
        return println(LOG_ID_MAIN, DEBUG, String::valueOf(tag), message);
    }

    template<typename... Args>
    static int d(const sp<String>& tag, const sp<String>& format, Args... args) {
        sp<String> message = String::format(format, args...);
        return println(LOG_ID_MAIN, DEBUG, tag, message);
    }

    /**
     * Send an {@link #INFO} log message.
     * @param tag Used to identify the source of a log message.  It usually identifies
     *        the class or activity where the log call occurs.
     * @param msg The message you would like logged.
     */
    template<typename... Args>
    static int i(const char* tag, const char* format, Args... args) {
        sp<String> message = String::format(format, args...);
        return println(LOG_ID_MAIN, INFO, String::valueOf(tag), message);
    }

    template<typename... Args>
    static int i(const sp<String>& tag, const sp<String>& format, Args... args) {
        sp<String> message = String::format(format, args...);
        return println(LOG_ID_MAIN, INFO, tag, message);
    }

    /**
     * Send a {@link #WARN} log message.
     * @param tag Used to identify the source of a log message.  It usually identifies
     *        the class or activity where the log call occurs.
     * @param msg The message you would like logged.
     */
    template<typename... Args>
    static int w(const char* tag, const char* format, Args... args) {
        sp<String> message = String::format(format, args...);
        return println(LOG_ID_MAIN, WARN, String::valueOf(tag), message);
    }

    template<typename... Args>
    static int w(const sp<String>& tag, const sp<String>& format, Args... args) {
        sp<String> message = String::format(format, args...);
        return println(LOG_ID_MAIN, WARN, tag, message);
    }

    /**
     * Send an {@link #ERROR} log message.
     * @param tag Used to identify the source of a log message.  It usually identifies
     *        the class or activity where the log call occurs.
     * @param msg The message you would like logged.
     */
    template<typename... Args>
    static int e(const char* tag, const char* format, Args... args) {
        sp<String> message = String::format(format, args...);
        return println(LOG_ID_MAIN, ERROR, String::valueOf(tag), message);
    }

    template<typename... Args>
    static int e(const sp<String>& tag, const sp<String>& format, Args... args) {
        sp<String> message = String::format(format, args...);
        return println(LOG_ID_MAIN, ERROR, tag, message);
    }

    /**
     * What a Terrible Failure: Report a condition that should never happen.
     * The error will always be logged at level ASSERT with the call stack.
     * Depending on system configuration, a report may be added to the
     * {@link android.os.DropBoxManager} and/or the process may be terminated
     * immediately with an error dialog.
     * @param tag Used to identify the source of a log message.
     * @param msg The message you would like logged.
     */
    template<typename... Args>
    static int wtf(const char* tag, const char* format, Args... args) {
        sp<String> message = String::format(format, args...);
        return println(LOG_ID_MAIN, WTF, String::valueOf(tag), message);
    }

    template<typename... Args>
    static int wtf(const sp<String>& tag, const sp<String>& format, Args... args) {
        sp<String> message = String::format(format, args...);
        return println(LOG_ID_MAIN, WTF, tag, message);
    }

    /**
     * @hide
     * @private
     */
    template<typename... Args>
    static void println(char priority, const char* tag, const char* format, Args... args) {
        sp<String> message = String::format(format, args...);
        ::printf("%c/%s: %s\n", priority, tag, message->c_str());
        ::fflush(stdout);
    }

    template<typename... Args>
    static void println(char priority, const sp<String>& tag, const sp<String>& format, Args... args) {
        sp<String> message = String::format(format, args...);
        ::printf("%c/%s: %s\n", priority, tag->c_str(), message->c_str());
        ::fflush(stdout);
    }

    /**
     * @hide
     * @private
     */
    static int println(int32_t id, int32_t priority, const char* tag, const char* msg) {
        return println(id, priority, String::valueOf(tag), String::valueOf(msg));
    }

    /**
     * @hide
     * @private
     */
    static int println(int32_t id, int32_t priority, const sp<String>& tag, const sp<String>& msg) {
#ifdef ANDROID
        int androidPriority = ANDROID_LOG_DEFAULT;
        switch (priority) {
        case Log::VERBOSE:
            androidPriority = ANDROID_LOG_VERBOSE;
            break;
        case Log::DEBUG:
            androidPriority = ANDROID_LOG_DEBUG;
            break;
        case Log::INFO:
            androidPriority = ANDROID_LOG_INFO;
            break;
        case Log::WARN:
            androidPriority = ANDROID_LOG_WARN;
            break;
        case Log::ERROR:
            androidPriority = ANDROID_LOG_ERROR;
            break;
        case Log::WTF:
            androidPriority = ANDROID_LOG_FATAL;
            break;
        }
        __android_log_write(androidPriority, tag->c_str(), msg->c_str());
#endif

        switch (id) {
        case LOG_ID_MAIN:
            MAIN_LOG_BUFFER->put(priority, tag, msg);
            if (sIntegrationTesting) {
                TEST_LOG_BUFFER->put(priority, tag, msg);
            }
            return 0;
        case LOG_ID_EVENTS:
            EVENT_LOG_BUFFER->put(priority, tag, msg);
            if (sIntegrationTesting) {
                TEST_LOG_BUFFER->put(priority, tag, msg);
            }
            return 0;
        case LOG_ID_TEST:
            TEST_LOG_BUFFER->put(priority, tag, msg);
            return 0;
        default:
            return -1;
        }
    }

    /**
     * @hide
     * @private
     */
    static int32_t parsePriority(const sp<String>& priority) {
        char c;
        if (priority->length() > 1 && priority->toUpperCase()->equals("WTF")) {
            c = 'A';
        } else {
            c = priority->charAt(0);
        }

        return parsePriority(c);
    }

    /**
     * @hide
     * @private
     */
    static int32_t parsePriority(const char priority) {
        switch (priority) {
        case 'V':
            return Log::VERBOSE;
        case 'D':
            return Log::DEBUG;
        case 'I':
            return Log::INFO;
        case 'W':
            return Log::WARN;
        case 'E':
            return Log::ERROR;
        case 'A':
            return Log::WTF;
        default:
            return -1;
        }
    }

    /**
     * @hide
     * @private
     */
    static sp<String> toPriority(int32_t priority) {
        static const char LOG_LEVELS[] = { 'V', 'D', 'I', 'W', 'E', 'A' };
        if (priority >= 0 && size_t(priority) < sizeof(LOG_LEVELS)) {
            return String::valueOf(LOG_LEVELS[priority]);
        } else {
            return nullptr;
        }
    }

    /**
     * @hide
     * @private
     */
    static sp<LogBuffer> getLogBuffer(int32_t id) {
        switch (id) {
        case LOG_ID_MAIN:
            return MAIN_LOG_BUFFER;
        case LOG_ID_EVENTS:
            return EVENT_LOG_BUFFER;
        case LOG_ID_TEST:
            return TEST_LOG_BUFFER;
        default:
            return nullptr;
        }
    }

    /**
     * @hide
     * @private
     */
    static const int32_t LOG_ID_MAIN = 0;
    /**
     * @hide
     * @private
     */
    static const int32_t LOG_ID_EVENTS = 1;
    /**
     * @hide
     * @private
     */
    static const int32_t LOG_ID_TEST = 2;

    /**
     * Integration testing.
     *
     * @hide
     * @private
     */
    static void setIntegrationTesting(bool integrationTesting) {
        sIntegrationTesting = integrationTesting;
    }

    /**
     * Integration testing.
     *
     * @hide
     * @private
     */
    static void clear();

private:
    static const size_t LOG_MESSAGE_SIZE = 256;

    static const sp<LogBuffer> MAIN_LOG_BUFFER;
    static const sp<LogBuffer> EVENT_LOG_BUFFER;
    static const sp<LogBuffer> TEST_LOG_BUFFER;

    static bool sIntegrationTesting;
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_LOG_H_ */
