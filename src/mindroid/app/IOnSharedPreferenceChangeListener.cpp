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

#include "mindroid/app/IOnSharedPreferenceChangeListener.h"

namespace mindroid {
namespace binder {

const char* const OnSharedPreferenceChangeListener::Stub::DESCRIPTOR = "mindroid.app.IOnSharedPreferenceChangeListener";

void OnSharedPreferenceChangeListener::Stub::onTransact(int32_t what, int32_t arg1, int32_t arg2, const sp<Object>& obj, const sp<Bundle>& data, const sp<Object>& result) {
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
        Binder::onTransact(what, arg1, arg2, obj, data, result);
    }
}

void OnSharedPreferenceChangeListener::Stub::Proxy::onSharedPreferenceChanged(const sp<String>& key) {
    mRemote->transact(MSG_ON_SHARED_PREFERENCE_CHANGED_WITH_KEY, object_cast<Object>(key), nullptr, FLAG_ONEWAY);
}

void OnSharedPreferenceChangeListener::Stub::Proxy::onSharedPreferenceChanged() {
    mRemote->transact(MSG_ON_SHARED_PREFERENCE_CHANGED, nullptr, FLAG_ONEWAY);
}

OnSharedPreferenceChangeListener::Stub::SmartProxy::SmartProxy(const sp<IBinder>& remote) {
    mRemote = remote;
    mStub = interface_cast<IOnSharedPreferenceChangeListener>(remote->queryLocalInterface(DESCRIPTOR));
    mProxy = new OnSharedPreferenceChangeListener::Stub::Proxy(remote);
}

void OnSharedPreferenceChangeListener::Stub::SmartProxy::onSharedPreferenceChanged(const sp<String>& key) {
    if (mRemote->runsOnSameThread()) {
        return mStub->onSharedPreferenceChanged(key);
    } else {
        return mProxy->onSharedPreferenceChanged(key);
    }
}

void OnSharedPreferenceChangeListener::Stub::SmartProxy::onSharedPreferenceChanged() {
    if (mRemote->runsOnSameThread()) {
        return mStub->onSharedPreferenceChanged();
    } else {
        return mProxy->onSharedPreferenceChanged();
    }
}

} /* namespace binder */
} /* namespace mindroid */
