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

#ifndef MINDROID_THREADPOOLEXECUTOR_H_
#define MINDROID_THREADPOOLEXECUTOR_H_

#include "mindroid/lang/Thread.h"
#include "mindroid/util/concurrent/Executor.h"
#include "mindroid/util/concurrent/LinkedBlockingQueue.h"

namespace mindroid {

class ThreadPoolExecutor :
        public Executor {
public:
    ThreadPoolExecutor(const char* name, uint32_t size) :
            ThreadPoolExecutor(String::valueOf(name), size, true) {
    }
    ThreadPoolExecutor(const char* name, uint32_t size, bool shutdownAllowed) :
            ThreadPoolExecutor(String::valueOf(name), size, shutdownAllowed) {
    }
    ThreadPoolExecutor(const sp<String>& name, uint32_t size) :
            ThreadPoolExecutor(name, size, true) {
    }
    ThreadPoolExecutor(const sp<String>& name, uint32_t size, bool shutdownAllowed);
    virtual ~ThreadPoolExecutor();
    ThreadPoolExecutor(const ThreadPoolExecutor&) = delete;
    ThreadPoolExecutor& operator=(const ThreadPoolExecutor&) = delete;

    virtual void execute(const sp<Runnable>& runnable);
    virtual bool cancel(const sp<Runnable>& runnable);

    bool shutdown() {
        return shutdown(mShutdownAllowed);
    }

private:
    class WorkerThread : public Thread {
    public:
        WorkerThread(const sp<String>& name) :
                Thread(name),
                mQueue(nullptr) {
        }
        virtual ~WorkerThread() = default;
        virtual void run();

    private:
        void setQueue(const sp<LinkedBlockingQueue<sp<Runnable>>>& queue);

        sp<LinkedBlockingQueue<sp<Runnable>>> mQueue;

        friend class ThreadPoolExecutor;
    };

    void start();
    bool shutdown(bool shutdownAllowed);

    static const char* const TAG;

    sp<String> mName;
    const uint32_t mSize;
    const bool mShutdownAllowed;
    sp<LinkedBlockingQueue<sp<Runnable>>> mQueue;
    sp<WorkerThread>* mWorkerThreads;
};

} /* namespace mindroid */

#endif /* MINDROID_THREADPOOLEXECUTOR_H_ */
