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

#ifndef MINDROID_UTIL_LOGGING_LOGGER_H_
#define MINDROID_UTIL_LOGGING_LOGGER_H_

#include <mindroid/util/logging/ILogger.h>
#include <mindroid/content/Context.h>
#include <mindroid/os/ServiceManager.h>

namespace mindroid {

class Logger : public Object {
public:
    static const char* const ACTION_LOG;
    static const char* const ACTION_DUMP_LOG;
    static const char* const ACTION_FLUSH_LOG;
    static const char* const ACTION_CLEAR_LOG;

    /** @hide */
    static char LOG_LEVELS[];

    /**
     * @hide
     */
    Logger() {
        mLogger = binder::Logger::Stub::asInterface(ServiceManager::getSystemService(Context::LOGGER_SERVICE));
    }

    Logger(const sp<Context>& context) {
        mLogger = binder::Logger::Stub::asInterface(context->getSystemService(Context::LOGGER_SERVICE));
    }

    sp<Promise<sp<String>>> assumeThat(const char* tag, const char* message, int64_t timeout) {
        return assumeThat(String::valueOf(tag), String::valueOf(message), timeout);
    }
    sp<Promise<sp<String>>> assumeThat(const sp<String>& tag, const sp<String>& message, int64_t timeout);

private:
    sp<ILogger> mLogger;
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_LOGGING_LOGGER_H_ */
