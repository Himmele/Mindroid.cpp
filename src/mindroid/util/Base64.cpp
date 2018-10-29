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

#include <mindroid/util/Base64.h>

namespace mindroid {

/* Base64 characters */
static const sp<String> sChars = String::valueOf(
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/");

/* Base64 offset index table */
static const int sIndex[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  62, 63, 62, 62, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0,  0,  0,  0,  0,  0,
    0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,  0,  0,  0,  63,
    0,  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

sp<ArrayList<uint8_t>> Base64::Decoder::decode(const sp<ArrayList<uint8_t>>& src) const {
    const sp<ArrayList<uint8_t>> dst = new ArrayList<uint8_t>();
    decode(src, dst);
    return dst;
}

int32_t Base64::Decoder::decode(const sp<ArrayList<uint8_t>>& src, const sp<ArrayList<uint8_t>>& dst) const {
    int32_t a = 0;
    int32_t b = -8;

    for (uint8_t c : src->arr()) {
        if (sIndex[c] == -1) {
            break;
        }
        a = (a << 6) + sIndex[c];
        b += 6;
        if (b >= 0) {
            dst->add(char((a >> b) & 0xFF));
            b -= 8;
        }
    }
    return dst->size();
}

sp<ArrayList<uint8_t>> Base64::Decoder::decode(const sp<String>& src) const {
    const sp<ByteArray> byteArray = src->getBytes();
    const sp<ArrayList<uint8_t>> list = new ArrayList<uint8_t>(byteArray->c_arr(), byteArray->size());
    return decode(list);
}

sp<ArrayList<uint8_t>> Base64::Encoder::encode(const sp<ArrayList<uint8_t>>& src) const {
    const sp<ArrayList<uint8_t>> dst = new ArrayList<uint8_t>();
    encode(src, dst);
    return dst;
}

int32_t Base64::Encoder::encode(const sp<ArrayList<uint8_t>>& src, const sp<ArrayList<uint8_t>>& dst) const {
    int32_t a = 0;
    int32_t b = -6;

    for (uint8_t c : src->arr()) {
        a = (a << 8) + c;
        b += 8;
        while (b >= 0) {
            dst->add(sChars->charAt((a >>b) & 0x3F));
            b-=6;
        }
    }

    if (b >- 6) {
        dst->add(sChars->charAt(((a << 8) >> (b + 8)) & 0x3F));
    }

    while (dst->size() % 4) {
        dst->add('=');
    }

    return dst->size();
}

sp<String> Base64::Encoder::encodeToString(const sp<ArrayList<uint8_t>>& src) const {
    const sp<ArrayList<uint8_t>> dst = new ArrayList<uint8_t>(src->size());
    encode(src, dst);
    dst->add('\0');
    return new String(dst->c_arr(), dst->size());
}

} /* namespace mindroid */
