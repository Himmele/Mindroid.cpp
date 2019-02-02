/*
 * Copyright (C) 2019 E.S.R.Labs
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

#include <gtest/gtest.h>
#include <mindroid/os/Binder.h>
#include <mindroid/os/HandlerThread.h>
#include <mindroid/os/Looper.h>
#include <mindroid/runtime/system/Runtime.h>
#include <mindroid/io/File.h>

using namespace mindroid;

class Stub : public Binder {
public:
    Stub(const sp<Looper>& looper) : Binder(looper) {
        sCreated = true;
    }

    ~Stub() override {
        sDestroyed = true;
    }

    static bool sCreated;
    static bool sDestroyed;
};
bool Stub::sCreated = false;
bool Stub::sDestroyed = false;

TEST(Mindroid, Binder) {
    Runtime::start(1, nullptr);

    sp<HandlerThread> handlerThread = new HandlerThread();
    handlerThread->start();

    {
        sp<Stub> stub = new Stub(handlerThread->getLooper());
    }
    EXPECT_EQ(Stub::sCreated, true);
    EXPECT_EQ(Stub::sDestroyed, true);

    handlerThread->quit();

    Runtime::shutdown();
}
