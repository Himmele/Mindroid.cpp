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

#include <mindroid/runtime/console/Console.h>
#include <mindroid/os/RemoteException.h>
#include <mindroid/lang/RuntimeException.h>

namespace mindroid {

bool Console::addCommand(const sp<String>& command, const sp<String>& description, const sp<CommandHandler>& commandHandler) {
    if ((mConsole != nullptr) && (command != nullptr) && (commandHandler != nullptr)) {
        try {
            return mConsole->addCommand(command, description, commandHandler->asInterface());
        } catch (const RemoteException& e) {
            throw RuntimeException("System failure", e);
        }
    } else {
        return false;
    }
}

bool Console::removeCommand(const sp<String>& command) {
    if ((mConsole != nullptr) && (command != nullptr)) {
        try {
            return mConsole->removeCommand(command);
        } catch (const RemoteException& e) {
            throw RuntimeException("System failure", e);
        }
    } else {
        return false;
    }
}

sp<Promise<sp<String>>> Console::executeCommand(const sp<String>& command, const std::initializer_list<const char*>& arguments) {
    return executeCommand(command, StringArray::valueOf(arguments));
}

sp<Promise<sp<String>>> Console::executeCommand(const sp<String>& command, const sp<StringArray>& arguments) {
    if ((mConsole != nullptr) && (command != nullptr)) {
        try {
            return mConsole->executeCommand(command, arguments);
        } catch (const RemoteException& e) {
            throw RuntimeException("System failure", e);
        }
    } else {
        return new Promise<sp<String>>(sp<Exception>(new ExecutionException()));
    }
}

} /* namespace mindroid */
