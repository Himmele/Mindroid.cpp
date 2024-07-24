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

#include <mindroid/runtime/inspection/IConsole.h>
#include <mindroid/runtime/system/Runtime.h>
#include <mindroid/lang/Boolean.h>
#include <mindroid/util/concurrent/Promise.h>

namespace mindroid {
namespace binder {

const char* const Console::Stub::DESCRIPTOR = "mindroid://interfaces/mindroid/runtime/inspection/IConsole";

void Console::Stub::onTransact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Thenable>& result) {
    switch (what) {
    case MSG_ADD_COMMAND: {
        sp<String> command = data->getString("command");
        sp<String> description = data->getString("description");
        sp<IBinder> binder = data->getBinder("binder");
        addCommand(command, description, binder::CommandHandler::Stub::asInterface(binder));
        object_cast<Promise<sp<Boolean>>, Thenable>(result)->complete(new Boolean(addCommand(command, description, binder::CommandHandler::Stub::asInterface(binder))));
        break;
    }
    case MSG_REMOVE_COMMAND: {
        object_cast<Promise<sp<Boolean>>, Thenable>(result)->complete(new Boolean(removeCommand(object_cast<String>(obj))));
        break;
    }
    case MSG_EXECUTE_COMMAND: {
        sp<String> command = data->getString("command");
        sp<StringArray> arguments = object_cast<StringArray>(data->getObject("arguments"));
        object_cast<Promise<sp<String>>, Thenable>(result)->completeWith(executeCommand(command, arguments));
        break;
    }
    case MSG_LIST_COMMANDS: {
        object_cast<Promise<sp<HashMap<sp<String>, sp<String>>>>, Thenable>(result)->complete(listCommands());
        break;
    }
    default:
        Binder::onTransact(what, num, obj, data, result);
    }
}

bool Console::Stub::Proxy::addCommand(const sp<String>& command, const sp<String>& description, const sp<ICommandHandler>& commandHandler) {
    sp<Bundle> data = new Bundle();
    data->putString("command", command);
    data->putString("description", description);
    data->putBinder("binder", commandHandler->asBinder());
    sp<Promise<sp<Boolean>>> promise = new Promise<sp<Boolean>>();
    mRemote->transact(MSG_ADD_COMMAND, 0, nullptr, data, promise, 0);
    return Binder::get(promise)->booleanValue();
}

bool Console::Stub::Proxy::removeCommand(const sp<String>& command) {
    sp<Promise<sp<Boolean>>> promise = new Promise<sp<Boolean>>();
    mRemote->transact(MSG_REMOVE_COMMAND, 0, command, nullptr, promise, 0);
    return Binder::get(promise)->booleanValue();
}

sp<Promise<sp<String>>> Console::Stub::Proxy::executeCommand(const sp<String>& command, const sp<StringArray>& arguments) {
    sp<Bundle> data = new Bundle();
    data->putString("command", command);
    data->putObject("arguments", arguments);
    sp<Promise<sp<String>>> promise = new Promise<sp<String>>();
    mRemote->transact(MSG_EXECUTE_COMMAND, 0, nullptr, data, promise, 0);
    return promise;
}

sp<HashMap<sp<String>, sp<String>>> Console::Stub::Proxy::listCommands() {
    sp<Promise<sp<HashMap<sp<String>, sp<String>>>>> promise = new Promise<sp<HashMap<sp<String>, sp<String>>>>();
    mRemote->transact(MSG_LIST_COMMANDS, 0, nullptr, nullptr, promise, 0);
    return Binder::get(promise);
}

Console::Proxy::Proxy(const sp<IBinder>& binder) {
    mBinder = binder;
    if (binder->getUri()->getScheme()->equals("mindroid")) {
        mStub = object_cast<Console::Stub>(binder->queryLocalInterface(Console::Stub::DESCRIPTOR));
        mProxy = new Console::Stub::Proxy(binder);
    } else {
        sp<Runtime> runtime = Runtime::getRuntime();
        mStub = object_cast<Console::Stub>(runtime->getBinder(binder->getId()));
        mProxy = object_cast<IConsole>(runtime->getProxy(binder));
    }
}

bool Console::Proxy::addCommand(const sp<String>& command, const sp<String>& description, const sp<ICommandHandler>& commandHandler) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->addCommand(command, description, binder::CommandHandler::Stub::asInterface(commandHandler->asBinder()));
    } else {
        return mProxy->addCommand(command, description, commandHandler);
    }
}

bool Console::Proxy::removeCommand(const sp<String>& command) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->removeCommand(command);
    } else {
        return mProxy->removeCommand(command);
    }
}

sp<Promise<sp<String>>> Console::Proxy::executeCommand(const sp<String>& command, const sp<StringArray>& arguments) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->executeCommand(command, arguments);
    } else {
        return mProxy->executeCommand(command, arguments);
    }
}

sp<HashMap<sp<String>, sp<String>>> Console::Proxy::listCommands() {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->listCommands();
    } else {
        return mProxy->listCommands();
    }
}

} /* namespace binder */
} /* namespace mindroid */
