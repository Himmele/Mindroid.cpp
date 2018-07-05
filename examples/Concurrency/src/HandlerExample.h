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

#include <mindroid/app/Service.h>
#include <mindroid/os/Handler.h>
#include <mindroid/os/Message.h>

namespace examples {
namespace concurrency {

using mindroid::sp;

class HandlerExample :
        public mindroid::Service {
public:
    HandlerExample();
    virtual ~HandlerExample();

    void onCreate() override;

    void onDestroy() override;

    sp<mindroid::IBinder> onBind(const sp<mindroid::Intent>& intent) override;

private:
    static const char* const TAG;
    static const int32_t SAY_HELLO = 0;
    static const int32_t SAY_WORLD = 1;

    class HelloHandler : public mindroid::Handler {
    public:
        HelloHandler(const sp<mindroid::Handler>& worldHandler) :
                mWorldHandler(worldHandler) {
        }

        void handleMessage(const sp<mindroid::Message>& msg) override;

    private:
        sp<mindroid::Handler> mWorldHandler;
    };

    class WorldHandler : public mindroid::Handler {
    public:
        WorldHandler() {
        }

        void handleMessage(const sp<mindroid::Message>& msg) override;
    };

    sp<mindroid::Handler> mHelloHandler;
    sp<mindroid::Handler> mWorldHandler;
};

} /* namespace concurrency */
} /* namespace examples */
