/*
 * Copyright (C) 2021 E.S.R.Labs
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

#include <gtest/gtest.h>
#include <mindroid/lang/Exception.h>
#include <mindroid/lang/RuntimeException.h>
#include <mindroid/util/Log.h>

using namespace mindroid;

TEST(Mindroid, ThrowExceptionWithCStringMessage) {
    EXPECT_ANY_THROW(throwException<Exception>("Test"));

    try {
        throwException<Exception>("Test");
    } catch (const Exception& e) {
        sp<String> message = e.getMessage();
        ASSERT_NE(message, nullptr);
        ASSERT_TRUE(message->equals("Test"));
    }
}

TEST(Mindroid, ThrowExceptionWithCauseAndCStringMessage) {
    sp<Exception> cause = new Exception("Cause");
    EXPECT_ANY_THROW(throwException<Exception>(cause, "Test"));

    try {
        throwException<Exception>(cause, "Test");
    } catch (const Exception& e) {
        sp<String> message = e.getMessage();
        ASSERT_NE(message, nullptr);
        ASSERT_TRUE(message->equals("Test"));
        ASSERT_EQ(e.getCause(), cause);
    }
}

TEST(Mindroid, ThrowExceptionWithStringMessage) {
    sp<String> message = String::valueOf("Test");
    EXPECT_ANY_THROW(throwException<Exception>(message));

    try {
        throwException<Exception>(message);
    } catch (const Exception& e) {
        sp<String> message = e.getMessage();
        ASSERT_NE(message, nullptr);
        ASSERT_TRUE(message->equals("Test"));
    }
}

TEST(Mindroid, ThrowExceptionWithCauseAndStringMessage) {
    sp<String> message = String::valueOf("Test");
    sp<Exception> cause = new Exception("Cause");
    EXPECT_ANY_THROW(throwException<Exception>(cause, message));

    try {
        throwException<Exception>(cause, message);
    } catch (const Exception& e) {
        sp<String> message = e.getMessage();
        ASSERT_NE(message, nullptr);
        ASSERT_TRUE(message->equals("Test"));
    }
}

TEST(Mindroid, ThrowExceptionWithFormatCString) {
    EXPECT_ANY_THROW(throwException<Exception>("%s", "Test"));

    try {
        throwException<Exception>("%s", "Test");
    } catch (const Exception& e) {
        sp<String> message = e.getMessage();
        ASSERT_NE(message, nullptr);
        ASSERT_TRUE(message->equals("Test"));
    }
}

TEST(Mindroid, ThrowExceptionWithCauseAndFormatCString) {
    sp<Exception> cause = new Exception("Cause");
    EXPECT_ANY_THROW(throwException<Exception>(cause, "%s", "Test"));

    try {
        throwException<Exception>(cause, "%s", "Test");
    } catch (const Exception& e) {
        sp<String> message = e.getMessage();
        ASSERT_NE(message, nullptr);
        ASSERT_TRUE(message->equals("Test"));
        ASSERT_EQ(e.getCause(), cause);
    }
}

TEST(Mindroid, ThrowExceptionWithFormatString) {
    sp<String> format = String::valueOf("%s");
    EXPECT_ANY_THROW(throwException<Exception>(format, "Test"));

    try {
        throwException<Exception>(format, "Test");
    } catch (const Exception& e) {
        sp<String> message = e.getMessage();
        ASSERT_NE(message, nullptr);
        ASSERT_TRUE(message->equals("Test"));
    }
}

TEST(Mindroid, ThrowExceptionWithCauseAndFormatString) {
    sp<String> format = String::valueOf("%s");
    sp<Exception> cause = new Exception("cause");
    EXPECT_ANY_THROW(throwException<Exception>(cause, format, "Test"));

    try {
        throwException<Exception>(cause, format, "Test");
    } catch (const Exception& e) {
        sp<String> message = e.getMessage();
        ASSERT_NE(message, nullptr);
        ASSERT_TRUE(message->equals("Test"));
    }
}

TEST(Mindroid, ThrowRuntimeException) {
    EXPECT_ANY_THROW(throwException<RuntimeException>("RuntimeException"));

    try {
        throwException<RuntimeException>("RuntimeException");
    } catch (const RuntimeException& e) {
        sp<String> message = e.getMessage();
        ASSERT_NE(message, nullptr);
        ASSERT_TRUE(message->equals("RuntimeException"));
    } catch (const Exception& e) {
        ASSERT_TRUE(false);
    }
}
