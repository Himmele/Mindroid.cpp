/*
 * Copyright (C) 2021 E.S.R.Labs
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

#ifndef MINDROID_IO_BUFFEREDINPUTSTREAM_H_
#define MINDROID_IO_BUFFEREDINPUTSTREAM_H_

#include <mindroid/io/InputStream.h>

namespace mindroid {

class BufferedInputStream : public InputStream {
public:
    BufferedInputStream(const sp<InputStream>& inputStream, size_t size = 8192);
    virtual ~BufferedInputStream() {
        close();
    }

    virtual size_t available() override;
    virtual void close() override;

    using InputStream::read;
    virtual int32_t read() override;
    virtual ssize_t read(const sp<ByteArray>& buffer, size_t offset, size_t count) override;

    virtual void mark(size_t limit) override;
    virtual bool markSupported() override;

    /**
     * Resets this stream to the last marked location.
     *
     * @throws IOException
     *             if this stream is closed, no mark has been set or the mark is
     *             no longer valid because more than {@code readlimit} bytes
     *             have been read since setting the mark.
     * @see #mark(int)
     */
    virtual void reset() override;

    /**
     * Skips {@code byteCount} bytes in this stream. Subsequent calls to
     * {@code read} will not return these bytes unless {@code reset} is
     * used.
     *
     * @param byteCount
     *            the number of bytes to skip. {@code skip} does nothing and
     *            returns 0 if {@code byteCount} is less than zero.
     * @return the number of bytes actually skipped.
     * @throws IOException
     *             if this stream is closed or another IOException occurs.
     */
    virtual size_t skip(size_t count) override;

private:
    ssize_t fillBuffer(const sp<InputStream>& inputStream, sp<ByteArray>& buffer);
    void failWithIoException() const;

    sp<InputStream> mInputStream;
    sp<ByteArray> mBuffer;
    size_t mCount = 0;
    size_t mMarkLimit = 0;
    ssize_t mMarkPosition = -1;
    size_t mPosition = 0;
};

} /* namespace mindroid */

#endif /* MINDROID_IO_BUFFEREDINPUTSTREAM_H_ */
