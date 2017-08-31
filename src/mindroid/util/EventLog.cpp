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

#include "mindroid/util/EventLog.h"
#include <cstdio>
#include <cstdarg>

namespace mindroid {

int EventLog::v(const char* tag, const char* format, ...) {
    char msg[LOG_MESSAGE_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, LOG_MESSAGE_SIZE, format, args);
    va_end(args);

    return Log::println(Log::LOG_ID_EVENTS, Log::VERBOSE, tag, msg);
}

int EventLog::d(const char* tag, const char* format, ...) {
    char msg[LOG_MESSAGE_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, LOG_MESSAGE_SIZE, format, args);
    va_end(args);

    return Log::println(Log::LOG_ID_EVENTS, Log::DEBUG, tag, msg);
}

int EventLog::i(const char* tag, const char* format, ...) {
    char msg[LOG_MESSAGE_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, LOG_MESSAGE_SIZE, format, args);
    va_end(args);

    return Log::println(Log::LOG_ID_EVENTS, Log::INFO, tag, msg);
}

int EventLog::w(const char* tag, const char* format, ...) {
    char msg[LOG_MESSAGE_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, LOG_MESSAGE_SIZE, format, args);
    va_end(args);

    return Log::println(Log::LOG_ID_EVENTS, Log::WARN, tag, msg);
}

int EventLog::e(const char* tag, const char* format, ...) {
    char msg[LOG_MESSAGE_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, LOG_MESSAGE_SIZE, format, args);
    va_end(args);

    return Log::println(Log::LOG_ID_EVENTS, Log::ERROR, tag, msg);
}

int EventLog::wtf(const char* tag, const char* format, ...) {
    char msg[LOG_MESSAGE_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, LOG_MESSAGE_SIZE, format, args);
    va_end(args);

    return Log::println(Log::LOG_ID_EVENTS, Log::WTF, tag, msg);
}

} /* namespace mindroid */
