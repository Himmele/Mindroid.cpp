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

#include <mindroid/lang/StringBuilder.h>
#include <mindroid/lang/IndexOutOfBoundsException.h>
#include <mindroid/util/Assert.h>
#include <algorithm>
#include <sstream>

namespace mindroid {

StringBuilder::StringBuilder(const std::string&& string) : mString(string) {
}

StringBuilder::StringBuilder(const char* string) : mString(string) {
}

StringBuilder::StringBuilder(const sp<String>& string) : mString(string->c_str()) {
}

sp<StringBuilder> StringBuilder::append(bool value) {
    return append(value ? "true" : "false");
}

sp<StringBuilder> StringBuilder::append(char value) {
    mString += value;
    return this;
}

sp<StringBuilder> StringBuilder::append(const sp<ByteArray>& charSequence) {
    mString.append(reinterpret_cast<const char*>(charSequence->c_arr()), charSequence->size());
    return this;
}

sp<StringBuilder> StringBuilder::append(const char* string, size_t size) {
    mString.append(string, size);
    return this;
}

sp<StringBuilder> StringBuilder::append(const sp<ByteArray>& charSequence, size_t offset, size_t size) {
    size = std::min(charSequence->size() - offset, size);
    mString.append(reinterpret_cast<const char*>(charSequence->c_arr()) + offset, size);
    return this;
}

const std::string toString(const bool& value) {
   return value ? "true" : "false";
}

sp<StringBuilder> StringBuilder::append(double value) {
    mString.append(toString(value));
    return this;
}

sp<StringBuilder> StringBuilder::append(float value) {
    mString.append(toString(value));
    return this;
}

sp<StringBuilder> StringBuilder::append(int8_t value) {
    mString.append(toString(value));
    return this;
}

sp<StringBuilder> StringBuilder::append(uint8_t value) {
    mString.append(toString(value));
    return this;
}

sp<StringBuilder> StringBuilder::append(int16_t value) {
    mString.append(toString(value));
    return this;
}

sp<StringBuilder> StringBuilder::append(uint16_t value) {
    mString.append(toString(value));
    return this;
}

sp<StringBuilder> StringBuilder::append(int32_t value) {
    mString.append(toString(value));
    return this;
}

sp<StringBuilder> StringBuilder::append(uint32_t value) {
    mString.append(toString(value));
    return this;
}

sp<StringBuilder> StringBuilder::append(int64_t value) {
    mString.append(toString(value));
    return this;
}

sp<StringBuilder> StringBuilder::append(uint64_t value) {
    mString.append(toString(value));
    return this;
}

sp<StringBuilder> StringBuilder::append(const sp<String>& string) {
    mString += string->c_str();
    return this;
}

sp<StringBuilder> StringBuilder::appendCodePoint(int32_t codePoint) {
    Assert::assertTrue<RuntimeException>("Invalid non-ASCII code point", codePoint < 256);
    mString += static_cast<char>(codePoint);
    return this;
}

size_t StringBuilder::capacity() const {
    return mString.capacity();
}

char StringBuilder::charAt(size_t index) const {
    Assert::assertFalse<IndexOutOfBoundsException>(index >= mString.length());
    return mString[index];
}

sp<StringBuilder> StringBuilder::deleteRange(size_t start, size_t end) {
    mString.erase(start, end - start);
    return this;
}

sp<StringBuilder> StringBuilder::deleteCharAt(size_t index) {
    mString.erase(index, 1);
    return this;
}

void StringBuilder::ensureCapacity(size_t minimumCapacity) {
    mString.reserve(minimumCapacity);
}

void StringBuilder::getChars(size_t srcBegin, size_t srcEnd, const sp<ByteArray>& dst, size_t dstBegin) const {
    size_t srcSize = std::min(srcEnd - srcBegin, mString.size() - srcBegin);
    size_t dstSize = std::min(srcSize, dst->size() - dstBegin);
    std::copy(mString.data() + srcBegin, mString.data() + srcBegin + dstSize, dst->c_arr() + dstBegin);
}

ssize_t StringBuilder::indexOf(const sp<String>& string) const {
    return mString.find_first_of(string->c_str());
}

ssize_t StringBuilder::indexOf(const sp<String>& string, size_t fromIndex) const {
    return mString.find_first_of(string->c_str(), fromIndex);
}

sp<StringBuilder> StringBuilder::insert(size_t offset, bool value) {
    mString.insert(offset, value ? "true" : "false");
    return this;
}

sp<StringBuilder> StringBuilder::insert(size_t offset, char value) {
    mString.insert(offset, 1, value);
    return this;
}

sp<StringBuilder> StringBuilder::insert(size_t offset, const sp<ByteArray>& charSequence) {
    mString.insert(offset, reinterpret_cast<const char*>(charSequence->c_arr()), charSequence->size());
    return this;
}

sp<StringBuilder> StringBuilder::insert(size_t index, const sp<ByteArray>& charSequence, size_t offset, size_t size) {
    mString.insert(offset, reinterpret_cast<const char*>(charSequence->c_arr()) + offset, std::min(size, charSequence->size() - offset));
    return this;
}

sp<StringBuilder> StringBuilder::insert(size_t offset, double value) {
    mString.insert(offset, toString(value));
    return this;
}

sp<StringBuilder> StringBuilder::insert(size_t offset, float value) {
    mString.insert(offset, toString(value));
    return this;
}

sp<StringBuilder> StringBuilder::insert(size_t offset, int32_t value) {
    mString.insert(offset, toString(value));
    return this;
}

sp<StringBuilder> StringBuilder::insert(size_t offset, uint32_t value) {
    mString.insert(offset, toString(value));
    return this;
}

sp<StringBuilder> StringBuilder::insert(size_t offset, int64_t value) {
    mString.insert(offset, toString(value));
    return this;
}

sp<StringBuilder> StringBuilder::insert(size_t offset, uint64_t value) {
    mString.insert(offset, toString(value));
    return this;
}

sp<StringBuilder> StringBuilder::insert(size_t offset, const sp<String>& string) {
    mString.insert(offset, string->c_str());
    return this;
}

ssize_t StringBuilder::lastIndexOf(const sp<String>& string) const {
    return mString.find_last_of(string->c_str());
}

ssize_t StringBuilder::lastIndexOf(const sp<String>& string, size_t fromIndex) const {
    return mString.find_last_of(string->c_str(), fromIndex);
}

size_t StringBuilder::length() const {
    return mString.size();
}

sp<StringBuilder> StringBuilder::replace(size_t start, size_t end, const sp<String>& string) {
    mString.replace(start, end - start , string->c_str());
    return this;
}

sp<StringBuilder> StringBuilder::reverse() {
    std::reverse(mString.begin(), mString.end());
    return this;
}

void StringBuilder::setCharAt(size_t index, char c) {
    Assert::assertFalse<IndexOutOfBoundsException>(index >= length());
    mString[index] = c;
}

void StringBuilder::setLength(size_t newLength) {
    mString.resize(newLength);
}

sp<String> StringBuilder::substring(size_t start) const {
    return new String(mString.substr(start).c_str());
}

sp<String> StringBuilder::substring(size_t start, size_t end) const {
    Assert::assertFalse<IndexOutOfBoundsException>(end < start);
    return new String(mString.substr(start, end - start).c_str());
}

sp<String> StringBuilder::toString() const {
    return new String(mString.c_str());
}

void StringBuilder::trimToSize() {
    mString.shrink_to_fit();
}

sp<StringBuilder> StringBuilder::append(const char* string) {
    mString += string;
    return this;
}

template<typename T>
const std::string StringBuilder::toString(const T& data) {
   std::ostringstream outputStream;
   outputStream << data;
   return outputStream.str();
}

} /* namespace mindroid */
