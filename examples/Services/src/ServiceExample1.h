/*
 * Copyright (C) 2018 Daniel Himmelein
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

#include <mindroid/app/Service.h>
#include <mindroid/content/Intent.h>
#include <mindroid/content/ComponentName.h>
#include <mindroid/content/ServiceConnection.h>
#include <mindroid/os/Handler.h>

namespace examples {
namespace services {

using mindroid::sp;

class ServiceExample1 :
        public mindroid::Service {
public:
    ServiceExample1();
    virtual ~ServiceExample1();

    virtual void onCreate();

    virtual int32_t onStartCommand(const sp<mindroid::Intent>& intent, int32_t flags, int32_t startId);

    virtual void onDestroy();

    virtual sp<mindroid::IBinder> onBind(const sp<mindroid::Intent>& intent) {
        return nullptr;
    }

    class ServiceConnection1 :
            public mindroid::ServiceConnection {
    public:
        virtual void onServiceConnected(const sp<mindroid::ComponentName>& name, const sp<mindroid::IBinder>& service) {
            mindroid::Log::d(TAG, "Service1::onServiceConnected: %s", name->toShortString()->c_str());
        }

        virtual void onServiceDisconnected(const sp<mindroid::ComponentName>& name) {
            mindroid::Log::d(TAG, "Service1::onServiceDisconnected: %s", name->toShortString()->c_str());
        }
    };

private:
    static const char* const TAG;

    sp<mindroid::Handler> mHandler;
    sp<mindroid::ServiceConnection> mServiceConnection;
};

} /* namespace services */
} /* namespace examples */
