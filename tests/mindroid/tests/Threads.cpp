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
#include <mindroid/lang/Thread.h>
#include <mindroid/util/concurrent/Promise.h>
#include <mindroid/lang/RuntimeException.h>

using namespace mindroid;

TEST(Mindroid, ThreadShutdown1) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<Thread> thread = new Thread([=] {
        promise->complete(true);
        Thread::sleep(100);
    });
    thread->start();
    promise->get();
    ASSERT_TRUE(thread->isAlive());
    thread->join();
    ASSERT_FALSE(thread->isAlive());
}

TEST(Mindroid, ThreadShutdown2) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<Thread> thread = new Thread([=] {
        promise->complete(true);
        Thread::sleep(100);
    });
    thread->start();
    promise->get();
    ASSERT_TRUE(thread->isAlive());
    thread->join(0);
    ASSERT_FALSE(thread->isAlive());
}

TEST(Mindroid, ThreadShutdown3) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<Thread> thread = new Thread([=] {
        promise->complete(true);
        Thread::sleep(100);
    });
    thread->start();
    promise->get();
    ASSERT_TRUE(thread->isAlive());
    thread->join(10);
    ASSERT_TRUE(thread->isAlive());
    thread->join(100);
    ASSERT_FALSE(thread->isAlive());
}

TEST(Mindroid, ThreadShutdown4) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<Thread> thread = new Thread([=] {
        promise->complete(true);
        Thread::sleep(1000);
    });
    thread->start();
    promise->get();
    ASSERT_TRUE(thread->isAlive());
    thread->join(10);
    ASSERT_TRUE(thread->isAlive());
    thread->join(100);
    ASSERT_TRUE(thread->isAlive());
    try {
        if (thread->isAlive()) {
            throw RuntimeException("Failed to shutdown thread");
        }
        ASSERT_TRUE(false);
    } catch (...) {
        ASSERT_TRUE(true);
    }
}
