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

#ifndef MINDROID_LOG_H_
#define MINDROID_LOG_H_

#include "mindroid/util/Logger.h"
#include <cstdio>
#include <cstdarg>

namespace mindroid {

class Log {
public:
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

	/**
	 * Set Logger instance that shall be used. If this method is not called a default
	 * implementation that prints logcat style to stdout is used.
	 * @param logger Logger instance to be set
	 */
	static void setLogger(const sp<Logger>& logger);

private:
    static const int DEFAULT_LOG_ID = 0;
    static const int LOG_MESSAGE_SIZE = 128;

    static sp<Logger> sLogger;
};

} /* namespace mindroid */

#endif /* MINDROID_LOG_H_ */
