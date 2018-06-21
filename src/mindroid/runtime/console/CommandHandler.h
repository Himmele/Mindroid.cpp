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

#ifndef MINDROID_RUNTIME_CONSOLE_COMMANDHANDLER_H_
#define MINDROID_RUNTIME_CONSOLE_COMMANDHANDLER_H_

#include <mindroid/runtime/console/ICommandHandler.h>

namespace mindroid {

class CommandHandler : public Object {
public:
    virtual ~CommandHandler() {
        mWrapper->dispose();
    }

    virtual sp<Promise<sp<String>>> execute(const sp<StringArray>& arguments) = 0;

    /** @hide */
    sp<ICommandHandler> asInterface() {
        return mWrapper;
    }

private:
    class CommandHandlerWrapper : public binder::CommandHandler::Stub {
    public:
        CommandHandlerWrapper(const wp<CommandHandler>& commandHandler) :
                mCommandHandler(commandHandler) {
        }

        sp<Promise<sp<String>>> execute(const sp<StringArray>& arguments) override {
            sp<CommandHandler> commandHandler = mCommandHandler.get();
            if (commandHandler != nullptr) {
                return commandHandler->execute(arguments);
            } else {
                throw mindroid::RuntimeException("CommandHandler::Stub is calling back to a dead CommandHandler");
            }
        }

        void dispose() override {
            Binder::dispose();
        }

    private:
        wp<CommandHandler> mCommandHandler;
    };

    const sp<CommandHandlerWrapper> mWrapper = new CommandHandlerWrapper(this);
};

} /* namespace mindroid */

#endif /* MINDROID_RUNTIME_CONSOLE_COMMANDHANDLER_H_ */
