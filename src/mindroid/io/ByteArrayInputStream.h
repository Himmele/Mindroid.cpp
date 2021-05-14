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

#ifndef MINDROID_IO_BYTEARRAYINPUTSTREAM_H_
#define MINDROID_IO_BYTEARRAYINPUTSTREAM_H_

#include <mindroid/io/InputStream.h>

namespace mindroid {

class ByteArrayInputStream : public InputStream {
public:
    /**
     * Constructs a new {@code ByteArrayInputStream} on the byte array
     * {@code buffer}.
     *
     * @param buffer
     *            the byte array to stream over.
     */
    ByteArrayInputStream(const sp<ByteArray>& buffer);

    /**
     * Constructs a new {@code ByteArrayInputStream} on the byte array
     * {@code buffer} with the initial position set to {@code offset} and the
     * number of bytes available set to {@code offset} + {@code count}.
     *
     * @param buffer
     *            the byte array to stream over.
     * @param offset
     *            the initial position in {@code buffer} to start streaming from.
     * @param count
     *            the number of bytes available for streaming.
     */
    ByteArrayInputStream(const sp<ByteArray>& buffer, size_t offset, size_t count);

    /**
     * Returns the number of remaining bytes.
     *
     * @return {@code count - position}
     */
    size_t available() override;

    /**
     * Closes this stream and frees resources associated with this stream.
     *
     * @throws IOException
     *             if an I/O error occurs while closing this stream.
     */
    void close() override;

    /**
     * Sets a mark position in this ByteArrayInputStream. The parameter
     * {@code limit} is ignored. Sending {@code reset()} will reposition the
     * stream back to the marked position.
     *
     * @param limit
     *            ignored.
     * @see #markSupported()
     * @see #reset()
     */
    void mark(size_t limit) override;

    /**
     * Indicates whether this stream supports the {@code mark()} and
     * {@code reset()} methods. Returns {@code true} since this class supports
     * these methods.
     *
     * @return always {@code true}.
     * @see #mark(int)
     * @see #reset()
     */
    bool markSupported() override;

    /**
     * Reads a single byte from the source byte array and returns it as an
     * integer in the range from 0 to 255. Returns -1 if the end of the source
     * array has been reached.
     *
     * @return the byte read or -1 if the end of this stream has been reached.
     */
    int32_t read() override;

    /**
     * Reads some number of bytes from the input stream and stores them into the buffer array.
     */
    ssize_t read(const sp<ByteArray>& buffer) override;

    /**
     * Reads up to count bytes of data from the input stream into an array of bytes.
     */
    ssize_t read(const sp<ByteArray>& buffer, size_t offset, size_t count) override;

    /**
     * Resets this stream to the last marked location. This implementation
     * resets the position to either the marked position, the start position
     * supplied in the constructor or 0 if neither has been provided.
     *
     * @see #mark(int)
     */
    void reset() override;

    /**
     * Skips {@code byteCount} bytes in this InputStream. Subsequent
     * calls to {@code read} will not return these bytes unless {@code reset} is
     * used. This implementation skips {@code byteCount} number of bytes in the
     * target stream. It does nothing and returns 0 if {@code byteCount} is negative.
     *
     * @return the number of bytes actually skipped.
     */
    size_t skip(size_t count) override;

protected:
    /**
     * The byte array containing the bytes written.
     */
    sp<ByteArray> mBuffer;

    /**
     * The current position within the byte array.
     */
    size_t mPosition = 0;

    /**
     * The current mark position. Initially set to 0 or the <code>offset</code>
     * parameter within the constructor.
     */
    size_t mMark;

    /**
     * The total number of bytes initially available in the byte array
     * {@code mBuffer}.
     */
    size_t mCount;
};

} /* namespace mindroid */

#endif /* MINDROID_IO_BYTEARRAYINPUTSTREAM_H_ */
