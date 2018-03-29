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

#include <mindroid/io/FileOutputStream.h>
#include <mindroid/io/File.h>
#include <mindroid/io/FileNotFoundException.h>
#include <mindroid/io/IOException.h>
#include <mindroid/lang/NullPointerException.h>

namespace mindroid {

FileOutputStream::FileOutputStream(const sp<File>& file) :
        FileOutputStream(file, false) {
}

FileOutputStream::FileOutputStream(const sp<File>& file, bool append) :
        mOutputFileStream(file->getPath()->c_str(), std::ifstream::out | std::ifstream::binary | (append ? std::ifstream::app : std::ifstream::trunc)) {
}

FileOutputStream::FileOutputStream(const sp<String>& file) :
        FileOutputStream(file, false) {
}

FileOutputStream::FileOutputStream(const sp<String>& file, bool append) :
        mOutputFileStream(file->c_str(), std::ifstream::out | std::ifstream::binary | (append ? std::ifstream::app : std::ifstream::trunc)) {
}

void FileOutputStream::close() {
    mOutputFileStream.close();
}

void FileOutputStream::flush() {
    mOutputFileStream.flush();
}

void FileOutputStream::write(int32_t b) {
    uint8_t data = (uint8_t) b;
    mOutputFileStream.write(reinterpret_cast<char*>(&data), sizeof(data));
    if (!mOutputFileStream.good()) {
        throw IOException();
    }
}

void FileOutputStream::write(const sp<ByteArray>& buffer) {
    write(buffer, 0, buffer->size());
}

void FileOutputStream::write(const sp<ByteArray>& buffer, size_t offset, size_t count) {
    if (buffer == nullptr) {
        throw NullPointerException();
    }
    if ((offset + count) > buffer->size()) {
        throw IndexOutOfBoundsException();
    }

    if (count == 0) {
        return;
    }

    mOutputFileStream.write(reinterpret_cast<char*>(buffer->c_arr()) + offset, count);
    if (!mOutputFileStream.good()) {
        throw IOException();
    }
}

} /* namespace mindroid */
