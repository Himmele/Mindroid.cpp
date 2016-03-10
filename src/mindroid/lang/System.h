/*
 * Copyright (C) 2016 E.S.R. Labs
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

#ifndef MINDROID_SYSTEM_H_
#define MINDROID_SYSTEM_H_

#include <cstdint>
#include <ctime>
#include <stdlib.h>

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME  0
#endif

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

namespace mindroid {

class System final {
public:
	System() noexcept = delete;
	~System() noexcept = delete;
	System(const System&) = delete;
	System& operator=(const System&) = delete;

	/**
	 * Returns the current time in milliseconds since January 1, 1970 00:00:00.0 UTC.
	 *
	 * <p>This method always returns UTC times, regardless of the system's time zone.
	 * This is often called "Unix time" or "epoch time".
	 *
	 * <p>This method shouldn't be used for measuring timeouts or
	 * other elapsed time measurements, as changing the system time can affect
	 * the results.
	 */
	static uint64_t currentTimeMillis() {
		timespec now;
		clock_gettime(CLOCK_REALTIME, &now);
		return (((uint64_t) now.tv_sec * 1000LL) + (now.tv_nsec / 1000000LL));
	}

	/**
	 * Causes the process to stop running and the program to exit with the given exit status.
	 */
	static void exit(int32_t status) {
		::exit(status);
	}
};

} /* namespace mindroid */

#endif /* MINDROID_SYSTEM_H_ */
