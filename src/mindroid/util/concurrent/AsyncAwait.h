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

#ifndef MINDROID_UTIL_CONCURRENT_ASYNCAWAIT_H_
#define MINDROID_UTIL_CONCURRENT_ASYNCAWAIT_H_

#include <mindroid/lang/Void.h>
#include <mindroid/util/concurrent/Promise.h>
#include <mindroid/util/concurrent/Executor.h>
#include <mindroid/util/concurrent/SerialExecutor.h>
#include <mindroid/util/concurrent/ThreadPoolExecutor.h>

namespace mindroid {

class Runnable;

class AsyncAwait :
        public Object {
public:
    static sp<SerialExecutor> SERIAL_EXECUTOR;
    static sp<ThreadPoolExecutor> THREAD_POOL_EXECUTOR;

    AsyncAwait() = delete;

    /** @hide */
    static void setUp() {
    }

    /** @hide */
    static void tearDown();

    static sp<Promise<sp<Void>>> async() {
        return async(sp<Executor>(SERIAL_EXECUTOR));
    }

    static sp<Promise<sp<Void>>> async(const sp<Handler>& handler) {
        if (handler == nullptr) {
            throw NullPointerException("Handler must not be null");
        }
        return async(handler->asExecutor());
    }

    static sp<Promise<sp<Void>>> async(const sp<Executor>& executor) {
        sp<Promise<sp<Void>>> promise = new Promise<sp<Void>>(executor);
        promise->complete(nullptr);
        return promise;
    }

    template<typename T>
    static void await(const sp<Promise<T>>& promise) {
        promise->get();
    }

    template<typename T>
    static void await(const sp<Promise<T>>& promise, int64_t timeout) {
        promise->get(timeout);
    }
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_CONCURRENT_ASYNCAWAIT_H_ */
