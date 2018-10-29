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
#include <mindroid/io/File.h>
#include <mindroid/lang/String.h>

using namespace mindroid;

TEST(Mindroid, Files) {
    sp<File> file = new File("/mindroid/test/");
    ASSERT_STREQ(file->getName()->c_str(), "test");
    ASSERT_STREQ(file->getPath()->c_str(), "/mindroid/test");
    ASSERT_STREQ(file->getParent()->c_str(), "/mindroid");
}

TEST(Mindroid, Root) {
    sp<File> file = new File("/");
    ASSERT_STREQ(file->getName()->c_str(), "");
    ASSERT_STREQ(file->getPath()->c_str(), "/");
    ASSERT_EQ(file->getParent(), nullptr);
}
