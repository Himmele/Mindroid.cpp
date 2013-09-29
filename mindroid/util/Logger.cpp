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
#include <stdio.h>

namespace mindroid {

char Logger::mProrities[] = { 'V', 'D', 'I', 'W', 'E', 'A' };

int Logger::println(int bufferId, uint8_t priority, const char* tag, const char* msg) {
	printf("%c/%s: %s\r\n", mProrities[priority], tag, msg);
	return 0;
}

} /* namespace mindroid */
