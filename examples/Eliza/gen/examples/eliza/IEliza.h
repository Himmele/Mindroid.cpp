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

#ifndef EXAMPLES_ELIZA_IELIZA_H
#define EXAMPLES_ELIZA_IELIZA_H

#include <mindroid/lang/Class.h>
#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/os/Binder.h>
#include <mindroid/util/ArrayList.h>
#include <mindroid/util/HashMap.h>
#include <mindroid/util/HashSet.h>
#include <mindroid/util/concurrent/Promise.h>
#include <examples/eliza/IElizaListener.h>

namespace examples {
namespace eliza {

using mindroid::sp;
using mindroid::ArrayList;
using mindroid::HashMap;
using mindroid::HashSet;
using mindroid::Object;
using mindroid::String;

class IEliza : public mindroid::IInterface {
public:
    virtual sp<String> ask1(const sp<String>& question) = 0;
    virtual sp<mindroid::Promise<sp<String>>> ask2(const sp<String>& question) = 0;
    virtual void ask3(const sp<String>& question, const sp<IElizaListener>& listener) = 0;
};

namespace binder {

class Eliza {
public:
    class Proxy;

    class Stub : public mindroid::Binder, public IEliza {
    public:
        Stub() {
            attachInterface(this, String::valueOf(DESCRIPTOR));
        }

        static sp<IEliza> asInterface(const sp<IBinder>& binder) {
            if (binder == nullptr) {
                return nullptr;
            }
            return new Eliza::Proxy(binder);
        }

        sp<IBinder> asBinder() override {
            return this;
        }

        void onTransact(int32_t what, const sp<mindroid::Parcel>& data, const sp<mindroid::Promise<sp<mindroid::Parcel>>>& result) override;

        class Proxy : public IEliza {
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

            sp<String> ask1(const sp<String>& question) override;
            sp<mindroid::Promise<sp<String>>> ask2(const sp<String>& question) override;
            void ask3(const sp<String>& question, const sp<IElizaListener>& listener) override;

        private:
            sp<IBinder> mRemote;
        };

    private:
        static const char* const DESCRIPTOR;
        static const int32_t MSG_ASK1 = 1;
        static const int32_t MSG_ASK2 = 2;
        static const int32_t MSG_ASK3 = 3;

        friend class Eliza::Proxy;
    };

    class Proxy : public IEliza {
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

        sp<String> ask1(const sp<String>& question) override;
        sp<mindroid::Promise<sp<String>>> ask2(const sp<String>& question) override;
        void ask3(const sp<String>& question, const sp<IElizaListener>& listener) override;

    private:
        sp<mindroid::IBinder> mBinder;
        sp<Eliza::Stub> mStub;
        sp<IEliza> mProxy;
    };
};

} /* namespace binder */
} /* namespace eliza */
} /* namespace examples */

#endif /* EXAMPLES_ELIZA_IELIZA_H */
