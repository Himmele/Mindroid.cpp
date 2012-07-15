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

#ifndef ANDROID_LANG_STRING_H_
#define ANDROID_LANG_STRING_H_

#include "android/os/Ref.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

namespace android {
namespace lang {

class String
{
public:
	String();
	String(const String& string);
	explicit String(const char* string);
	explicit String(const char* string, size_t size);
	~String() {}

	inline String& operator=(String const& string);
	inline String& operator=(const char* string);
	inline String& operator+=(const String& string);
	inline String& operator+=(const char* string);
	inline String& operator+=(const char character);
	inline String operator+(const char* string) const;
	inline String operator+(const String& string) const;

	inline bool operator==(const char* string) const;
	inline bool operator==(const String& string) const;
	inline bool operator!=(const char* string) const;
	inline bool operator!=(const String& string) const;

	inline size_t size() const {
		return mString->mSize;
	}

	inline bool isNull() const {
		return mString->mData == NULL;
	}

	inline bool isEmpty() const {
	    return size() == 0;
	}

	inline const char* c_str() const {
		return mString->mData;
	}

	inline operator const char*() const {
		return c_str();
	}

	inline char operator[](const size_t index) const {
		assert(index < size());
		return mString->mData[index];
	}

	inline char charAt(uint32_t index) const {
		assert(index < size());
		return mString->mData[index];
	}

	bool contains(const char* subString) const;
	bool contains(const String& subString) const;

	bool startsWith(const char* prefix) const;
	bool startsWith(const String& prefix) const;

	bool endsWith(const char* suffix) const;
	bool endsWith(const String& suffix) const;

	String substr(uint32_t beginIndex) const;
	String substr(uint32_t beginIndex, uint32_t endIndex) const;

	ssize_t indexOf(const char* string) const;
	ssize_t indexOf(const String& string) const;

	String trim() const;

	String left(size_t n) const;
	String right(size_t n) const;

	String& append(const char* data, size_t size);
	String& appendFormatted(const char* format, ...) __attribute__((format (printf, 2, 3)));

	static String format(const char* format, ...) __attribute__((format (printf, 1, 2)));

private:
	class StringBuffer :
		public android::os::LightweightRef<StringBuffer> {
	public:
		StringBuffer() :
			mData(NULL),
			mSize(0) {
		}

		StringBuffer(size_t size) {
			mSize = size;
			if (size > 0) {
				mData = (char*) malloc(mSize + 1);
				mData[size] = '\0';
			} else {
				mData = NULL;
			}
		}

		StringBuffer(const char* string, size_t size) {
			mSize = size;
			if (string != NULL) {
				mData = (char*) malloc(mSize + 1);
				memcpy(mData, string, size);
				mData[size] = '\0';
			} else {
				mData = NULL;
			}
		}

		StringBuffer(const char* string1, size_t size1, const char* string2, size_t size2) {
			mSize = size1 + size2;
			mData = (char*) malloc(mSize + 1);
			memcpy(mData, string1, size1);
			memcpy(mData + size1, string2, size2);
			mData[size1 + size2] = '\0';
		}

		~StringBuffer() {
			if (mData != NULL) {
				free(mData);
			}
		}

	private:
		char* mData;
		size_t mSize;

		friend class String;
	};

	String& appendFormattedWithVarArgList(const char* format, va_list args);

	android::os::sp<StringBuffer> mString;
	static android::os::sp<StringBuffer> sNullString;
	static android::os::sp<StringBuffer> sEmptyString;
};

inline String& String::operator=(String const& string) {
	mString = string.mString;
	return *this;
}

inline String& String::operator=(const char* string) {
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
	return *this;
}

inline String& String::operator+=(const char* string) {
	return append(string, strlen(string));
}

inline String& String::operator+=(const String& string) {
	return append(string.c_str(), string.size());
}

inline String& String::operator+=(const char character) {
	return append(&character, 1);
}

inline String String::operator+(const char* string) const {
    String tmp(*this);
    tmp += string;
    return tmp;
}

inline String String::operator+(String const& string) const {
	String tmp(*this);
	tmp += string;
	return tmp;
}

inline bool String::operator==(const char* string) const {
    return strcmp(mString->mData, string) == 0;
}

inline bool String::operator==(const String& string) const {
    return strcmp(mString->mData, string.mString->mData) == 0;
}

inline bool String::operator!=(const char* string) const {
    return strcmp(mString->mData, string) != 0;
}

inline bool String::operator!=(const String& string) const {
    return strcmp(mString->mData, string.mString->mData) != 0;
}

} /* namespace lang */
} /* namespace android */

bool operator==(const char* lhs, const android::lang::String& rhs);
bool operator!=(const char* lhs, const android::lang::String& rhs);

#endif /* ANDROID_LANG_STRING_H_ */
