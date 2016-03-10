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

#ifndef MINDROID_LOGGER_H_
#define MINDROID_LOGGER_H_

#include "mindroid/lang/Object.h"

namespace mindroid {

class Logger :
		public Object {
public:
	Logger() { }
	virtual ~Logger() { }
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;
	virtual int println(int bufferId, uint8_t priority, const char* tag, const char* msg);

	static const uint8_t VERBOSE = 0;
	static const uint8_t DEBUG = 1;
	static const uint8_t INFO = 2;
	static const uint8_t WARN = 3;
	static const uint8_t ERROR = 4;
	static const uint8_t WTF = 5;
	
private:
	static int getTime(char *time, size_t size);

	static char sProrities[];
};

} /* namespace mindroid */

#endif /* MINDROID_LOGGER_H_ */
