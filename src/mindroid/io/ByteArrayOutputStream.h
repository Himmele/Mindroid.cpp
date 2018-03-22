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

#ifndef MINDROID_BYTEARRAYOUTPUTSTREAM_H_
#define MINDROID_BYTEARRAYOUTPUTSTREAM_H_

#include <mindroid/io/OutputStream.h>
#include <mindroid/lang/ByteArray.h>

namespace mindroid {

class ByteArrayOutputStream : public OutputStream {
public:
    /**
     * Constructs a new ByteArrayOutputStream with a default size of 64 bytes.
     * If more than 64 bytes are written to this instance, the underlying byte
     * array will expand.
     */
    ByteArrayOutputStream();

    /**
     * Constructs a new {@code ByteArrayOutputStream} with a default size of
     * {@code size} bytes. If more than {@code size} bytes are written to this
     * instance, the underlying byte array will expand.
     *
     * @param size
     *            initial size for the underlying byte array, must be
     *            non-negative.
     */
    ByteArrayOutputStream(size_t size);

    ByteArrayOutputStream(const sp<ByteArray>& buffer);

    /**
     * Closes this stream. This releases system resources used for this stream.
     *
     * @throws IOException
     *             if an error occurs while attempting to close this stream.
     */
    void close() override;

    void flush() override;

    /**
     * Resets this stream to the beginning of the underlying byte array. All
     * subsequent writes will overwrite any bytes previously stored in this
     * stream.
     */
    void reset();

    /**
     * Returns the total number of bytes written to this stream so far.
     *
     * @return the number of bytes written to this stream.
     */
    size_t size() const;

    /**
     * Returns the contents of this ByteArrayOutputStream as a byte array. Any
     * changes made to the receiver after returning will not be reflected in the
     * byte array returned to the caller.
     *
     * @return this stream's current contents as a byte array.
     */
    sp<ByteArray> toByteArray() const;

    sp<ByteArray> getByteArray() {
        return mBuffer;
    }

    void write(const sp<ByteArray>& buffer) override;

    /**
     * Writes {@code count} bytes from the byte array {@code buffer} starting at
     * offset {@code index} to this stream.
     *
     * @param buffer
     *            the buffer to be written.
     * @param offset
     *            the initial position in {@code buffer} to retrieve bytes.
     * @param count
     *            the number of bytes of {@code buffer} to write.
     * @throws NullPointerException
     *             if {@code buffer} is {@code null}.
     * @throws IndexOutOfBoundsException
     *             if {@code offset < 0} or {@code len < 0}, or if
     *             {@code offset + len} is greater than the length of
     *             {@code buffer}.
     */
    void write(const sp<ByteArray>& buffer, size_t offset, size_t count) override;

    /**
     * Writes the specified byte {@code oneByte} to the OutputStream. Only the
     * low order byte of {@code oneByte} is written.
     *
     * @param b
     *            the byte to be written.
     */
    void write(int32_t b) override;

    /**
     * Takes the contents of this stream and writes it to the output stream
     * {@code out}.
     *
     * @param outputStream
     *            an OutputStream on which to write the contents of this stream.
     * @throws IOException
     *             if an error occurs while writing to {@code out}.
     */
    void writeTo(const sp<OutputStream>& outputStream) {
        outputStream->write(mBuffer, 0, mCount);
    }

protected:
    /**
     * The byte array containing the bytes written.
     */
    sp<ByteArray> mBuffer;

    /**
     * The number of bytes written.
     */
    size_t mCount;

private:
    void expand(size_t i);
};

} /* namespace mindroid */

#endif /* MINDROID_BYTEARRAYOUTPUTSTREAM_H_ */
