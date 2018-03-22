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

#ifndef MINDROID_INPUTSTREAM_H_
#define MINDROID_INPUTSTREAM_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/lang/ByteArray.h>

namespace mindroid {

/**
 * A readable source of bytes.
 *
 * <p>Most clients will use input streams that read data from the file system
 * ({@link FileInputStream}), the network ({@link java.net.Socket#getInputStream()}/{@link
 * java.net.HttpURLConnection#getInputStream()}), or from an in-memory byte
 * array ({@link ByteArrayInputStream}).
 *
 * <p>Use {@link InputStreamReader} to adapt a byte stream like this one into a
 * character stream.
 *
 * <p>Most clients should wrap their input stream with {@link
 * BufferedInputStream}. Callers that do only bulk reads may omit buffering.
 *
 * <p>Some implementations support marking a position in the input stream and
 * resetting back to this position later. Implementations that don't return
 * false from {@link #markSupported()} and throw an {@link IOException} when
 * {@link #reset()} is called.
 *
 * <h3>Subclassing InputStream</h3>
 * Subclasses that decorate another input stream should consider subclassing
 * {@link FilterInputStream}, which delegates all calls to the source input
 * stream.
 *
 * <p>All input stream subclasses should override <strong>both</strong> {@link
 * #read() read()} and {@link #read(byte[],int,int) read(byte[],int,int)}. The
 * three argument overload is necessary for bulk access to the data. This is
 * much more efficient than byte-by-byte access.
 *
 * @see OutputStream
 */
class InputStream : public Object {
public:
    /**
     * This constructor does nothing. It is provided for signature
     * compatibility.
     */
    InputStream() {
    }

    virtual ~InputStream() noexcept = default;

    /**
     * Returns an estimated number of bytes that can be read or skipped without blocking for more
     * input.
     *
     * <p>Note that this method provides such a weak guarantee that it is not very useful in
     * practice.
     *
     * <p>Firstly, the guarantee is "without blocking for more input" rather than "without
     * blocking": a read may still block waiting for I/O to complete&nbsp;&mdash; the guarantee is
     * merely that it won't have to wait indefinitely for data to be written. The result of this
     * method should not be used as a license to do I/O on a thread that shouldn't be blocked.
     *
     * <p>Secondly, the result is a
     * conservative estimate and may be significantly smaller than the actual number of bytes
     * available. In particular, an implementation that always returns 0 would be correct.
     * In general, callers should only use this method if they'd be satisfied with
     * treating the result as a boolean yes or no answer to the question "is there definitely
     * data ready?".
     *
     * <p>Thirdly, the fact that a given number of bytes is "available" does not guarantee that a
     * read or skip will actually read or skip that many bytes: they may read or skip fewer.
     *
     * <p>It is particularly important to realize that you <i>must not</i> use this method to
     * size a container and assume that you can read the entirety of the stream without needing
     * to resize the container. Such callers should probably write everything they read to a
     * {@link ByteArrayOutputStream} and convert that to a byte array. Alternatively, if you're
     * reading from a file, {@link File#length} returns the current length of the file (though
     * assuming the file's length can't change may be incorrect, reading a file is inherently
     * racy).
     *
     * <p>The default implementation of this method in {@code InputStream} always returns 0.
     * Subclasses should override this method if they are able to indicate the number of bytes
     * available.
     *
     * @return the estimated number of bytes available
     * @throws IOException if this stream is closed or an error occurs
     */
    virtual size_t available() {
        return 0;
    }

    /**
     * Closes this stream. Concrete implementations of this class should free
     * any resources during close. This implementation does nothing.
     *
     * @throws IOException
     *             if an error occurs while closing this stream.
     */
    virtual void close() {
    }

    /**
     * Sets a mark position in this InputStream. The parameter {@code readlimit}
     * indicates how many bytes can be read before the mark is invalidated.
     * Sending {@code reset()} will reposition the stream back to the marked
     * position provided {@code readLimit} has not been surpassed.
     * <p>
     * This default implementation does nothing and concrete subclasses must
     * provide their own implementation.
     *
     * @param readlimit
     *            the number of bytes that can be read from this stream before
     *            the mark is invalidated.
     * @see #markSupported()
     * @see #reset()
     */
    virtual void mark(size_t limit) {
    }

    /**
     * Indicates whether this stream supports the {@code mark()} and
     * {@code reset()} methods. The default implementation returns {@code false}.
     *
     * @return always {@code false}.
     * @see #mark(int)
     * @see #reset()
     */
    virtual bool markSupported() {
        return false;
    }

    /**
     * Reads a single byte from this stream and returns it as an integer in the
     * range from 0 to 255. Returns -1 if the end of the stream has been
     * reached. Blocks until one byte has been read, the end of the source
     * stream is detected or an exception is thrown.
     *
     * @throws IOException
     *             if the stream is closed or another IOException occurs.
     */
    virtual int32_t read() = 0;

    /**
     * Equivalent to {@code read(buffer, 0, buffer.length)}.
     */
    virtual ssize_t read(const sp<ByteArray>& buffer) {
        return read(buffer, 0, buffer->size());
    }

    /**
     * Reads up to {@code count} bytes from this stream and stores them in
     * the byte array {@code buffer} starting at {@code byteOffset}.
     * Returns the number of bytes actually read or -1 if the end of the stream
     * has been reached.
     *
     * @throws IndexOutOfBoundsException
     *   if {@code byteOffset < 0 || byteCount < 0 || byteOffset + byteCount > buffer.length}.
     * @throws IOException
     *             if the stream is closed or another IOException occurs.
     */
    virtual ssize_t read(const sp<ByteArray>& buffer, size_t offset, size_t count);

    /**
     * Resets this stream to the last marked location. Throws an
     * {@code IOException} if the number of bytes read since the mark has been
     * set is greater than the limit provided to {@code mark}, or if no mark
     * has been set.
     * <p>
     * This implementation always throws an {@code IOException} and concrete
     * subclasses should provide the proper implementation.
     *
     * @throws IOException
     *             if this stream is closed or another IOException occurs.
     */
    virtual void reset();

    /**
     * Skips at most {@code byteCount} bytes in this stream. The number of actual
     * bytes skipped may be anywhere between 0 and {@code byteCount}. If
     * {@code byteCount} is negative, this method does nothing and returns 0, but
     * some subclasses may throw.
     *
     * <p>Note the "at most" in the description of this method: this method may
     * choose to skip fewer bytes than requested. Callers should <i>always</i>
     * check the return value.
     *
     * <p>This default implementation reads bytes into a temporary buffer. Concrete
     * subclasses should provide their own implementation.
     *
     * @return the number of bytes actually skipped.
     * @throws IOException if this stream is closed or another IOException
     *             occurs.
     */
    virtual size_t skip(size_t count);
};

} /* namespace mindroid */

#endif /* MINDROID_INPUTSTREAM_H_ */
