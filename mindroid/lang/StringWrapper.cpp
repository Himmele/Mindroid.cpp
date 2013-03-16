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

#include "mindroid/lang/String.h"
#include <stdio.h>

namespace mindroid {

sp<String> StringWrapper::sEmptyString = new String("");

StringWrapper::StringWrapper() {
	mString = sEmptyString;
}

StringWrapper::StringWrapper(const StringWrapper& string) {
	mString = string.mString;
}

StringWrapper::StringWrapper(const char* string) {
	if (string != NULL) {
		size_t size = strlen(string);
		if (size > 0) {
			mString = new String(string, size);
		} else {
			mString = sEmptyString;
		}
	} else {
		mString = sEmptyString;
	}
}

StringWrapper::StringWrapper(const char* string, size_t size) {
	if (string != NULL) {
		if (size > 0) {
			mString = new String(string, size);
		} else {
			mString = sEmptyString;
		}
	} else {
		mString = sEmptyString;
	}
}

bool StringWrapper::contains(const char* subString) const {
	if (c_str() && subString) {
		return strstr(c_str(), subString) != NULL;
	} else {
		return false;
	}
}

StringWrapper& StringWrapper::operator=(const char* string) {
	if (string != NULL) {
		size_t size = strlen(string);
		if (size > 0) {
			mString = new String(string, size);
		} else {
			mString = sEmptyString;
		}
	} else {
		mString = sEmptyString;
	}
	return *this;
}

bool StringWrapper::operator<(const char* string) const {
	if (mString->mStringBuffer->mData && string) {
		return strcmp(mString->mStringBuffer->mData, string) < 0;
	} else {
		return false;
	}
}

bool StringWrapper::operator<(const StringWrapper& string) const {
	if (mString->mStringBuffer->mData && string.mString->mStringBuffer->mData) {
		return strcmp(mString->mStringBuffer->mData, string.mString->mStringBuffer->mData) < 0;
	} else {
		return false;
	}
}

bool StringWrapper::operator<=(const char* string) const {
	if (mString->mStringBuffer->mData && string) {
		return strcmp(mString->mStringBuffer->mData, string) <= 0;
	} else {
		return false;
	}
}

bool StringWrapper::operator<=(const StringWrapper& string) const {
	if (mString->mStringBuffer->mData && string.mString->mStringBuffer->mData) {
		return strcmp(mString->mStringBuffer->mData, string.mString->mStringBuffer->mData) <= 0;
	} else {
		return false;
	}
}

bool StringWrapper::operator==(const char* string) const {
	if (mString->mStringBuffer->mData && string) {
		return strcmp(mString->mStringBuffer->mData, string) == 0;
	} else {
		if (mString->mStringBuffer->mData == NULL && string == NULL) {
			return true;
		} else {
			return false;
		}
	}
}

bool StringWrapper::operator==(const StringWrapper& string) const {
	if (mString->mStringBuffer->mData && string.mString->mStringBuffer->mData) {
		return strcmp(mString->mStringBuffer->mData, string.mString->mStringBuffer->mData) == 0;
	} else {
		if (mString->mStringBuffer->mData == NULL && string.mString->mStringBuffer->mData == NULL) {
			return true;
		} else {
			return false;
		}
	}
}

bool StringWrapper::operator!=(const char* string) const {
	if (mString->mStringBuffer->mData && string) {
		return strcmp(mString->mStringBuffer->mData, string) != 0;
	} else {
		if (mString->mStringBuffer->mData == NULL && string == NULL) {
			return false;
		} else {
			return true;
		}
	}
}

bool StringWrapper::operator!=(const StringWrapper& string) const {
	if (mString->mStringBuffer->mData && string.mString->mStringBuffer->mData) {
		return strcmp(mString->mStringBuffer->mData, string.mString->mStringBuffer->mData) != 0;
	} else {
		if (mString->mStringBuffer->mData == NULL && string.mString->mStringBuffer->mData == NULL) {
			return false;
		} else {
			return true;
		}
	}
}

bool StringWrapper::operator>=(const char* string) const {
	if (mString->mStringBuffer->mData && string) {
		return strcmp(mString->mStringBuffer->mData, string) >= 0;
	} else {
		return false;
	}
}

bool StringWrapper::operator>=(const StringWrapper& string) const {
	if (mString->mStringBuffer->mData && string.mString->mStringBuffer->mData) {
		return strcmp(mString->mStringBuffer->mData, string.mString->mStringBuffer->mData) >= 0;
	} else {
		return false;
	}
}

bool StringWrapper::operator>(const char* string) const {
	if (mString->mStringBuffer->mData && string) {
		return strcmp(mString->mStringBuffer->mData, string) > 0;
	} else {
		return false;
	}
}

bool StringWrapper::operator>(const StringWrapper& string) const {
	if (mString->mStringBuffer->mData && string.mString->mStringBuffer->mData) {
		return strcmp(mString->mStringBuffer->mData, string.mString->mStringBuffer->mData) > 0;
	} else {
		return false;
	}
}

bool StringWrapper::equals(const char* string) const {
	return mString->equals(string);
}

bool StringWrapper::equals(const sp<String>& string) const {
	return mString->equals(string);
}

bool StringWrapper::equalsIgnoreCase(const char* string) const {
	return toLowerCase() == StringWrapper(string).toLowerCase();
}

bool StringWrapper::equalsIgnoreCase(const StringWrapper& string) const {
	return toLowerCase() == string.toLowerCase();
}

bool StringWrapper::contains(const StringWrapper& subString) const {
	return contains(subString.c_str());
}

bool StringWrapper::startsWith(const char* prefix) const {
	size_t prefixSize = strlen(prefix);
	if (size() >= prefixSize) {
		return strncmp(c_str(), prefix, prefixSize) == 0;
	} else {
		return false;
	}
}

bool StringWrapper::startsWith(const StringWrapper& prefix) const {
	return startsWith(prefix.c_str());
}

bool StringWrapper::endsWith(const char* suffix) const {
	size_t suffixSize = strlen(suffix);
	if (size() >= suffixSize) {
		return strncmp(c_str() + size() - suffixSize, suffix, suffixSize) == 0;
	} else {
		return false;
	}
}

bool StringWrapper::endsWith(const StringWrapper& suffix) const {
	return endsWith(suffix.c_str());
}

StringWrapper StringWrapper::substr(size_t beginIndex) const {
	if (beginIndex < size()) {
		return StringWrapper(c_str() + beginIndex, size() - beginIndex);
	} else {
		return StringWrapper();
	}
}

StringWrapper StringWrapper::substr(size_t beginIndex, size_t endIndex) const {
	if (beginIndex < size()) {
		return StringWrapper(c_str() + beginIndex, endIndex - beginIndex);
	} else {
		return StringWrapper();
	}
}

StringWrapper StringWrapper::toLowerCase() const {
	sp<StringBuffer> string = new StringBuffer(size());
	for (size_t i = 0; i < size(); i++) {
		string->mData[i] = tolower(mString->mData[i]);
	}
	StringWrapper tmp(string);
	return tmp;
}

StringWrapper StringWrapper::toUpperCase() const {
	sp<StringBuffer> string = new StringBuffer(size());
	for (size_t i = 0; i < size(); i++) {
		string->mData[i] = toupper(mString->mData[i]);
	}
	StringWrapper tmp(string);
	return tmp;
}

ssize_t StringWrapper::indexOf(const char* string) const {
	const char* substr = strstr(c_str(), string);
	if (substr != NULL) {
		return substr - c_str();
	} else {
		return -1;
	}
}

ssize_t StringWrapper::indexOf(const StringWrapper& string) const {
	return indexOf(string.c_str());
}

ssize_t StringWrapper::indexOf(const char* string, size_t fromIndex) const {
	const char* substr = strstr(c_str() + fromIndex, string);
	if (substr != NULL) {
		return substr - c_str();
	} else {
		return -1;
	}
}

ssize_t StringWrapper::indexOf(const StringWrapper& string, size_t fromIndex) const {
	return indexOf(string.c_str() + fromIndex);
}

StringWrapper StringWrapper::trim() const {
	StringWrapper tmp(*this);
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
	if (beginIndex == 0 && endIndex == (ssize_t) tmp.size() - 1) {
		return tmp;
	} else {
		if (beginIndex != tmp.size()) {
			tmp.mString = new StringBuffer(tmp.mString->mData + beginIndex,
					endIndex - beginIndex + 1);
		} else {
			tmp.mString = sEmptyString;
		}
		return tmp;
	}
}

StringWrapper StringWrapper::left(size_t n) const {
	StringWrapper tmp(*this);
	n = (n > tmp.size()) ? tmp.size() : n;
	tmp.mString = new StringBuffer(tmp.mString->mData, n);
	return tmp;
}

StringWrapper StringWrapper::right(size_t n) const {
	StringWrapper tmp(*this);
	n = (n > tmp.size()) ? tmp.size() : n;
	tmp.mString = new StringBuffer(tmp.mString->mData + tmp.size() - n, n);
	return tmp;
}

sp<List<StringWrapper> > StringWrapper::split(const char* separator) const {
	sp<List<StringWrapper> > strings = new List<StringWrapper>();
	ssize_t curIndex = 0;
	ssize_t prevCurIndex;
	while (curIndex >= 0 && (size_t) curIndex < size()) {
		prevCurIndex = curIndex;
		curIndex = indexOf(separator, curIndex);
		if (curIndex >= 0) {
			strings->push_back(substr(prevCurIndex, curIndex));
			curIndex += strlen(separator);
		} else {
			strings->push_back(substr(prevCurIndex, size()));
		}
	}
	return strings;
}

sp<List<StringWrapper> > StringWrapper::split(const StringWrapper& separator) const {
	return split(separator.c_str());
}

StringWrapper& StringWrapper::append(const char* data, size_t size) {
	if (data != NULL && size > 0) {
		sp<StringBuffer> oldString = mString;
		mString = new StringBuffer(oldString->mData, oldString->mSize, data, size);
	}
	return *this;
}

StringWrapper& StringWrapper::appendFormatted(const char* format, ...) {
	va_list args;
	va_start(args, format);
	appendFormattedWithVarArgList(format, args);
	va_end(args);
	return *this;
}

StringWrapper StringWrapper::format(const char* format, ...) {
	va_list args;
	va_start(args, format);
	StringWrapper string;
	string.appendFormattedWithVarArgList(format, args);
	va_end(args);
	return string;
}

StringWrapper& StringWrapper::appendFormattedWithVarArgList(const char* format, va_list args) {
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

StringWrapper::StringBuffer::StringBuffer(size_t size) {
	mSize = size;
	if (size > 0) {
		mData = (char*) malloc(mSize + 1);
		mData[size] = '\0';
	} else {
		mData = NULL;
	}
}

StringWrapper::StringBuffer::StringBuffer(const char* string, size_t size) {
	mSize = size;
	if (string != NULL) {
		mData = (char*) malloc(mSize + 1);
		memcpy(mData, string, size);
		mData[size] = '\0';
	} else {
		mData = NULL;
	}
}

StringWrapper::StringBuffer::StringBuffer(const char* string1, size_t size1,
		const char* string2, size_t size2) {
	mSize = size1 + size2;
	mData = (char*) malloc(mSize + 1);
	memcpy(mData, string1, size1);
	memcpy(mData + size1, string2, size2);
	mData[size1 + size2] = '\0';
}

} /* namespace mindroid */

bool operator<(const char* lhs, const mindroid::StringWrapper& rhs) {
	return rhs < lhs;
}

bool operator<=(const char* lhs, const mindroid::StringWrapper& rhs) {
	return rhs <= lhs;
}

bool operator==(const char* lhs, const mindroid::StringWrapper& rhs) {
	return rhs == lhs;
}

bool operator!=(const char* lhs, const mindroid::StringWrapper& rhs) {
	return rhs != lhs;
}

bool operator>=(const char* lhs, const mindroid::StringWrapper& rhs) {
	return rhs >= lhs;
}

bool operator>(const char* lhs, const mindroid::StringWrapper& rhs) {
	return rhs > lhs;
}
