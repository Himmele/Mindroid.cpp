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

#ifndef MINDROID_UTIL_BASE64_H_
#define MINDROID_UTIL_BASE64_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/nio/ByteBuffer.h>
#include <mindroid/util/ArrayList.h>

namespace mindroid {


class Base64 final {
public:
    /** This class implements a decoder for decoding byte data using the Base64 encoding scheme */
    class Decoder : public Object {
    public:
        /** Decodes all bytes from the input byte array using the Base64 encoding scheme, writing the results into a newly-allocated output byte array. */
        sp<ArrayList<uint8_t>> decode(const sp<ArrayList<uint8_t>>& src) const;

        /** Decodes all bytes from the input byte array using the Base64 encoding scheme, writing the results into the given output byte array, starting at offset 0. */
        int32_t decode(const sp<ArrayList<uint8_t>>& src, const sp<ArrayList<uint8_t>>& dst) const;

        /** Decodes a Base64 encoded String into a newly-allocated byte array using the Base64 encoding scheme. */
        sp<ArrayList<uint8_t>> decode(const sp<String>& src) const;
    };

    /** This class implements an encoder for encoding byte data using the Base64 encoding scheme */
    class Encoder : public Object {
    public:
        /** Encodes all bytes from the specified byte array into a newly-allocated byte array using the Base64 encoding scheme. */
        sp<ArrayList<uint8_t>> encode(const sp<ArrayList<uint8_t>>& src) const;

        /** Encodes all bytes from the specified byte array using the Base64 encoding scheme, writing the resulting bytes to the given output byte array, starting at offset 0. */
        int32_t encode(const sp<ArrayList<uint8_t>>& src, const sp<ArrayList<uint8_t>>& dst) const;

        /** Encodes the specified byte array into a String using the Base64 encoding scheme. */
        sp<String> encodeToString(const sp<ArrayList<uint8_t>>& src) const;
    };

    /** Returns a Base64.Decoder that decodes using the Basic type base64 encoding scheme. */
    static sp<Decoder> getDecoder() {
        return new Decoder();
    }
    /** Returns a Base64.Encoder that encodes using the Basic type base64 encoding scheme. */
    static sp<Encoder> getEncoder() {
        return new Encoder();
    }
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_BASE64_H_ */
