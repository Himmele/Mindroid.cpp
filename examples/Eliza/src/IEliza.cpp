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

#include "IEliza.h"
#include "IElizaListener.h"
#include <mindroid/os/Parcel.h>
#include <mindroid/net/URI.h>
#include <mindroid/runtime/system/Runtime.h>
#include <mindroid/util/concurrent/Promise.h>

using namespace mindroid;

namespace examples {
namespace eliza {
namespace binder {

const char* const Eliza::Stub::DESCRIPTOR = "mindroid://interfaces/examples/eliza/IEliza";

void Eliza::Stub::onTransact(int32_t what, const sp<Parcel>& data, const sp<Promise<sp<Parcel>>>& result) {
    switch (what) {
    case MSG_ASK1: {
        sp<String> question = data->getString();
        sp<String> reply = ask1(question);
        sp<Parcel> parcel = Parcel::obtain();
        parcel->putString(reply);
        result->complete(parcel);
        break;
    }
    case MSG_ASK2: {
        sp<String> question = data->getString();
        sp<Promise<sp<String>>> reply = ask2(question);
        reply->then([=] (const sp<String>& value, const sp<Exception>& exception) {
            if (exception == nullptr) {
                sp<Parcel> parcel = Parcel::obtain();
                try {
                    parcel->putString(reply->get());
                    result->complete(parcel);
                } catch (const Exception& e) {
                    result->completeWith(e);
                }
            } else {
                result->completeWith(exception);
            }
        });
        break;
    }
    case MSG_ASK3: {
        sp<String> question = data->getString();
        sp<IBinder> binder = data->getBinder();
        ask3(question, binder::ElizaListener::Stub::asInterface(binder));
    }
    default:
        Binder::onTransact(what, data, result);
    }
}

sp<String> Eliza::Stub::Proxy::ask1(const sp<String>& question) {
    sp<Promise<sp<String>>> promise = new Promise<sp<String>>();
    sp<Parcel> data = Parcel::obtain();
    data->putString(question);
    mRemote->transact(MSG_ASK1, data, 0)
        ->then([=] (const sp<Parcel>& parcel, const sp<Exception>& exception) {
            if (exception == nullptr) {
                try {
                    sp<String> reply = parcel->getString();
                    promise->complete(reply);
                } catch (const RemoteException& e) {
                    promise->completeWith(e);
                }
            } else {
                promise->completeWith(exception);
            }
        });
    return Binder::get(promise);
}

sp<mindroid::Promise<sp<String>>> Eliza::Stub::Proxy::ask2(const sp<String>& question) {
    sp<Promise<sp<String>>> promise = new Promise<sp<String>>();
    sp<Parcel> data = Parcel::obtain();
    data->putString(question);
    mRemote->transact(MSG_ASK2, data, 0)
        ->then([=] (const sp<Parcel>& parcel, const sp<Exception>& exception) {
            if (exception == nullptr) {
                try {
                    sp<String> reply = parcel->getString();
                    promise->complete(reply);
                } catch (const RemoteException& e) {
                    promise->completeWith(e);
                }
            } else {
                promise->completeWith(exception);
            }
        });
    return promise;
}

void Eliza::Stub::Proxy::ask3(const sp<String>& question, const sp<IElizaListener>& listener) {
    sp<Parcel> data = Parcel::obtain();
    data->putString(question);
    data->putBinder(mRemote, listener->asBinder());
    mRemote->transact(MSG_ASK3, data, FLAG_ONEWAY);
}

Eliza::Proxy::Proxy(const sp<IBinder>& binder) {
    mBinder = binder;
    if (binder->getUri()->getScheme()->equals("mindroid")) {
        mStub = object_cast<Eliza::Stub>(binder->queryLocalInterface(Eliza::Stub::DESCRIPTOR));
        mProxy = new Eliza::Stub::Proxy(binder);
    } else {
        sp<Runtime> runtime = Runtime::getRuntime();
        mStub = object_cast<Eliza::Stub>(runtime->getBinder(binder->getId()));
        mProxy = object_cast<IEliza>(runtime->getProxy(binder));
    }
}

sp<String> Eliza::Proxy::ask1(const sp<String>& question) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->ask1(question);
    } else {
        return mProxy->ask1(question);
    }
}

sp<mindroid::Promise<sp<String>>> Eliza::Proxy::ask2(const sp<String>& question) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->ask2(question);
    } else {
        return mProxy->ask2(question);
    }
}

void Eliza::Proxy::ask3(const sp<String>& question, const sp<IElizaListener>& listener) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->ask3(question, binder::ElizaListener::Stub::asInterface(listener->asBinder()));
    } else {
        return mProxy->ask3(question, listener);
    }
}

} /* namespace binder */
} /* namespace eliza */
} /* namespace examples */
