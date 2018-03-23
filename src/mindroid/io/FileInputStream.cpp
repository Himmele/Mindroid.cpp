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

#include <mindroid/io/FileInputStream.h>
#include <mindroid/io/File.h>
#include <mindroid/io/FileNotFoundException.h>
#include <mindroid/io/IOException.h>

namespace mindroid {

FileInputStream::FileInputStream(const sp<File>& file) :
        mInputFileStream(file->getPath()->c_str(), std::ifstream::in | std::ifstream::binary) {
}

size_t FileInputStream::available() {
    return mInputFileStream.rdbuf()->in_avail();
}

void FileInputStream::close() {
    mInputFileStream.close();
}

int32_t FileInputStream::read() {
    int32_t data;
    mInputFileStream.read(reinterpret_cast<char*>(&data), sizeof(uint8_t));
    Assert::assertTrue<IOException>(mInputFileStream.good());
    return data;
}

ssize_t FileInputStream::read(const sp<ByteArray>& buffer) {
    return read(buffer, 0, buffer->size());
}

ssize_t FileInputStream::read(const sp<ByteArray>& buffer, size_t offset, size_t count) {
    Assert::assertNotNull(buffer);
    Assert::assertFalse<IndexOutOfBoundsException>((offset < 0) || (count < 0) || ((offset + count) > buffer->size()));

    if (count == 0) {
        return 0;
    }
    if (mInputFileStream.eof()) {
        return -1;
    }

    return mInputFileStream.read(reinterpret_cast<char*>(buffer->c_arr() + offset), count).gcount();
    Assert::assertTrue<IOException>(mInputFileStream.good());
}

size_t FileInputStream::skip(size_t count) {
    mInputFileStream.seekg(count, std::ios_base::cur);
    return count;
}

} /* namespace mindroid */
