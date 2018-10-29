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

#include <gtest/gtest.h>
#include <mindroid/os/Looper.h>
#include <mindroid/os/Handler.h>
#include <mindroid/util/function/Function.h>
#include <mindroid/util/concurrent/Promise.h>

using namespace mindroid;

TEST(Mindroid, Handler1) {
    sp<HandlerThread> thread = new HandlerThread();
    thread->start();
    sp<Handler> handler = new Handler(thread->getLooper());

    sp<Promise<int32_t>> promise = new Promise<int32_t>();
    handler->post([promise] { promise->complete(123); });
    ASSERT_EQ(promise->get(), 123);

    thread->quit();
}

TEST(Mindroid, Handler2) {
    sp<HandlerThread> thread = new HandlerThread();
    thread->start();
    sp<Handler> handler = new Handler(thread->getLooper());

    sp<Promise<int32_t>> promise = new Promise<int32_t>();
    handler->postDelayed([promise] { promise->complete(123); }, 100);
    ASSERT_EQ(promise->get(), 123);

    thread->quit();
}

TEST(Mindroid, Handler3) {
    sp<HandlerThread> thread = new HandlerThread();
    thread->start();
    sp<Handler> handler = new Handler(thread->getLooper());

    sp<Promise<int32_t>> promise = new Promise<int32_t>();
    sp<Runnable> runnable = new Runnable([promise] { promise->complete(42); });
    handler->postDelayed(runnable, 100);
    ASSERT_EQ(promise->get(), 42);

    thread->quit();
}

TEST(Mindroid, Handler4) {
    sp<HandlerThread> thread = new HandlerThread();
    thread->start();
    sp<Handler> handler = new Handler(thread->getLooper());

    sp<Promise<int32_t>> promise = new Promise<int32_t>();
    sp<Runnable> runnable = new Runnable([promise] { promise->complete(42); });
    handler->postDelayed(runnable, 1000);
    ASSERT_EQ(handler->removeCallbacks(runnable), true);

    thread->quit();
}
