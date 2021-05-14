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

#ifndef MINDROID_IO_BUFFEREDOUTPUTSTREAM_H_
#define MINDROID_IO_BUFFEREDOUTPUTSTREAM_H_

#include <mindroid/io/OutputStream.h>

namespace mindroid {

class BufferedOutputStream : public OutputStream {
public:
    BufferedOutputStream(const sp<OutputStream>& outputStream, size_t size = 8192);
    virtual ~BufferedOutputStream() {
        close();
    }

    virtual void close() override;
    virtual void flush() override;

    using OutputStream::write;
    virtual void write(const sp<ByteArray>& buffer, size_t offset, size_t count) override;
    virtual void write(int32_t b) override;

private:
    void checkValidity() const;
    void flushBuffer();

    sp<OutputStream> mOutputStream;
    sp<ByteArray> mBuffer;
    size_t mCount = 0;
};

} /* namespace mindroid */

#endif /* MINDROID_IO_BUFFEREDOUTPUTSTREAM_H_ */
