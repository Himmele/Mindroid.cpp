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

#include "ElizaService.h"
#include "Eliza.h"
#include "util/Eliza.h"
#include <mindroid/os/Bundle.h>
#include <mindroid/os/ServiceManager.h>
#include <mindroid/runtime/system/Runtime.h>

using namespace mindroid;

namespace examples {
namespace eliza {

void ElizaService::onCreate() {
    if (Runtime::getRuntime()->getNodeId() == 1) {
        mBinder = new Stub();
        ServiceManager::addService(Eliza::ELIZA, mBinder);
    }
}

void ElizaService::onDestroy() {
    if (mBinder != nullptr) {
        ServiceManager::removeService(mBinder);
    }
}

sp<IBinder> ElizaService::onBind(const sp<Intent>& intent) {
    return mBinder;
}

ElizaService::Stub::Stub() : mHandler(new Handler()), mEliza(new ::eliza::Eliza()) {
}

sp<String> ElizaService::Stub::ask1(const sp<String>& question) {
    return talk(question);
}

sp<Promise<sp<String>>> ElizaService::Stub::ask2(const sp<String>& question) {
    sp<Promise<sp<String>>> promise = new Promise<sp<String>>();
    mHandler->postDelayed([=] {
        promise->complete(talk(question));
    }, 1000);
    return promise;
}

void ElizaService::Stub::ask3(const sp<String>& question, const sp<IElizaListener>& listener) {
    mHandler->postDelayed([=] {
        try {
            listener->onReply(talk(question));
        } catch (const RemoteException& e) {
        }
    }, 1000);
}

sp<String> ElizaService::Stub::talk(const sp<String>& input) {
    return mEliza->talk(input);
}

} /* namespace eliza */
} /* namespace examples */
