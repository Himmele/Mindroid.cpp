/*
 * Copyright (C) 2016 Daniel Himmelein
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

#ifndef MINDROID_OS_ISERVICEMANAGER_H_
#define MINDROID_OS_ISERVICEMANAGER_H_

#include <mindroid/util/concurrent/Promise.h>
#include <mindroid/lang/Object.h>
#include <mindroid/lang/Class.h>
#include <mindroid/lang/Boolean.h>
#include <mindroid/os/Binder.h>
#include <mindroid/content/ComponentName.h>
#include <mindroid/content/Intent.h>
#include <mindroid/content/ServiceConnection.h>

namespace mindroid {

class IRemoteCallback;

class IServiceManager :
        public IInterface {
public:
    virtual sp<Promise<sp<ComponentName>>> startService(const sp<Intent>& service) = 0;
    virtual sp<Promise<sp<Boolean>>> stopService(const sp<Intent>& service) = 0;
    virtual sp<Promise<sp<Boolean>>> bindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) = 0;
    virtual void unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn) = 0;
    virtual void unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, const sp<IRemoteCallback>& callback) = 0;
    virtual sp<Promise<sp<ComponentName>>> startSystemService(const sp<Intent>& service) = 0;
    virtual sp<Promise<sp<Boolean>>> stopSystemService(const sp<Intent>& service) = 0;
};

namespace binder {

class ServiceManager {
public:
    class Proxy;

    class Stub : public Binder, public IServiceManager {
    public:
        Stub() {
            attachInterface(this, String::valueOf(DESCRIPTOR));
        }

        Stub(const sp<Looper>& looper) : Binder(looper) {
            attachInterface(this, String::valueOf(DESCRIPTOR));
        }

        static sp<IServiceManager> asInterface(const sp<IBinder>& binder) {
            if (binder == nullptr) {
                return nullptr;
            }
            return new ServiceManager::Stub::Proxy(binder);
        }

        sp<IBinder> asBinder() override {
            return this;
        }

        void onTransact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Thenable>& result) override;

        class Proxy : public IServiceManager {
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

            sp<Promise<sp<ComponentName>>> startService(const sp<Intent>& service) override;
            sp<Promise<sp<Boolean>>> stopService(const sp<Intent>& service) override;
            sp<Promise<sp<Boolean>>> bindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) override;
            void unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn) override;
            void unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, const sp<IRemoteCallback>& callback) override;
            sp<Promise<sp<ComponentName>>> startSystemService(const sp<Intent>& service) override;
            sp<Promise<sp<Boolean>>> stopSystemService(const sp<Intent>& service) override;

        private:
            sp<IBinder> mRemote;
        };

    private:
        static const char* const DESCRIPTOR;
        static const int32_t MSG_START_SERVICE = 1;
        static const int32_t MSG_STOP_SERVICE = 2;
        static const int32_t MSG_BIND_SERVICE = 3;
        static const int32_t MSG_UNBIND_SERVICE = 4;
        static const int32_t MSG_START_SYSTEM_SERVICE = 5;
        static const int32_t MSG_STOP_SYSTEM_SERVICE = 6;

        friend class ServiceManager::Proxy;
    };
};

} /* namespace binder */
} /* namespace mindroid */

#endif /* MINDROID_OS_ISERVICEMANAGER_H_ */
