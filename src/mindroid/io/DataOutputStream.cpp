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

#include <mindroid/io/DataOutputStream.h>
#include <mindroid/lang/Float.h>
#include <mindroid/lang/Double.h>
#include <mindroid/lang/NullPointerException.h>

namespace mindroid {

DataOutputStream::DataOutputStream(const sp<OutputStream>& outputStream) :
        mOutputStream(outputStream),
        mScratchpad(new ByteArray(8)) {
}

void DataOutputStream::close() {
    flush();
    mOutputStream->close();
}

void DataOutputStream::flush() {
    mOutputStream->flush();
}

void DataOutputStream::write(const sp<ByteArray>& buffer) {
    write(buffer, 0, buffer->size());
}

void DataOutputStream::write(const sp<ByteArray>& buffer, size_t offset, size_t count) {
    mOutputStream->write(buffer, offset, count);
}

void DataOutputStream::write(int32_t b) {
    mOutputStream->write(b);
}

void DataOutputStream::writeBoolean(bool value) {
    mOutputStream->write(value ? 1 : 0);
}

void DataOutputStream::writeByte(uint8_t value) {
    mOutputStream->write(value);
}

void DataOutputStream::writeBytes(const sp<String>& string) {
    if (string == nullptr) {
        throw NullPointerException();
    }
    if (string->length() == 0) {
        return;
    }
    mOutputStream->write(string->getBytes());
}

void DataOutputStream::writeChar(char value) {
    writeByte((uint8_t) value);
}

void DataOutputStream::writeChars(const sp<String>& string) {
    if (string == nullptr) {
        throw NullPointerException();
    }
    if (string->length() == 0) {
        return;
    }
    mOutputStream->write(string->getBytes());
}

void DataOutputStream::writeDouble(double value) {
    writeLong(Double::doubleToLongBits(value));
}

void DataOutputStream::writeFloat(float value) {
    writeInt(Float::floatToIntBits(value));
}

void DataOutputStream::writeShort(uint16_t value) {
    uint8_t* scratchpad = mScratchpad->c_arr();
    scratchpad[0] = (uint8_t) ((value >>  8) & 0xff);
    scratchpad[1] = (uint8_t) ((value >>  0) & 0xff);
    mOutputStream->write(mScratchpad, 0, 2);
}

void DataOutputStream::writeInt(int32_t value) {
    uint8_t* scratchpad = mScratchpad->c_arr();
    scratchpad[0] = (uint8_t) ((value >> 24) & 0xff);
    scratchpad[1] = (uint8_t) ((value >> 16) & 0xff);
    scratchpad[2] = (uint8_t) ((value >>  8) & 0xff);
    scratchpad[3] = (uint8_t) ((value >>  0) & 0xff);
    mOutputStream->write(mScratchpad, 0, 4);
}

void DataOutputStream::writeLong(uint64_t value) {
    uint8_t* scratchpad = mScratchpad->c_arr();
    scratchpad[0] = (uint8_t) ((value >> 56) & 0xff);
    scratchpad[1] = (uint8_t) ((value >> 48) & 0xff);
    scratchpad[2] = (uint8_t) ((value >> 40) & 0xff);
    scratchpad[3] = (uint8_t) ((value >> 32) & 0xff);
    scratchpad[4] = (uint8_t) ((value >> 24) & 0xff);
    scratchpad[5] = (uint8_t) ((value >> 16) & 0xff);
    scratchpad[6] = (uint8_t) ((value >>  8) & 0xff);
    scratchpad[7] = (uint8_t) ((value >>  0) & 0xff);
    mOutputStream->write(mScratchpad, 0, 8);
}

void DataOutputStream::writeUTF(const sp<String>& string) {
    if (string == nullptr) {
        throw NullPointerException();
    }
    if (string->length() == 0) {
        return;
    }
    writeShort(string->length());
    mOutputStream->write(string->getBytes());
}

} /* namespace mindroid */
