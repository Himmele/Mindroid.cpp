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

#include <mindroid/app/IOnSharedPreferenceChangeListener.h>
#include <mindroid/net/URI.h>
#include <mindroid/runtime/system/Runtime.h>

namespace mindroid {
namespace binder {

const char* const OnSharedPreferenceChangeListener::Stub::DESCRIPTOR = "mindroid://interfaces/mindroid/app/IOnSharedPreferenceChangeListener";

void OnSharedPreferenceChangeListener::Stub::onTransact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Promise<sp<Object>>>& result) {
    switch (what) {
    case MSG_ON_SHARED_PREFERENCE_CHANGED_WITH_KEY: {
        sp<String> key = object_cast<String>(obj);
        onSharedPreferenceChanged(key);
        break;
    }
    case MSG_ON_SHARED_PREFERENCE_CHANGED: {
        onSharedPreferenceChanged();
        break;
    }
    default:
        Binder::onTransact(what, num, obj, data, result);
    }
}

void OnSharedPreferenceChangeListener::Stub::Proxy::onSharedPreferenceChanged(const sp<String>& key) {
    mRemote->transact(MSG_ON_SHARED_PREFERENCE_CHANGED_WITH_KEY, 0, key, nullptr, nullptr, FLAG_ONEWAY);
}

void OnSharedPreferenceChangeListener::Stub::Proxy::onSharedPreferenceChanged() {
    mRemote->transact(MSG_ON_SHARED_PREFERENCE_CHANGED, 0, nullptr, nullptr, nullptr, FLAG_ONEWAY);
}

OnSharedPreferenceChangeListener::Proxy::Proxy(const sp<IBinder>& binder) {
    mBinder = binder;
    if (binder->getUri()->getScheme()->equals("mindroid")) {
        mStub = object_cast<OnSharedPreferenceChangeListener::Stub>(binder->queryLocalInterface(OnSharedPreferenceChangeListener::Stub::DESCRIPTOR));
        mProxy = new OnSharedPreferenceChangeListener::Stub::Proxy(binder);
    } else {
        sp<Runtime> runtime = Runtime::getRuntime();
        mStub = object_cast<OnSharedPreferenceChangeListener::Stub>(runtime->getBinder(binder->getId()));
        mProxy = object_cast<IOnSharedPreferenceChangeListener>(runtime->getProxy(binder));
    }
}

void OnSharedPreferenceChangeListener::Proxy::onSharedPreferenceChanged(const sp<String>& key) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->onSharedPreferenceChanged(key);
    } else {
        return mProxy->onSharedPreferenceChanged(key);
    }
}

void OnSharedPreferenceChangeListener::Proxy::onSharedPreferenceChanged() {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->onSharedPreferenceChanged();
    } else {
        return mProxy->onSharedPreferenceChanged();
    }
}

} /* namespace binder */
} /* namespace mindroid */
