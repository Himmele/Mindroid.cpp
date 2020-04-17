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

#ifndef MINDROID_NIO_BUFFER_H_
#define MINDROID_NIO_BUFFER_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/lang/ByteArray.h>

namespace mindroid {

class Buffer : public Object {
public:
    virtual ~Buffer() = default;

    /**
     * Returns the array that backs this buffer (optional operation).
     */
    virtual sp<ByteArray> array() const = 0;

    /**
     * Returns the offset within this buffer's backing array of the first element of the buffer (optional operation).
     */
    virtual size_t arrayOffset() const = 0;

    /**
     * Returns this buffer's capacity.
     */
    size_t capacity() const;

    /**
     * Clears this buffer. The position is set to zero, the limit is set to the capacity, and the mark is discarded.
     */
    sp<Buffer> clear();

    /**
     * Flips this buffer. The limit is set to the current position and then the position is set to zero.
     * If the mark is defined then it is discarded.
     */
    sp<Buffer> flip();

    /**
     * Tells whether there are any elements between the current position and the limit.
     */
    virtual bool hasRemaining() const;

    /**
     * Tells whether or not this buffer is read-only.
     */
    virtual bool isReadOnly() const;

    /**
     * Returns this buffer's limit.
     */
    size_t limit() const;

    /**
     * Sets this buffer's limit.
     */
    sp<Buffer> limit(size_t newLimit);

    /**
     * Returns this buffer's position.
     */
    size_t position() const;

    /**
     * Sets this buffer's position.
     */
    sp<Buffer> position(size_t newPosition);

    /**
     * Returns the number of elements between the current position and the limit.
     */
    virtual size_t remaining() const;

    /**
     * Rewinds this buffer. The position is set to zero and the mark is discarded.
     */
    sp<Buffer> rewind();

    sp<String> toString() const;

protected:
    Buffer(size_t position, size_t limit, size_t capacity, bool readOnly);

    /* 0 <= mark <= position <= limit <= capacity */
    size_t mPosition;
    size_t mLimit;
    const size_t mCapacity;
    const bool mReadOnly;
};

} /* namespace mindroid */

#endif /* MINDROID_NIO_BUFFER_H_ */
