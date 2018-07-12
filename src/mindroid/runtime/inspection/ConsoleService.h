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

#ifndef MINDROID_RUNTIME_INSPECTION_CONSOLESERVICE_H_
#define MINDROID_RUNTIME_INSPECTION_CONSOLESERVICE_H_

#include <mindroid/app/Service.h>
#include <mindroid/runtime/inspection/IConsole.h>
#include <mindroid/runtime/inspection/ICommandHandler.h>
#include <mindroid/util/HashMap.h>
#include <mindroid/util/Log.h>
#include <functional>

namespace mindroid {

class ConsoleService : public Service {
public:
    void onCreate() override;
    void onDestroy() override;

    sp<IBinder> onBind(const sp<Intent>& intent) override {
        return mBinder;
    }

private:
    static const char* const TAG;

    class Command : public Object {
    public:
        sp<String> description;
        std::function<sp<Promise<sp<String>>> (sp<StringArray>)> commandHandler;

        Command(const sp<String>& description, const std::function<sp<Promise<sp<String>>> (sp<StringArray>)>& commandHandler) :
                description(description),
                commandHandler(commandHandler) {
        }

        sp<Promise<sp<String>>> execute(const sp<StringArray>& arguments) {
            return commandHandler(arguments);
        }
    };

    class ConsoleServiceImpl : public binder::Console::Stub {
    public:
        ConsoleServiceImpl(const sp<ConsoleService>& consoleService) : mConsoleService(consoleService) { }

        bool addCommand(const sp<String>& command, const sp<String>& description, const sp<ICommandHandler>& commandHandler) override {
            return mConsoleService->addCommand(command, description, commandHandler);
        }

        bool removeCommand(const sp<String>& command) override {
            return mConsoleService->removeCommand(command);
        }

        sp<Promise<sp<String>>> executeCommand(const sp<String>& command, const sp<StringArray>& arguments) override {
            return mConsoleService->executeCommand(command, arguments);
        }

        sp<HashMap<sp<String>, sp<String>>> listCommands() override {
            return mConsoleService->listCommands();
        }

    private:
        sp<ConsoleService> mConsoleService;
    };

    bool addCommand(const sp<String>& command, const sp<String>& description, const sp<ICommandHandler>& commandHandler);
    void addCommand(const char* command, const char* description, const std::function<sp<String> (sp<StringArray>)>& function) {
        return addCommand(String::valueOf(command), String::valueOf(description), function);
    }
    void addCommand(const sp<String>& command, const sp<String>& description, const std::function<sp<String> (sp<StringArray>)>& function);
    bool removeCommand(const sp<String>& command);
    sp<Promise<sp<String>>> executeCommand(const sp<String>& command, const sp<StringArray>& arguments);
    sp<HashMap<sp<String>, sp<String>>> listCommands();

    void addCommands();

    sp<binder::Console::Stub> mBinder = new ConsoleServiceImpl(this);
    sp<HashMap<sp<String>, sp<Command>>> mCommands = new HashMap<sp<String>, sp<Command>>();
};

} /* namespace mindroid */

#endif /* MINDROID_RUNTIME_INSPECTION_CONSOLESERVICE_H_ */
