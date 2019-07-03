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

#ifndef MINDROID_LANG_NOSUCHMETHODEXCEPTION_H_
#define MINDROID_LANG_NOSUCHMETHODEXCEPTION_H_

#include <mindroid/lang/Exception.h>

namespace mindroid {

class NoSuchMethodException : public Exception {
public:
    NoSuchMethodException() = default;

    NoSuchMethodException(const char* message) : Exception(message) {
    }

    NoSuchMethodException(const sp<String>& message) : Exception(message) {
    }
};

} /* namespace mindroid */

#endif /* MINDROID_LANG_NOSUCHMETHODEXCEPTION_H_ */
