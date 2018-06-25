/*
 * Copyright (C) 2018 E.S.R.Labs
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

#include <mindroid/tests/Services.h>
#include <mindroid/app/Service.h>
#include <mindroid/util/Log.h>
#include <mindroid/util/concurrent/Promise.h>
#include <mindroid/util/concurrent/Executors.h>

using namespace mindroid;

class TestService : public Service {
public:
    void onCreate() override {
        Log::d(TAG, "onCreate");
    }

    int32_t onStartCommand(const sp<Intent>& intent, int32_t flags, int32_t startId) override  {
        Log::d(TAG, "onStartCommand");

        sp<Bundle> test = intent->getBundleExtra("test");
        if (test != nullptr && test->containsKey("promise")) {
            sp<Promise<bool>> promise = object_cast<Promise<bool>>(test->getObject("promise"));
            promise->complete(true);
        }
        return 0;
    }

    void onDestroy() override {
        Log::d(TAG, "onDestroy");
    }

    sp<IBinder> onBind(const sp<Intent>& intent) override {
        return nullptr;
    }

private:
    static const char* const TAG;
};

const char* const TestService::TAG = "TestService";

CLASS(TestService);

TEST_F(Services, test) {
    sp<Promise<bool>> promise = new Promise<bool>(Executors::SYNCHRONOUS_EXECUTOR);
    sp<IServiceManager> serviceManager = ServiceManager::getServiceManager();
    sp<Intent> intent = new Intent();
    intent->setComponent(new ComponentName("", "TestService"));
    sp<Bundle> test = new Bundle();
    test->putObject("promise", promise);
    intent->putExtra("test", test);
    try {
        serviceManager->startSystemService(intent)->get(10000);
    } catch (...) {
        FAIL();
    }

    // FIXME: Wait for onStartCommand log.
    Thread::sleep(100);

    try {
        serviceManager->stopSystemService(intent)->get(10000);
    } catch (...) {
        FAIL();
    }

    try {
        Promise<sp<Void>>::allOf(Executors::SYNCHRONOUS_EXECUTOR, { promise })->orTimeout(10000)->get();
    } catch (...) {
        FAIL();
    }
}
