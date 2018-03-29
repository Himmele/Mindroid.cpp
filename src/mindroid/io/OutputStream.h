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

#ifndef MINDROID_IO_OUTPUTSTREAM_H_
#define MINDROID_IO_OUTPUTSTREAM_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/ByteArray.h>

namespace mindroid {

/**
 * A writable sink for bytes.
 *
 * <p>Most clients will use output streams that write data to the file system
 * ({@link FileOutputStream}), the network ({@link java.net.Socket#getOutputStream()}/{@link
 * java.net.HttpURLConnection#getOutputStream()}), or to an in-memory byte array
 * ({@link ByteArrayOutputStream}).
 *
 * <p>Use {@link OutputStreamWriter} to adapt a byte stream like this one into a
 * character stream.
 *
 * <p>Most clients should wrap their output stream with {@link
 * BufferedOutputStream}. Callers that do only bulk writes may omit buffering.
 *
 * <h3>Subclassing OutputStream</h3>
 * Subclasses that decorate another output stream should consider subclassing
 * {@link FilterOutputStream}, which delegates all calls to the target output
 * stream.
 *
 * <p>All output stream subclasses should override <strong>both</strong> {@link
 * #write(int)} and {@link #write(byte[],int,int) write(byte[],int,int)}. The
 * three argument overload is necessary for bulk access to the data. This is
 * much more efficient than byte-by-byte access.
 *
 * @see InputStream
 */
class OutputStream : public Object {
public:
    /**
     * Default constructor.
     */
    OutputStream() {
    }

    virtual ~OutputStream() noexcept = default;

    /**
     * Closes this stream. Implementations of this method should free any
     * resources used by the stream. This implementation does nothing.
     *
     * @throws IOException
     *             if an error occurs while closing this stream.
     */
    virtual void close() {
    }

    /**
     * Flushes this stream. Implementations of this method should ensure that
     * any buffered data is written out. This implementation does nothing.
     *
     * @throws IOException
     *             if an error occurs while flushing this stream.
     */
    virtual void flush() {
    }

    /**
     * Equivalent to {@code write(buffer, 0, buffer.length)}.
     */
    virtual void write(const sp<ByteArray>& buffer) {
        write(buffer, 0, buffer->size());
    }

    /**
     * Writes {@code count} bytes from the byte array {@code buffer} starting at
     * position {@code offset} to this stream.
     *
     * @param buffer
     *            the buffer to be written.
     * @param offset
     *            the start position in {@code buffer} from where to get bytes.
     * @param count
     *            the number of bytes from {@code buffer} to write to this
     *            stream.
     * @throws IOException
     *             if an error occurs while writing to this stream.
     * @throws IndexOutOfBoundsException
     *             if {@code offset < 0} or {@code count < 0}, or if
     *             {@code offset + count} is bigger than the length of
     *             {@code buffer}.
     */
    virtual void write(const sp<ByteArray>& buffer, size_t offset, size_t count);

    virtual void write(int32_t b) = 0;

    /**
     * Returns true if this writer has encountered and suppressed an error. Used
     * by PrintStreams as an alternative to checked exceptions.
     */
    virtual bool checkError() {
        return false;
    }
};

} /* namespace mindroid */

#endif /* MINDROID_IO_OUTPUTSTREAM_H_ */
