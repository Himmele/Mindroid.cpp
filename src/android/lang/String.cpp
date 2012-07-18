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

#include "android/lang/String.h"
#include <stdio.h>

using namespace android::os;
using namespace android::util;

namespace android {
namespace lang {

sp<String::StringBuffer> String::sNullString = new String::StringBuffer();
sp<String::StringBuffer> String::sEmptyString = new String::StringBuffer("", 0);

String::String() {
	mString = sNullString;
}

String::String(const String& string) {
	mString = string.mString;
}

String::String(const char* string) {
	if (string != NULL) {
		size_t size = strlen(string);
		if (size > 0) {
			mString = new StringBuffer(string, size);
		} else {
			mString = sEmptyString;
		}
	} else {
		mString = sNullString;
	}
}

String::String(const char* string, size_t size) {
	if (string != NULL) {
		if (size > 0) {
			mString = new StringBuffer(string, size);
		} else {
			mString = sEmptyString;
		}
	} else {
		mString = sNullString;
	}
}

bool String::contains(const char* subString) const {
	if (c_str() && subString) {
		return strstr(c_str(), subString) != NULL;
	} else {
		return false;
	}
}

bool String::contains(const String& subString) const {
	return contains(subString.c_str());
}

bool String::startsWith(const char* prefix) const {
	size_t prefixSize = strlen(prefix);
	if (size() >= prefixSize) {
		return strncmp(c_str(), prefix, prefixSize) == 0;
	} else {
		return false;
	}
}

bool String::startsWith(const String& prefix) const {
	return startsWith(prefix.c_str());
}

bool String::endsWith(const char* suffix) const {
	size_t suffixSize = strlen(suffix);
	if (size() >= suffixSize) {
		return strncmp(c_str() + size() - suffixSize, suffix, suffixSize) == 0;
	} else {
		return false;
	}
}

bool String::endsWith(const String& suffix) const {
	return endsWith(suffix.c_str());
}

String String::substr(size_t beginIndex) const {
	if (beginIndex < size()) {
		return String(c_str() + beginIndex, size() - beginIndex);
	} else {
		return String();
	}
}

String String::substr(size_t beginIndex, size_t endIndex) const {
	if (beginIndex < size()) {
		return String(c_str() + beginIndex, endIndex - beginIndex);
	} else {
		return String();
	}
}

ssize_t String::indexOf(const char* string) const {
	const char* substr = strstr(c_str(), string);
	if (substr != NULL) {
		return substr - c_str();
	} else {
		return -1;
	}
}

ssize_t String::indexOf(const String& string) const {
	return indexOf(string.c_str());
}

ssize_t String::indexOf(const char* string, size_t fromIndex) const {
	const char* substr = strstr(c_str() + fromIndex, string);
	if (substr != NULL) {
		return substr - c_str();
	} else {
		return -1;
	}
}

ssize_t String::indexOf(const String& string, size_t fromIndex) const {
	return indexOf(string.c_str() + fromIndex);
}

String String::trim() const {
	String tmp(*this);
	size_t beginIndex;
	ssize_t endIndex;
	for (beginIndex = 0; beginIndex < tmp.size(); beginIndex++) {
		if (!isspace(tmp.mString->mData[beginIndex])) {
			break;
		}
	}
	for (endIndex = tmp.size() - 1; endIndex >= 0; endIndex--) {
		if (!isspace(tmp.mString->mData[endIndex])) {
			break;
		}
	}
	if (beginIndex == 0 && endIndex == (ssize_t)tmp.size() - 1) {
		return tmp;
	} else {
		if (beginIndex != tmp.size()) {
			tmp.mString = new StringBuffer(tmp.mString->mData + beginIndex, endIndex - beginIndex + 1);
		} else {
			tmp.mString = sEmptyString;
		}
		return tmp;
	}
}

String String::left(size_t n) const {
	String tmp(*this);
	n = (n > tmp.size()) ? tmp.size() : n;
	tmp.mString = new StringBuffer(tmp.mString->mData, n);
	return tmp;
}

String String::right(size_t n) const {
	String tmp(*this);
	n = (n > tmp.size()) ? tmp.size() : n;
	tmp.mString = new StringBuffer(tmp.mString->mData + tmp.size() - n , n);
	return tmp;
}

List<String> String::split(const char* separator) const {
	List<String> strings;
	ssize_t curIndex = 0;
	ssize_t prevCurIndex;
	while (curIndex >= 0 && (size_t)curIndex < size()) {
		prevCurIndex = curIndex;
		curIndex = indexOf(separator, curIndex);
		if (curIndex >= 0) {
			strings.push_back(substr(prevCurIndex, curIndex));
			curIndex += strlen(separator);
		} else {
			strings.push_back(substr(prevCurIndex, size()));
		}
	}
	return strings;
}

List<String> String::split(const String& separator) const {
	return split(separator.c_str());
}

String& String::append(const char* data, size_t size) {
	if (data != NULL && size > 0) {
		sp<StringBuffer> oldString = mString;
		mString = new StringBuffer(oldString->mData, oldString->mSize, data, size);
	}
	return *this;
}

String& String::appendFormatted(const char* format, ...) {
	va_list args;
	va_start(args, format);
	appendFormattedWithVarArgList(format, args);
	va_end(args);
	return *this;
}

String String::format(const char* format, ...) {
	va_list args;
	va_start(args, format);
	String string;
	string.appendFormattedWithVarArgList(format, args);
	va_end(args);
	return string;
}

String& String::appendFormattedWithVarArgList(const char* format, va_list args) {
	// see http://stackoverflow.com/questions/9937505/va-list-misbehavior-on-linux
	va_list copyOfArgs;
	va_copy(copyOfArgs, args);
	int size = vsnprintf(NULL, 0, format, copyOfArgs);
	va_end(copyOfArgs);

    if (size != 0) {
        sp<StringBuffer> oldString = mString;
        mString = new StringBuffer(oldString->mSize + size);
        memcpy(mString->mData, oldString->mData, oldString->mSize);
        vsnprintf(mString->mData + oldString->mSize, size + 1, format, args);
    }

    return *this;
}

size_t String::size(const char* string) {
	return strlen(string);
}

} /* namespace lang */
} /* namespace android */

bool operator==(const char* lhs, const android::lang::String& rhs) {
	return rhs == lhs;
}

bool operator!=(const char* lhs, const android::lang::String& rhs) {
	return rhs != lhs;
}
