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

#include "AsyncTaskExample.h"
#include <mindroid/lang/System.h>
#include <mindroid/util/Log.h>

using namespace mindroid;

namespace examples {
namespace concurrency {

const char* const AsyncTaskExample::TAG = "AsyncTaskExample";

AsyncTaskExample::AsyncTaskExample() {
    Log::d(TAG, "AsyncTaskExample::ctor");
}

AsyncTaskExample::~AsyncTaskExample() {
    Log::d(TAG, "AsyncTaskExample::dtor");
}

void AsyncTaskExample::onCreate() {
    Log::d(TAG, "AsyncTaskExample::onCreate");
    sp<AsyncTask<std::vector<int32_t>, int32_t, int32_t>> asyncTask = new Task();
    asyncTask->execute({1, 2, 3});
}

void AsyncTaskExample::onDestroy() {
    Log::d(TAG, "AsyncTaskExample::onDestroy");
}

sp<IBinder> AsyncTaskExample::onBind(const sp<Intent>& intent) {
    Log::d(TAG, "AsyncTaskExample::onBind: %s", intent->getComponent()->toShortString()->c_str());
    return nullptr;
}

bool AsyncTaskExample::onUnbind(const sp<Intent>& intent) {
    Log::d(TAG, "AsyncTaskExample::onUnbind: %s", intent->getComponent()->toShortString()->c_str());
    return true;
}

void AsyncTaskExample::Task::onPreExecute() {
    Log::i(TAG, "onPreExecute");
}

int32_t AsyncTaskExample::Task::doInBackground(std::vector<int32_t> params) {
    Log::i(TAG, "doInBackground");
    for (size_t i = 0; i < params.size(); i++) {
        Thread::sleep(1000);
        publishProgress(params[i]);
    }
    return 42;
}

void AsyncTaskExample::Task::onProgressUpdate(int32_t value) {
    Log::i(TAG, "onProgressUpdate: %d", value);
}

void AsyncTaskExample::Task::onPostExecute(int32_t result) {
    Log::i(TAG, "onPostExecute: %d", result);
}

void AsyncTaskExample::Task::onCancelled() {
    Log::i(TAG, "onCancelled");
}

} /* namespace concurrency */
} /* namespace examples */
