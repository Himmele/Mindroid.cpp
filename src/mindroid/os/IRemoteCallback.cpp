/*
 * Copyright (C) 2016 Daniel Himmelein
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

#include <mindroid/os/IRemoteCallback.h>
#include <mindroid/net/URI.h>
#include <mindroid/runtime/system/Runtime.h>

namespace mindroid {
namespace binder {

const char* const RemoteCallback::Stub::DESCRIPTOR = "mindroid://interfaces/mindroid/os/IRemoteCallback";

void RemoteCallback::Stub::onTransact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Promise<sp<Object>>>& result) {
    switch (what) {
    case MSG_SEND_RESULT: {
        sendResult(data);
        break;
    }
    default:
        Binder::onTransact(what, num, obj, data, result);
        break;
    }
}

void RemoteCallback::Stub::Proxy::sendResult(const sp<Bundle>& data) {
    mRemote->transact(MSG_SEND_RESULT, 0, nullptr, data, nullptr, FLAG_ONEWAY);
}

RemoteCallback::Proxy::Proxy(const sp<IBinder>& binder) {
    mBinder = binder;
    if (binder->getUri()->getScheme()->equals("mindroid")) {
        mStub = object_cast<RemoteCallback::Stub>(binder->queryLocalInterface(RemoteCallback::Stub::DESCRIPTOR));
        mProxy = new RemoteCallback::Stub::Proxy(binder);
    } else {
        sp<Runtime> runtime = Runtime::getRuntime();
        mStub = object_cast<RemoteCallback::Stub>(runtime->getBinder(binder->getId()));
        mProxy = object_cast<IRemoteCallback>(runtime->getProxy(binder));
    }
}

void RemoteCallback::Proxy::sendResult(const sp<Bundle>& data) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        mStub->sendResult(data);
    } else {
        mProxy->sendResult(data);
    }
}

} /* namespace binder */
} /* namespace mindroid */
