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

#include "mindroid/util/Log.h"

namespace mindroid {

Logger Log::sLogger;

int Log::v(const char* tag, const char* format, ...) {
	char msg[LOG_RECORD_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, LOG_RECORD_SIZE, format, args);
	va_end(args);

	return sLogger.println(DEFAULT_LOG_ID, VERBOSE, tag, msg);
}

int Log::d(const char* tag, const char* format, ...) {
	char msg[LOG_RECORD_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, LOG_RECORD_SIZE, format, args);
	va_end(args);

	return sLogger.println(DEFAULT_LOG_ID, DEBUG, tag, msg);
}

int Log::i(const char* tag, const char* format, ...) {
	char msg[LOG_RECORD_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, LOG_RECORD_SIZE, format, args);
	va_end(args);

	return sLogger.println(DEFAULT_LOG_ID, INFO, tag, msg);
}

int Log::w(const char* tag, const char* format, ...) {
	char msg[LOG_RECORD_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, LOG_RECORD_SIZE, format, args);
	va_end(args);

	return sLogger.println(DEFAULT_LOG_ID, WARN, tag, msg);
}

int Log::e(const char* tag, const char* format, ...) {
	char msg[LOG_RECORD_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, LOG_RECORD_SIZE, format, args);
	va_end(args);

	return sLogger.println(DEFAULT_LOG_ID, ERROR, tag, msg);
}

int Log::wtf(const char* tag, const char* format, ...) {
	char msg[LOG_RECORD_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, LOG_RECORD_SIZE, format, args);
	va_end(args);

	return sLogger.println(DEFAULT_LOG_ID, WTF, tag, msg);
}

} /* namespace mindroid */
