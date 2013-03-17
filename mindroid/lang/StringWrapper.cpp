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

#include "mindroid/lang/StringWrapper.h"
#include <stdio.h>

namespace mindroid {

StringWrapper::StringWrapper() {
	mString = String::EMPTY_STRING;
}

StringWrapper::StringWrapper(const StringWrapper& string) :
		mString(string.mString) {
}

StringWrapper::StringWrapper(const char* string) {
	mString = new String(string);
}

StringWrapper::StringWrapper(const char* string, size_t size) {
	mString = new String(string, size);
}

StringWrapper& StringWrapper::operator=(const char* string) {
	if (string != NULL) {
		size_t size = strlen(string);
		if (size > 0) {
			mString = new String(string, size);
		} else {
			mString = String::EMPTY_STRING;
		}
	} else {
		mString = String::EMPTY_STRING;
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

bool StringWrapper::equals(const StringWrapper& string) const {
	return mString->equals(string);
}

bool StringWrapper::equalsIgnoreCase(const char* string) const {
	return mString->equalsIgnoreCase(string);
}

bool StringWrapper::equalsIgnoreCase(const StringWrapper& string) const {
	return mString->equalsIgnoreCase(string);
}

bool StringWrapper::contains(const char* subString) const {
	return mString->contains(subString);
}

bool StringWrapper::contains(const StringWrapper& subString) const {
	return mString->contains(subString);
}

bool StringWrapper::startsWith(const char* prefix) const {
	return mString->startsWith(prefix);
}

bool StringWrapper::startsWith(const StringWrapper& prefix) const {
	return mString->startsWith(prefix);
}

bool StringWrapper::endsWith(const char* suffix) const {
	return mString->endsWith(suffix);
}

bool StringWrapper::endsWith(const StringWrapper& suffix) const {
	return mString->endsWith(suffix);
}

StringWrapper StringWrapper::substr(size_t beginIndex) const {
	return StringWrapper(mString->substr(beginIndex));
}

StringWrapper StringWrapper::substr(size_t beginIndex, size_t endIndex) const {
	return StringWrapper(mString->substr(beginIndex, endIndex));
}

StringWrapper StringWrapper::toLowerCase() const {
	return StringWrapper(mString->toLowerCase());
}

StringWrapper StringWrapper::toUpperCase() const {
	return StringWrapper(mString->toUpperCase());
}

ssize_t StringWrapper::indexOf(const char* string) const {
	return mString->indexOf(string);
}

ssize_t StringWrapper::indexOf(const StringWrapper& string) const {
	return mString->indexOf(string);
}

ssize_t StringWrapper::indexOf(const char* string, size_t fromIndex) const {
	return mString->indexOf(string, fromIndex);
}

ssize_t StringWrapper::indexOf(const StringWrapper& string, size_t fromIndex) const {
	return mString->indexOf(string, fromIndex);
}

StringWrapper StringWrapper::trim() const {
	return StringWrapper(mString->trim());
}

StringWrapper StringWrapper::left(size_t n) const {
	return StringWrapper(mString->left(n));
}

StringWrapper StringWrapper::right(size_t n) const {
	return StringWrapper(mString->right(n));
}

sp< List<StringWrapper> > StringWrapper::split(const char* separator) const {
	sp< List<StringWrapper> > strings = new List<StringWrapper>();
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

sp< List<StringWrapper> > StringWrapper::split(const StringWrapper& separator) const {
	return split(separator.c_str());
}

StringWrapper& StringWrapper::append(const char* data, size_t size) {
	mString = mString->append(data, size);
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
	StringWrapper stringWrapper;
	stringWrapper.appendFormattedWithVarArgList(format, args);
	va_end(args);
	return stringWrapper;
}

StringWrapper& StringWrapper::appendFormattedWithVarArgList(const char* format, va_list args) {
	mString = mString->appendFormattedWithVarArgList(format, args);
	return *this;
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
