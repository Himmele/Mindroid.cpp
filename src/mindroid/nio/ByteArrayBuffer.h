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

#ifndef MINDROID_NIO_BYTEARRAYBUFFER_H_
#define MINDROID_NIO_BYTEARRAYBUFFER_H_

#include <mindroid/nio/ByteBuffer.h>

namespace mindroid {

class ByteArrayBuffer : public ByteBuffer {
public:
    ByteArrayBuffer(size_t capacity);
    ByteArrayBuffer(const sp<ByteArray>& byteArray);

    sp<ByteArray> array() const override;
    sp<ByteBuffer> compact() override;
    sp<ByteBuffer> duplicate() override;
    sp<ByteBuffer> slice() override;

private:
    ByteArrayBuffer(const sp<ByteArray>& byteArray, size_t offset, size_t size);

    sp<ByteArray> mByteArray;

    friend class ByteBuffer;
};

} /* namespace mindroid */

#endif /* MINDROID_NIO_BYTEARRAYBUFFER_H_ */
