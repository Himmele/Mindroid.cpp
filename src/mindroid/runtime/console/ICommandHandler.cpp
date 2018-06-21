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

#include <mindroid/runtime/console/ICommandHandler.h>
#include <mindroid/runtime/system/Runtime.h>
#include <mindroid/util/concurrent/Promise.h>

namespace mindroid {
namespace binder {

const char* const CommandHandler::Stub::DESCRIPTOR = "mindroid://interfaces/mindroid/runtime/console/ICommandHandler";

void CommandHandler::Stub::onTransact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Promise<sp<Object>>>& result) {
    switch (what) {
    case MSG_EXECUTE: {
        object_cast<Promise<sp<String>>, Object>(result)->completeWith(execute(object_cast<StringArray>(obj)));
        break;
    }
    default:
        Binder::onTransact(what, num, obj, data, result);
    }
}

sp<Promise<sp<String>>> CommandHandler::Stub::Proxy::execute(const sp<StringArray>& arguments) {
    sp<Promise<sp<String>>> promise = new Promise<sp<String>>();
    mRemote->transact(MSG_EXECUTE, 0, arguments, nullptr, object_cast<Promise<sp<Object>>, Object>(promise), 0);
    return promise;
}

CommandHandler::Proxy::Proxy(const sp<IBinder>& binder) {
    mBinder = binder;
    if (binder->getUri()->getScheme()->equals("mindroid")) {
        mStub = object_cast<CommandHandler::Stub>(binder->queryLocalInterface(CommandHandler::Stub::DESCRIPTOR));
        mProxy = new CommandHandler::Stub::Proxy(binder);
    } else {
        sp<Runtime> runtime = Runtime::getRuntime();
        mStub = object_cast<CommandHandler::Stub>(runtime->getBinder(binder->getId()));
        mProxy = object_cast<ICommandHandler>(runtime->getProxy(binder));
    }
}

sp<Promise<sp<String>>> CommandHandler::Proxy::execute(const sp<StringArray>& arguments) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->execute(arguments);
    } else {
        return mProxy->execute(arguments);
    }
}

} /* namespace binder */
} /* namespace mindroid */
