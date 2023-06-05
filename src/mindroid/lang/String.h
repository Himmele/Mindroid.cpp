/*
 * Copyright (C) 2012 Daniel Himmelein
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

#ifndef MINDROID_LANG_STRING_H_
#define MINDROID_LANG_STRING_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/ByteArray.h>
#include <mindroid/lang/StringArray.h>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cctype>
#include <string>

namespace mindroid {

class String :
        public Object {
public:
    static const sp<String> EMPTY_STRING;

    String();
    explicit String(const char* string);
    explicit String(const char* string, size_t size);
    explicit String(const uint8_t* string, size_t size) : String(reinterpret_cast<const char*>(string), size) { }
    explicit String(const char c);
    explicit String(const sp<String>& string);
    explicit String(const sp<ByteArray>& byteArray);
    virtual ~String() { }

    bool equals(const sp<Object>& other) const override;
    size_t hashCode() const override;

    bool equals(const char* string) const;
    bool equals(const sp<String>& string) const;
    bool equalsIgnoreCase(const char* string) const;
    bool equalsIgnoreCase(const sp<String>& string) const;

    inline size_t length() const {
        return mStringBuffer->mSize;
    }

    inline bool isEmpty() const {
        return length() == 0;
    }

    inline const char* c_str() const {
        return mStringBuffer->mData;
    }

    inline operator const char*() const {
        return c_str();
    }

    char operator[](const size_t index) const;
    char charAt(size_t index) const;

    bool matches(const char* regex) const;
    bool matches(const sp<String>& regex) const;

    bool contains(const char* subString) const;
    bool contains(const sp<String>& subString) const;

    /**
     * @brief Checks if the string starts with the prefix
     *
     * @param prefix null terminated string
     * @return true when the string starts with the prefix
     * @return false when the string does not start with the prefix
     */
    bool startsWith(const char* prefix) const;
    bool startsWith(const sp<String>& prefix) const;

    /**
     * @brief Checks if the string ends with the suffix
     *
     * @param suffix null terminated string
     * @return true when the string ends with the suffix
     * @return false when the string does not end with the suffix
     */
    bool endsWith(const char* suffix) const;
    bool endsWith(const sp<String>& suffix) const;

    sp<String> substring(size_t beginIndex) const;
    sp<String> substring(size_t beginIndex, size_t endIndex) const;

    sp<String> toLowerCase() const;
    sp<String> toUpperCase() const;

    ssize_t indexOf(const char c) const;
    ssize_t indexOf(const char* string) const;
    ssize_t indexOf(const sp<String>& string) const;
    ssize_t indexOf(const char c, size_t fromIndex) const;
    ssize_t indexOf(const char* string, size_t fromIndex) const;
    ssize_t indexOf(const sp<String>& string, size_t fromIndex) const;

    ssize_t lastIndexOf(const char c) const;
    ssize_t lastIndexOf(const char* string) const;
    ssize_t lastIndexOf(const sp<String>& string) const;
    ssize_t lastIndexOf(const char c, size_t fromIndex) const;
    ssize_t lastIndexOf(const char* string, size_t fromIndex) const;
    ssize_t lastIndexOf(const sp<String>& string, size_t fromIndex) const;

    sp<String> trim() const;

    /**
     * @brief Splits the string at the separator
     *
     * @param separator null terminated string
     * @return sp<StringArray> array of strings split at separator, omitting the separator
     */
    sp<StringArray> split(const char* separator) const;
    sp<StringArray> split(const sp<String>& separator) const;

    inline static sp<String> valueOf(const char* string) {
        return (string != nullptr) ? new String(string) : nullptr;
    }

    inline static sp<String> valueOf(const char* string, size_t size) {
        if (string != nullptr) {
            return (size > 0) ? new String(string, size) : EMPTY_STRING;
        } else {
            return nullptr;
        }
    }

    inline static sp<String> valueOf(bool b) {
        return new String(b ? "true" : "false");
    }

    inline static sp<String> valueOf(char c) {
        return new String(c);
    }

    static sp<String> valueOf(int8_t value);
    static sp<String> valueOf(uint8_t value);
    static sp<String> valueOf(int16_t value);
    static sp<String> valueOf(uint16_t value);
    static sp<String> valueOf(int32_t value);
    static sp<String> valueOf(uint32_t value);
    static sp<String> valueOf(float value);
    static sp<String> valueOf(double value);

    /**
     * @brief Calculates the string length of the input
     *
     * @param string for which to get the length, null terminated
     * @return size_t length of the string
     */
    static size_t length(const char* string) {
        return strlen(string);
    }

    /**
     * Compares the specified string to this string and compares the specified
     * range of characters to determine if they are the same.
     *
     * @param toffset
     *            the starting offset in this string.
     * @param string
     *            the string to compare.
     * @param offset
     *            the starting offset in the specified string.
     * @param len
     *            the number of characters to compare.
     * @return {@code true} if the ranges of characters are equal, {@code false}
     *         otherwise
     * @throws NullPointerException
     *             if {@code string} is {@code null}.
     */
    bool regionMatches(size_t toffset, const sp<String>& string, size_t offset, size_t len);
    bool regionMatches(size_t toffset, const char* string, size_t offset, size_t len);

    sp<String> replace(char oldChar, char newChar);

    sp<String> append(const char c) const {
        return append(&c, 1);
    }

    /**
     * @brief Append a string to the current string
     *
     * @param string to append, null terminated
     * @return sp<String> the new string
     */
    sp<String> append(const char* string) const {
        return append(string, strlen(string));
    }

    sp<String> append(const sp<String>& string) const {
        return append(string->c_str(), strlen(string->c_str()));
    }

    sp<String> append(const char* string, size_t size) const;
    sp<String> append(const char* string, size_t offset, size_t size) const;
    sp<String> append(const sp<String>& string, size_t offset, size_t size) const;
    sp<String> appendFormatted(const char* format, ...) const __attribute__((format (printf, 2, 3)));

    sp<ByteArray> getBytes() const {
        return new ByteArray(reinterpret_cast<uint8_t*>(mStringBuffer->mData), mStringBuffer->mSize);
    }

    static sp<String> format(const char* string) {
        return new String(string);
    }

    static sp<String> format(const sp<String>& string) {
        return new String(string);
    }

    template<typename... Args>
    static sp<String> format(const char* format, Args... args) {
        return EMPTY_STRING->appendFormatted(format, toValue(args)...);
    }

    template<typename... Args>
    static sp<String> format(const sp<String>& format, Args... args) {
        return EMPTY_STRING->appendFormatted(format->c_str(), toValue(args)...);
    }

private:
    class StringBuffer : public LightweightObject<StringBuffer> {
    public:
        StringBuffer() :
                mData(nullptr),
                mSize(0) {
        }

        StringBuffer(size_t size);
        StringBuffer(const char* string, size_t size);
        StringBuffer(const char* string1, size_t size1, const char* string2, size_t size2);

        ~StringBuffer() {
            if (mData != nullptr) {
                free(mData);
            }
        }

    private:
        char* mData;
        size_t mSize;

        friend class String;
        friend class StringBuilder;
    };

    String(const sp<StringBuffer>& string) :
            mStringBuffer(string) {
    }

    sp<String> appendFormattedWithVarArgList(const char* format, va_list args) const;

    template<typename T, typename... Rest>
    struct is_any : std::false_type {};

    template<typename T, typename First>
    struct is_any<T, First> : std::is_same<T, First> {};

    template<typename T, typename First, typename... Rest>
    struct is_any<T, First, Rest...> : std::integral_constant<bool, std::is_same<T, First>::value || is_any<T, Rest...>::value> {};

    template<typename T, typename std::enable_if<!is_any<T, sp<String>, std::string>::value>::type* = nullptr> static T toValue(const T& value) { return value; }
    template<typename T, typename std::enable_if<std::is_same<T, sp<String>>::value>::type* = nullptr> static const char* toValue(const T& value) { return (value != nullptr && value->c_str() != nullptr) ? value->c_str() : "nullptr"; }
    template<typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr> static const char* toValue(const T& value) { return (value.c_str() != nullptr) ? value.c_str() : "nullptr"; }

    sp<StringBuffer> mStringBuffer;

    static const sp<StringBuffer> NULL_STRING_BUFFER;
    static const sp<StringBuffer> EMPTY_STRING_BUFFER;

    friend class StringBuilder;
};

} /* namespace mindroid */

#endif /* MINDROID_LANG_STRING_H_ */
