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

#ifndef MINDROID_BYTEBUFFER_H_
#define MINDROID_BYTEBUFFER_H_

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
    size_t arrayOffset() override;

    /**
     * Compacts this buffer  (optional operation).
     */
    virtual sp<ByteBuffer> compact() = 0;

    /**
     * Compares this buffer to another.
     */
    int compareTo(const sp<ByteBuffer>& other) const;

    int compareTo(const ByteBuffer& other) const;

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
     * Relative bulk get method.
     */
    sp<ByteBuffer> get(const sp<ByteArray>& dst);

    /**
     * Relative bulk get method.
     */
    sp<ByteBuffer> get(const sp<ByteArray> &dst, size_t offset, size_t size);

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
     * Relative put method  (optional operation).
     */
    virtual sp<ByteBuffer> put(uint8_t value);

    /**
     * Relative bulk put method  (optional operation).
     */
    sp<ByteBuffer> put(sp<ByteArray>& byteArray);

    /**
     * Relative bulk put method  (optional operation).
     */
    sp<ByteBuffer> put(sp<ByteArray>& byteArray, size_t offset, size_t size);

    /**
     * Relative bulk put method  (optional operation).
     */
    sp<ByteBuffer> put(const sp<ByteBuffer>& byteArray);

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
     * Absolute put method for writing a long value  (optional operation).
     */
    virtual sp<ByteBuffer> putLong(size_t index, int64_t value);

    /**
     * Relative put method for writing a long value  (optional operation).
     */
    virtual sp<ByteBuffer> putLong(int64_t value);

    /**
     * Absolute put method for writing a short value  (optional operation).
     */
    virtual sp<ByteBuffer> putShort(size_t index, int16_t value);

    /**
     * Relative put method for writing a short value  (optional operation).
     */
    virtual sp<ByteBuffer> putShort(int16_t value);

    /**
     * Creates a new byte buffer whose content is a shared subsequence of this buffer's content.
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
    ByteBuffer(const sp<ByteArray>& buffer, size_t offset, size_t size, bool readOnly);
    void checkBufferOverflow(size_t index, size_t amount);

    sp<ByteArray> mBuffer;
};

} /* namespace mindroid */

#endif /* MINDROID_HEAPBYTEBUFFER_H_ */
