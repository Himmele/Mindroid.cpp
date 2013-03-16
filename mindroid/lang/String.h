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

#ifndef MINDROID_STRING_H_
#define MINDROID_STRING_H_

#include "mindroid/os/Ref.h"
#include "mindroid/util/List.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

namespace mindroid {

class String :
		public Ref
{
public:
	String();
	explicit String(const char* string);
	explicit String(const char* string, size_t size);
	virtual ~String() { }

	bool equals(const char* string) const;
	bool equals(const sp<String>& string) const;
	bool equalsIgnoreCase(const char* string) const;
	bool equalsIgnoreCase(const sp<String>& string) const;

	inline size_t size() const {
		return mStringBuffer->mSize;
	}

	inline bool isEmpty() const {
		return size() == 0;
	}

	inline const char* c_str() const {
		return mStringBuffer->mData;
	}

	inline operator const char*() const {
		return c_str();
	}

	inline char operator[](const size_t index) const {
		assert(index < size());
		return mStringBuffer->mData[index];
	}

	inline char charAt(size_t index) const {
		assert(index < size());
		return mStringBuffer->mData[index];
	}

	bool contains(const char* subString) const;
	bool contains(const sp<String>& subString) const;

	bool startsWith(const char* prefix) const;
	bool startsWith(const sp<String>& prefix) const;

	bool endsWith(const char* suffix) const;
	bool endsWith(const sp<String>& suffix) const;

	sp<String> substr(size_t beginIndex) const;
	sp<String> substr(size_t beginIndex, size_t endIndex) const;

	sp<String> toLowerCase() const;
	sp<String> toUpperCase() const;

	ssize_t indexOf(const char* string) const;
	ssize_t indexOf(const sp<String>& string) const;
	ssize_t indexOf(const char* string, size_t fromIndex) const;
	ssize_t indexOf(const sp<String>& string, size_t fromIndex) const;

	sp<String> trim() const;

	sp<String> left(size_t n) const;
	sp<String> right(size_t n) const;

	sp< List< sp<String> > > split(const char* separator) const;
	sp< List< sp<String> > > split(const sp<String>& separator) const;

	sp<String> append(const char* data, size_t size);
	sp<String> appendFormatted(const char* format, ...) __attribute__((format (printf,2, 3)));

	static sp<String> format(const char* format, ...) __attribute__((format (printf, 1, 2)));

	static size_t size(const char* string) {
		return strlen(string);
	}

private:
	class StringBuffer : public LightweightRef<StringBuffer>
	{
	public:
		StringBuffer() :
				mData(NULL),
				mSize(0) {
		}

		StringBuffer(size_t size);

		StringBuffer(const char* string, size_t size);

		StringBuffer(const char* string1, size_t size1, const char* string2, size_t size2);

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

	String(const sp<StringBuffer>& string) : mStringBuffer(string) { }

	sp<String> appendFormattedWithVarArgList(const char* format, va_list args);

	sp<StringBuffer> mStringBuffer;
	static sp<StringBuffer> sEmptyStringBuffer;
};

} /* namespace mindroid */

#endif /* MINDROID_STRING_H_ */
