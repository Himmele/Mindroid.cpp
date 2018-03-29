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

#ifndef MINDROID_LANG_STRINGBUILDER_H_
#define MINDROID_LANG_STRINGBUILDER_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/lang/ByteArray.h>
#include <string>

namespace mindroid {

class StringBuilder : public Object {
public:
    StringBuilder() = default;
    StringBuilder(const char* string);
    StringBuilder(const sp<String>& string);

    /**
     * Appends the string representation of the boolean argument to the sequence.
     */
    sp<StringBuilder> append(bool value);

    /**
     * Appends the string representation of the char argument to this sequence.
     */
    sp<StringBuilder> append(char value);

    /**
     * Appends the string representation of the byte array argument to this sequence.
     */
    sp<StringBuilder> append(const sp<ByteArray>& charSequence);

    /**
     * Appends the C-style string given.
     */
    sp<StringBuilder> append(const char* string);

    /**
     * Appends the string from the given buffer and size.
     */

    sp<StringBuilder> append(const char* string, size_t size);

    /**
     * Appends the string representation of a subarray of the char array argument to this sequence.
     */
    sp<StringBuilder> append(const sp<ByteArray>& charSequence, size_t offset, size_t size);

    /**
     * Appends the string representation of the int8_t argument to this sequence.
     */
    sp<StringBuilder> append(int8_t value);

    /**
     * Appends the string representation of the uint8_t argument to this sequence.
     */
    sp<StringBuilder> append(uint8_t value);

    /**
     * Appends the string representation of the int16_t argument to this sequence.
     */
    sp<StringBuilder> append(int16_t value);

    /**
     * Appends the string representation of the uint16_t argument to this sequence.
     */
    sp<StringBuilder> append(uint16_t value);

    /**
     * Appends the string representation of the int32_t argument to this sequence.
     */
    sp<StringBuilder> append(int32_t value);

    /**
     * Appends the string representation of the uint32_t argument to this sequence.
     */
    sp<StringBuilder> append(uint32_t value);

    /**
     * Appends the string representation of the int64_t argument to this sequence.
     */
    sp<StringBuilder> append(int64_t value);

    /**
     * Appends the string representation of the uint64_t argument to this sequence.
     */
    sp<StringBuilder> append(uint64_t value);

    /**
     * Appends the string representation of the float argument to this sequence.
     */
    sp<StringBuilder> append(float value);

    /**
     * Appends the string representation of the double argument to this sequence.
     */
    sp<StringBuilder> append(double value);

    /**
     * Appends the specified string to this character sequence.
     */
    sp<StringBuilder> append(const sp<String>& string);

    /**
     * Appends the string representation of the codePoint argument to this sequence.
     */
    sp<StringBuilder> appendCodePoint(int32_t codePoint);

    /**
     * Returns the current capacity.
     */
    size_t capacity() const;

    /**
     * Returns the char value in this sequence at the specified index.
     */
    char charAt(size_t index) const;

    /**
     * Removes the characters in a substring of this sequence.
     */
    sp<StringBuilder> deleteRange(size_t start, size_t end);

    /**
     * Removes the char at the specified position in this sequence.
     */
    sp<StringBuilder> deleteCharAt(size_t index);

    /**
     * Ensures that the capacity is at least equal to the specified minimum.
     */
    void ensureCapacity(size_t minimumCapacity);

    /**
     * Characters are copied from this sequence into the destination character array dst.
     */
    void getChars(size_t srcBegin, size_t srcEnd, const sp<ByteArray>& dst, size_t dstBegin) const;

    /**
     * Returns the index within this string of the first occurrence of the specified substring.
     */
    ssize_t indexOf(const sp<String>& string) const;

    /**
     * Returns the index within this string of the first occurrence of the specified substring, starting at the specified index.
     */
    ssize_t indexOf(const sp<String>& string, size_t fromIndex) const;

    /**
     * Inserts the string representation of the boolean argument into this sequence.
     */
    sp<StringBuilder> insert(size_t offset, bool value);

    /**
     * Inserts the string representation of the char argument into this sequence.
     */
    sp<StringBuilder> insert(size_t offset, char value);

    /**
     * Inserts the string representation of the char array argument into this sequence.
     */
    sp<StringBuilder> insert(size_t offset, const sp<ByteArray>& charSequence);

    /**
     * Inserts the string representation of a subarray of the str array argument into this sequence.
     */
    sp<StringBuilder> insert(size_t index, const sp<ByteArray>& charSequence, size_t offset, size_t size);

    /**
     * Inserts the string representation of the double argument into this sequence.
     */
    sp<StringBuilder> insert(size_t offset, double value);

    /**
     * Inserts the string representation of the float argument into this sequence.
     */
    sp<StringBuilder> insert(size_t offset, float value);

    /**
     * Inserts the string representation of the int32_t argument into this sequence.
     */
    sp<StringBuilder> insert(size_t offset, int32_t value);

    /**
     * Inserts the string representation of the uint32_t argument into this sequence.
     */
    sp<StringBuilder> insert(size_t offset, uint32_t value);

    /**
     * Inserts the string representation of the long argument into this sequence.
     */
    sp<StringBuilder> insert(size_t offset, int64_t value);

    /**
     * Inserts the string representation of the uint64_t argument into this sequence.
     */
    sp<StringBuilder> insert(size_t offset, uint64_t value);

    /**
     * Inserts the string into this character sequence.
     */
    sp<StringBuilder> insert(size_t offset, const sp<String>& string);

    /**
     * Returns the index within this string of the rightmost occurrence of the specified substring.
     */
    ssize_t lastIndexOf(const sp<String>& string) const;

    /**
     * Returns the index within this string of the last occurrence of the specified substring.
     */
    ssize_t lastIndexOf(const sp<String>& string, size_t fromIndex) const;

    /**
     * Returns the length (character count).
     */
    size_t length() const;

    /**
     * Replaces the characters in a substring of this sequence with characters in the specified String.
     */
    sp<StringBuilder> replace(size_t start, size_t end, const sp<String>& str);

    /**
     * Causes this character sequence to be replaced by the reverse of the sequence.
     */
    sp<StringBuilder> reverse();

    /**
     * The character at the specified index is set to ch.
     */
    void setCharAt(size_t index, char c);

    /**
     * Sets the length of the character sequence.
     */
    void setLength(size_t newLength);

    /**
     * Returns a new String that contains a subsequence of characters currently contained in this character sequence.
     */
    sp<String> substring(size_t start) const;

    /**
     * Returns a new String that contains a subsequence of characters currently contained in this sequence.
     */
    sp<String> substring(size_t start, size_t end) const;

    /**
     * Returns a string representing the data in this sequence.
     */
    sp<String> toString() const;

    /**
     * Attempts to reduce storage used for the character sequence.
     */
    void trimToSize();

private:
    StringBuilder(const std::string&& otherString);

    // Crafting the string uses the mutable std::string class.
    std::string mString;
};

} /* namespace mindroid */

#endif /* MINDROID_LANG_STRINGBUILDER_H_ */
