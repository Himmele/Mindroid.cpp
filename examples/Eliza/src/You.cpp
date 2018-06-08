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

#include "You.h"
#include "Eliza.h"
#include "util/Eliza.h"
#include <mindroid/os/Bundle.h>

using namespace mindroid;

namespace examples {

const char* const You::TAG = "Rogerian therapy";

void You::onCreate() {
    sp<Bundle> extras = new Bundle();
    sp<Eliza> eliza = new Eliza(this, extras);
    Log::d(TAG, "You: Hello");
    try {
        sp<String> reply = eliza->ask1(String::valueOf("Hello"));
        Log::d(TAG, "Eliza: %s", reply->c_str());

        Log::d(TAG, "You: Well...");
        eliza->ask2(String::valueOf("Well..."))->then([=] (const sp<String>& reply) {
            Log::d(TAG, "Eliza: %s", reply->c_str());

            Log::d(TAG, "You: What is 1 + 1?");
            try {
                eliza->ask3(String::valueOf("What is 1 + 1?"), [=] (const sp<String>& reply) {
                    Log::d(TAG, "Eliza: %s", reply->c_str());
                });
            } catch (const RemoteException& e) {
                Log::e(TAG, "You: Cannot talk to Eliza");
            }
        })->catchException([=] (const sp<Exception>& exception) {
            Log::e(TAG, "You: Cannot talk to Eliza");
        });
    } catch (const RemoteException& e) {
        Log::e(TAG, "You: Cannot talk to Eliza");
    }
}

void You::onDestroy() {
}

sp<IBinder> You::onBind(const sp<Intent>& intent) {
    return nullptr;
}

} /* namespace examples */
