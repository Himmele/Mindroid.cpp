/*
 * Copyright (C) 2016 E.S.R.Labs
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

#ifndef MINDROID_LANG_EXCEPTION_H_
#define MINDROID_LANG_EXCEPTION_H_

#include <mindroid/lang/String.h>
#include <exception>

namespace mindroid {

class Exception :
        public Object,
        public std::exception {
public:
    Exception() = default;
    ~Exception() noexcept {
    }

    Exception(const char* message) : mMessage(String::valueOf(message)) {
    }

    Exception(const sp<String>& message) : mMessage(message) {
    }

    Exception(const char* message, const sp<Exception>& cause) : mMessage(String::valueOf(message)), mCause(cause) {
    }

    Exception(const sp<String>& message, const sp<Exception>& cause) : mMessage(message), mCause(cause) {
    }

    Exception(const char* message, const Exception& cause) : mMessage(String::valueOf(message)), mCause(new Exception(cause)) {
    }

    Exception(const sp<String>& message, const Exception& cause) : mMessage(message), mCause(new Exception(cause)) {
    }

    Exception(const char* message, const std::exception& cause) : mMessage(String::valueOf(message)), mCause(new Exception(cause)) {
    }

    Exception(const sp<String>& message, const std::exception& cause) : mMessage(message), mCause(new Exception(cause)) {
    }

    Exception(const sp<Exception>& cause) : mMessage(cause->getMessage()), mCause(cause) {
    }

    Exception(const std::exception& cause) : mMessage(String::valueOf(cause.what())) {
    }

    // Copy constructor.
    Exception(const Exception& exception) : mMessage(exception.getMessage()), mCause(exception.getCause()) {
    }

    sp<String> getMessage() const {
        return mMessage;
    }

    sp<Exception> getCause() const {
        return mCause;
    }

    virtual sp<Exception> clone() const {
        return new Exception(mMessage, mCause);
    }

private:
    sp<String> mMessage;
    sp<Exception> mCause;
};

template<typename T, typename... Args>
void throwException(const char* format, Args... args) {
    sp<String> message = String::format(format, args...);
    throw T(message);
};

template<typename T, typename... Args>
void throwException(const sp<String>& format, Args... args) {
    sp<String> message = String::format(format, args...);
    throw T(message);
};

template<typename T, typename... Args>
void throwException(const Exception& cause, const char* format, Args... args) {
    sp<String> message = String::format(format, args...);
    throw T(message, cause);
};

template<typename T, typename... Args>
void throwException(const Exception& cause, const sp<String>& format, Args... args) {
    sp<String> message = String::format(format, args...);
    throw T(message, cause);
};

template<typename T, typename... Args>
void throwException(const sp<Exception>& cause, const char* format, Args... args) {
    sp<String> message = String::format(format, args...);
    throw T(message, cause);
};

template<typename T, typename... Args>
void throwException(const sp<Exception>& cause, const sp<String>& format, Args... args) {
    sp<String> message = String::format(format, args...);
    throw T(message, cause);
};

} /* namespace mindroid */

#endif /* MINDROID_LANG_EXCEPTION_H_ */
