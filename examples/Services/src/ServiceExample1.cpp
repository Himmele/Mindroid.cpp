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

#include "ServiceExample1.h"

using namespace mindroid;

namespace examples {
namespace services {

const char* const ServiceExample1::TAG = "ServiceExample1";

ServiceExample1::ServiceExample1() {
    Log::d(TAG, "ServiceExample1::ctor");
}

ServiceExample1::~ServiceExample1() {
    Log::d(TAG, "ServiceExample1::dtor");
}

void ServiceExample1::onCreate() {
    mHandler = new Handler();
    Log::d(TAG, "ServiceExample1::onCreate");

    sp<Intent> intent = new Intent();
    intent->setComponent(new ComponentName("examples", "ServiceExample2"));
    mServiceConnection = new ServiceConnection1();
    bindService(intent, mServiceConnection, 0);
    startService(intent);

    mHandler->postDelayed([=] {
        stopService(intent);
    }, 8000);

    mHandler->postDelayed([=] {
        unbindService(mServiceConnection);
        mServiceConnection = nullptr;
    }, 4000);
}

int32_t ServiceExample1::onStartCommand(const sp<Intent>& intent, int32_t flags, int32_t startId) {
    Log::d(TAG, "Service1::onStartCommand");
    return 0;
}

void ServiceExample1::onDestroy() {
    Log::d(TAG, "Service1::onDestroy");
}

} /* namespace services */
} /* namespace examples */