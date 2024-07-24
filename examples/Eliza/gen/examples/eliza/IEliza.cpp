/*
 * Copyright (C) 2018 ESR Labs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <mindroid/os/Parcel.h>
#include <mindroid/net/URI.h>
#include <mindroid/runtime/system/Runtime.h>
#include <mindroid/util/concurrent/Promise.h>
#include <examples/eliza/IEliza.h>

using namespace mindroid;

namespace examples {
namespace eliza {
namespace binder {

const char* const Eliza::Stub::DESCRIPTOR = "mindroid://interfaces/examples/eliza/IEliza";

void Eliza::Stub::onTransact(int32_t what, const sp<Parcel>& data, const sp<Thenable>& result) {
    switch (what) {
    case MSG_ASK1: {
        sp<String> _question = data->getString();
        sp<String> _reply = ask1(_question);
        sp<Parcel> _parcel = Parcel::obtain();
        _parcel->putString(_reply);
        object_cast<Promise<sp<Parcel>>, Thenable>(result)->complete(_parcel);
        break;
    }
    case MSG_ASK2: {
        sp<String> _question = data->getString();
        sp<Promise<sp<String>>> _reply = ask2(_question);
        _reply->then([=] (const sp<String>& value, const sp<Exception>& exception) {
            if (exception == nullptr) {
                sp<Parcel> _parcel = Parcel::obtain();
                try {
                    _parcel->putString(value);
                    object_cast<Promise<sp<Parcel>>, Thenable>(result)->complete(_parcel);
                } catch (const RemoteException& e) {
                    object_cast<Promise<sp<Parcel>>, Thenable>(result)->completeWith(e);
                }
            } else {
                object_cast<Promise<sp<Parcel>>, Thenable>(result)->completeWith(exception);
            }
        });
        break;
    }
    case MSG_ASK3: {
        sp<String> _question = data->getString();
        sp<IElizaListener> _listener = binder::ElizaListener::Stub::asInterface(data->getBinder());
        ask3(_question, _listener);
        break;
    }
    default:
        Binder::onTransact(what, data, result);
    }
}

sp<String> Eliza::Stub::Proxy::ask1(const sp<String>& question) {
    sp<Promise<sp<String>>> _promise = new Promise<sp<String>>();
    sp<Parcel> _data = Parcel::obtain();
    _data->putString(question);
    mRemote->transact(MSG_ASK1, _data, 0)
            ->then([=] (const sp<Parcel>& parcel, const sp<Exception>& exception) {
                if (exception == nullptr) {
                    try {
                        sp<String> _reply = parcel->getString();
                        _promise->complete(_reply);
                    } catch (const RemoteException& e) {
                        _promise->completeWith(e);
                    }
                } else {
                    _promise->completeWith(exception);
                }
            });
    return Binder::get(_promise);
}

sp<Promise<sp<String>>> Eliza::Stub::Proxy::ask2(const sp<String>& question) {
    sp<Promise<sp<String>>> _promise = new Promise<sp<String>>();
    sp<Parcel> _data = Parcel::obtain();
    _data->putString(question);
    mRemote->transact(MSG_ASK2, _data, 0)
            ->then([=] (const sp<Parcel>& parcel, const sp<Exception>& exception) {
                if (exception == nullptr) {
                    try {
                        sp<String> _reply = parcel->getString();
                        _promise->complete(_reply);
                    } catch (const RemoteException& e) {
                        _promise->completeWith(e);
                    }
                } else {
                    _promise->completeWith(exception);
                }
            });
    return _promise;
}

void Eliza::Stub::Proxy::ask3(const sp<String>& question, const sp<IElizaListener>& listener) {
    sp<Parcel> _data = Parcel::obtain();
    _data->putString(question);
    _data->putBinder(listener->asBinder());
    mRemote->transact(MSG_ASK3, _data, FLAG_ONEWAY);
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

sp<Promise<sp<String>>> Eliza::Proxy::ask2(const sp<String>& question) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->ask2(question);
    } else {
        return mProxy->ask2(question);
    }
}

void Eliza::Proxy::ask3(const sp<String>& question, const sp<IElizaListener>& listener) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        mStub->ask3(question, binder::ElizaListener::Stub::asInterface(listener->asBinder()));
    } else {
        mProxy->ask3(question, listener);
    }
}

} /* namespace binder */
} /* namespace eliza */
} /* namespace examples */
