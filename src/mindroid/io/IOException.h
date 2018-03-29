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

#ifndef MINDROID_IO_IOEXCEPTION_H_
#define MINDROID_IO_IOEXCEPTION_H_

#include <mindroid/lang/Exception.h>

namespace mindroid {

/**
 * Class for I/O exceptions.
 */
class IOException : public Exception {
public:
    IOException() = default;

    IOException(const char* message) : Exception(message) {
    }

    IOException(const sp<String>& message) : Exception(message) {
    }
};

} /* namespace mindroid */

#endif /* MINDROID_IO_IOEXCEPTION_H_ */
