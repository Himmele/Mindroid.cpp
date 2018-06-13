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

#include <mindroid/os/Parcel.h>
#include <mindroid/os/RemoteException.h>
#include <mindroid/lang/Math.h>
#include <mindroid/lang/IllegalStateException.h>
#include <mindroid/io/IOException.h>
#include <mindroid/net/URISyntaxException.h>
#include <mindroid/runtime/system/Runtime.h>

namespace mindroid {

Parcel::Parcel() {
    mOutputStream = new ByteArrayOutputStream();
    mDataOutputStream = new DataOutputStream(mOutputStream);
}

Parcel::Parcel(size_t size) {
    mOutputStream = new ByteArrayOutputStream(size);
    mDataOutputStream = new DataOutputStream(mOutputStream);
}

Parcel::Parcel(const sp<ByteArray>& buffer, size_t offset, size_t size) {
    if (offset == 0 && size == buffer->size()) {
        mOutputStream = new ByteArrayOutputStream(buffer);
    } else {
        mOutputStream = new ByteArrayOutputStream(size);
        mOutputStream->write(buffer, offset, size);
    }
    mDataOutputStream = new DataOutputStream(mOutputStream);
    asInputStream();
}

sp<Parcel> Parcel::obtain() {
    return new Parcel();
}

sp<Parcel> Parcel::obtain(size_t size) {
    return new Parcel(size);
}

sp<Parcel> Parcel::obtain(const sp<ByteArray>& buffer) {
    if (buffer == nullptr) {
        throw NullPointerException();
    }
    return new Parcel(buffer, 0, buffer->size());
}

sp<Parcel> Parcel::obtain(const sp<ByteArray>& buffer, size_t offset, size_t size) {
    if (buffer == nullptr) {
        throw NullPointerException();
    }
    return new Parcel(buffer, offset, size);
}

void Parcel::putBoolean(bool value) {
    checkOutput();
    try {
        mDataOutputStream->writeBoolean(value);
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

void Parcel::putByte(uint8_t value) {
    checkOutput();
    try {
        mDataOutputStream->writeByte(value);
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

void Parcel::putChar(char value) {
    checkOutput();
    try {
        mDataOutputStream->writeChar(value);
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

void Parcel::putShort(short value) {
    checkOutput();
    try {
        mDataOutputStream->writeShort(value);
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

void Parcel::putInt(int32_t value) {
    checkOutput();
    try {
        mDataOutputStream->writeInt(value);
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

void Parcel::putLong(uint64_t value) {
    checkOutput();
    try {
        mDataOutputStream->writeLong(value);
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

void Parcel::putFloat(float value) {
    checkOutput();
    try {
        mDataOutputStream->writeFloat(value);
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

void Parcel::putDouble(double value) {
    checkOutput();
    try {
        mDataOutputStream->writeDouble(value);
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

void Parcel::putString(const sp<String>& value) {
    checkOutput();
    try {
        mDataOutputStream->writeUTF(value);
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

void Parcel::putBytes(const sp<ByteArray>& buffer) {
    checkOutput();
    mOutputStream->write(buffer);
}

void Parcel::putBytes(const sp<ByteArray>& buffer, size_t offset, size_t size) {
    checkOutput();
    mOutputStream->write(buffer, offset, size);
}

void Parcel::putBinder(const sp<IBinder>& binder) {
    try {
        sp<URI> descriptor = new URI(binder->getInterfaceDescriptor());
        sp<URI> uri = new URI(binder->getUri()->getScheme(), binder->getUri()->getAuthority(), String::format("/if=%s", descriptor->getPath()->substring(1)->c_str()), nullptr, nullptr);
        putString(uri->toString());
    } catch (const URISyntaxException& e) {
        throw RemoteException(e);
    }
}

void Parcel::putBinder(const sp<IBinder>& base, const sp<IBinder>& binder) {
    try {
        sp<URI> descriptor = new URI(binder->getInterfaceDescriptor());
        sp<URI> uri = new URI(base->getUri()->getScheme(), binder->getUri()->getAuthority(), String::format("/if=%s", descriptor->getPath()->substring(1)->c_str()), nullptr, nullptr);
        putString(uri->toString());
    } catch (const URISyntaxException& e) {
        throw RemoteException(e);
    }
}

bool Parcel::getBoolean() {
    checkInput();
    try {
        return mDataInputStream->readBoolean();
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

uint8_t Parcel::getByte() {
    checkInput();
    try {
        return mDataInputStream->readByte();
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

char Parcel::getChar() {
    checkInput();
    try {
        return mDataInputStream->readChar();
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

uint16_t Parcel::getShort() {
    checkInput();
    try {
        return mDataInputStream->readShort();
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

int32_t Parcel::getInt() {
    checkInput();
    try {
        return mDataInputStream->readInt();
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

uint64_t Parcel::getLong() {
    checkInput();
    try {
        return mDataInputStream->readLong();
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

float Parcel::getFloat() {
    checkInput();
    try {
        return mDataInputStream->readFloat();
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

double Parcel::getDouble() {
    checkInput();
    try {
        return mDataInputStream->readDouble();
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

sp<String> Parcel::getString() {
    checkInput();
    try {
        return mDataInputStream->readUTF();
    } catch (const IOException& e) {
        throw RemoteException(e);
    }
}

sp<ByteArray> Parcel::getBytes() {
    checkInput();
    sp<ByteArray> buffer = new ByteArray(mInputStream->available());
    mInputStream->read(buffer);
    return buffer;
}

sp<ByteArray> Parcel::getBytes(size_t size) {
    checkInput();
    size = Math::min(mInputStream->available(), size);
    sp<ByteArray> buffer = new ByteArray(size);
    mInputStream->read(buffer, 0, size);
    return buffer;
}

sp<IBinder> Parcel::getBinder() {
    sp<URI> uri;
    try {
        uri = new URI(getString());
    } catch (const URISyntaxException& e) {
        return nullptr;
    }
    return Runtime::getRuntime()->getBinder(uri);
}

sp<ByteArray> Parcel::toByteArray() {
    return mOutputStream->getByteArray();
}

sp<ByteArrayInputStream> Parcel::asInputStream() {
    if (mInputStream == nullptr) {
        mInputStream = new ByteArrayInputStream(mOutputStream->getByteArray());
        mDataInputStream = new DataInputStream(mInputStream);
    }
    return mInputStream;
}

sp<ByteArrayOutputStream> Parcel::asOutputStream() {
    if (mInputStream != nullptr) {
        mInputStream = nullptr;
        mDataInputStream = nullptr;
    }
    return mOutputStream;
}

sp<Parcel> Parcel::asInput() {
    asInputStream();
    return this;
}

sp<Parcel> Parcel::asOutput() {
    asOutputStream();
    return this;
}

void Parcel::checkOutput() {
    if (mInputStream != nullptr) {
        throw IllegalStateException("Parcel is in input mode");
    }
}

void Parcel::checkInput() {
    if (mInputStream == nullptr) {
        throw IllegalStateException("Parcel is in output mode");
    }
}

sp<URI> Parcel::toUri(const sp<IBinder>& base, const sp<IBinder>& binder) {
    try {
        sp<URI> descriptor = new URI(binder->getInterfaceDescriptor());
        sp<URI> uri = new URI(base->getUri()->getScheme(), binder->getUri()->getAuthority(), String::format("/if=%s", descriptor->getPath()->substring(1)->c_str()), nullptr, nullptr);
        return uri;
    } catch (const URISyntaxException& e) {
        throw RemoteException(e);
    }
}

sp<IBinder> Parcel::fromUri(const sp<URI>& uri) {
    return Runtime::getRuntime()->getBinder(uri);
}

sp<Parcel> Parcel::putExtra(const sp<String>& name, bool value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putBoolean(name, value);
    return this;
}

sp<Parcel> Parcel::putExtra(const sp<String>& name, uint8_t value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putByte(name, value);
    return this;
}

sp<Parcel> Parcel::putExtra(const sp<String>& name, char value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putChar(name, value);
    return this;
}

sp<Parcel> Parcel::putExtra(const sp<String>& name, int16_t value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putShort(name, value);
    return this;
}

sp<Parcel> Parcel::putExtra(const sp<String>& name, int32_t value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putInt(name, value);
    return this;
}

sp<Parcel> Parcel::putExtra(const sp<String>& name, int64_t value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putLong(name, value);
    return this;
}

sp<Parcel> Parcel::putExtra(const sp<String>& name, float value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putFloat(name, value);
    return this;
}

sp<Parcel> Parcel::putExtra(const sp<String>& name, double value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putDouble(name, value);
    return this;
}

sp<Parcel> Parcel::putExtra(const sp<String>& name, const sp<String>& value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putString(name, value);
    return this;
}

sp<Parcel> Parcel::putIntegerArrayListExtra(const sp<String>& name, sp<ArrayList<int32_t>> value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putIntegerArrayList(name, value);
    return this;
}

sp<Parcel> Parcel::putStringArrayListExtra(const sp<String>& name, const sp<ArrayList<sp<String>>>& value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putStringArrayList(name, value);
    return this;
}

sp<Parcel> Parcel::putExtras(const sp<Bundle>& extras) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putAll(extras);
    return this;
}

void Parcel::removeExtra(const sp<String>& name) {
    if (mExtras != nullptr) {
        mExtras->remove(name);
        if (mExtras->size() == 0) {
            mExtras = nullptr;
        }
    }
}

} /* namespace mindroid */
