/*
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (C) 2011 Daniel Himmelein
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

#include <mindroid/util/concurrent/AsyncAwait.h>

namespace mindroid {

sp<SerialExecutor> AsyncAwait::SERIAL_EXECUTOR = new SerialExecutor("AsyncAwait", false);
sp<ThreadPoolExecutor> AsyncAwait::THREAD_POOL_EXECUTOR = new ThreadPoolExecutor("AsyncAwait", 4, false);

void AsyncAwait::tearDown() {
    SERIAL_EXECUTOR->shutdown();
    THREAD_POOL_EXECUTOR->shutdown();
}

} /* namespace mindroid */
