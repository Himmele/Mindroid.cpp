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

#include <mindroid/tests/BufferedInputStreamTests.h>
#include <mindroid/lang/ByteArray.h>
#include <mindroid/io/ByteArrayInputStream.h>
#include <mindroid/io/ByteArrayOutputStream.h>
#include <mindroid/io/BufferedInputStream.h>

using namespace mindroid;

TEST_F(BufferedInputStreamTests, InputStreamReadData) {
    sp<ByteArray> array = new ByteArray({0, 1, 2, 3, 4, 5, 6, 7});

    sp<ByteArrayInputStream> inputStream = new ByteArrayInputStream(array);
    sp<BufferedInputStream> bufferedInputStream = new BufferedInputStream(inputStream);

    for (int32_t i = 0; i < 8; ++i) {
        auto readValue = bufferedInputStream->read();
        EXPECT_EQ(readValue, i);
    }
    EXPECT_EQ(inputStream->read(), -1);
    bufferedInputStream->close();
}

TEST_F(BufferedInputStreamTests, InputStreamReadToBuffer) {
    sp<ByteArray> array = new ByteArray({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

    sp<ByteArrayInputStream> inputStream = new ByteArrayInputStream(array);
    sp<BufferedInputStream> bufferedInputStream = new BufferedInputStream(inputStream, 5);

    sp<ByteArray> buffer = new ByteArray(2);

    EXPECT_EQ(bufferedInputStream->read(buffer), 2);
    EXPECT_EQ(buffer->get(0), 0);
    EXPECT_EQ(buffer->get(1), 1);

    EXPECT_EQ(bufferedInputStream->read(buffer), 2);
    EXPECT_EQ(buffer->get(0), 2);
    EXPECT_EQ(buffer->get(1), 3);

    EXPECT_EQ(bufferedInputStream->read(buffer), 2);
    EXPECT_EQ(buffer->get(0), 4);
    EXPECT_EQ(buffer->get(1), 5);

    EXPECT_EQ(bufferedInputStream->read(buffer), 2);
    EXPECT_EQ(buffer->get(0), 6);
    EXPECT_EQ(buffer->get(1), 7);

    EXPECT_EQ(bufferedInputStream->read(buffer), 2);
    EXPECT_EQ(buffer->get(0), 8);
    EXPECT_EQ(buffer->get(1), 9);

    EXPECT_EQ(bufferedInputStream->read(buffer), -1);
}

TEST_F(BufferedInputStreamTests, InputStreamSkip) {
    sp<ByteArray> array = new ByteArray({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

    sp<ByteArrayInputStream> inputStream = new ByteArrayInputStream(array);
    sp<BufferedInputStream> bufferedInputStream = new BufferedInputStream(inputStream, 5);

    sp<ByteArray> buffer = new ByteArray(2);

    EXPECT_EQ(bufferedInputStream->read(buffer), 2);
    EXPECT_EQ(buffer->get(0), 0);
    EXPECT_EQ(buffer->get(1), 1);

    bufferedInputStream->skip(5);

    EXPECT_EQ(bufferedInputStream->read(buffer), 2);
    EXPECT_EQ(buffer->get(0), 7);
    EXPECT_EQ(buffer->get(1), 8);

    EXPECT_EQ(bufferedInputStream->read(buffer), 1);
    EXPECT_EQ(buffer->get(0), 9);
}

TEST_F(BufferedInputStreamTests, InputStreamMark) {
    sp<ByteArray> array = new ByteArray({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

    sp<ByteArrayInputStream> inputStream = new ByteArrayInputStream(array);
    sp<BufferedInputStream> bufferedInputStream = new BufferedInputStream(inputStream, 5);

    sp<ByteArray> buffer = new ByteArray(2);

    EXPECT_EQ(bufferedInputStream->read(buffer), 2);
    EXPECT_EQ(buffer->get(0), 0);
    EXPECT_EQ(buffer->get(1), 1);

    bufferedInputStream->mark(10);

    EXPECT_EQ(bufferedInputStream->read(buffer), 2);
    EXPECT_EQ(buffer->get(0), 2);
    EXPECT_EQ(buffer->get(1), 3);

    bufferedInputStream->reset();

    EXPECT_EQ(bufferedInputStream->read(buffer), 2);
    EXPECT_EQ(buffer->get(0), 2);
    EXPECT_EQ(buffer->get(1), 3);

    EXPECT_EQ(bufferedInputStream->read(buffer), 2);
    EXPECT_EQ(buffer->get(0), 4);
    EXPECT_EQ(buffer->get(1), 5);

    EXPECT_EQ(bufferedInputStream->read(buffer), 2);
    EXPECT_EQ(buffer->get(0), 6);
    EXPECT_EQ(buffer->get(1), 7);

    EXPECT_EQ(bufferedInputStream->read(buffer), 2);
    EXPECT_EQ(buffer->get(0), 8);
    EXPECT_EQ(buffer->get(1), 9);
}
