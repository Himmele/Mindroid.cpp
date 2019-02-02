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

#ifndef MINDROID_OS_IBINDER_H_
#define MINDROID_OS_IBINDER_H_

#include <mindroid/lang/Object.h>

namespace mindroid {

class Bundle;
class IInterface;
class Looper;
class Message;
class String;
class Variant;
class URI;
class Parcel;
template<typename T> class Promise;

/**
 * Base interface for a remotable object, the core part of a lightweight remote procedure call
 * mechanism designed for high performance when performing in-process and cross-process calls. This
 * interface describes the abstract protocol for interacting with a remotable object. Do not
 * implement this interface directly, instead extend from {@link Binder}.
 *
 * @see Binder
 */
class IBinder :
        public virtual Object {
public:
    /**
     * Flag to {@link #transact}: this is a one-way call, meaning that the caller returns
     * immediately, without waiting for a result from the callee. Applies only if the caller and
     * callee are in different processes.
     */
    static const int32_t FLAG_ONEWAY = 0x00000001;

    /**
     * Returns the binder's id.
     */
    virtual uint64_t getId() const = 0;

    /**
     * Returns the binder's URI.
     */
    virtual sp<URI> getUri() const = 0;

    /**
     * Get the canonical name of the interface supported by this binder.
     */
    virtual sp<String> getInterfaceDescriptor() const = 0;

    /**
     * Attempt to retrieve a local implementation of an interface for this Binder object. If null is
     * returned, you will need to instantiate a proxy class to marshall calls through the transact()
     * method.
     */
    virtual sp<IInterface> queryLocalInterface(const char* descriptor) = 0;
    virtual sp<IInterface> queryLocalInterface(const sp<String>& descriptor) = 0;

    /**
     * Perform a generic operation with the object.
     *
     * @param what The action to perform.
     * @param data data to send to the target. Must not be null.
     * @param flags Additional operation flags. Either 0 for a normal RPC, or {@link #FLAG_ONEWAY}
     * for a one-way RPC.
     */
    virtual sp<Promise<sp<Parcel>>> transact(int32_t what, const sp<Parcel>& data, int32_t flags) = 0;

    /**
     * Perform a lightweight operation with the object.
     *
     * @param what The action to perform.
     * @param num A number to send to the target.
     * @param obj An object to send to the target.
     * @param data data to send to the target.
     * @param flags Additional operation flags. Either 0 for a normal RPC, or {@link #FLAG_ONEWAY}
     * for a one-way RPC.
     */
    virtual void transact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Promise<sp<Object>>>& promise, int32_t flags) = 0;

    class Supervisor : public Object {
    public:
        virtual void onExit(int32_t reason) = 0;
    };

    virtual void link(const sp<Supervisor>& supervisor, int32_t flags) = 0;

    virtual bool unlink(const sp<Supervisor>& supervisor, int32_t flags) = 0;
};

} /* namespace mindroid */

#endif /* MINDROID_OS_IBINDER_H_ */
