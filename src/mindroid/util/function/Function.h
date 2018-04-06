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

#ifndef MINDROID_UTIL_FUNCTION_FUNCTION_H_
#define MINDROID_UTIL_FUNCTION_FUNCTION_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/Runnable.h>
#include <functional>

namespace mindroid {

/**
 * Represents a function that accepts one argument and produces a result.
 */
template<typename T, typename R>
class Function :
        public Object {
public:
    Function(const std::function<R (T)>& func) : mFunc(func) {
    }

    Function(std::function<R (T)>&& func) : mFunc(std::move(func)) {
    }

    R apply(T t) {
        return mFunc(t);
    }

private:
    std::function<R (T)> mFunc;
};

template<>
class Function<void, void> :
        public Runnable {
public:
    Function(const std::function<void (void)>& func) : Runnable(func) {
    }

    Function(std::function<void (void)>&& func) : Runnable(func) {
    }

    void apply(void) {
        run();
    }
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_FUNCTION_FUNCTION_H_ */
