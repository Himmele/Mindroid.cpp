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

#include <mindroid/nio/ByteArrayBuffer.h>
#include <cstring>

namespace mindroid {

ByteArrayBuffer::ByteArrayBuffer(size_t capacity) :
        ByteArrayBuffer(new ByteArray(capacity)) {
}

ByteArrayBuffer::ByteArrayBuffer(const sp<ByteArray>& byteArray) :
        ByteArrayBuffer(byteArray, 0, byteArray->size()) {
}

ByteArrayBuffer::ByteArrayBuffer(const sp<ByteArray>& byteArray, size_t offset, size_t size) :
        ByteBuffer(byteArray, offset, size, false), mByteArray(byteArray) {
    mOffset = offset;
}

sp<ByteArray> ByteArrayBuffer::array() const {
    return mByteArray;
}

sp<ByteBuffer> ByteArrayBuffer::compact() {
    size_t count = mLimit - mPosition;
    std::memmove(mBuffer->c_arr(), mBuffer->c_arr() + mPosition, count);
    mPosition = count;
    mLimit = mCapacity;
    return this;
}

sp<ByteBuffer> ByteArrayBuffer::duplicate() {
    return new ByteArrayBuffer(array()->clone());
}

sp<ByteBuffer> ByteArrayBuffer::slice() {
    return new ByteArrayBuffer(mByteArray, mPosition + mOffset, mLimit - mPosition);
}

} /* namespace mindroid */
