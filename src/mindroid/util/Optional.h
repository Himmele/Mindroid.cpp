/*
 * Copyright (C) 2019 E.S.R.Labs
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

#ifndef MINDROID_UTIL_OPTIONAL_H_
#define MINDROID_UTIL_OPTIONAL_H_

#include <mindroid/lang/Class.h>
#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/util/NoSuchElementException.h>

namespace mindroid {

template<typename T>
class Optional :
        public Object { 
public:
    /** Returns an empty Optional instance. */
    static sp<Optional<T>> empty() {
        return new Optional();
    }

    /** Indicates whether some other object is "equal to" this Optional. */
    bool equals(const sp<Object>& obj) const override {
        if (obj == mValue) {
            return true;
        } else if (mValue != nullptr) {
            if (Class<T>::isInstance(obj)) {
                sp<T> s = Class<T>::cast(obj);
                return mValue->equals(s);
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    /** If a value is present in this Optional, returns the value, otherwise throws NoSuchElementException. */
    sp<T> get() const {
        if (mValue == nullptr) {
            throw NoSuchElementException("No value present");
        } else {
            return mValue;
        }
    }

    /** Returns the hash code value of the present value, if any, or 0 (zero) if no value is present. */
    size_t hashCode() const override {
        if (isPresent()) {
            return mValue->hashCode();
        } else {
            return 0;
        }
    }

    /** Return true if there is a value present, otherwise false. */
    bool isPresent() const {
        return mValue != nullptr;
    }

    /** Returns an Optional with the specified present non-null value. */
    static sp<Optional<T>> of(const sp<T>& value) {
        sp<Optional<T>> optional = empty();
        optional->mValue = value;
        return optional;
    }

    /** Returns an Optional describing the specified value, if non-null, otherwise returns an empty Optional. */
    static sp<Optional<T>> ofNullable(const sp<T>& value) {
        if (value != nullptr) {
            return of(value);
        } else {
            return empty();
        }
    }

    /** Return the value if present, otherwise return other. */
    sp<T> orElse(const sp<T>& other) const {
        if (isPresent()) {
            return get();
        } else {
            return other;
        }
    }

    /** Returns a non-empty string representation of this Optional suitable for debugging. */
    sp<String> toString() const {
        if (isPresent()) {
            return String::format("Optional[%s]", mValue->toString());
        } else {
            return String::valueOf("Optional.empty");
        }
    }

private:
    Optional() = default;

    sp<T> mValue;
};

template<>
sp<String> Optional<String>::toString() const {
    if (isPresent()) {
        return String::format("Optional[%s]", mValue->c_str());
    } else {
        return String::valueOf("Optional.empty");
    }
}

} /* namespace mindroid */

#endif /* MINDROID_UTIL_OPTIONAL_H_ */
