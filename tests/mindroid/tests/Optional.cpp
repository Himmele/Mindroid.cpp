/*
 * Copyright (C) 2019 E.S.R.Labs
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
#include <mindroid/util/Optional.h>
#include <mindroid/lang/String.h>
#include <mindroid/lang/Void.h>

using namespace mindroid;

TEST(Mindroid, Optional) {
    auto test = Optional<String>::of(String::valueOf("test"));
    auto empty = Optional<String>::empty();

    ASSERT_EQ(Optional<String>::empty()->isPresent(), false);

    ASSERT_EQ(test->isPresent(), true);
    ASSERT_STREQ(test->get()->c_str(), "test");

    ASSERT_TRUE(test->get()->equals(String::valueOf("test")));

    ASSERT_EQ(empty->hashCode(), 0);

    ASSERT_STREQ(empty->orElse(String::valueOf("abc"))->c_str(), "abc");

    ASSERT_FALSE(Optional<String>::ofNullable(nullptr)->isPresent());
    ASSERT_TRUE(Optional<String>::ofNullable(new String())->isPresent());

    ASSERT_STREQ(empty->orElse(String::valueOf("test"))->c_str(), "test");
    ASSERT_STREQ(test->orElse(nullptr)->c_str(), "test");

    ASSERT_STREQ(empty->toString()->c_str(), "Optional.empty");

    ASSERT_ANY_THROW(empty->get());
}
