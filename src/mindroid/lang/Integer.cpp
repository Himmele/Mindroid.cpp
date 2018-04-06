/*
 * Copyright (C) 2016 Daniel Himmelein
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

#include <mindroid/lang/Integer.h>
#include <mindroid/lang/String.h>
#include <mindroid/lang/NumberFormatException.h>
#include <cstdlib>

namespace mindroid {

sp<Integer> Integer::valueOf(const char* string) {
    return valueOf(string, 10);
}

sp<Integer> Integer::valueOf(const sp<String>& string) {
    return valueOf(string, 10);
}

sp<Integer> Integer::valueOf(const char* string, int32_t radix) {
    return valueOf(String::valueOf(string), radix);
}

sp<Integer> Integer::valueOf(const sp<String>& string, int32_t radix) {
    if (string == nullptr) {
        throw NumberFormatException("String is null");
    }
    int32_t value = strtol(string->c_str(), nullptr, radix);
    if (value == 0 && !string->equals("0")) {
        throw NumberFormatException(String::format("Invalid integral value: %s", string->c_str()));
    }
    return new Integer(value);
}

} /* namespace mindroid */
