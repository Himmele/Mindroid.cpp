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

#ifndef MINDROID_UTIL_ASSERT_H_
#define MINDROID_UTIL_ASSERT_H_

#include <mindroid/lang/Object.h>
#include <cassert>

namespace mindroid {

class Assert final {
public:
    Assert() noexcept = delete;
    ~Assert() noexcept = delete;
    Assert(const Assert&) = delete;
    Assert& operator=(const Assert&) = delete;

    static void assertTrue(bool condition) {
        assertTrue(nullptr, condition);
    }

    static void assertTrue(const char* message, bool condition) {
        if (!condition) {
            println(TAG, "%s (%u)", message, condition);
        }
        assert(condition);
    }

    static void assertFalse(bool condition) {
        assertFalse(nullptr, condition);
    }

    static void assertFalse(const char* message, bool condition) {
        if (condition) {
            println(TAG, "%s (%u)", message, condition);
        }
        assert(!condition);
    }

    static void assertNotNull(void* ptr) {
        assertNotNull(nullptr, ptr);
    }

    static void assertNotNull(const char* message, const void* ptr) {
        if (ptr == nullptr) {
            println(TAG, "%s (0x%x)", message, ptr);
        }
        assert(ptr != nullptr);
    }

    static void assertNotNull(const sp<Object>& object) {
        assertNotNull(nullptr, object);
    }

    static void assertNotNull(const char* message, const sp<Object>& object) {
        if (object == nullptr) {
            println(TAG, "%s (0x%x)", message, object.getPointer());
        }
        assert(object.getPointer() != nullptr);
    }

    template<typename T>
    static void assertEquals(T expected, T actual) {
        assertEquals(nullptr, expected, actual);
    }

    template<typename T>
    static void assertEquals(const char* message, T expected, T actual) {
        if (expected != actual) {
            println(TAG, "%s (%d, %d)", message, expected, actual);
        }
        assert(expected == actual);
    }

    static void fail() __attribute__ ((noreturn)) {
        fail(nullptr);
    }
    static void fail(const char* message) __attribute__ ((noreturn));

    static void println(const char* tag, const char* format, ...);

private:
    static const char* const TAG;
    static const size_t LOG_MESSAGE_SIZE = 256;
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_ASSERT_H_ */
