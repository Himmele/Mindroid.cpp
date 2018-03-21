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

#ifndef MINDROID_BYTEARRAY_H_
#define MINDROID_BYTEARRAY_H_

#include <mindroid/lang/Object.h>
#include <vector>

namespace mindroid {

class String;

class ByteArray : public Object {
public:
    ByteArray(size_t size);

    ByteArray(const sp<ByteArray>& other);

    ByteArray(const std::vector<uint8_t>& other);

    ByteArray(const uint8_t* buffer, size_t size);

    /**
     * Creates a ByteArray using an initializer list.
     */
    static sp<ByteArray> valueOf(const std::initializer_list<uint8_t>& list);

    /**
     * Returns the array size.
     */
    size_t size() const;

    /**
     * Returns the array as a const C array.
     */
    const uint8_t* c_arr() const;

    /**
     * Returns the array as a C array.
     */
    uint8_t* c_arr();

    sp<String> toString() const;

    /**
     * Duplicates this array.
     */
    sp<ByteArray> clone() const;

    /**
     * Returns the const element in the given position.
     *
     * Asserts IndexOutOfBoundsException if position is out of bounds.
     */
    const uint8_t& get(size_t position) const;
    const uint8_t& operator[](size_t position) const;

    /**
     * Returns the element in the given position.
     *
     * Asserts IndexOutOfBoundsException if position is out of bounds.
     */
    uint8_t& get(size_t position);
    void set(size_t position, uint8_t value);
    uint8_t& operator[](size_t position);

    /**
     * Compares this array against another array.
     */
    bool operator==(const ByteArray& other) const;

    /**
     * Compares this array against another array.
     */
    bool operator==(const sp<ByteArray>& other) const;

    bool equals(const sp<Object>& object) const override;

private:
    std::vector<uint8_t> mBuffer;
};

} /* namespace mindroid */

#endif /* MINDROID_BYTEARRAY_H_ */
