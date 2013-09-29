/*
 * Copyright (C) 2010 The Android Open Source Project
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

#include "mindroid/util/Buffer.h"
#include <stdlib.h>
#include <assert.h>

namespace mindroid {

Buffer::Buffer(size_t capacity) :
		mData(malloc(capacity)),
		mCapacity(capacity),
		mOffset(0),
		mSize(capacity),
		mId(0) {
	assert(mData != NULL);
}

Buffer::Buffer(void* data, size_t capacity) :
		mData(data),
		mCapacity(capacity),
		mOffset(0),
		mSize(capacity),
		mId(0) {
}

Buffer::~Buffer() {
	if (mData != NULL) {
		free(mData);
		mData = NULL;
	}
}

void Buffer::setRange(size_t offset, size_t size) {
	assert(offset <= mCapacity);
	assert(offset + size <= mCapacity);

	mOffset = offset;
	mSize = size;
}

bool Buffer::hasMetaData() const {
	return mMetaData != NULL;
}

sp<Bundle> Buffer::metaData() {
	if (mMetaData == NULL) {
		mMetaData = new Bundle();
	}
	return mMetaData;
}

} /* namespace mindroid */
