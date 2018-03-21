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

#include <mindroid/lang/StringArray.h>
#include <mindroid/lang/Class.h>
#include <mindroid/lang/IndexOutOfBoundsException.h>
#include <mindroid/lang/String.h>
#include <mindroid/util/ArrayList.h>
#include <mindroid/util/Assert.h>
#include <mindroid/lang/StringBuilder.h>

namespace mindroid {

sp<StringArray> StringArray::valueOf(const std::initializer_list<const char*>& list) {
    sp<StringArray> sa = new StringArray(list.size());
    size_t i = 0;
    std::initializer_list<const char*>::iterator itr;
    for (itr = list.begin(); itr != list.end(); ++itr) {
        sa->mArray[i++] = new String(*itr);
    }
    return sa;
}

sp<StringArray> StringArray::valueOf(const sp<ArrayList<sp<String>>>& list) {
    if (list != nullptr) {
        sp<StringArray> sa = new StringArray(list->size());
        size_t i = 0;
        auto itr = list->iterator();
        while (itr.hasNext()) {
            sa->mArray[i++] = itr.next();
        }
        return sa;
    } else {
        return nullptr;
    }
}

const sp<String>& StringArray::get(size_t position) const {
    Assert::assertFalse<IndexOutOfBoundsException>(position >= mSize);
    return mArray[position];
}


const sp<String>& StringArray::operator[](size_t position) const {
    Assert::assertFalse<IndexOutOfBoundsException>(position >= mSize);
    return mArray[position];
}

sp<String>& StringArray::get(size_t position) {
    Assert::assertFalse<IndexOutOfBoundsException>(position >= mSize);
    return mArray[position];
}

void StringArray::set(size_t position, const sp<String>& string) {
    Assert::assertFalse<IndexOutOfBoundsException>(position >= mSize);
    mArray[position] = string;
}

sp<String>& StringArray::operator[](size_t position) {
    Assert::assertFalse<IndexOutOfBoundsException>(position >= mSize);
    return mArray[position];
}

} /* namespace mindroid */
