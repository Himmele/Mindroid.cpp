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

#ifndef MINDROID_IO_DATAINPUTSTREAM_H_
#define MINDROID_IO_DATAINPUTSTREAM_H_

#include <mindroid/io/InputStream.h>

namespace mindroid {

class DataInputStream : public InputStream {
public:
    /**
     * Constructs a new DataInputStream on the InputStream {@code in}. All
     * reads are then filtered through this stream. Note that data read by this
     * stream is not in a human readable format and was most likely created by a
     * DataOutputStream.
     *
     * <p><strong>Warning:</strong> passing a null source creates an invalid
     * {@code DataInputStream}. All operations on such a stream will fail.
     *
     * @param inputStream
     *            the source InputStream the filter reads from.
     * @see DataOutputStream
     * @see RandomAccessFile
     */
    DataInputStream(const sp<InputStream>& inputStream);

    size_t available() override;

    /**
     * Closes this stream. This implementation closes the filtered stream.
     *
     * @throws IOException
     *             if an error occurs while closing this stream.
     */
    void close() override;

    /**
     * Sets a mark position in this stream. The parameter {@code limit}
     * indicates how many bytes can be read before the mark is invalidated.
     * Sending {@code reset()} will reposition this stream back to the marked
     * position, provided that {@code limit} has not been surpassed.
     * <p>
     * This implementation sets a mark in the filtered stream.
     *
     * @param limit
     *            the number of bytes that can be read from this stream before
     *            the mark is invalidated.
     * @see #markSupported()
     * @see #reset()
     */
    void mark(size_t limit) override;

    /**
     * Indicates whether this stream supports {@code mark()} and {@code reset()}.
     * This implementation returns whether or not the filtered stream supports
     * marking.
     *
     * @return {@code true} if {@code mark()} and {@code reset()} are supported,
     *         {@code false} otherwise.
     * @see #mark(int)
     * @see #reset()
     * @see #skip(long)
     */
    bool markSupported() override;

    /**
     * Reads a single byte from the filtered stream and returns it as an integer
     * in the range from 0 to 255. Returns -1 if the end of this stream has been
     * reached.
     *
     * @return the byte read or -1 if the end of the filtered stream has been
     *         reached.
     * @throws IOException
     *             if the stream is closed or another IOException occurs.
     */
    int32_t read() override;

    /**
     * Reads some number of bytes from the contained input stream and stores them into the buffer array b.
     */
    ssize_t read(const sp<ByteArray>& buffer) override;

    /**
     * Reads up to len bytes of data from the contained input stream into an array of bytes.
     */
    ssize_t read(const sp<ByteArray>& buffer, size_t offset, size_t count) override;

    /**
     * Resets this stream to the last marked location. This implementation
     * resets the target stream.
     *
     * @throws IOException
     *             if this stream is already closed, no mark has been set or the
     *             mark is no longer valid because more than {@code limit}
     *             bytes have been read since setting the mark.
     * @see #mark(int)
     * @see #markSupported()
     */
    void reset() override;

    /**
     * Skips {@code byteCount} bytes in this stream. Subsequent
     * calls to {@code read} will not return these bytes unless {@code reset} is
     * used. This implementation skips {@code byteCount} bytes in the
     * filtered stream.
     *
     * @return the number of bytes actually skipped.
     * @throws IOException
     *             if this stream is closed or another IOException occurs.
     * @see #mark(int)
     * @see #reset()
     */
    size_t skip(size_t count) override;

    bool readBoolean();

    uint8_t readByte();

    char readChar();

    double readDouble();

    float readFloat();

    void readFully(const sp<ByteArray>& buffer);

    void readFully(const sp<ByteArray>& buffer, size_t offset, size_t count);

    int16_t readShort();

    int32_t readInt();

    int64_t readLong();

    uint8_t readUnsignedByte();

    uint16_t readUnsignedShort();

    sp<String> readUTF();

    size_t skipBytes(size_t count);

protected:
    /**
     * The source input stream that is filtered.
     */
    sp<InputStream> mInputStream;

private:
    sp<ByteArray> mScratchpad;
};

} /* namespace mindroid */

#endif /* MINDROID_IO_DATAINPUTSTREAM_H_ */
