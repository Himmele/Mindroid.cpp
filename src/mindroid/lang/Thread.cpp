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

#include <mindroid/lang/Thread.h>
#include <mindroid/util/concurrent/Promise.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

namespace mindroid {

Thread::Thread(const sp<Runnable>& runnable, const sp<String>& name) :
        mName(name),
        mRunnable(runnable),
        mInterrupted(false) {
}

Thread::Thread(pthread_t thread) :
        mThread(thread),
        mInterrupted(false) {
}

void Thread::start() {
    if (mExecution == nullptr) {
        mSelf = this;
        mExecution = new Promise<bool>(sp<Executor>(nullptr));

        // Detached thread creation to prevent resource leaks: https://stackoverflow.com/questions/13865375/why-do-unjoined-pthreads-leak-resources-when-thread-is-not-detached-after-pthrea?rq=1
        pthread_attr_t threadAttributes;
        pthread_attr_init(&threadAttributes);
        pthread_attr_setdetachstate(&threadAttributes, PTHREAD_CREATE_DETACHED);
        if (::pthread_create(&mThread, &threadAttributes, &Thread::exec, this) != 0) {
            mSelf.clear();
            object_cast<Promise<bool>>(mExecution)->complete(false);
        }
        pthread_attr_destroy(&threadAttributes);
    }
}

void Thread::sleep(uint32_t milliseconds) {
    ::sleep(milliseconds / 1000);
    ::usleep((milliseconds % 1000) * 1000);
}

void Thread::join() const {
    if (mThread != 0) {
        try {
            mExecution->get();
        } catch (const ExecutionException& ignore) {
        }
    }
}

void Thread::join(uint64_t millis) const {
    if (mThread != 0) {
        if (millis == 0) {
            join();
        } else {
            try {
                mExecution->get(millis);
            } catch (const TimeoutException& ignore) {
            } catch (const ExecutionException& ignore) {
            }
        }
    }
}

void* Thread::exec(void* args) {
    Thread* const self = (Thread*) args;
#ifndef __APPLE__
    if (self->mName != nullptr) {
        ::pthread_setname_np(::pthread_self(), self->mName->c_str());
    }
#endif
    sp<Runnable> runnable = (self->mRunnable != nullptr) ? self->mRunnable : self;
    sp<Promise<bool>> execution = object_cast<Promise<bool>>(self->mExecution);
    runnable->run();
    runnable.clear();
    self->mSelf.clear();
    execution->complete(true);
    return nullptr;
}

void Thread::interrupt() {
    mInterrupted.store(true, std::memory_order_relaxed);
}

bool Thread::isInterrupted() const {
    return mInterrupted.load(std::memory_order_relaxed);
}

bool Thread::isAlive() const {
    return mSelf != nullptr;
}

uint64_t Thread::getId() const {
    uint64_t threadId = 0;
    std::memcpy(&threadId, &mThread, std::min(sizeof(threadId), sizeof(mThread)));
    return threadId;
}

sp<Thread> Thread::currentThread() {
    return new Thread(::pthread_self());
}

void Thread::setName(const sp<String>& name) {
    if (name != nullptr) {
        mName = name;
#ifndef __APPLE__
        if (mThread != 0) {
            ::pthread_setname_np(mThread, mName->c_str());
        }
#endif
    }
}

void Thread::setPriority(int32_t priority) {
    sched_param schedulingParameters;
    std::memset(&schedulingParameters, 0, sizeof(schedulingParameters));
    int32_t policy = 0;
    ::pthread_getschedparam(mThread, &policy, &schedulingParameters);
    schedulingParameters.sched_priority = priority;
    ::pthread_setschedparam(mThread, policy, &schedulingParameters);
}

} /* namespace mindroid */
