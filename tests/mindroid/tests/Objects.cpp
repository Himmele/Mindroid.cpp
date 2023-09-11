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

TEST(Mindroid, Objects) {
    sp<Object> o1 = new Object();
    sp<Object> o2 = new Object();
    ASSERT_EQ(o1->equals(o2), false);
    sp<Object> o3 = o1;
    ASSERT_EQ(o1->equals(o3), true);
}

TEST(Mindroid, ObjectsBuiltByCreatorFunction) {
    auto o1 = sp<Object>::of();
    auto o2 = sp<Object>::of();
    ASSERT_EQ(o1->equals(o2), false);
    ASSERT_EQ(o1->getStrongReferenceCount(), 1);
    ASSERT_EQ(o2->getStrongReferenceCount(), 1);
    sp<Object> o3 = o1;
    ASSERT_EQ(o1->equals(o3), true);
    ASSERT_EQ(o1->getStrongReferenceCount(), 2);
    ASSERT_EQ(o3->getStrongReferenceCount(), 2);
    ASSERT_EQ(o2->getStrongReferenceCount(), 1);
}

TEST(Mindroid, ObjectsBuiltByCreatorFunctionWithArgs) {
    class TestObject : public Object {
    public:
        TestObject(int32_t value) : value(value) {}
        const int32_t value;
    };
    const int32_t value1 = 1;
    const int32_t value2 = 1;

    auto o1 = sp<TestObject>::of(value1);
    auto o2 = sp<TestObject>::of(value2);

    ASSERT_EQ(o1->value, value1);
    ASSERT_EQ(o2->value, value2);
    ASSERT_EQ(o1->equals(o2), false);
    ASSERT_EQ(o1->getStrongReferenceCount(), 1);
    ASSERT_EQ(o2->getStrongReferenceCount(), 1);
    sp<TestObject> o3 = o1;
    ASSERT_EQ(o1->value, value1);
    ASSERT_EQ(o3->value, value1);
    ASSERT_EQ(o1->equals(o3), true);
    ASSERT_EQ(o1->getStrongReferenceCount(), 2);
    ASSERT_EQ(o3->getStrongReferenceCount(), 2);
    ASSERT_EQ(o2->getStrongReferenceCount(), 1);
}
