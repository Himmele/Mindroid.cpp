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
#include <mindroid/lang/Object.h>

using namespace mindroid;

class Test : public Object {
public:
    Test(int32_t id) : mId(id) {
    }

    virtual ~Test() {
    }

    int32_t id() const {
        return mId;
    }

private:
    int32_t mId;
};

TEST(Mindroid, ReferenceCounting) {
    sp<::Test> ref1 = new ::Test(1);

    {
        sp<::Test> ref2 = new ::Test(2);
    }

    wp<::Test> ref3 = ref1;
    sp<::Test> ref31 = ref3.get();
    ASSERT_NE(ref31, nullptr);
    ref31 = nullptr;

    ref1 = nullptr;
    ref31 = ref3.get();
    ASSERT_EQ(ref31, nullptr);

    wp<::Test> ref4 = new ::Test(4);
    ASSERT_NE(ref4, nullptr);
    wp<::Test> ref5 = ref4;
    ASSERT_NE(ref5, nullptr);
    ref4 = nullptr;
    ASSERT_EQ(ref4, nullptr);
    ASSERT_NE(ref5.get(), nullptr);
    sp<::Test> ref6 = ref5.get();
    ref6 = nullptr;
    ASSERT_NE(ref5, nullptr);
    ASSERT_EQ(ref5.get(), nullptr);
}
