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

#include "PromiseExample.h"
#include <mindroid/lang/Boolean.h>
#include <mindroid/lang/System.h>
#include <mindroid/util/Log.h>

using namespace mindroid;

namespace examples {
namespace concurrency {

const char* const PromiseExample::TAG = "PromiseExample";

PromiseExample::PromiseExample() {
    Log::d(TAG, "PromiseExample::ctor");
}

PromiseExample::~PromiseExample() {
    Log::d(TAG, "PromiseExample::dtor");
}

void PromiseExample::onCreate() {
    Log::d(TAG, "PromiseExample::onCreate");
    mHandler = new Handler();
    mExecutor = new SerialExecutor("Executor");

    mPromise1->completeWith(mPromise2);
    mPromise1->orTimeout(10000)
    ->then<sp<Boolean>>([=] (const sp<Integer>& i) {
        Log::i(TAG, "Promise stage 1: %d", i->intValue());
        if (System::currentTimeMillis() % 2 == 0) {
            throw RuntimeException("Test");
        }
        return new Boolean(true);
    })->then<sp<Integer>>([=] (const sp<Boolean>& b) {
        Log::i(TAG, "Promise stage 2: %s", b->booleanValue() ? "true" : "false");
        return new Integer(17);
    })->catchException([=] (const sp<Exception>& exception) {
        Log::i(TAG, "Promise error stage 1: %s", exception->getMessage()->c_str());
    })->then(object_cast<Executor>(mExecutor), [=] (const sp<Integer>& i, const sp<Exception>& exception) {
        Thread::sleep(1000);
        if (exception != nullptr) {
            Log::i(TAG, "Promise error stage 2: %s", exception->getMessage()->c_str());
        } else {
            Log::i(TAG, "Promise stage 3: %d", i->intValue());
        }
    })->then<sp<Integer>>([=] (const sp<Integer>& i, const sp<Exception>& exception) {
        if (exception != nullptr) {
            Log::i(TAG, "Promise error stage 3: %s", exception->getMessage()->c_str());
        } else {
            Log::i(TAG, "Promise stage 4: %d", i->intValue());
        }
        return new Integer(12345);
    })->then([=] (const sp<Integer>& i) {
        Log::i(TAG, "Promise stage 5: %d", i->intValue());
    })->then([&] {
        Log::i(TAG, "Promise stage 6");
    });

    sp<Handler> h = new Handler();
    h->postDelayed([=] {
        mPromise2->complete(new Integer(42));
    }, 5000);


    action1(42)
        ->thenCompose<int32_t>([=] (int32_t value, const sp<Exception>& exception) { return action2(value, exception); })
        ->thenCompose<int32_t>(object_cast<Executor>(mExecutor), [=] (int32_t value) { return action3(value); })
        ->thenCompose<int32_t>([=] (int32_t value) { return action4(value); })
        ->then([=] (const int32_t& value, const sp<Exception>& exception) {
            if (exception != nullptr) {
                Log::i(TAG, "Exception: %s", exception->getMessage()->c_str());
            } else {
                Log::i(TAG, "Result: %d", value);
            }
        });


    sp<Promise<int32_t>>(new Promise<int32_t>(42))
        ->thenApply<sp<String>>([=] (int32_t value) { return String::valueOf(value); })
        ->thenAccept([=] (const sp<String>& value) { Log::i(TAG, "Result: %s", value->c_str()); });


    h->postDelayed([=] {
        mPromise3->complete(new Integer(17));
    }, 7500);

    h->postDelayed([=] {
        mPromise4->complete(new Integer(0));
    }, 8000);

    uint64_t startTime = SystemClock::uptimeMillis();
    sp<Promise<sp<Void>>> allOf = Promise<sp<Void>>::allOf({mPromise1, mPromise2, mPromise3, mPromise4});
    allOf->then([=] (const sp<Void>&, const sp<Exception>& exception) {
        uint64_t now = SystemClock::uptimeMillis();
        if (exception != nullptr) {
            Log::i(TAG, "AllOf exception after %lldms: %s", now - startTime, exception->getMessage()->c_str());
        } else {
            Log::i(TAG, "AllOf result after %lldms", now - startTime);
        }
    });

    sp<Promise<sp<Integer>>> anyOf = Promise<sp<Integer>>::anyOf({mPromise1, mPromise2, mPromise3, mPromise4});
    anyOf->then([=] (const sp<Integer>& value, const sp<Exception>& exception) {
        uint64_t now = SystemClock::uptimeMillis();
        if (exception != nullptr) {
            Log::i(TAG, "AnyOf exception after %lldms: %s", now - startTime, exception->getMessage()->c_str());
        } else {
            Log::i(TAG, "AnyOf result after %lldms: %d", now - startTime, value->intValue());
        }
    });
}

void PromiseExample::onDestroy() {
    Log::d(TAG, "PromiseExample::onDestroy");
    mExecutor->shutdown();
}

sp<IBinder> PromiseExample::onBind(const sp<Intent>& intent) {
    Log::d(TAG, "PromiseExample::onBind: %s", intent->getComponent()->toShortString()->c_str());
    return nullptr;
}

bool PromiseExample::onUnbind(const sp<Intent>& intent) {
    Log::d(TAG, "PromiseExample::onUnbind: %s", intent->getComponent()->toShortString()->c_str());
    return true;
}

sp<Promise<int32_t>> PromiseExample::action1(int32_t value) {
    Log::i(TAG, "Action 1: %d", value);
    sp<Promise<int32_t>> promise = new Promise<int32_t>();
    mHandler->postDelayed([=] { promise->complete(value + 1); }, 1000);
    return promise;
}

sp<Promise<int32_t>> PromiseExample::action2(int32_t value, const sp<Exception>& exception) {
    Log::i(TAG, "Action 2: %d", value);
    sp<Promise<int32_t>> promise = new Promise<int32_t>();
    mHandler->postDelayed([=] { promise->complete(value + 2); }, 1000);
    return promise;
}

sp<Promise<int32_t>> PromiseExample::action3(int32_t value) {
    Log::i(TAG, "Action 3: %d", value);
    Thread::sleep(1000);
    sp<Promise<int32_t>> promise = new Promise<int32_t>(value + 3);
    return promise;
}

sp<Promise<int32_t>> PromiseExample::action4(int32_t value) {
    Log::i(TAG, "Action 4: %d", value);
    sp<Promise<int32_t>> promise = new Promise<int32_t>();
    mHandler->postDelayed([=] { promise->complete(value + 4); }, 1000);
    return promise;
}

} /* namespace concurrency */
} /* namespace examples */
