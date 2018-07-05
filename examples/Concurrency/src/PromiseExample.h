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
#include <mindroid/lang/Integer.h>
#include <mindroid/util/concurrent/Promise.h>
#include <mindroid/util/concurrent/SerialExecutor.h>

namespace examples {
namespace concurrency {

using mindroid::sp;

class PromiseExample :
        public mindroid::Service {
public:
    PromiseExample();
    virtual ~PromiseExample();

    void onCreate() override;

    void onDestroy() override;

    sp<mindroid::IBinder> onBind(const sp<mindroid::Intent>& intent) override;

    bool onUnbind(const sp<mindroid::Intent>& intent) override;

private:
    sp<mindroid::Promise<int32_t>> action1(int32_t value);
    sp<mindroid::Promise<int32_t>> action2(int32_t value, const sp<mindroid::Exception>& exception);
    sp<mindroid::Promise<int32_t>> action3(int32_t value);
    sp<mindroid::Promise<int32_t>> action4(int32_t value);

    sp<mindroid::Handler> mHandler;
    sp<mindroid::SerialExecutor> mExecutor;
    sp<mindroid::Promise<sp<mindroid::Integer>>> mPromise1 = new mindroid::Promise<sp<mindroid::Integer>>();
    sp<mindroid::Promise<sp<mindroid::Integer>>> mPromise2 = new mindroid::Promise<sp<mindroid::Integer>>();
    sp<mindroid::Promise<sp<mindroid::Integer>>> mPromise3 = new mindroid::Promise<sp<mindroid::Integer>>();
    sp<mindroid::Promise<sp<mindroid::Integer>>> mPromise4 = new mindroid::Promise<sp<mindroid::Integer>>();

    static const char* const TAG;
};

} /* namespace concurrency */
} /* namespace examples */