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

#ifndef MINDROID_LANG_DOUBLE_H_
#define MINDROID_LANG_DOUBLE_H_

#include <mindroid/lang/Object.h>

namespace mindroid {

class String;

class Double :
        public Object {
public:
    /**
     * Constant for the maximum {@code double} value, (2 - 2<sup>-52</sup>) *
     * 2<sup>1023</sup>.
     */
    static const double MAX_VALUE;
    /**
     * Constant for the minimum {@code double} value, 2<sup>-1074</sup>.
     */
    static const double MIN_VALUE;

    /* 4.94065645841246544e-324 gets rounded to 9.88131e-324 */

    /**
     * Constant for the number of bits needed to represent a {@code double} in
     * two's complement form.
     */
    static const int32_t SIZE = 64;

    explicit Double(double value) : mValue(value) {
    }

    static sp<Double> valueOf(const char* s);
    static sp<Double> valueOf(const sp<String>& s);

    double doubleValue() const {
        return mValue;
    }

    double value() const {
        return doubleValue();
    }

    /**
     * Indicates whether this object is a <em>Not-a-Number (NaN)</em> value.
     *
     * @return {@code true} if this double is <em>Not-a-Number</em>;
     *         {@code false} if it is a (potentially infinite) double number.
     */
    bool isNaN() const {
        return isNaN(mValue);
    }

    /**
     * Indicates whether the specified double is a <em>Not-a-Number (NaN)</em>
     * value.
     *
     * @param d
     *            the double value to check.
     * @return {@code true} if {@code d} is <em>Not-a-Number</em>;
     *         {@code false} if it is a (potentially infinite) double number.
     */
    static bool isNaN(double d) {
        return d != d;
    }

    /**
     * Returns an integer corresponding to the bits of the given
     * <a href="http://en.wikipedia.org/wiki/IEEE_754-1985">IEEE 754</a> double precision
     * {@code value}. All <em>Not-a-Number (NaN)</em> values are converted to a single NaN
     * representation ({@code 0x7ff8000000000000L}) (compare to {@link #doubleToRawLongBits}).
     */
    static uint64_t doubleToLongBits(double value) {
        if (value != value) {
            return 0x7ff8000000000000L; // NaN.
        } else {
            return *reinterpret_cast<uint64_t*>(&value);
        }
    }

    /**
     * Returns the <a href="http://en.wikipedia.org/wiki/IEEE_754-1985">IEEE 754</a>
     * double precision float corresponding to the given {@code bits}.
     */
    static double longBitsToDouble(uint64_t bits) {
        return *reinterpret_cast<double*>(&bits);
    }

private:
    double mValue;
};

} /* namespace mindroid */

#endif /* MINDROID_LANG_DOUBLE_H_ */
