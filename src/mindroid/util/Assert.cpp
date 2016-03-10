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

#include "mindroid/util/Assert.h"
#include "mindroid/util/Log.h"

namespace mindroid {

const char* const Assert::LOG_TAG = "Assert";

void Assert::assertTrue(const char* message, bool condition, const char* file, long line) {
	if (!condition) {
		Log::wtf(LOG_TAG, "%s: %d (%s:%d)", message, condition, file, line);
	}
	assert(condition);
}

void Assert::assertFalse(const char* message, bool condition, const char* file, long line) {
	if (condition) {
		Log::wtf(LOG_TAG, "%s: %d (%s:%d)", message, condition, file, line);
	}
	assert(!condition);
}

void Assert::assertNull(const char* message, void* ptr, const char* file, long line) {
	if (ptr != nullptr) {
		Log::wtf(LOG_TAG, "%s: 0x%x (%s:%d)", message, ptr, file, line);
	}
	assert(ptr == nullptr);
}

void Assert::assertNull(const char* message, const sp<Object>& object, const char* file, long line) {
	if (object != nullptr) {
		Log::wtf(LOG_TAG, "%s: 0x%x (%s:%d)", message, object.getPointer(), file, line);
	}
	assert(object == nullptr);
}

void Assert::assertNotNull(const char* message, void* ptr, const char* file, long line) {
	if (ptr == nullptr) {
		Log::wtf(LOG_TAG, "%s: 0x%x (%s:%d)", message, ptr, file, line);
	}
	assert(ptr != nullptr);
}

void Assert::assertNotNull(const char* message, const sp<Object>& object, const char* file, long line) {
	if (object == nullptr) {
		Log::wtf(LOG_TAG, "%s: 0x%x (%s:%d)", message, object.getPointer(), file, line);
	}
	assert(object != nullptr);
}

void Assert::assertEquals(const char* message, bool expected, bool actual, const char* file, long line) {
	if (expected != actual) {
		Log::wtf(LOG_TAG, "%s: %d != %d (%s:%d)", message, expected, actual, file, line);
	}
	assert(expected == actual);
}

void Assert::assertEquals(const char* message, int8_t expected, int8_t actual, const char* file, long line) {
	if (expected != actual) {
		Log::wtf(LOG_TAG, "%s: %d != %d (%s:%d)", message, expected, actual, file, line);
	}
	assert(expected == actual);
}

void Assert::assertEquals(const char* message, uint8_t expected, uint8_t actual, const char* file, long line) {
	if (expected != actual) {
		Log::wtf(LOG_TAG, "%s: %d != %d (%s:%d)", message, expected, actual, file, line);
	}
	assert(expected == actual);
}

void Assert::assertEquals(const char* message, int16_t expected, int16_t actual, const char* file, long line) {
	if (expected != actual) {
		Log::wtf(LOG_TAG, "%s: %d != %d (%s:%d)", message, expected, actual, file, line);
	}
	assert(expected == actual);
}

void Assert::assertEquals(const char* message, uint16_t expected, uint16_t actual, const char* file, long line) {
	if (expected != actual) {
		Log::wtf(LOG_TAG, "%s: %d != %d (%s:%d)", message, expected, actual, file, line);
	}
	assert(expected == actual);
}

void Assert::assertEquals(const char* message, int32_t expected, int32_t actual, const char* file, long line) {
	if (expected != actual) {
		Log::wtf(LOG_TAG, "%s: %d != %d (%s:%d)", message, expected, actual, file, line);
	}
	assert(expected == actual);
}

void Assert::assertEquals(const char* message, uint32_t expected, uint32_t actual, const char* file, long line) {
	if (expected != actual) {
		Log::wtf(LOG_TAG, "%s: %d != %d (%s:%d)", message, expected, actual, file, line);
	}
	assert(expected == actual);
}

void Assert::assertEquals(const char* message, int64_t expected, int64_t actual, const char* file, long line) {
	if (expected != actual) {
		Log::wtf(LOG_TAG, "%s: %d != %d (%s:%d)", message, expected, actual, file, line);
	}
	assert(expected == actual);
}

void Assert::assertEquals(const char* message, uint64_t expected, uint64_t actual, const char* file, long line) {
	if (expected != actual) {
		Log::wtf(LOG_TAG, "%s: %d != %d (%s:%d)", message, expected, actual, file, line);
	}
	assert(expected == actual);
}

void Assert::assertEquals(const char* message, void* expected, void* actual, const char* file, long line) {
	if (expected != actual) {
		Log::wtf(LOG_TAG, "%s: 0x%x != 0x%x (%s:%d)", message, expected, actual, file, line);
	}
	assert(expected == actual);
}

void Assert::fail(const char* message, const char* file, long line) {
	Log::wtf(LOG_TAG, "%s (%s:%d)", message, file, line);
	assert(false);
}

} /* namespace mindroid */
