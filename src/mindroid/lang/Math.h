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

#ifndef MINDROID_LANG_MATH_H_
#define MINDROID_LANG_MATH_H_

#include <cstdint>
#include <random>
#include <algorithm>

namespace mindroid {

class Math {
public:
    Math() noexcept = delete;
    ~Math() noexcept = delete;
    Math(const Math&) = delete;
    Math& operator=(const Math&) = delete;

    template<typename T>
    static T min(T a, T b) {
        return ::std::min(a, b);
    }

    template<typename T>
    static T max(T a, T b) {
        return ::std::max(a, b);
    }

    /**
     * Returns a pseudo-random double {@code n}, where {@code n >= 0.0 && n < 1.0}.
     * This method reuses a single instance of {@link java.util.Random}.
     * This method is thread-safe because access to the {@code Random} is synchronized,
     * but this harms scalability. Applications may find a performance benefit from
     * allocating a {@code Random} for each of their threads.
     *
     * @return a pseudo-random number.
     */
    static double random();

private:
    static std::random_device sRandomDevice; // Seed.
};

} /* namespace mindroid */

#endif /* MINDROID_LANG_MATH_H_ */
