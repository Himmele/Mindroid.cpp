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

#include <mindroid/runtime/console/ConsoleService.h>
#include <mindroid/lang/StringBuilder.h>

namespace mindroid {

const char* const ConsoleService::TAG = "Console";

void ConsoleService::onCreate() {
    addCommands();
    ServiceManager::addService(Context::CONSOLE_SERVICE, mBinder);
}

void ConsoleService::onDestroy() {
    ServiceManager::removeService(mBinder);
}

bool ConsoleService::addCommand(const sp<String>& command, const sp<String>& description, const sp<ICommandHandler>& commandHandler) {
    if (!mCommands->containsKey(command)) {
        mCommands->put(command, new Command(description, [=] (const sp<StringArray>& arguments) {
            try {
                return commandHandler->execute(arguments);
            } catch (const RemoteException& e) {
                return sp<Promise<sp<String>>>(new Promise<sp<String>>(sp<Exception>(new Exception(e))));
            }
        }));
        return true;
    } else {
        return false;
    }
}

void ConsoleService::addCommand(const sp<String>& command, const sp<String>& description, const std::function<sp<String> (sp<StringArray>)>& function) {
    mCommands->put(command, new Command(description, [=] (const sp<StringArray>& arguments) {
        try {
            return sp<Promise<sp<String>>>(new Promise<sp<String>>(function(arguments)));
        } catch (const Exception& e) {
            return sp<Promise<sp<String>>>(new Promise<sp<String>>(sp<Exception>(new Exception(e))));
        }
    }));
}

bool ConsoleService::removeCommand(const sp<String>& command) {
    return mCommands->remove(command) != nullptr;
}

sp<Promise<sp<String>>> ConsoleService::executeCommand(const sp<String>& command, const sp<StringArray>& arguments) {
    sp<Promise<sp<String>>> result = new Promise<sp<String>>();
    sp<Command> c = mCommands->get(command);
    if (c != nullptr) {
        result->completeWith(c->execute(arguments));
    } else {
        result->completeWith(sp<Exception>(new ExecutionException("Invalid command")));
    }
    return result;
}

void ConsoleService::addCommands() {
    addCommand("help", "Print commands", [=] (const sp<StringArray>& arguments) {
        sp<StringBuilder> sb = new StringBuilder();
        auto itr = mCommands->iterator();
        while (itr.hasNext()) {
            auto entry = itr.next();
            sp<String> string = entry.getKey();
            sp<Command> command = entry.getValue();
            sb->append(string);
            sb->append(": ");
            sb->append(command->description);
            if (itr.hasNext()) {
                sb->append("\n");
            }
        }
        return sb->toString();
    });
}

} /* namespace mindroid */
