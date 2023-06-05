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

#include "Eliza.h"
#include <mindroid/content/Context.h>
#include <mindroid/content/Intent.h>
#include <mindroid/os/Bundle.h>
#include <mindroid/lang/IllegalArgumentException.h>

using namespace mindroid;

namespace examples {
namespace eliza {

const sp<URI> Eliza::ELIZA = URI::create("mindroid://eliza");

Eliza::Eliza(const sp<Context>& context) {
    sp<IBinder> binder = nullptr;
    for (int32_t i = 0; i < 4; i++) {
        if ((binder = context->getSystemService(ELIZA)) != nullptr) {
            break;
        }
        Thread::sleep(1000);
    }
    mService = binder::Eliza::Stub::asInterface(binder);
}

Eliza::Eliza(const sp<Context>& context, const sp<Bundle>& extras) {
    sp<URI> uri = nullptr;
    if (extras != nullptr) {
        sp<String> scheme = extras->getString("scheme");
        if (scheme != nullptr) {
            try {
                uri = URI::create(String::format("%s://%s", scheme->c_str(), ELIZA->getAuthority()->c_str()));
            } catch (const IllegalArgumentException& e) {
                mService = nullptr;
            }
        } else {
            uri = ELIZA;
        }
    } else {
        uri = ELIZA;
    }

    for (int32_t i = 0; i < 10; i++) {
        if (context->getSystemService(uri) != nullptr) {
            break;
        }
        Thread::sleep(1000);
    }
    mService = binder::Eliza::Stub::asInterface(context->getSystemService(uri));
}

sp<String> Eliza::ask1(const sp<String>& question) {
    if (mService == nullptr) {
        throw RemoteException();
    }

    return mService->ask1(question);
}

sp<Promise<sp<String>>> Eliza::ask2(const sp<String>& question) {
    if (mService == nullptr) {
        throw RemoteException();
    }

    return mService->ask2(question);
}

void Eliza::ask3(const sp<String>& question, const std::function<void (const sp<String>&)>& listener) {
    if (mService == nullptr) {
        throw RemoteException();
    }

    sp<Function<sp<String>, void>> f = new Function<sp<String>, void>(listener);
    sp<ListenerWrapper> wrapper = new ListenerWrapper(f, this);
    mListeners->add(wrapper);
    try {
        mService->ask3(question, wrapper);
    } catch (const RemoteException& e) {
        mListeners->remove(wrapper);
        throw;
    }
}

} /* namespace eliza */
} /* namespace examples */
