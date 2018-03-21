/*
 * Copyright (C) 2014 Daniel Himmelein
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

#include <mindroid/util/Assert.h>
#include <mindroid/util/Log.h>
#include <mindroid/lang/System.h>

namespace mindroid {

const char* const Assert::TAG = "Assert";

void Assert::fail(const char* message) {
    Log::wtf(TAG, "%s", message);
    assert(false);
    System::exit(-1);
}

void Assert::println(const char* tag, const char* format, ...) {
    char msg[LOG_MESSAGE_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, LOG_MESSAGE_SIZE, format, args);
    va_end(args);

    Log::println(Log::LOG_ID_MAIN, Log::WTF, tag, msg);
}

} /* namespace mindroid */
