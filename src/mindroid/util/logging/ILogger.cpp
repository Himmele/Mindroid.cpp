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

#include <mindroid/util/logging/ILogger.h>
#include <mindroid/util/concurrent/Promise.h>
#include <mindroid/runtime/system/Runtime.h>

namespace mindroid {
namespace binder {

const char* const Logger::Stub::DESCRIPTOR = "mindroid://interfaces/mindroid/util/logging/ILogger";

void Logger::Stub::onTransact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Promise<sp<Object>>>& result) {
    switch (what) {
    case MSG_ASSUME_THAT: {
        sp<String> tag = data->getString("tag");
        sp<String> message = data->getString("message");
        int64_t timeout = data->getLong("timeout");
        object_cast<Promise<sp<String>>, Object>(result)->completeWith(assumeThat(tag, message, timeout));
        break;
    }
    case MSG_MARK: {
        mark();
        break;
    }
    case MSG_RESET: {
        reset();
        break;
    }
    default:
        Binder::onTransact(what, num, obj, data, result);
    }
}

sp<Promise<sp<String>>> Logger::Stub::Proxy::assumeThat(const sp<String>& tag, const sp<String>& message, int64_t timeout) {
    sp<Promise<sp<String>>> promise = new Promise<sp<String>>();
    sp<Bundle> data = new Bundle();
    data->putString("tag", tag);
    data->putString("message", message);
    data->putLong("timeout", timeout);
    mRemote->transact(MSG_ASSUME_THAT, 0, nullptr, data, object_cast<Promise<sp<Object>>, Object>(promise), 0);
    return promise;
}

void Logger::Stub::Proxy::mark() {
    mRemote->transact(MSG_MARK, 0, nullptr, nullptr, nullptr, FLAG_ONEWAY);
}

void Logger::Stub::Proxy::reset() {
    mRemote->transact(MSG_RESET, 0, nullptr, nullptr, nullptr, FLAG_ONEWAY);
}

Logger::Proxy::Proxy(const sp<IBinder>& binder) {
    mBinder = binder;
    if (binder->getUri()->getScheme()->equals("mindroid")) {
        mStub = object_cast<Logger::Stub>(binder->queryLocalInterface(Logger::Stub::DESCRIPTOR));
        mProxy = new Logger::Stub::Proxy(binder);
    } else {
        sp<Runtime> runtime = Runtime::getRuntime();
        mStub = object_cast<Logger::Stub>(runtime->getBinder(binder->getId()));
        mProxy = object_cast<ILogger>(runtime->getProxy(binder));
    }
}

sp<Promise<sp<String>>> Logger::Proxy::assumeThat(const sp<String>& tag, const sp<String>& message, int64_t timeout) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->assumeThat(tag, message, timeout);
    } else {
        return mProxy->assumeThat(tag, message, timeout);
    }
}

void Logger::Proxy::mark() {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        mStub->mark();
    } else {
        mProxy->mark();
    }
}

void Logger::Proxy::reset() {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        mStub->reset();
    } else {
        mProxy->reset();
    }
}

} /* namespace binder */
} /* namespace mindroid */
