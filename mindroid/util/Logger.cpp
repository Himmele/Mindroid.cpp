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

#include "mindroid/util/Logger.h"
#include <cstdio>
#include <cstring>
#include <pthread.h>

namespace mindroid {

char Logger::sProrities[] = { 'V', 'D', 'I', 'W', 'E', 'A' };

int Logger::println(int bufferId, uint8_t priority, const char* tag, const char* msg) {
	char time[64];
	getTime(time, sizeof(time));
	printf("%s  %08x  %c %s: %s\n", time, (uint32_t) pthread_self(), sProrities[priority], tag, msg);
	return 0;
}

int Logger::getTime(char *time, size_t size) {
    struct timespec ts;
    struct tm t;

    clock_gettime(CLOCK_REALTIME, &ts);

    tzset();
    if (localtime_r(&(ts.tv_sec), &t) == NULL) {
        return -1;
    }

    size_t s = strftime(time, size, "%F %T.000", &t);
    if (s == 0) {
        return -1;
    }

    snprintf(&time[strlen(time) - 4], 5, ".%03ld", ts.tv_nsec / 1000000);

    return 0;
}

} /* namespace mindroid */
