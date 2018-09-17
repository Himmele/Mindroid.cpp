/*
 * Copyright (C) 2018 ESR Labs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef EXAMPLES_ELIZA_IELIZALISTENER_H
#define EXAMPLES_ELIZA_IELIZALISTENER_H

#include <mindroid/lang/Class.h>
#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/os/Binder.h>
#include <mindroid/util/ArrayList.h>
#include <mindroid/util/HashMap.h>
#include <mindroid/util/HashSet.h>
#include <mindroid/util/concurrent/Promise.h>

namespace examples {
namespace eliza {

using mindroid::sp;
using mindroid::ArrayList;
using mindroid::HashMap;
using mindroid::HashSet;
using mindroid::Object;
using mindroid::String;

class IElizaListener : public mindroid::IInterface {
public:
    virtual void onReply(const sp<String>& reply) = 0;
};

namespace binder {

class ElizaListener {
public:
    class Proxy;

    class Stub : public mindroid::Binder, public IElizaListener {
    public:
        Stub() {
            attachInterface(this, String::valueOf(DESCRIPTOR));
        }

        static sp<IElizaListener> asInterface(const sp<IBinder>& binder) {
            if (binder == nullptr) {
                return nullptr;
            }
            return new ElizaListener::Proxy(binder);
        }

        sp<IBinder> asBinder() override {
            return this;
        }

        void onTransact(int32_t what, const sp<mindroid::Parcel>& data, const sp<mindroid::Promise<sp<mindroid::Parcel>>>& result) override;

        class Proxy : public IElizaListener {
        public:
            Proxy(const sp<IBinder>& remote) {
                mRemote = remote;
            }

            sp<IBinder> asBinder() override {
                return mRemote;
            }

            bool equals(const sp<Object>& obj) const override {
                if (obj == nullptr) return false;
                if (obj == this) return true;
                if (mindroid::Class<Stub::Proxy>::isInstance(obj)) {
                    sp<Stub::Proxy> other = mindroid::Class<Stub::Proxy>::cast(obj);
                    return mRemote->equals(other->mRemote);
                } else {
                    return false;
                }
            }

            size_t hashCode() const override {
                return mRemote->hashCode();
            }

            void onReply(const sp<String>& reply) override;

        private:
            sp<IBinder> mRemote;
        };

    private:
        static const char* const DESCRIPTOR;
        static const int32_t MSG_ON_REPLY = 1;

        friend class ElizaListener::Proxy;
    };

    class Proxy : public IElizaListener {
    public:
        Proxy(const sp<mindroid::IBinder>& binder);

        sp<mindroid::IBinder> asBinder() override {
            return mBinder;
        }

        bool equals(const sp<Object>& obj) const override {
            if (obj == nullptr) return false;
            if (obj == this) return true;
            if (mindroid::Class<Proxy>::isInstance(obj)) {
                sp<Proxy> other = mindroid::Class<Proxy>::cast(obj);
                return mBinder->equals(other->mBinder);
            } else {
                return false;
            }
        }

        size_t hashCode() const override {
            return mBinder->hashCode();
        }

        void onReply(const sp<String>& reply) override;

    private:
        sp<mindroid::IBinder> mBinder;
        sp<ElizaListener::Stub> mStub;
        sp<IElizaListener> mProxy;
    };
};

} /* namespace binder */
} /* namespace eliza */
} /* namespace examples */

#endif /* EXAMPLES_ELIZA_IELIZALISTENER_H */
