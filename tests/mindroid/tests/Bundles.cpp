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
#include <mindroid/os/Bundle.h>
#include <mindroid/lang/String.h>

using namespace mindroid;

class Test :
    public Object
{
public:
    Test(uint32_t value) : mValue(value) { }
    virtual ~Test() { }
    uint32_t getValue() { return mValue; }

private:
    uint32_t mValue;
};

TEST(Mindroid, Bundle) {
    sp<Bundle> bundle = new Bundle();
    bundle->putBoolean("1", true);
    ASSERT_EQ(bundle->getBoolean("1", false), true);

    bundle->putInt("2", 12345);
    ASSERT_EQ(bundle->getInt("2", 0), 12345);
    ASSERT_EQ(bundle->getLong("2", 0), 0);

    bundle->putObject("3", new ::Test(17));
    sp<::Test> test = object_cast<::Test>(bundle->getObject("3"));
    ASSERT_EQ(test->getValue(), 17);

    bundle->putString("4", "Test");
    sp<String> string = bundle->getString("4");
    ASSERT_EQ(string->equals("Test"), true);
}
