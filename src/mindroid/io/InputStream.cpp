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

#include <mindroid/io/InputStream.h>
#include <mindroid/io/IOException.h>
#include <mindroid/lang/Math.h>
#include <mindroid/lang/NullPointerException.h>
#include <mindroid/lang/IndexOutOfBoundsException.h>

namespace mindroid {

ssize_t InputStream::read(const sp<ByteArray>& buffer, size_t offset, size_t count) {
    if (buffer == nullptr) {
        throw NullPointerException();
    }
    if ((offset + count) > buffer->size()) {
        throw IndexOutOfBoundsException();
    }

    for (size_t i = 0; i < count; ++i) {
        int32_t c;
        if ((c = read()) == -1) {
            return i == 0 ? -1 : i;
        }
        buffer->set(offset + i, (uint8_t) c);
    }
    return count;
}

void InputStream::reset() {
    throw IOException();
}

size_t InputStream::skip(size_t count) {
    sp<ByteArray> buffer = new ByteArray(4096);

    size_t skipped = 0;
    while (skipped < count) {
        size_t size = Math::min(count - skipped, buffer->size());
        ssize_t result = read(buffer, 0, size);
        if (result == -1) {
            break;
        }
        skipped += result;
        if (((size_t) result) < size) {
            break;
        }
    }

    return skipped;
}

} /* namespace mindroid */
