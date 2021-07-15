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

#include <mindroid/util/Log.h>
#include <cstdio>
#include <cstdarg>

namespace mindroid {

const sp<LogBuffer> Log::MAIN_LOG_BUFFER = new LogBuffer(LOG_ID_MAIN, 262144); // 256KB
const sp<LogBuffer> Log::EVENT_LOG_BUFFER = new LogBuffer(LOG_ID_EVENTS, 262144); // 256KB
const sp<LogBuffer> Log::TEST_LOG_BUFFER = new LogBuffer(LOG_ID_TEST, 262144); // 256KB
bool Log::sIntegrationTesting = false;

void Log::clear() {
    MAIN_LOG_BUFFER->reset();
    EVENT_LOG_BUFFER->reset();
    TEST_LOG_BUFFER->reset();
}

} /* namespace mindroid */
