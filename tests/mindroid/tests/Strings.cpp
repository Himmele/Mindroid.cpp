/*
 * Copyright (C) 2018 Daniel Himmelein
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
#include <mindroid/lang/String.h>
#include <mindroid/lang/Integer.h>

using namespace mindroid;

TEST(Mindroid, Strings) {
    sp<String> s1 = new String("Hello");
    ASSERT_STREQ(s1->c_str(), "Hello");
    sp<String> s2 = s1->append(" World");
    ASSERT_STREQ(s1->c_str(), "Hello");
    ASSERT_STREQ(s2->c_str(), "Hello World");
    sp<String> s3 = s2->append("!");
    ASSERT_STREQ(s1->c_str(), "Hello");
    ASSERT_STREQ(s2->c_str(), "Hello World");
    ASSERT_STREQ(s3->c_str(), "Hello World!");
    s1 = new String("Test");
    ASSERT_STREQ(s1->c_str(), "Test");

    sp<String> s4 = String::format("%d", 123);
    ASSERT_STREQ(s4->c_str(), "123");
    s4 = s4->appendFormatted("%s%d", "456", 789);
    ASSERT_STREQ(s4->c_str(), "123456789");

    sp<String> s5 = s4;
    s4 = nullptr;
    ASSERT_STREQ(s5->c_str(), "123456789");

    sp<String> s6 = new String("    1234    ");
    ASSERT_EQ(s6->length(), 12);
    ASSERT_STREQ(s6->c_str(), "    1234    ");
    sp<String> s7 = s6->trim();
    ASSERT_EQ(s7->length(), 4);
    ASSERT_STREQ(s7->c_str(), "1234");
    ASSERT_STREQ(s6->c_str(), "    1234    ");

    sp<String> s8 = new String("\r\n");
    ASSERT_EQ(s8->length(), 2);
    ASSERT_EQ(s8->equals("\r\n"), true);
    sp<String> s9 = s8->trim();
    ASSERT_EQ(s9->length(), 0);
    ASSERT_EQ(s9->equals(""), true);
    ASSERT_EQ(s9 == String::EMPTY_STRING, true);

    sp<String> s10 = new String();
    ASSERT_EQ(s10->isEmpty(), true);

    sp<String> s11 = new String("Test");
    ASSERT_EQ(s11->startsWith("Te"), true);
    ASSERT_EQ(s11->startsWith("abc"), false);
    ASSERT_EQ(s11->substring(1)->equals("est"), true);
    ASSERT_EQ(s11->substring(1, 3)->equals("es"), true);
    ASSERT_EQ(s11->indexOf("es"), 1);
    ASSERT_EQ(s11->endsWith("st"), true);

    sp<String> s12 = new String("abc\n\n\ndef\r\nxyz");
    sp<StringArray> strings = s12->split("\n");
    ASSERT_EQ(strings->size(), 5);
    for (size_t i = 0; i < strings->size(); i++) {
        sp<String> string = strings->get(i);
        switch (i) {
        case 0:
            ASSERT_STREQ(string->c_str(), "abc");
            break;
        case 1:
            ASSERT_STREQ(string->c_str(), "");
            break;
        case 2:
            ASSERT_STREQ(string->c_str(), "");
            break;
        case 3:
            ASSERT_STREQ(string->c_str(), "def\r");
            break;
        case 4:
            ASSERT_STREQ(string->c_str(), "xyz");
            break;
        default:
            ASSERT_TRUE(false);
            break;
        }
    }

    strings = s12->split("\r\n");
    ASSERT_EQ(strings->size(), 2);
    for (size_t i = 0; i < strings->size(); i++) {
        sp<String> string = strings->get(i);
        switch (i) {
        case 0:
            ASSERT_STREQ(string->c_str(), "abc\n\n\ndef");
            break;
        case 1:
            ASSERT_STREQ(string->c_str(), "xyz");
            break;
        default:
            ASSERT_TRUE(false);
            break;
        }
    }

    sp<String> s13 = new String("Hello");
    sp<String> s14 = new String("Hello");
    ASSERT_EQ(s13->equals(s14), true);
    ASSERT_EQ(s13->equals("Hello"), true);
    sp<String> s15 = new String("World");
    ASSERT_EQ(s13->equals(s15), false);
    ASSERT_EQ(s13->equals(nullptr), false);
    sp<Object> o = new Object();
    ASSERT_EQ(s13->equals(o), false);
    sp<Integer> integer = new Integer(123);
    ASSERT_EQ(s13->equals(object_cast<Object>(integer)), false);

    sp<String> s16 = String::format("%s", "Test");
    ASSERT_EQ(s16->equals("Test"), true);
    sp<String> s17 = String::format("%i", 42);
    ASSERT_EQ(s17->equals("42"), true);
    sp<String> s18 = String::format("%s", String::valueOf("Test"));
    ASSERT_EQ(s18->equals("Test"), true);
    std::string cppString("C++ string");
    sp<String> s19 = String::format("%s", cppString);
    ASSERT_EQ(s19->equals("C++ string"), true);
    sp<String> s20 = String::format("Test");
    ASSERT_EQ(s20->equals("Test"), true);

    sp<String> s21 = nullptr;
    sp<String> s22 = String::format("%s, %s", nullptr, s21);
    ASSERT_EQ(s22->equals("(null), nullptr"), true);
}
