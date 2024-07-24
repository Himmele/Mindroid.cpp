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

#include <mindroid/os/Binder.h>
#include <mindroid/os/Parcel.h>
#include <mindroid/lang/Integer.h>
#include <mindroid/lang/NullPointerException.h>
#include <mindroid/lang/NumberFormatException.h>
#include <mindroid/net/URI.h>
#include <mindroid/net/URISyntaxException.h>
#include <mindroid/runtime/system/Runtime.h>
#include <mindroid/util/concurrent/Executors.h>
#include <mindroid/util/concurrent/Promise.h>

namespace mindroid {

const char* const Binder::TAG = "Binder";
const sp<String> Binder::EXCEPTION_MESSAGE = String::valueOf("Binder transaction failure");
const sp<String> Binder::Proxy::EXCEPTION_MESSAGE = String::valueOf("Binder transaction failure");

Binder::Binder() {
    setObjectLifetime(Object::OBJECT_LIFETIME_WEAK_REFERENCE);
    mRuntime = Runtime::getRuntime();
    mTarget = new Messenger(*this);
    mId = mRuntime->attachBinder(this);
    setObjectLifetime(Object::OBJECT_LIFETIME_STRONG_REFERENCE);
}

Binder::Binder(const sp<Looper>& looper) {
    setObjectLifetime(Object::OBJECT_LIFETIME_WEAK_REFERENCE);
    mRuntime = Runtime::getRuntime();
    mTarget = new Messenger(*this, looper);
    mId = mRuntime->attachBinder(this);
    setObjectLifetime(Object::OBJECT_LIFETIME_STRONG_REFERENCE);
}

Binder::Binder(const sp<Executor>& executor) {
    setObjectLifetime(Object::OBJECT_LIFETIME_WEAK_REFERENCE);
    mRuntime = Runtime::getRuntime();
    mTarget = new ExecutorMessenger(*this, executor);
    mId = mRuntime->attachBinder(this);
    setObjectLifetime(Object::OBJECT_LIFETIME_STRONG_REFERENCE);
}

Binder::Binder(const sp<Binder>& binder) {
    mRuntime = binder->mRuntime;
    if (Class<Messenger>::isInstance(binder->mTarget)) {
        mTarget = new Messenger(*this, Class<Messenger>::cast(binder->mTarget)->mHandler->getLooper());
    } else {
        mTarget = binder->mTarget;
    }
    mId = binder->mId;
}

Binder::~Binder() {
    mRuntime->detachBinder(mId, mUri);
}

void Binder::attachInterface(const wp<IInterface>& owner, const sp<String>& descriptor) {
    mOwner = owner;
    mDescriptor = descriptor;

    setObjectLifetime(Object::OBJECT_LIFETIME_WEAK_REFERENCE);
    try {
        sp<URI> uri = new URI(mDescriptor);
        if (uri->getScheme() == nullptr) {
            throw URISyntaxException("Scheme must not be null");
        }
        uint32_t nodeId = (uint32_t) ((mId >> 32) & 0xFFFFFFFFL);
        uint32_t id = (uint32_t) (mId & 0xFFFFFFFFL);
        mUri = new URI(uri->getScheme(), String::format("%u.%u", nodeId, id), nullptr, nullptr, nullptr);
        mRuntime->attachBinder(mUri, this);
    } catch (const URISyntaxException& e) {
        Log::e(TAG, "Failed to attach interface to runtime system");
    }
    setObjectLifetime(Object::OBJECT_LIFETIME_STRONG_REFERENCE);
}

sp<Promise<sp<Parcel>>> Binder::transact(int32_t what, const sp<Parcel>& data, int32_t flags) {
    if (data != nullptr) {
        data->asInput();
    }
    sp<Message> message = Message::obtain();
    message->what = TRANSACTION;
    message->arg1 = what;
    message->obj = data;
    sp<Promise<sp<Parcel>>> promise;
    if ((flags & FLAG_ONEWAY) != 0 && (flags & (FLAG_ONEWAY_WITH_EXCEPTION_HANDLING ^ FLAG_ONEWAY)) == 0) {
        message->result = nullptr;
        promise = nullptr;
    } else {
        sp<Promise<sp<Parcel>>> p = new Promise<sp<Parcel>>(Executors::SYNCHRONOUS_EXECUTOR);
        message->result = p;
        promise = p->then([=] (const sp<Parcel>& parcel) {
            parcel->asInput();
        });
    }
    if (!mTarget->send(message)) {
        throw RemoteException(EXCEPTION_MESSAGE);
    }
    return promise;
}

void Binder::transact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Thenable>& result, int32_t flags) {
    sp<Message> message = Message::obtain();
    message->what = LIGHTWEIGHT_TRANSACTION;
    message->arg1 = what;
    message->arg2 = num;
    message->obj = obj;
    message->setData(data);
    message->result = result;
    if (!mTarget->send(message)) {
        throw RemoteException(EXCEPTION_MESSAGE);
    }
}

void Binder::onTransact(const sp<Message>& message) {
    try {
        switch (message->what) {
        case TRANSACTION:
            onTransact(message->arg1, object_cast<Parcel>(message->obj), object_cast<Promise<sp<Parcel>>>(message->result));
            break;
        case LIGHTWEIGHT_TRANSACTION:
            onTransact(message->arg1, message->arg2, message->obj, message->peekData(), message->result);
            break;
        default:
            break;
        }
    } catch (const RemoteException& e) {
        if (message->result != nullptr) {
            switch (message->what) {
            case TRANSACTION:
                object_cast<Promise<sp<Parcel>>>(message->result)->completeWith(e);
                break;
            case LIGHTWEIGHT_TRANSACTION:
                object_cast<Promise<sp<Object>>>(message->result)->completeWith(e);
                break;
            default:
                Log::w(TAG, EXCEPTION_MESSAGE->c_str());
                break;
            }
        } else {
            Log::w(TAG, EXCEPTION_MESSAGE->c_str());
        }
    }
    message->result = nullptr;
}

void Binder::link(const sp<Supervisor>& supervisor, const sp<Bundle>& extras) {
}

bool Binder::unlink(const sp<Supervisor>& supervisor, const sp<Bundle>& extras) {
    return true;
}

bool Binder::equals(const sp<Object>& obj) const {
    if (obj == nullptr) return false;
    if (obj == this) return true;
    if (Class<IBinder>::isInstance(obj)) {
        sp<IBinder> other = Class<IBinder>::cast(obj);
        return getId() == other->getId();
    } else {
        return false;
    }
}

size_t Binder::hashCode() const {
    const uint64_t id = getId();

    size_t hash = 0;
    for (size_t i = 0; i < sizeof(id); i++) {
        hash = 31 * hash + reinterpret_cast<const uint8_t*>(&id)[i];
    }
    return hash;
}

void Binder::setId(uint64_t id) {
    mId = id;
    mUri = URI::create(String::format("%s://%u.%u", mUri->getScheme()->c_str(), (uint32_t) ((mId >> 32) & 0xFFFFFFFFL), (uint32_t) (mId & 0xFFFFFFFFL)));
}

sp<Binder::Proxy> Binder::Proxy::create(const sp<URI>& uri) {
    sp<Binder::Proxy> proxy = new Binder::Proxy(uri);
    proxy->mProxyId = proxy->mRuntime->attachProxy(proxy);
    return proxy;
}

Binder::Proxy::Proxy(const sp<URI>& uri) {
    if (uri == nullptr) {
        throw IllegalArgumentException("Invalid URI: nullptr");
    }
    mRuntime = Runtime::getRuntime();
    sp<String> authority = uri->getAuthority();
    sp<StringArray> parts = authority->split(".");
    if (parts->size() == 2) {
        try {
            mId = ((uint64_t) Integer::valueOf(parts->get(0))->intValue() << 32) | ((uint64_t) Integer::valueOf(parts->get(1))->intValue() & 0xFFFFFFFFL);
        } catch (const NumberFormatException& e) {
            throw IllegalArgumentException(String::format("Invalid URI: %s", uri->toString()->c_str()));
        }
    } else {
        throw IllegalArgumentException(String::format("Invalid URI: %s", uri->toString()->c_str()));
    }
    sp<String> path = uri->getPath();
    if (path != nullptr && !path->isEmpty()) {
        sp<StringArray> pairs = path->substring(1)->split(",");
        for (size_t i = 0; i < pairs->size(); i++) {
            sp<String> pair = pairs->get(i);
            ssize_t j = pair->indexOf("=");
            if (j >= 0) {
                sp<String> key = pair->substring(0, j)->trim();
                sp<String> value = pair->substring(j + 1)->trim();
                if (key->equals("if")) {
                    if (uri->getQuery() == nullptr) {
                        mDescriptor = String::format("%s://interfaces/%s", uri->getScheme()->c_str(), value->c_str());
                    } else {
                        mDescriptor = String::format("%s://interfaces/%s?%s", uri->getScheme()->c_str(), value->c_str(), uri->getQuery()->c_str());
                    }
                    break;
                }
            }
        }
    }
    if (mDescriptor == nullptr) {
        throw IllegalArgumentException(String::format("Invalid URI: %s", uri->toString()->c_str()));
    }
    mUri = URI::create(String::format("%s://%s", uri->getScheme()->c_str(), uri->getAuthority()->c_str()));
}

Binder::Proxy::~Proxy() {
    mRuntime->detachProxy(mId, mUri, mProxyId);
}

sp<Promise<sp<Parcel>>> Binder::Proxy::transact(int32_t what, const sp<Parcel>& data, int32_t flags) {
    return mRuntime->transact(this, what, data, flags);
}

void Binder::Proxy::transact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Thenable>& result, int32_t flags) {
    throw RemoteException(EXCEPTION_MESSAGE);
}

void Binder::Proxy::link(const sp<Supervisor>& supervisor, const sp<Bundle>& extras) {
    if (supervisor == nullptr) {
        throw NullPointerException("supervisor is null");
    }
    mRuntime->link(this, supervisor, extras);
}

bool Binder::Proxy::unlink(const sp<Supervisor>& supervisor, const sp<Bundle>& extras) {
    return mRuntime->unlink(this, supervisor, extras);
}

bool Binder::Proxy::equals(const sp<Object>& obj) const {
    if (obj == nullptr) return false;
    if (obj == this) return true;
    if (Class<IBinder>::isInstance(obj)) {
        sp<IBinder> other = Class<IBinder>::cast(obj);
        return getId() == other->getId();
    } else {
        return false;
    }
}

size_t Binder::Proxy::hashCode() const {
    const uint64_t id = getId();

    size_t hash = 0;
    for (size_t i = 0; i < sizeof(id); i++) {
        hash = 31 * hash + reinterpret_cast<const uint8_t*>(&id)[i];
    }
    return hash;
}

} /* namespace mindroid */
