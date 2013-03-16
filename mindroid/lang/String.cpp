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

sp<String::StringBuffer> String::sEmptyStringBuffer = new String::StringBuffer("", 0);

String::String() {
	mStringBuffer = sEmptyStringBuffer;
}

String::String(const char* string) {
	if (string != NULL) {
		size_t size = strlen(string);
		if (size > 0) {
			mStringBuffer = new StringBuffer(string, size);
		} else {
			mStringBuffer = sEmptyStringBuffer;
		}
	} else {
		mStringBuffer = sEmptyStringBuffer;
	}
}

String::String(const char* string, size_t size) {
	if (string != NULL) {
		if (size > 0) {
			mStringBuffer = new StringBuffer(string, size);
		} else {
			mStringBuffer = sEmptyStringBuffer;
		}
	} else {
		mStringBuffer = sEmptyStringBuffer;
	}
}

bool String::equals(const char* string) const {
	if (mStringBuffer->mData && string) {
		return strcmp(mStringBuffer->mData, string) == 0;
	} else {
		if (mStringBuffer->mData == NULL && string == NULL) {
			return true;
		} else {
			return false;
		}
	}
}

bool String::equals(const sp<String>& string) const {
	if (mStringBuffer->mData && string->mStringBuffer->mData) {
		return strcmp(mStringBuffer->mData, string->mStringBuffer->mData) == 0;
	} else {
		if (mStringBuffer->mData == NULL && string->mStringBuffer->mData == NULL) {
			return true;
		} else {
			return false;
		}
	}
}

bool String::equalsIgnoreCase(const char* string) const {
	sp<String> tmp = new String(string);
	return toLowerCase()->equals(tmp->toLowerCase());
}

bool String::equalsIgnoreCase(const sp<String>& string) const {
	return toLowerCase()->equals(string->toLowerCase());
}

bool String::contains(const char* subString) const {
	if (c_str() && subString) {
		return strstr(c_str(), subString) != NULL;
	} else {
		return false;
	}
}

bool String::contains(const sp<String>& subString) const {
	return contains(subString->c_str());
}

bool String::startsWith(const char* prefix) const {
	size_t prefixSize = strlen(prefix);
	if (size() >= prefixSize) {
		return strncmp(c_str(), prefix, prefixSize) == 0;
	} else {
		return false;
	}
}

bool String::startsWith(const sp<String>& prefix) const {
	return startsWith(prefix->c_str());
}

bool String::endsWith(const char* suffix) const {
	size_t suffixSize = strlen(suffix);
	if (size() >= suffixSize) {
		return strncmp(c_str() + size() - suffixSize, suffix, suffixSize) == 0;
	} else {
		return false;
	}
}

bool String::endsWith(const sp<String>& suffix) const {
	return endsWith(suffix->c_str());
}

sp<String> String::substr(size_t beginIndex) const {
	if (beginIndex < size()) {
		return new String(c_str() + beginIndex, size() - beginIndex);
	} else {
		return new String();
	}
}

sp<String> String::substr(size_t beginIndex, size_t endIndex) const {
	if (beginIndex < size()) {
		return new String(c_str() + beginIndex, endIndex - beginIndex);
	} else {
		return new String();
	}
}

sp<String> String::toLowerCase() const {
	sp<StringBuffer> stringBuffer = new StringBuffer(size());
	for (size_t i = 0; i < size(); i++) {
		stringBuffer->mData[i] = tolower(mStringBuffer->mData[i]);
	}
	return new String(stringBuffer);
}

sp<String> String::toUpperCase() const {
	sp<StringBuffer> stringBuffer = new StringBuffer(size());
	for (size_t i = 0; i < size(); i++) {
		stringBuffer->mData[i] = toupper(mStringBuffer->mData[i]);
	}
	return new String(stringBuffer);
}

ssize_t String::indexOf(const char* string) const {
	const char* substr = strstr(c_str(), string);
	if (substr != NULL) {
		return substr - c_str();
	} else {
		return -1;
	}
}

ssize_t String::indexOf(const sp<String>& string) const {
	return indexOf(string->c_str());
}

ssize_t String::indexOf(const char* string, size_t fromIndex) const {
	const char* substr = strstr(c_str() + fromIndex, string);
	if (substr != NULL) {
		return substr - c_str();
	} else {
		return -1;
	}
}

ssize_t String::indexOf(const sp<String>& string, size_t fromIndex) const {
	return indexOf(string->c_str() + fromIndex);
}

sp<String> String::trim() const {
	size_t beginIndex;
	ssize_t endIndex;
	for (beginIndex = 0; beginIndex < size(); beginIndex++) {
		if (!isspace(mStringBuffer->mData[beginIndex])) {
			break;
		}
	}
	for (endIndex = size() - 1; endIndex >= 0; endIndex--) {
		if (!isspace(mStringBuffer->mData[endIndex])) {
			break;
		}
	}
	if (beginIndex == 0 && endIndex == (ssize_t) size() - 1) {
		return const_cast<String*>(this);
	} else {
		if (beginIndex != size()) {
			return new String(new StringBuffer(mStringBuffer->mData + beginIndex,
					endIndex - beginIndex + 1));
		} else {
			return new String(sEmptyStringBuffer);
		}
	}
}

sp<String> String::left(size_t n) const {
	n = (n > size()) ? size() : n;
	return new String(new StringBuffer(mStringBuffer->mData, n));
}

sp<String> String::right(size_t n) const {
	n = (n > size()) ? size() : n;
	return new String(new StringBuffer(mStringBuffer->mData + size() - n, n));
}

sp< List< sp<String> > > String::split(const char* separator) const {
	sp< List< sp<String> > > strings = new List< sp<String> >();
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

sp< List< sp<String> > > String::split(const sp<String>& separator) const {
	return split(separator->c_str());
}

sp<String> String::append(const char* data, size_t size) const {
	if (data != NULL && size > 0) {
		return new String(new StringBuffer(mStringBuffer->mData, mStringBuffer->mSize, data, size));
	}
	return const_cast<String*>(this);
}

sp<String> String::appendFormatted(const char* format, ...) const {
	va_list args;
	va_start(args, format);
	sp<String> formattedString = appendFormattedWithVarArgList(format, args);
	va_end(args);
	return formattedString;
}

sp<String> String::format(const char* format, ...) {
	va_list args;
	va_start(args, format);
	sp<String> emptyString = new String(sEmptyStringBuffer);
	sp<String> formattedString = emptyString->appendFormattedWithVarArgList(format, args);
	va_end(args);
	return formattedString;
}

sp<String> String::appendFormattedWithVarArgList(const char* format, va_list args) const {
	// see http://stackoverflow.com/questions/9937505/va-list-misbehavior-on-linux
	va_list copyOfArgs;
	va_copy(copyOfArgs, args);
	int size = vsnprintf(NULL, 0, format, copyOfArgs);
	va_end(copyOfArgs);

	if (size != 0) {
		sp<StringBuffer> stringBuffer = new StringBuffer(mStringBuffer->mSize + size);
		memcpy(stringBuffer->mData, mStringBuffer->mData, mStringBuffer->mSize);
		vsnprintf(stringBuffer->mData + mStringBuffer->mSize, size + 1, format, args);
		return new String(stringBuffer);
	}

	return const_cast<String*>(this);
}

String::StringBuffer::StringBuffer(size_t size) {
	mSize = size;
	if (size > 0) {
		mData = (char*) malloc(mSize + 1);
		mData[size] = '\0';
	} else {
		mData = NULL;
	}
}

String::StringBuffer::StringBuffer(const char* string, size_t size) {
	mSize = size;
	if (string != NULL) {
		mData = (char*) malloc(mSize + 1);
		memcpy(mData, string, size);
		mData[size] = '\0';
	} else {
		mData = NULL;
	}
}

String::StringBuffer::StringBuffer(const char* string1, size_t size1,
		const char* string2, size_t size2) {
	mSize = size1 + size2;
	mData = (char*) malloc(mSize + 1);
	memcpy(mData, string1, size1);
	memcpy(mData + size1, string2, size2);
	mData[size1 + size2] = '\0';
}

} /* namespace mindroid */
