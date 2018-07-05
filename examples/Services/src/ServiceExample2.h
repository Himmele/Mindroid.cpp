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
#include <mindroid/util/Log.h>

namespace examples {
namespace services {

using mindroid::sp;

class ServiceExample2 :
        public mindroid::Service {
public:
    ServiceExample2() {
        mindroid::Log::d(TAG, "Service2::ctor");
    }

    virtual ~ServiceExample2() {
        mindroid::Log::d(TAG, "Service2::dtor");
    }

    void onCreate() override {
        mindroid::Log::d(TAG, "Service2::onCreate");
    }

    void onDestroy() override {
        mindroid::Log::d(TAG, "Service2::onDestroy");
    }

    sp<mindroid::IBinder> onBind(const sp<mindroid::Intent>& intent) override {
        mindroid::Log::d(TAG, "Service2::onBind: %s", intent->getComponent()->toShortString()->c_str());
        return nullptr;
    }

    bool onUnbind(const sp<mindroid::Intent>& intent) override {
        mindroid::Log::d(TAG, "Service2::onUnbind: %s", intent->getComponent()->toShortString()->c_str());
        return true;
    }

private:
    static const char* const TAG;
};

const char* const ServiceExample2::TAG = "Service2";

} /* namespace services */
} /* namespace examples */
