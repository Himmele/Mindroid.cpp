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

#ifndef MINDROID_LANG_WRAPPER_H_
#define MINDROID_LANG_WRAPPER_H_

#include <mindroid/lang/Object.h>

namespace mindroid {

template<class T>
class Wrapper : public T, public Object {
public:
    Wrapper() {
    }

    Wrapper(const T& value) : T(value) {
    }

    Wrapper(const T&& value) : T(std::move(value)) {
    }

    virtual ~Wrapper() = default;

    static sp<Wrapper<T>> newInstance() {
        return new Wrapper<T>();
    }

    template<typename ... Args>
    static sp<Wrapper<T>> newInstance(Args&& ... args) {
        return new Wrapper<T>(T(std::forward<Args>(args)...));
    }

    static sp<Wrapper<T>> cast(const sp<Object>& o) {
        return object_cast<Wrapper<T>>(o);
    }
};

} /* namespace mindroid */

#endif /* MINDROID_LANG_WRAPPER_H_ */
