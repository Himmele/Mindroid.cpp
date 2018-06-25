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

#ifndef MINDROID_RUNTIME_INSPECTION_CONSOLE_H_
#define MINDROID_RUNTIME_INSPECTION_CONSOLE_H_

#include <mindroid/runtime/inspection/IConsole.h>
#include <mindroid/runtime/inspection/CommandHandler.h>
#include <mindroid/content/Context.h>
#include <mindroid/os/ServiceManager.h>
#include <mindroid/lang/StringArray.h>

namespace mindroid {

class Console : public Object {
public:
    /**
     * @hide
     */
    Console() {
        mConsole = binder::Console::Stub::asInterface(ServiceManager::getSystemService(Context::CONSOLE_SERVICE));
    }

    Console(const sp<Context>& context) {
        mConsole = binder::Console::Stub::asInterface(context->getSystemService(Context::CONSOLE_SERVICE));
    }

    bool addCommand(const char* command, const char* description, const sp<CommandHandler>& commandHandler) {
        return addCommand(String::valueOf(command), String::valueOf(description), commandHandler);
    }
    bool addCommand(const sp<String>& command, const sp<String>& description, const sp<CommandHandler>& commandHandler);

    bool removeCommand(const char* command) {
        return removeCommand(String::valueOf(command));
    }
    bool removeCommand(const sp<String>& command);

    sp<Promise<sp<String>>> executeCommand(const char* command, const std::initializer_list<const char*>& arguments) {
        return executeCommand(String::valueOf(command), arguments);
    }
    sp<Promise<sp<String>>> executeCommand(const sp<String>& command, const std::initializer_list<const char*>& arguments);
    sp<Promise<sp<String>>> executeCommand(const char* command, const sp<StringArray>& arguments) {
        return executeCommand(String::valueOf(command), arguments);
    }
    sp<Promise<sp<String>>> executeCommand(const sp<String>& command, const sp<StringArray>& arguments);

private:
    sp<IConsole> mConsole;
};

} /* namespace mindroid */

#endif /* MINDROID_RUNTIME_INSPECTION_CONSOLE_H_ */
