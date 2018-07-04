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
#include <mindroid/lang/Boolean.h>
#include <mindroid/lang/Integer.h>
#include <mindroid/lang/Void.h>
#include <mindroid/util/Log.h>
#include <mindroid/util/logging/Logger.h>
#include <mindroid/util/concurrent/AsyncAwait.h>
#include <mindroid/util/concurrent/Executors.h>
#include <mindroid/util/concurrent/ExecutionException.h>
#include <mindroid/util/concurrent/Promise.h>

using namespace mindroid;

class TestService : public Service {
public:
    void onCreate() override {
        Log::d(TAG, "onCreate");
    }

    int32_t onStartCommand(const sp<Intent>& intent, int32_t flags, int32_t startId) override  {
        Log::d(TAG, "onStartCommand");
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

TEST_F(Services, test1) {
    sp<Logger> logger = new Logger();

    sp<Promise<sp<String>>> p = AsyncAwait::async()
    ->thenCompose<sp<ComponentName>>([=] (const sp<Void>& value) {
        return startService();
    })
    ->thenCompose<sp<String>>([=] (const sp<ComponentName>& value) {
        return logger->assumeThat("ServiceManager", "Service TestService has been created in process main", 10000);
    })
    ->thenCompose<sp<String>>([=] (const sp<String>& value) {
        return logger->assumeThat("TestService", "onStartCommand", 10000);
    })
    ->thenCompose<sp<Boolean>>([=] (const sp<String>& value) {
        return stopService();
    })
    ->thenCompose<sp<String>>([=] (const sp<Boolean>& value) {
        return logger->assumeThat("ServiceManager", "Service TestService has been stopped", 10000);
    });

    try {
        AsyncAwait::await(p, 10000);
    } catch (...) {
        FAIL();
    }
}

TEST_F(Services, test2) {
    sp<Logger> logger = new Logger();

    try {
        startService()->get(10000);
        logger->assumeThat("ServiceManager", "Service TestService has been created in process main", 10000)->get(10000);
        logger->assumeThat("TestService", "onStartCommand", 10000)->get(10000);
        stopService()->get(10000);
        logger->assumeThat("ServiceManager", "Service TestService has been stopped", 10000)->get(10000);
    } catch (...) {
        FAIL();
    }
}

sp<Promise<sp<ComponentName>>> Services::startService() {
    sp<IServiceManager> serviceManager = ServiceManager::getServiceManager();
    sp<Intent> intent = new Intent();
    intent->setComponent(new ComponentName("", "TestService"))
            ->putExtra("name", "TestService")
            ->putExtra("process", "main");
    try {
        return serviceManager->startSystemService(intent);
    } catch (...) {
        return new Promise<sp<ComponentName>>(sp<Exception>(new ExecutionException()));
    }
}

sp<Promise<sp<Boolean>>> Services::stopService() {
    sp<IServiceManager> serviceManager = ServiceManager::getServiceManager();
    sp<Intent> intent = new Intent();
    intent->setComponent(new ComponentName("", "TestService"));
    try {
        return serviceManager->stopSystemService(intent);
    } catch (...) {
        return new Promise<sp<Boolean>>(sp<Exception>(new ExecutionException()));
    }
}
