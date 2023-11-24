/*
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

#include <mindroid/util/concurrent/ThreadPoolExecutor.h>
#include <mindroid/util/Log.h>

namespace mindroid {

const char* const ThreadPoolExecutor::TAG = "ThreadPoolExecutor";

ThreadPoolExecutor::ThreadPoolExecutor(const sp<String>& name, uint32_t size, bool shutdownAllowed) :
        mName(name),
        mSize(size),
        mShutdownAllowed(shutdownAllowed),
        mQueue(new LinkedBlockingQueue<sp<Runnable>>()),
        mWorkerThreads(nullptr) {
    start();
}

ThreadPoolExecutor::~ThreadPoolExecutor() {
    shutdown(true);
}

void ThreadPoolExecutor::start() {
    mWorkerThreads = new sp<WorkerThread>[mSize];
    for (uint32_t i = 0; i < mSize; i++) {
        sp<String> name = String::format("%s%c%s%d%c", (mName != nullptr ? mName->c_str() : "ThreadPoolExecutor"), '[', "Worker", i, ']');
        mWorkerThreads[i] = new WorkerThread(name);
        mWorkerThreads[i]->setQueue(mQueue);
        mWorkerThreads[i]->start();
    }
}

bool ThreadPoolExecutor::shutdown(bool shutdownAllowed) {
    if (!shutdownAllowed) {
        Log::w(TAG, "Worker threads are not allowed to shut down");
        return false;
    }
    if (mWorkerThreads != nullptr) {
        for (uint32_t i = 0; i < mSize; i++) {
            mWorkerThreads[i]->interrupt();
            mQueue->put(nullptr);
        }
        for (uint32_t i = 0; i < mSize; i++) {
            mWorkerThreads[i]->join();
            mWorkerThreads[i] = nullptr;
        }
        delete [] mWorkerThreads;
        mWorkerThreads = nullptr;
    }
    return true;
}

void ThreadPoolExecutor::WorkerThread::run() {
    while (!isInterrupted()) {
        sp<Runnable> runnable = mQueue->take();
        if (runnable != nullptr) {
            runnable->run();
        } else {
            break;
        }
    }
}

void ThreadPoolExecutor::WorkerThread::setQueue(const sp<LinkedBlockingQueue<sp<Runnable>>>& queue) {
    mQueue = queue;
}

void ThreadPoolExecutor::execute(const sp<Runnable>& runnable) {
    mQueue->put(runnable);
}

bool ThreadPoolExecutor::cancel(const sp<Runnable>& runnable) {
    return mQueue->remove(runnable);
}

} /* namespace mindroid */
