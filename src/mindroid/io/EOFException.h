/*
 * Copyright (C) 2017 The Android Open Source Project
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

#ifndef MINDROID_IO_EOFEXCEPTION_H_
#define MINDROID_IO_EOFEXCEPTION_H_

#include <mindroid/io/IOException.h>

namespace mindroid {

class EOFException : public IOException {
public:
    EOFException() = default;

    EOFException(const char* message) : IOException(message) {
    }

    EOFException(const sp<String>& message) : IOException(message) {
    }
};

} /* namespace mindroid */

#endif /* MINDROID_IO_EOFEXCEPTION_H_ */
