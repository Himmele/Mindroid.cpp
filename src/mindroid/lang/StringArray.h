/*
 * Copyright (C) 2018 E.S.R.Labs
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

#ifndef MINDROID_LANG_STRINGARRAY_H_
#define MINDROID_LANG_STRINGARRAY_H_

#include <mindroid/lang/Object.h>
#include <vector>

namespace mindroid {

class String;
template<typename T> class ArrayList;

class StringArray : public Object {
public:
    StringArray(size_t size) : mSize(size) {
        mArray = new sp<String>[size];
    }

    virtual ~StringArray() {
        delete [] mArray;
    }

    static sp<StringArray> valueOf(const std::initializer_list<const char*>& list);

    static sp<StringArray> valueOf(const sp<ArrayList<sp<String>>>& list);

    /**
     * Returns the array size.
     */
    size_t size() const {
        return mSize;
    }

    /**
     * Returns the const element in the given position.
     *
     * throws IndexOutOfBoundsException if position is out of bounds.
     */
    const sp<String>& get(size_t position) const;
    const sp<String>& operator[](size_t position) const;

    /**
     * Returns the element in the given position.
     *
     * throws IndexOutOfBoundsException if position is out of bounds.
     */
    sp<String>& get(size_t position);
    void set(size_t position, const sp<String>& string);
    sp<String>& operator[](size_t position);

private:
    const size_t mSize;
    sp<String>* mArray;
};

} /* namespace mindroid */

#endif /* MINDROID_LANG_STRINGARRAY_H_ */
