/*
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (C) 2013 Daniel Himmelein
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

#ifndef MINDROID_OS_PARCEL_H_
#define MINDROID_OS_PARCEL_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/lang/NullPointerException.h>
#include <mindroid/os/IBinder.h>
#include <mindroid/io/ByteArrayOutputStream.h>
#include <mindroid/io/ByteArrayInputStream.h>
#include <mindroid/io/DataOutputStream.h>
#include <mindroid/io/DataInputStream.h>
#include <mindroid/net/URI.h>

namespace mindroid {

class Parcel final : public Object {
public:
    /**
     * Retrieve a new Parcel object from the pool.
     */
    static sp<Parcel> obtain();

    static sp<Parcel> obtain(size_t size);

    static sp<Parcel> obtain(const sp<ByteArray>& buffer);

    static sp<Parcel> obtain(const sp<ByteArray>& buffer, size_t offset, size_t size);

    /**
     * Put a Parcel object back into the pool.  You must not touch
     * the object after this call.
     */
    void recycle() {
    }

    /**
     * Returns the total amount of data contained in the parcel.
     */
    size_t size() {
        return mOutputStream->size();
    }

    void putBoolean(bool value);

    void putByte(uint8_t value);

    void putChar(char value);

    void putShort(short value);

    /**
     * Write an integer value into the parcel at the current dataPosition(),
     * growing dataCapacity() if needed.
     */
    void putInt(int32_t value);

    /**
     * Write a long integer value into the parcel at the current dataPosition(),
     * growing dataCapacity() if needed.
     */
    void putLong(uint64_t value);

    /**
     * Write a floating point value into the parcel at the current
     * dataPosition(), growing dataCapacity() if needed.
     */
    void putFloat(float value);

    /**
     * Write a double precision floating point value into the parcel at the
     * current dataPosition(), growing dataCapacity() if needed.
     */
    void putDouble(double value);

    /**
     * Write a string value into the parcel at the current dataPosition(),
     * growing dataCapacity() if needed.
     */
    void putString(const sp<String>& value);

    void putBytes(const sp<ByteArray>& buffer);

    void putBytes(const sp<ByteArray>& buffer, size_t offset, size_t size);

    void putBinder(const sp<IBinder>& binder);

    void putBinder(const sp<IBinder>& base, const sp<IBinder>& binder);

    bool getBoolean();

    uint8_t getByte();

    char getChar();

    uint16_t getShort();

    /**
     * Read an integer value from the parcel at the current dataPosition().
     */
    int32_t getInt();

    /**
     * Read a long integer value from the parcel at the current dataPosition().
     */
    uint64_t getLong();

    /**
     * Read a floating point value from the parcel at the current
     * dataPosition().
     * @throws IOException 
     */
    float getFloat();

    /**
     * Read a double precision floating point value from the parcel at the
     * current dataPosition().
     * @throws IOException 
     */
    double getDouble();

    /**
     * Read a string value from the parcel at the current dataPosition().
     * @throws IOException 
     */
    sp<String> getString();

    sp<ByteArray> getBytes();

    sp<ByteArray> getBytes(size_t size);

    sp<IBinder> getBinder();

    sp<ByteArray> toByteArray();

    sp<ByteArrayInputStream> asInputStream();

    sp<ByteArrayOutputStream> asOutputStream();

    sp<Parcel> asInput();

    sp<Parcel> asOutput();

    void checkOutput();

    void checkInput();

    static sp<URI> toUri(const sp<IBinder>& base, const sp<IBinder>& binder);

    static sp<IBinder> fromUri(const sp<URI>& uri);

private:
    Parcel();
    Parcel(size_t size);
    Parcel(const sp<ByteArray>& buffer, size_t offset, size_t size);

    sp<ByteArrayOutputStream> mOutputStream;
    sp<ByteArrayInputStream> mInputStream;
    sp<DataOutputStream> mDataOutputStream;
    sp<DataInputStream> mDataInputStream;
};

} /* namespace mindroid */

#endif /* MINDROID_OS_PARCEL_H_ */
