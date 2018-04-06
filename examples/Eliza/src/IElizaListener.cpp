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

#include "IElizaListener.h"
#include <mindroid/os/Parcel.h>
#include <mindroid/net/URI.h>
#include <mindroid/runtime/system/Runtime.h>
#include <mindroid/util/concurrent/Promise.h>

using namespace mindroid;

namespace examples {
namespace binder {

const char* const ElizaListener::Stub::DESCRIPTOR = "mindroid://interfaces/examples/eliza/IElizaListener";

void ElizaListener::Stub::onTransact(int32_t what, const sp<Parcel>& data, const sp<Promise<sp<Parcel>>>& result) {
    switch (what) {
    case MSG_ON_REPLY: {
        onReply(data->getString());
        break;
    }
    default:
        Binder::onTransact(what, data, result);
    }
}

void ElizaListener::Stub::Proxy::onReply(const sp<String>& reply) {
    sp<Parcel> data = Parcel::obtain();
    data->putString(reply);
    mRemote->transact(MSG_ON_REPLY, data, FLAG_ONEWAY);
}

ElizaListener::Proxy::Proxy(const sp<IBinder>& binder) {
    mBinder = binder;
    if (binder->getUri()->getScheme()->equals("mindroid")) {
        mStub = object_cast<ElizaListener::Stub>(binder->queryLocalInterface(ElizaListener::Stub::DESCRIPTOR));
        mProxy = new ElizaListener::Stub::Proxy(binder);
    } else {
        sp<Runtime> runtime = Runtime::getRuntime();
        mStub = object_cast<ElizaListener::Stub>(runtime->getBinder(binder->getId()));
        mProxy = object_cast<IElizaListener>(runtime->getProxy(binder));
    }
}

void ElizaListener::Proxy::onReply(const sp<String>& reply) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->onReply(reply);
    } else {
        return mProxy->onReply(reply);
    }
}

} /* namespace binder */
} /* namespace examples */
