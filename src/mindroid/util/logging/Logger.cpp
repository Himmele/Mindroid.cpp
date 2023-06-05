/*
 * Copyright (C) 2012 Daniel Himmelein
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

#include <mindroid/util/logging/Logger.h>
#include <mindroid/util/concurrent/ExecutionException.h>
#include <mindroid/lang/RuntimeException.h>
#include <mindroid/os/RemoteException.h>

namespace mindroid {

const char* const Logger::ACTION_LOG = "mindroid.util.logging.LOG";
const char* const Logger::ACTION_DUMP_LOG = "mindroid.util.logging.DUMP_LOG";
const char* const Logger::ACTION_FLUSH_LOG = "mindroid.util.logging.FLUSH_LOG";
const char* const Logger::ACTION_CLEAR_LOG = "mindroid.util.logging.CLEAR_LOG";
char Logger::LOG_LEVELS[] = { 'V', 'D', 'I', 'W', 'E', 'A' };

sp<Promise<sp<String>>> Logger::assumeThat(const sp<String>& tag, const sp<String>& message, int64_t timeout) {
    if ((mLogger != nullptr) && (tag != nullptr) && (message != nullptr) && (timeout >= 0)) {
        try {
            return mLogger->assumeThat(tag, message, timeout);
        } catch (const RemoteException& e) {
            throw RuntimeException("System failure", e);
        }
    } else {
        return new Promise<sp<String>>(sp<Exception>(new ExecutionException()));
    }
}

void Logger::mark() {
    if (mLogger != nullptr) {
        try {
            return mLogger->mark();
        } catch (const RemoteException& e) {
            throw RuntimeException("System failure", e);
        }
    }
}

void Logger::reset() {
    if (mLogger != nullptr) {
        try {
            return mLogger->reset();
        } catch (const RemoteException& e) {
            throw RuntimeException("System failure", e);
        }
    }
}

} /* namespace mindroid */
