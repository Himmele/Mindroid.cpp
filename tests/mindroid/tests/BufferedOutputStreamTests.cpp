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

#include <mindroid/tests/BufferedOutputStreamTests.h>
#include <mindroid/lang/ByteArray.h>
#include <mindroid/io/ByteArrayInputStream.h>
#include <mindroid/io/ByteArrayOutputStream.h>
#include <mindroid/io/BufferedOutputStream.h>
#include <mindroid/io/BufferedInputStream.h>

using namespace mindroid;

TEST_F(BufferedOutputStreamTests, OutputStreamWriteData) {
    sp<ByteArrayOutputStream> outputStream = new ByteArrayOutputStream();
    sp<BufferedOutputStream> bufferedOutputStream = new BufferedOutputStream(outputStream);

    for (int32_t i = 0; i < 256; ++i) {
        bufferedOutputStream->write(i);
    }

    bufferedOutputStream->flush();

    auto byteArray = outputStream->toByteArray();

    sp<ByteArrayInputStream> input = new ByteArrayInputStream(byteArray);

    for (int32_t i = 0; i < 256; ++i) {
        auto readValue = input->read();
        EXPECT_EQ(i, readValue);
    }
    bufferedOutputStream->close();
}

TEST_F(BufferedOutputStreamTests, OutputStreamBufferData) {
    sp<ByteArrayOutputStream> outputStream = new ByteArrayOutputStream();
    sp<BufferedOutputStream> bufferedOutputStream = new BufferedOutputStream(outputStream, 4);

    sp<ByteArray> bufferFirstChunk = new ByteArray({0, 1, 2});
    sp<ByteArray> bufferSecondChunk = new ByteArray({3, 4, 5});

    bufferedOutputStream->write(bufferFirstChunk);

    auto byteArray = outputStream->toByteArray();

    EXPECT_EQ(byteArray->size(), 0);

    bufferedOutputStream->write(bufferSecondChunk);

    byteArray = outputStream->toByteArray();

    EXPECT_EQ(byteArray->size(), 3);
    EXPECT_EQ(byteArray->get(0), 0);
    EXPECT_EQ(byteArray->get(1), 1);
    EXPECT_EQ(byteArray->get(2), 2);
}
