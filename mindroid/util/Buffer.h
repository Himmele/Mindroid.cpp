/*
 * Copyright (C) 2010 The Android Open Source Project
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

#ifndef MINDROID_BUFFER_H_
#define MINDROID_BUFFER_H_

#include <sys/types.h>
#include <stdint.h>
#include <mindroid/util/Utils.h>
#include <mindroid/os/Ref.h>
#include <mindroid/os/Bundle.h>

namespace mindroid {

class Buffer :
		public Ref
{
public:
	Buffer(size_t capacity);
	Buffer(void* data, size_t capacity);
	virtual ~Buffer();

	uint8_t* base() {
		return (uint8_t*) mData;
	}

	uint8_t* data() {
		return (uint8_t*) mData + mOffset;
	}

	size_t capacity() const {
		return mCapacity;
	}

	size_t size() const {
		return mSize;
	}

	size_t offset() const {
		return mOffset;
	}

	void setRange(size_t offset, size_t size);

	void setId(int32_t id) {
		mId = id;
	}

	int32_t getId() const {
		return mId;
	}

	bool hasMetaData() const;
	sp<Bundle> metaData();

private:
	void* mData;
	size_t mCapacity;
	size_t mOffset;
	size_t mSize;
	int32_t mId;
	sp<Bundle> mMetaData;

	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(Buffer)
};

} /* namespace mindroid */

#endif /* MINDROID_BUFFER_H_ */
