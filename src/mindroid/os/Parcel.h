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

#include <mindroid/io/ByteArrayInputStream.h>
#include <mindroid/io/ByteArrayOutputStream.h>
#include <mindroid/io/DataInputStream.h>
#include <mindroid/io/DataOutputStream.h>
#include <mindroid/lang/Boolean.h>
#include <mindroid/lang/Byte.h>
#include <mindroid/lang/Character.h>
#include <mindroid/lang/Double.h>
#include <mindroid/lang/Float.h>
#include <mindroid/lang/Integer.h>
#include <mindroid/lang/Long.h>
#include <mindroid/lang/NullPointerException.h>
#include <mindroid/lang/Object.h>
#include <mindroid/lang/Short.h>
#include <mindroid/lang/String.h>
#include <mindroid/net/URI.h>
#include <mindroid/os/Bundle.h>
#include <mindroid/os/IBinder.h>
#include <mindroid/util/ArrayList.h>

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

    /**
     * Write a boolean value into the parcel at the current dataPosition(),
     * growing dataCapacity() if needed.
     */
    void putBoolean(bool value);

    /**
     * Write a boolean value into the parcel at the current dataPosition(),
     * growing dataCapacity() if needed.
     */
    void putBoolean(const sp<Boolean>& value) {
        putBoolean(value->value());
    }

    /**
     * Write a byte value into the parcel at the current dataPosition(),
     * growing dataCapacity() if needed.
     */
    void putByte(uint8_t value);

    /**
     * Write a byte value into the parcel at the current dataPosition(),
     * growing dataCapacity() if needed.
     */
    void putByte(const sp<Byte>& value) {
        putByte(value->value());
    }

    /**
     * Write a character value into the parcel at the current dataPosition(),
     * growing dataCapacity() if needed.
     */
    void putChar(char value);

    /**
     * Write a character value into the parcel at the current dataPosition(),
     * growing dataCapacity() if needed.
     */
    void putChar(const sp<Character>& value) {
        putChar(value->value());
    }

    /**
     * Write a short integer value into the parcel at the current dataPosition(),
     * growing dataCapacity() if needed.
     */
    void putShort(int16_t value);

    /**
     * Write a short integer value into the parcel at the current dataPosition(),
     * growing dataCapacity() if needed.
     */
    void putShort(const sp<Short>& value) {
        putShort(value->value());
    }

    /**
     * Write an integer value into the parcel at the current dataPosition(),
     * growing dataCapacity() if needed.
     */
    void putInt(int32_t value);

    /**
     * Write an integer value into the parcel at the current dataPosition(),
     * growing dataCapacity() if needed.
     */
    void putInt(const sp<Integer>& value) {
        putInt(value->value());
    }

    /**
     * Write a long integer value into the parcel at the current dataPosition(),
     * growing dataCapacity() if needed.
     */
    void putLong(int64_t value);

    /**
     * Write a long integer value into the parcel at the current dataPosition(),
     * growing dataCapacity() if needed.
     */
    void putLong(const sp<Long>& value) {
        putLong(value->value());
    }

    /**
     * Write a floating point value into the parcel at the current
     * dataPosition(), growing dataCapacity() if needed.
     */
    void putFloat(float value);

    /**
     * Write a floating point value into the parcel at the current
     * dataPosition(), growing dataCapacity() if needed.
     */
    void putFloat(const sp<Float>& value) {
        putFloat(value->value());
    }

    /**
     * Write a double precision floating point value into the parcel at the
     * current dataPosition(), growing dataCapacity() if needed.
     */
    void putDouble(double value);

    /**
     * Write a double precision floating point value into the parcel at the
     * current dataPosition(), growing dataCapacity() if needed.
     */
    void putDouble(const sp<Double>& value) {
        putDouble(value->value());
    }

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

    sp<ByteArray> getByteArray();

    sp<ByteArrayInputStream> asInputStream();

    sp<ByteArrayOutputStream> asOutputStream();

    sp<Parcel> asInput();

    sp<Parcel> asOutput();

    void checkOutput();

    void checkInput();

    static sp<URI> toUri(const sp<IBinder>& base, const sp<IBinder>& binder);

    static sp<IBinder> fromUri(const sp<URI>& uri);

    /**
     * Returns true if an extra value is associated with the given name.
     *
     * @param name the extra's name
     * @return true if the given extra is present.
     */
    bool hasExtra(const char* name) {
        return hasExtra(String::valueOf(name));
    }
    bool hasExtra(const sp<String>& name) {
        return mExtras != nullptr && mExtras->containsKey(name);
    }

    /**
     * Retrieve extended data from the parcel.
     *
     * @param name The name of the desired item.
     * @param defaultValue the value to be returned if no value of the desired type is stored with
     * the given name.
     *
     * @return the value of an item that previously added with putExtra() or the default value if
     * none was found.
     *
     * @see #putExtra(String, boolean)
     */
    bool getBooleanExtra(const char* name, bool defaultValue) {
        return getBooleanExtra(String::valueOf(name), defaultValue);
    }
    bool getBooleanExtra(const sp<String>& name, bool defaultValue) {
        return mExtras == nullptr ? defaultValue :
            mExtras->getBoolean(name, defaultValue);
    }

    /**
     * Retrieve extended data from the parcel.
     *
     * @param name The name of the desired item.
     * @param defaultValue the value to be returned if no value of the desired type is stored with
     * the given name.
     *
     * @return the value of an item that previously added with putExtra() or the default value if
     * none was found.
     *
     * @see #putExtra(String, byte)
     */
    uint8_t getByteExtra(const char* name, uint8_t defaultValue) {
        return getByteExtra(String::valueOf(name), defaultValue);
    }
    uint8_t getByteExtra(const sp<String>& name, uint8_t defaultValue) {
        return mExtras == nullptr ? defaultValue :
            mExtras->getByte(name, defaultValue);
    }

    /**
     * Retrieve extended data from the parcel.
     *
     * @param name The name of the desired item.
     * @param defaultValue the value to be returned if no value of the desired type is stored with
     * the given name.
     *
     * @return the value of an item that previously added with putExtra() or the default value if
     * none was found.
     *
     * @see #putExtra(String, short)
     */
    uint16_t getShortExtra(const char* name, uint16_t defaultValue) {
        return getShortExtra(String::valueOf(name), defaultValue);
    }
    uint16_t getShortExtra(const sp<String>& name, uint16_t defaultValue) {
        return mExtras == nullptr ? defaultValue :
            mExtras->getShort(name, defaultValue);
    }

    /**
     * Retrieve extended data from the parcel.
     *
     * @param name The name of the desired item.
     * @param defaultValue the value to be returned if no value of the desired type is stored with
     * the given name.
     *
     * @return the value of an item that previously added with putExtra() or the default value if
     * none was found.
     *
     * @see #putExtra(String, char)
     */
    char getCharExtra(const char* name, char defaultValue) {
        return getCharExtra(String::valueOf(name), defaultValue);
    }
    char getCharExtra(const sp<String>& name, char defaultValue) {
        return mExtras == nullptr ? defaultValue :
            mExtras->getChar(name, defaultValue);
    }

    /**
     * Retrieve extended data from the parcel.
     *
     * @param name The name of the desired item.
     * @param defaultValue the value to be returned if no value of the desired type is stored with
     * the given name.
     *
     * @return the value of an item that previously added with putExtra() or the default value if
     * none was found.
     *
     * @see #putExtra(String, int)
     */
    int32_t getIntExtra(const char* name, int32_t defaultValue) {
        return getIntExtra(String::valueOf(name), defaultValue);
    }
    int32_t getIntExtra(const sp<String>& name, int32_t defaultValue) {
        return mExtras == nullptr ? defaultValue :
            mExtras->getInt(name, defaultValue);
    }

    /**
     * Retrieve extended data from the parcel.
     *
     * @param name The name of the desired item.
     * @param defaultValue the value to be returned if no value of the desired type is stored with
     * the given name.
     *
     * @return the value of an item that previously added with putExtra() or the default value if
     * none was found.
     *
     * @see #putExtra(String, long)
     */
    int64_t getLongExtra(const char* name, int64_t defaultValue) {
        return getLongExtra(String::valueOf(name), defaultValue);
    }
    int64_t getLongExtra(const sp<String>& name, int64_t defaultValue) {
        return mExtras == nullptr ? defaultValue :
            mExtras->getLong(name, defaultValue);
    }

    /**
     * Retrieve extended data from the parcel.
     *
     * @param name The name of the desired item.
     * @param defaultValue the value to be returned if no value of the desired type is stored with
     * the given name.
     *
     * @return the value of an item that previously added with putExtra(), or the default value if
     * no such item is present
     *
     * @see #putExtra(String, float)
     */
    float getFloatExtra(const char* name, float defaultValue) {
        return getFloatExtra(String::valueOf(name), defaultValue);
    }
    float getFloatExtra(const sp<String>& name, float defaultValue) {
        return mExtras == nullptr ? defaultValue :
            mExtras->getFloat(name, defaultValue);
    }

    /**
     * Retrieve extended data from the parcel.
     *
     * @param name The name of the desired item.
     * @param defaultValue the value to be returned if no value of the desired type is stored with
     * the given name.
     *
     * @return the value of an item that previously added with putExtra() or the default value if
     * none was found.
     *
     * @see #putExtra(String, double)
     */
    double getDoubleExtra(const char* name, double defaultValue) {
        return getDoubleExtra(String::valueOf(name), defaultValue);
    }
    double getDoubleExtra(const sp<String>& name, double defaultValue) {
        return mExtras == nullptr ? defaultValue :
            mExtras->getDouble(name, defaultValue);
    }

    /**
     * Retrieve extended data from the parcel.
     *
     * @param name The name of the desired item.
     *
     * @return the value of an item that previously added with putExtra() or null if no String value
     * was found.
     *
     * @see #putExtra(String, String)
     */
    sp<String> getStringExtra(const char* name) {
        return getStringExtra(String::valueOf(name));
    }
    sp<String> getStringExtra(const sp<String>& name) {
        return mExtras == nullptr ? nullptr : mExtras->getString(name);
    }

    /**
     * Retrieve extended data from the parcel.
     *
     * @param name The name of the desired item.
     *
     * @return the value of an item that previously added with putExtra() or null if no
     * ArrayList<Integer> value was found.
     *
     * @see #putIntegerArrayListExtra(String, ArrayList)
     */
    sp<ArrayList<int32_t>> getIntegerArrayListExtra(const char* name) {
        return getIntegerArrayListExtra(String::valueOf(name));
    }
    sp<ArrayList<int32_t>> getIntegerArrayListExtra(const sp<String>& name) {
        return mExtras == nullptr ? nullptr : mExtras->getIntegerArrayList(name);
    }

    /**
     * Retrieve extended data from the parcel.
     *
     * @param name The name of the desired item.
     *
     * @return the value of an item that previously added with putExtra() or null if no
     * ArrayList<String> value was found.
     *
     * @see #putStringArrayListExtra(String, ArrayList)
     */
    sp<ArrayList<sp<String>>> getStringArrayListExtra(const char* name) {
        return getStringArrayListExtra(String::valueOf(name));
    }
    sp<ArrayList<sp<String>>> getStringArrayListExtra(const sp<String>& name) {
        return mExtras == nullptr ? nullptr : mExtras->getStringArrayList(name);
    }

    /**
     * Retrieve extended data from the parcel.
     *
     * @param name The name of the desired item.
     *
     * @return the value of an item that previously added with putExtra() or null if no Bundle value
     * was found.
     *
     * @see #putExtra(String, Bundle)
     */
    sp<Bundle> getBundleExtra(const char* name) {
        return getBundleExtra(String::valueOf(name));
    }
    sp<Bundle> getBundleExtra(const sp<String>& name) {
        return mExtras == nullptr ? nullptr : mExtras->getBundle(name);
    }

    /**
     * Retrieves a map of extended data from the parcel.
     *
     * @return the map of all extras previously added with putExtra(), or null if none have been
     * added.
     */
    sp<Bundle> getExtras() {
        return (mExtras != nullptr)
                ? new Bundle(mExtras)
                : nullptr;
    }

    /**
     * Add extended data to the parcel.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The boolean data value.
     *
     * @return Returns the same Parcel object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getBooleanExtra(String, boolean)
     */
    sp<Parcel> putExtra(const char* name, bool value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Parcel> putExtra(const sp<String>& name, bool value);

    /**
     * Add extended data to the parcel.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The byte data value.
     *
     * @return Returns the same Parcel object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getByteExtra(String, byte)
     */
    sp<Parcel> putExtra(const char* name, uint8_t value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Parcel> putExtra(const sp<String>& name, uint8_t value);

    /**
     * Add extended data to the parcel.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The char data value.
     *
     * @return Returns the same Parcel object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getCharExtra(String, char)
     */
    sp<Parcel> putExtra(const char* name, char value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Parcel> putExtra(const sp<String>& name, char value);

    /**
     * Add extended data to the parcel.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The short data value.
     *
     * @return Returns the same Parcel object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getShortExtra(String, short)
     */
    sp<Parcel> putExtra(const char* name, int16_t value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Parcel> putExtra(const sp<String>& name, int16_t value);

    /**
     * Add extended data to the parcel.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The integer data value.
     *
     * @return Returns the same Parcel object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getIntExtra(String, int)
     */
    sp<Parcel> putExtra(const char* name, int32_t value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Parcel> putExtra(const sp<String>& name, int32_t value);

    /**
     * Add extended data to the parcel.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The long data value.
     *
     * @return Returns the same Parcel object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getLongExtra(String, long)
     */
    sp<Parcel> putExtra(const char* name, int64_t value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Parcel> putExtra(const sp<String>& name, int64_t value);

    /**
     * Add extended data to the parcel.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The float data value.
     *
     * @return Returns the same Parcel object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getFloatExtra(String, float)
     */
    sp<Parcel> putExtra(const char* name, float value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Parcel> putExtra(const sp<String>& name, float value);

    /**
     * Add extended data to the parcel.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The double data value.
     *
     * @return Returns the same Parcel object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getDoubleExtra(String, double)
     */
    sp<Parcel> putExtra(const char* name, double value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Parcel> putExtra(const sp<String>& name, double value);

    /**
     * Add extended data to the parcel.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The String data value.
     *
     * @return Returns the same Parcel object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getStringExtra(String)
     */
    sp<Parcel> putExtra(const char* name, const char* value) {
        return putExtra(String::valueOf(name), String::valueOf(value));
    }
    sp<Parcel> putExtra(const char* name, const sp<String>& value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Parcel> putExtra(const sp<String>& name, const sp<String>& value);

    /**
     * Add extended data to the parcel.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The ArrayList<Integer> data value.
     *
     * @return Returns the same Parcel object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getIntegerArrayListExtra(String)
     */
    sp<Parcel> putIntegerArrayListExtra(const char* name, sp<ArrayList<int32_t>> value) {
        return putIntegerArrayListExtra(String::valueOf(name), value);
    }
    sp<Parcel> putIntegerArrayListExtra(const sp<String>& name, sp<ArrayList<int32_t>> value);

    /**
     * Add extended data to the parcel.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The ArrayList<String> data value.
     *
     * @return Returns the same Parcel object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getStringArrayListExtra(String)
     */
    sp<Parcel> putStringArrayListExtra(const char* name, const sp<ArrayList<sp<String>>>& value) {
        return putStringArrayListExtra(String::valueOf(name), value);
    }
    sp<Parcel> putStringArrayListExtra(const sp<String>& name, const sp<ArrayList<sp<String>>>& value);

    /**
     * Add extended data to the parcel. The name must include a package prefix, for example the app
     * com.android.contacts would use names like "com.android.contacts.ShowAll".
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The Bundle data value.
     *
     * @return Returns the same Parcel object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getBundleExtra(String)
     */
    sp<Parcel> putExtra(const char* name, const sp<Bundle>& value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Parcel> putExtra(const sp<String>& name, const sp<Bundle>& value);

    /**
     * Add a set of extended data to the parcel.
     *
     * @param extras The Bundle of extras to add to this parcel.
     *
     * @see #putExtra
     * @see #removeExtra
     */
    sp<Parcel> putExtras(const sp<Bundle>& extras);

    /**
     * Completely replace the extras in the Parcel with the given Bundle of extras.
     *
     * @param extras The new set of extras in the Parcel, or null to erase all extras.
     */
    sp<Parcel> replaceExtras(const sp<Bundle>& extras) {
        mExtras = extras != nullptr ? new Bundle(extras) : nullptr;
        return this;
    }

    /**
     * Remove extended data from the parcel.
     *
     * @see #putExtra
     */
    void removeExtra(const char* name) {
        removeExtra(String::valueOf(name));
    }
    void removeExtra(const sp<String>& name);

private:
    Parcel();
    Parcel(size_t size);
    Parcel(const sp<ByteArray>& buffer, size_t offset, size_t size);

    sp<ByteArrayOutputStream> mOutputStream;
    sp<ByteArrayInputStream> mInputStream;
    sp<DataOutputStream> mDataOutputStream;
    sp<DataInputStream> mDataInputStream;
    sp<Bundle> mExtras;
};

} /* namespace mindroid */

#endif /* MINDROID_OS_PARCEL_H_ */
