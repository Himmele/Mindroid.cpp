/*
 * Copyright (C) 2021 Daniel Himmelein
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

TEST(Mindroid, Promise) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<Thread> thread = new Thread([=] {
        promise->complete(true);
    });
    thread->start();
    try {
        bool value = promise->get();
        ASSERT_TRUE(value);
    } catch (...) {
        ASSERT_TRUE(false);
    }
    thread->join();
}

TEST(Mindroid, PromiseCompletedExceptionally) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<Thread> thread = new Thread([=] {
        sp<RuntimeException> runtimeException = new RuntimeException("RuntimeException");
        promise->completeWith(sp<Exception>(new CompletionException("CompletionException", runtimeException)));
    });
    thread->start();
    try {
        bool value = promise->get();
        ASSERT_TRUE(false);
    } catch (const ExecutionException& e) {
        ASSERT_TRUE(true);
        sp<Exception> cause = e.getCause();
        if (cause != nullptr) {
            if (Class<RuntimeException>::isInstance(cause)) {
                sp<RuntimeException> runtimeException = object_cast<RuntimeException>(cause);
                ASSERT_STREQ(runtimeException->getMessage()->c_str(), "RuntimeException");
            } else {
                ASSERT_TRUE(false);
            }
        } else {
            ASSERT_TRUE(false);
        }
    }
    thread->join();
}

TEST(Mindroid, PromiseChain) {
    sp<Promise<int32_t>>(new Promise<int32_t>(Executors::SYNCHRONOUS_EXECUTOR, 42))
            ->thenApply<sp<String>>([=] (int32_t value) { return String::valueOf(value); })
            ->thenAccept([=] (const sp<String>& value) { ASSERT_STREQ(value->c_str(), "42"); })
            ->catchException([=] (const sp<Exception>& exception) { ASSERT_TRUE(false); });
}

TEST(Mindroid, PromiseChainCompletedExceptionally) {
    sp<Promise<int32_t>>(new Promise<int32_t>(Executors::SYNCHRONOUS_EXECUTOR, 42))
            ->thenApply<sp<String>>([=] (int32_t value) -> sp<String> {
                throw RuntimeException("RuntimeException");
            })
            ->thenAccept([=] (const sp<String>& value) { ASSERT_TRUE(false); })
            ->catchException([=] (const sp<Exception>& exception) { ASSERT_STREQ(exception->getMessage()->c_str(), "RuntimeException"); });
}
