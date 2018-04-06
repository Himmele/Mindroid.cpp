/*
 * Copyright (C) 2013 Daniel Himmelein
 * Copyright (C) 2016 E.S.R.Labs
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

#ifndef MINDROID_APP_IONSHAREDPREFERENCECHANGELISTENER_H_
#define MINDROID_APP_IONSHAREDPREFERENCECHANGELISTENER_H_

#include <mindroid/os/Binder.h>
#include <mindroid/os/IInterface.h>
#include <mindroid/lang/Class.h>

namespace mindroid {

class IOnSharedPreferenceChangeListener :
        public IInterface {
public:
    virtual void onSharedPreferenceChanged(const sp<String>& key) = 0;
    virtual void onSharedPreferenceChanged() = 0;
};

namespace binder {

class OnSharedPreferenceChangeListener {
public:
    class Proxy;

    class Stub : public Binder, public IOnSharedPreferenceChangeListener {
    public:
        Stub() {
            attachInterface(this, String::valueOf(DESCRIPTOR));
        }

        static sp<IOnSharedPreferenceChangeListener> asInterface(const sp<IBinder>& binder) {
            if (binder == nullptr) {
                return nullptr;
            }
            return new OnSharedPreferenceChangeListener::Proxy(binder);
        }

        sp<IBinder> asBinder() override {
            return this;
        }

        void onTransact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Promise<sp<Object>>>& result) override;

        class Proxy : public IOnSharedPreferenceChangeListener {
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
                if (Class<Stub::Proxy>::isInstance(obj)) {
                    sp<Stub::Proxy> other = Class<Stub::Proxy>::cast(obj);
                    return mRemote->equals(other->mRemote);
                } else {
                    return false;
                }
            }

            size_t hashCode() const override {
                return mRemote->hashCode();
            }

            void onSharedPreferenceChanged(const sp<String>& key) override;
            void onSharedPreferenceChanged() override;

        private:
            sp<IBinder> mRemote;
        };

    private:
        static const char* const DESCRIPTOR;
        static const int32_t MSG_ON_SHARED_PREFERENCE_CHANGED_WITH_KEY = 1;
        static const int32_t MSG_ON_SHARED_PREFERENCE_CHANGED = 2;

        friend class OnSharedPreferenceChangeListener::Proxy;
    };

    class Proxy : public IOnSharedPreferenceChangeListener {
    public:
        Proxy(const sp<IBinder>& binder);

        sp<IBinder> asBinder() override {
            return mBinder;
        }

        bool equals(const sp<Object>& obj) const override {
            if (obj == nullptr) return false;
            if (obj == this) return true;
            if (Class<Proxy>::isInstance(obj)) {
                sp<Proxy> other = Class<Proxy>::cast(obj);
                return mBinder->equals(other->mBinder);
            } else {
                return false;
            }
        }

        size_t hashCode() const override {
            return mBinder->hashCode();
        }

        void onSharedPreferenceChanged(const sp<String>& key) override;
        void onSharedPreferenceChanged() override;

    private:
        sp<IBinder> mBinder;
        sp<OnSharedPreferenceChangeListener::Stub> mStub;
        sp<IOnSharedPreferenceChangeListener> mProxy;
    };
};

} /* namespace binder */
} /* namespace mindroid */

#endif /* MINDROID_APP_IONSHAREDPREFERENCECHANGELISTENER_H_ */
