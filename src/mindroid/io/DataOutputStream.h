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

#ifndef MINDROID_IO_DATAOUTPUTSTREAM_H_
#define MINDROID_IO_DATAOUTPUTSTREAM_H_

#include <mindroid/io/OutputStream.h>
#include <mindroid/lang/String.h>
#include <mindroid/lang/ByteArray.h>

namespace mindroid {

class DataOutputStream : public OutputStream {
public:
    /**
     * Constructs a new {@code DataOutputStream} on the {@code OutputStream}
     * {@code out}. Note that data written by this stream is not in a human
     * readable form but can be reconstructed by using a {@link DataInputStream}
     * on the resulting output.
     *
     * @param outputStream
     *            the target stream for writing.
     */
    DataOutputStream(const sp<OutputStream>& outputStream);

    /**
     * Closes this stream. This implementation closes the target stream.
     *
     * @throws IOException
     *             if an error occurs attempting to close this stream.
     */
    void close() override;

    /**
     * Flushes this stream to ensure all pending data is sent out to the target
     * stream. This implementation then also flushes the target stream.
     *
     * @throws IOException
     *             if an error occurs attempting to flush this stream.
     */
    void flush() override;

    /**
     * Equivalent to {@code write(buffer, 0, buffer.length)}.
     */
    void write(const sp<ByteArray>& buffer) override;

    /**
     * Writes {@code count} bytes from the byte array {@code buffer} starting at
     * {@code offset} to the target stream.
     *
     * @param buffer
     *            the buffer to write to the target stream.
     * @param offset
     *            the index of the first byte in {@code buffer} to write.
     * @param count
     *            the number of bytes from the {@code buffer} to write.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     * @throws NullPointerException
     *             if {@code buffer} is {@code null}.
     */
    void write(const sp<ByteArray>& buffer, size_t offset, size_t count) override;

    /**
     * Writes a byte to the target stream. Only the least significant byte of
     * the integer {@code oneByte} is written.
     *
     * @param b
     *            the byte to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     * @see DataInputStream#readByte()
     */
    void write(int32_t b) override;

    /**
     * Writes a boolean to the target stream.
     *
     * @param value
     *            the boolean value to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     * @see DataInputStream#readBoolean()
     */
    void writeBoolean(bool value);

    /**
     * Writes an 8-bit byte to the target stream. Only the least significant
     * byte of the integer {@code val} is written.
     *
     * @param value
     *            the byte value to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     * @see DataInputStream#readByte()
     * @see DataInputStream#readUnsignedByte()
     */
    void writeByte(uint8_t value);

    void writeBytes(const sp<String>& string);

    void writeChar(char value);

    void writeChars(const sp<String>& string);

    void writeDouble(double value);

    void writeFloat(float value);

    void writeShort(uint16_t value);

    void writeInt(int32_t value);

    void writeLong(uint64_t value);

    void writeUTF(const sp<String>& string);

protected:
    /**
     * The target output stream for this filter stream.
     */
    sp<OutputStream> mOutputStream;

private:
    sp<ByteArray> mScratchpad;
};

} /* namespace mindroid */

#endif /* MINDROID_IO_DATAOUTPUTSTREAM_H_ */
