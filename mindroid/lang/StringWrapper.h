/*
 * Copyright (C) 2013 Daniel Himmelein
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

#ifndef MINDROID_STRINGWRAPPER_H_
#define MINDROID_STRINGWRAPPER_H_

#include "mindroid/lang/String.h"
#include "mindroid/util/List.h"

namespace mindroid {

class StringWrapper
{
public:
	StringWrapper();
	StringWrapper(const StringWrapper& string);
	StringWrapper(const sp<String>& string) : mString(string) { }
	explicit StringWrapper(const char* string);
	explicit StringWrapper(const char* string, size_t size);
	~StringWrapper() { }

	inline StringWrapper& operator=(StringWrapper const& string);
	StringWrapper& operator=(const char* string);
	inline StringWrapper& operator+=(const StringWrapper& string);
	inline StringWrapper& operator+=(const char* string);
	inline StringWrapper& operator+=(const char character);
	inline StringWrapper operator+(const char* string) const;
	inline StringWrapper operator+(const StringWrapper& string) const;

	bool operator<(const char* string) const;
	bool operator<(const StringWrapper& string) const;
	bool operator<=(const char* string) const;
	bool operator<=(const StringWrapper& string) const;
	bool operator==(const char* string) const;
	bool operator==(const StringWrapper& string) const;
	bool operator!=(const char* string) const;
	bool operator!=(const StringWrapper& string) const;
	bool operator>=(const char* string) const;
	bool operator>=(const StringWrapper& string) const;
	bool operator>(const char* string) const;
	bool operator>(const StringWrapper& string) const;

	bool equals(const char* string) const;
	bool equals(const StringWrapper& string) const;
	bool equalsIgnoreCase(const char* string) const;
	bool equalsIgnoreCase(const StringWrapper& string) const;

	inline size_t size() const {
		return mString->size();
	}

	inline bool isEmpty() const {
		return mString->isEmpty();
	}

	inline const char* c_str() const {
		return mString->c_str();
	}

	inline operator const char*() const {
		return c_str();
	}

	inline char operator[](const size_t index) const {
		return mString->charAt(index);
	}

	inline char charAt(size_t index) const {
		return mString->charAt(index);
	}

	inline sp<String> toString() {
		return mString;
	}

	bool contains(const char* subString) const;
	bool contains(const StringWrapper& subString) const;

	bool startsWith(const char* prefix) const;
	bool startsWith(const StringWrapper& prefix) const;

	bool endsWith(const char* suffix) const;
	bool endsWith(const StringWrapper& suffix) const;

	StringWrapper substr(size_t beginIndex) const;
	StringWrapper substr(size_t beginIndex, size_t endIndex) const;

	StringWrapper toLowerCase() const;
	StringWrapper toUpperCase() const;

	ssize_t indexOf(const char* string) const;
	ssize_t indexOf(const StringWrapper& string) const;
	ssize_t indexOf(const char* string, size_t fromIndex) const;
	ssize_t indexOf(const StringWrapper& string, size_t fromIndex) const;

	StringWrapper trim() const;

	StringWrapper left(size_t n) const;
	StringWrapper right(size_t n) const;

	sp< List<StringWrapper> > split(const char* separator) const;
	sp< List<StringWrapper> > split(const StringWrapper& separator) const;

	StringWrapper& append(const char* string) {
		return append(string, strlen(string));
	}

	StringWrapper& append(const sp<String>& string) {
		return append(string->c_str(), strlen(string->c_str()));
	}

	StringWrapper& append(const char* data, size_t size);
	StringWrapper& appendFormatted(const char* format, ...) __attribute__((format (printf,2, 3)));

	static StringWrapper format(const char* format, ...) __attribute__((format (printf, 1, 2)));

	static size_t size(const char* string) {
		return strlen(string);
	}

private:
	StringWrapper& appendFormattedWithVarArgList(const char* format, va_list args);

	sp<String> mString;
};

inline StringWrapper& StringWrapper::operator=(StringWrapper const& string) {
	mString = string.mString;
	return *this;
}

inline StringWrapper& StringWrapper::operator+=(const char* string) {
	return append(string, strlen(string));
}

inline StringWrapper& StringWrapper::operator+=(const StringWrapper& string) {
	return append(string.c_str(), string.size());
}

inline StringWrapper& StringWrapper::operator+=(const char character) {
	return append(&character, 1);
}

inline StringWrapper StringWrapper::operator+(const char* string) const {
	StringWrapper tmp(*this);
	tmp += string;
	return tmp;
}

inline StringWrapper StringWrapper::operator+(StringWrapper const& string) const {
	StringWrapper tmp(*this);
	tmp += string;
	return tmp;
}

} /* namespace mindroid */

bool operator<(const char* lhs, const mindroid::StringWrapper& rhs);
bool operator<=(const char* lhs, const mindroid::StringWrapper& rhs);
bool operator==(const char* lhs, const mindroid::StringWrapper& rhs);
bool operator!=(const char* lhs, const mindroid::StringWrapper& rhs);
bool operator>=(const char* lhs, const mindroid::StringWrapper& rhs);
bool operator>(const char* lhs, const mindroid::StringWrapper& rhs);

#endif /* MINDROID_STRINGWRAPPER_H_ */
