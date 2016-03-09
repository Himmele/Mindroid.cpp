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

#include "mindroid/util/Variant.h"

namespace mindroid {

Variant::~Variant() {
	switch (mType) {
	case String:
	case StringSet:
	case StringArrayList:
	case IntegerArrayList:
	case Object:
		if (mValue.object != nullptr) {
			mValue.object->decStrongReference(this);
		}
		break;
	case Binder:
		if (mValue.binder != nullptr) {
			mValue.binder->decStrongReference(this);
		}
		break;
	default:
		break;
	}
}

size_t Variant::hashCode() const {
	switch (mType) {
	case Null:
		return mType;
	case Bool:
		return hashCode(mType, &mValue.boolValue, sizeof(mValue.boolValue));
	case Byte:
		return hashCode(mType, &mValue.byteValue, sizeof(mValue.byteValue));
	case Char:
		return hashCode(mType, &mValue.charValue, sizeof(mValue.charValue));
	case Short:
		return hashCode(mType, &mValue.shortValue, sizeof(mValue.shortValue));
	case UnsignedShort:
		return hashCode(mType, &mValue.unsignedShortValue, sizeof(mValue.unsignedShortValue));
	case Int:
		return hashCode(mType, &mValue.intValue, sizeof(mValue.intValue));
	case UnsignedInt:
		return hashCode(mType, &mValue.unsignedIntValue, sizeof(mValue.unsignedIntValue));
	case Long:
		return hashCode(mType, &mValue.longValue, sizeof(mValue.longValue));
	case UnsignedLong:
		return hashCode(mType, &mValue.unsignedLongValue, sizeof(mValue.unsignedLongValue));
	case Float:
		return hashCode(mType, &mValue.floatValue, sizeof(mValue.floatValue));
	case Double:
		return hashCode(mType, &mValue.doubleValue, sizeof(mValue.doubleValue));
	case String:
	case StringSet:
	case StringArrayList:
	case IntegerArrayList:
	case Object:
		if (mValue.object != nullptr) {
			return mValue.object->hashCode();
		} else {
			return 0;
		}
		break;
	case Binder:
		if (mValue.binder != nullptr) {
			return mValue.binder->hashCode();
		} else {
			return 0;
		}
		break;
	default:
		return 0;
		break;
	}
}

size_t Variant::hashCode(size_t seed, const void* data, const size_t size) {
	size_t hash = seed;
	for (size_t i = 0; i < size; i++) {
		hash = 31 * hash + static_cast<const uint8_t*>(data)[i];
	}
	return hash;
}

} /* namespace mindroid */
