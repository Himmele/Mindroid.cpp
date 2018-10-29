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
#include <mindroid/util/HashSet.h>
#include <mindroid/lang/String.h>

using namespace mindroid;

TEST(Mindroid, SetOfInteger) {
    sp<HashSet<int32_t>> set = new HashSet<int32_t>();
    set->add(1);
    ASSERT_EQ(set->contains(1), true);
    ASSERT_EQ(set->contains(2), false);
    set->add(2);
    ASSERT_EQ(set->contains(1), true);
    ASSERT_EQ(set->contains(2), true);
    set->add(3);
    ASSERT_EQ(set->contains(1), true);
    ASSERT_EQ(set->contains(2), true);
    ASSERT_EQ(set->contains(3), true);
    set->remove(3);
    ASSERT_EQ(set->contains(1), true);
    ASSERT_EQ(set->contains(2), true);
    ASSERT_EQ(set->contains(3), false);
    auto itr = set->iterator();
    while (itr.hasNext()) {
        itr.next();
        itr.remove();
    }
    ASSERT_EQ(set->size(), 0);
}

TEST(Mindroid, SetOfString) {
    sp<HashSet<sp<String>>> set = new HashSet<sp<String>>();
    set->add(String::valueOf("1"));
    ASSERT_EQ(set->contains(String::valueOf("1")), true);
    ASSERT_EQ(set->contains(String::valueOf("2")), false);
    set->add(String::valueOf("2"));
    ASSERT_EQ(set->contains(String::valueOf("1")), true);
    ASSERT_EQ(set->contains(String::valueOf("2")), true);
    set->add(String::valueOf("3"));
    ASSERT_EQ(set->contains(String::valueOf("1")), true);
    ASSERT_EQ(set->contains(String::valueOf("2")), true);
    ASSERT_EQ(set->contains(String::valueOf("3")), true);
    set->remove(String::valueOf("3"));
    ASSERT_EQ(set->contains(String::valueOf("1")), true);
    ASSERT_EQ(set->contains(String::valueOf("2")), true);
    ASSERT_EQ(set->contains(String::valueOf("3")), false);
    auto itr = set->iterator();
    while (itr.hasNext()) {
        itr.next();
        itr.remove();
    }
    ASSERT_EQ(set->size(), 0);
}
