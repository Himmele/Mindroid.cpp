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
#include <mindroid/io/ByteArrayInputStream.h>
#include <mindroid/io/ByteArrayOutputStream.h>
#include <mindroid/io/DataInputStream.h>
#include <mindroid/io/DataOutputStream.h>
#include <mindroid/io/File.h>
#include <mindroid/io/FileInputStream.h>
#include <mindroid/io/FileOutputStream.h>

using namespace mindroid;

TEST(Mindroid, ByteArrayStreams) {
    sp<ByteArrayOutputStream> outputStream = new ByteArrayOutputStream();
    for (int32_t i = 0; i < 16; i++) {
        outputStream->write(i);
    }
    sp<ByteArray> buffer1 = ByteArray::valueOf({ 0, 1, 2, 3, 4, 5, 6, 7 });
    outputStream->write(buffer1);
    outputStream->write(buffer1, 4, 4);
    ASSERT_EQ(outputStream->size(), 28);

    sp<ByteArrayInputStream> inputStream = new ByteArrayInputStream(outputStream->toByteArray(), 0, outputStream->size());
    ASSERT_EQ(inputStream->available(), 28);
    for (int32_t i = 0; i < 16; i++) {
        int32_t b = inputStream->read();
        ASSERT_EQ(b, i);
    }
    sp<ByteArray> buffer2 = new ByteArray(16);
    ssize_t rc = inputStream->read(buffer2);
    ASSERT_EQ(rc, 12);
    for (int32_t i = 0; i < 8; i++) {
        ASSERT_EQ(buffer2->get(i), i);
    }
    for (int32_t i = 8; i < 12; i++) {
        ASSERT_EQ(buffer2->get(i), i - 4);
    }
}

TEST(Mindroid, DataStreams) {
    sp<ByteArrayOutputStream> os = new ByteArrayOutputStream();
    sp<DataOutputStream> outputStream = new DataOutputStream(os);
    outputStream->writeBoolean(true);
    outputStream->writeByte(42);
    outputStream->writeChar('X');
    outputStream->writeDouble(123.456);
    outputStream->writeFloat(17.42f);
    outputStream->writeShort(17);
    outputStream->writeInt(12345);
    outputStream->writeLong(123456789);
    sp<String> s = new String("Hello");
    outputStream->writeUTF(s);

    sp<DataInputStream> inputStream = new DataInputStream(new ByteArrayInputStream(os->getByteArray()));
    ASSERT_EQ(inputStream->readBoolean(), true);
    ASSERT_EQ(inputStream->readByte(), 42);
    ASSERT_EQ(inputStream->readChar(), 'X');
    ASSERT_EQ(inputStream->readDouble(), 123.456);
    ASSERT_EQ(inputStream->readFloat(), 17.42f);
    ASSERT_EQ(inputStream->readShort(), 17);
    ASSERT_EQ(inputStream->readInt(), 12345);
    ASSERT_EQ(inputStream->readLong(), 123456789);
    ASSERT_STREQ(inputStream->readUTF()->c_str(), "Hello");
}

TEST(Mindroid, FileStreams) {
    sp<File> file = new File("Test.log");
    sp<FileOutputStream> outputStream = new FileOutputStream(file);
    sp<String> s = new String("Hello");
    outputStream->write(s->getBytes());
    outputStream->close();

    sp<FileInputStream> inputStream = new FileInputStream(file);
    sp<ByteArray> buffer = new ByteArray(16);
    ssize_t rc = inputStream->read(buffer);
    ASSERT_EQ(rc, 5);
    s = new String(buffer);
    ASSERT_STREQ(s->c_str(), "Hello");
    ASSERT_EQ(file->remove(), true);
}
