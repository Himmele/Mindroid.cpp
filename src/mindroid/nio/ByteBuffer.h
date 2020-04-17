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

#ifndef MINDROID_NIO_BYTEBUFFER_H_
#define MINDROID_NIO_BYTEBUFFER_H_

#include <mindroid/lang/ByteArray.h>
#include <mindroid/nio/Buffer.h>

namespace mindroid {

class ByteBuffer : public Buffer {
public:
    virtual ~ByteBuffer() = default;

    /**
     * Allocates a new byte buffer.
     */
    static sp<ByteBuffer> allocate(size_t capacity);

    /**
     * Returns the byte array that backs this buffer  (optional operation).
     */
    sp<ByteArray> array() const override = 0;

    /**
     * Returns the offset within this buffer's backing array of the first element of the buffer  (optional operation).
     */
    size_t arrayOffset() const override;

    /**
     * Compacts this byte buffer.
     * <p>
     * The remaining bytes will be moved to the head of the
     * buffer, starting from position zero. Then the position is set to
     * {@code remaining()}; the limit is set to capacity; the mark is
     * cleared.
     *
     * @return {@code this}
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual sp<ByteBuffer> compact() = 0;

    /**
     * Compares the remaining bytes of this buffer to another byte buffer's
     * remaining bytes.
     *
     * @param other
     *            another byte buffer.
     * @return a negative value if this is less than {@code other}; 0 if this
     *         equals to {@code other}; a positive value if this is greater
     *         than {@code other}.
     * @exception ClassCastException
     *                if {@code other} is not a byte buffer.
     */
    int32_t compareTo(const sp<ByteBuffer>& other) const;

    int32_t compareTo(const ByteBuffer& other) const;

    /**
     * Creates a new byte buffer that shares this buffer's content.
     */
    virtual sp<ByteBuffer> duplicate() = 0;

    /**
     * Tells whether or not this buffer is equal to another object.
     */
    bool equals(const sp<Object>& other) const override;

    bool operator==(const ByteBuffer& other) const;
    bool operator!=(const ByteBuffer& other) const;

    /**
     * Relative get method.
     */
    virtual uint8_t get();

    /**
     * Absolute get method.
     */
    virtual uint8_t get(size_t index);

    /**
     * Relative get method for reading a char value.
     */
    virtual char getChar();

    /**
     * Absolute get method for reading a char value.
     */
    virtual char getChar(size_t index);

    /**
     * Relative get method for reading a double value.
     */
    virtual double getDouble();

    /**
     * Absolute get method for reading a double value.
     */
    virtual double getDouble(size_t index);

    /**
     * Relative get method for reading a float value.
     */
    virtual float getFloat();

    /**
     * Absolute get method for reading a float value.
     */
    virtual float getFloat(size_t index);

    /**
     * Relative get method for reading an int value.
     */
    virtual int32_t getInt();

    /**
     * Absolute get method for reading an int value.
     */
    virtual int32_t getInt(size_t index);

    /**
     * Relative get method for reading a long value.
     */
    virtual int64_t getLong();

    /**
     * Absolute get method for reading a long value.
     */
    virtual int64_t getLong(size_t index);

    /**
     * Relative get method for reading a short value.
     */
    virtual int16_t getShort();

    /**
     * Absolute get method for reading a short value.
     */
    virtual int16_t getShort(size_t index);

    /**
     * Relative bulk get method.
     */
    sp<ByteBuffer> get(const sp<ByteArray>& dst);

    /**
     * Relative bulk get method.
     */
    sp<ByteBuffer> get(const sp<ByteArray>& dst, size_t offset, size_t count);

    /**
     * Relative put method  (optional operation).
     */
    virtual sp<ByteBuffer> put(uint8_t value);

    /**
     * Absolute put method  (optional operation).
     */
    virtual sp<ByteBuffer> put(size_t index, uint8_t value);

    /**
     * Relative put method for writing a char value  (optional operation).
     */
    virtual sp<ByteBuffer> putChar(char value);

    /**
     * Absolute put method for writing a char value  (optional operation).
     */
    virtual sp<ByteBuffer> putChar(size_t index, char value);

    /**
     * Relative put method for writing a double value  (optional operation).
     */
    virtual sp<ByteBuffer> putDouble(double value);

    /**
     * Absolute put method for writing a double value  (optional operation).
     */
    virtual sp<ByteBuffer> putDouble(size_t index, double value);

    /**
     * Relative put method for writing a float value  (optional operation).
     */
    virtual sp<ByteBuffer> putFloat(float value);

    /**
     * Absolute put method for writing a float value  (optional operation).
     */
    virtual sp<ByteBuffer> putFloat(size_t index, float value);

    /**
     * Relative put method for writing an int value  (optional operation).
     */
    virtual sp<ByteBuffer> putInt(int32_t value);

    /**
     * Absolute put method for writing an int value  (optional operation).
     */
    virtual sp<ByteBuffer> putInt(size_t index, int32_t value);

    /**
     * Relative put method for writing a long value  (optional operation).
     */
    virtual sp<ByteBuffer> putLong(int64_t value);

    /**
     * Absolute put method for writing a long value  (optional operation).
     */
    virtual sp<ByteBuffer> putLong(size_t index, int64_t value);

    /**
     * Relative put method for writing a short value  (optional operation).
     */
    virtual sp<ByteBuffer> putShort(int16_t value);

    /**
     * Absolute put method for writing a short value  (optional operation).
     */
    virtual sp<ByteBuffer> putShort(size_t index, int16_t value);

    /**
     * Relative bulk put method  (optional operation).
     */
    sp<ByteBuffer> put(const sp<ByteArray>& byteArray);

    /**
     * Relative bulk put method  (optional operation).
     */
    sp<ByteBuffer> put(const sp<ByteArray>& byteArray, size_t offset, size_t size);

    /**
     * Returns a sliced buffer that shares its content with this buffer.
     * <p>
     * The sliced buffer's capacity will be this buffer's
     * {@code remaining()}, and it's zero position will correspond to
     * this buffer's current position. The new buffer's position will be 0,
     * limit will be its capacity, and its mark is cleared. The new buffer's
     * read-only property and byte order are the same as this buffer's.
     * <p>
     * The new buffer's position will be zero, its capacity and
     * its limit will be the number of bytes remaining in this buffer,
     * and its mark will be undefined.
     * The new buffer will be direct if, and only if, this buffer is direct,
     * and it will be read-only if, and only if, this buffer is read-only.
     *
     * @return a sliced buffer that shares its content with this buffer.
     */
    virtual sp<ByteBuffer> slice() = 0;

    /**
     * Returns a string summarizing the state of this buffer.
     */
    sp<String> toString() const;

    /**
     * Wraps a byte array into a buffer.
     */
    static sp<ByteBuffer> wrap(const sp<ByteArray>& array);

    /**
     * Wraps a byte array into a buffer.
     */
    static sp<ByteBuffer> wrap(const sp<ByteArray>& array, size_t offset, size_t size);

protected:
    ByteBuffer(const sp<ByteArray>& buffer, bool readOnly);
    ByteBuffer(const sp<ByteArray>& buffer, size_t position, size_t limit, size_t capacity, bool readOnly, size_t offset);
    void checkBufferOverflow(size_t index, size_t amount);
    void checkBufferUnderflow(size_t index, size_t amount);

    sp<ByteArray> mBuffer;
    const size_t mOffset;
};

} /* namespace mindroid */

#endif /* MINDROID_NIO_BYTEBUFFER_H_ */
