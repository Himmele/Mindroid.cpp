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

#ifndef MINDROID_UTIL_CONCURRENT_EXECUTORS_H_
#define MINDROID_UTIL_CONCURRENT_EXECUTORS_H_

#include <mindroid/lang/Runnable.h>
#include <mindroid/util/concurrent/Executor.h>

namespace mindroid {

class Executors {
public:
    Executors() = delete;

    class SynchronousExecutor : public Executor {
    public:
        void execute(const sp<Runnable>& command) override {
            command->run();
        }

        bool cancel(const sp<Runnable>& runnable) override {
            return false;
        }
    };

    static const sp<Executor> SYNCHRONOUS_EXECUTOR;
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_CONCURRENT_EXECUTORS_H_ */
