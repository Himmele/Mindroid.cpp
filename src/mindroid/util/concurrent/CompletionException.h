/*
 * Copyright (C) 2018 Daniel Himmelein
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

#ifndef MINDROID_COMPLETIONEXCEPTION_H_
#define MINDROID_COMPLETIONEXCEPTION_H_

#include <mindroid/lang/Exception.h>

namespace mindroid {

class CompletionException : public Exception {
public:
    CompletionException() = default;

    CompletionException(const char* message) : Exception(message) {
    }

    CompletionException(const sp<String>& message) : Exception(message) {
    }

    CompletionException(const char* message, const Exception& cause) : Exception(message, cause) {
    }

    CompletionException(const sp<String>& message, const Exception& cause) : Exception(message, cause) {
    }

    CompletionException(const sp<Exception>& cause) : Exception(cause->getMessage(), cause) {
    }

    CompletionException(const std::exception& cause) : Exception(String::valueOf(cause.what())) {
    }
};

} /* namespace mindroid */

#endif /* MINDROID_COMPLETIONEXCEPTION_H_ */
