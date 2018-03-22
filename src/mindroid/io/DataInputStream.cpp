/*
 * Copyright (C) 2017 E.S.R.Labs
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

#include <mindroid/io/DataInputStream.h>
#include <mindroid/io/EOFException.h>
#include <mindroid/lang/Float.h>
#include <mindroid/lang/Double.h>
#include <mindroid/util/Assert.h>

namespace mindroid {

DataInputStream::DataInputStream(const sp<InputStream>& inputStream) :
        mInputStream(inputStream),
        mScratchpad(new ByteArray(8)) {
}

size_t DataInputStream::available() {
    return mInputStream->available();
}

void DataInputStream::close() {
    mInputStream->close();
}

void DataInputStream::mark(size_t limit) {
    mInputStream->mark(limit);
}

bool DataInputStream::markSupported() {
    return mInputStream->markSupported();
}

int32_t DataInputStream::read() {
    return mInputStream->read();
}

ssize_t DataInputStream::read(const sp<ByteArray>& buffer) {
    return mInputStream->read(buffer);
}

ssize_t DataInputStream::read(const sp<ByteArray>& buffer, size_t offset, size_t count) {
    return mInputStream->read(buffer, offset, count);
}

void DataInputStream::reset() {
    mInputStream->reset();
}

size_t DataInputStream::skip(size_t count) {
    return mInputStream->skip(count);
}


bool DataInputStream::readBoolean() {
    int32_t value = mInputStream->read();
    Assert::assertFalse<EOFException>(value < 0);
    return value != 0;
}

uint8_t DataInputStream::readByte() {
    int32_t value = mInputStream->read();
    Assert::assertFalse<EOFException>(value < 0);
    return (uint8_t) value;
}

char DataInputStream::readChar() {
    return (char) readByte();
}

double DataInputStream::readDouble() {
    return Double::longBitsToDouble(readLong());
}

float DataInputStream::readFloat() {
    return Float::intBitsToFloat(readInt());
}

void DataInputStream::readFully(const sp<ByteArray>& buffer) {
    readFully(buffer, 0, buffer->size());
}

void DataInputStream::readFully(const sp<ByteArray>& buffer, size_t offset, size_t size) {
    if (size == 0) {
        return;
    }
    Assert::assertNotNull("buffer == null", buffer);
    Assert::assertFalse<IndexOutOfBoundsException>((offset < 0) || (size < 0) || ((offset + size) > buffer->size()));

    while (size > 0) {
        int count = mInputStream->read(buffer, offset, size);
        Assert::assertFalse<EOFException>(count < 0);
        offset += count;
        size -= count;
    }
}

int16_t DataInputStream::readShort() {
    readFully(mScratchpad, 0, 2);
    uint8_t* scratchpad = mScratchpad->c_arr();
    return (int16_t) ((scratchpad[0] << 8) | (scratchpad[1] & 0xff));
}

int32_t DataInputStream::readInt() {
    readFully(mScratchpad, 0, 4);
    uint8_t* scratchpad = mScratchpad->c_arr();
    return (((scratchpad[0] & 0xff) << 24) |
            ((scratchpad[1] & 0xff) << 16) |
            ((scratchpad[2] & 0xff) <<  8) |
            ((scratchpad[3] & 0xff) <<  0));
}

int64_t DataInputStream::readLong() {
    readFully(mScratchpad, 0, 8);
    uint8_t* scratchpad = mScratchpad->c_arr();
    return ((((int64_t) (scratchpad[0] & 0xff)) << 56) |
            (((int64_t) (scratchpad[1] & 0xff)) << 48) |
            (((int64_t) (scratchpad[2] & 0xff)) << 40) |
            (((int64_t) (scratchpad[3] & 0xff)) << 32) |
            (((int64_t) (scratchpad[4] & 0xff)) << 24) |
            ((scratchpad[5] & 0xff) << 16) |
            ((scratchpad[6] & 0xff) <<  8) |
            ((scratchpad[7] & 0xff) <<  0));

}

uint8_t DataInputStream::readUnsignedByte() {
    int32_t value = mInputStream->read();
    Assert::assertFalse<EOFException>(value < 0);
    return (uint8_t) value;
}

uint16_t DataInputStream::readUnsignedShort() {
    return ((uint16_t) readShort()) & 0xffff;
}

sp<String> DataInputStream::readUTF() {
    size_t size = readUnsignedShort();
    sp<ByteArray> string = new ByteArray(size);
    readFully(string, 0, size);
    return new String((char*) string->c_arr(), size);
}

size_t DataInputStream::skipBytes(size_t count) {
    size_t skipped = 0;
    size_t result;
    while (skipped < count && (result = mInputStream->skip(count - skipped)) != 0) {
        skipped += result;
    }
    return skipped;
}

} /* namespace mindroid */
