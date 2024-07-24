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

#ifndef MINDROID_UTIL_CONCURRENT_THENABLE_H_
#define MINDROID_UTIL_CONCURRENT_THENABLE_H_

#include <mindroid/lang/Runnable.h>
#include <mindroid/lang/Exception.h>
#include <mindroid/util/concurrent/Executor.h>
#include <mindroid/util/concurrent/atomic/AtomicBoolean.h>
#include <mindroid/util/concurrent/locks/ReentrantLock.h>
#include <mindroid/util/LinkedList.h>

namespace mindroid {

class Handler;

class Thenable :
        public Object {
public:
    Thenable();

    Thenable(const sp<Handler>& handler);

    Thenable(const sp<Executor>& executor);

    /**
     * Returns {@code true} if completed in any fashion: normally,
     * exceptionally, or via cancellation.
     *
     * @return {@code true} if completed
     */
    virtual bool isDone() const = 0;

    /**
     * If not already completed, completes this Future with
     * a {@link CancellationException}. Dependent Futures
     * that have not already completed will also complete
     * exceptionally, with a {@link CompletionException} caused by
     * this {@code CancellationException}.
     *
     * @return {@code true} if this task is now cancelled
     */
    virtual bool cancel() = 0;

    /**
     * Returns {@code true} if this Future was cancelled
     * before it completed normally.
     *
     * @return {@code true} if this Future was cancelled
     * before it completed normally
     */
    virtual bool isCancelled() const = 0;

    /**
     * Returns {@code true} if this Future completed
     * exceptionally, in any way. Possible causes include
     * cancellation, explicit invocation of {@code
     * completeWith(Throwable)}, and abrupt termination of a
     * Future action.
     *
     * @return {@code true} if this Future completed
     * exceptionally
     */
    virtual bool isCompletedExceptionally() const  = 0;

protected:
    class Action : public Runnable {
    public:
        Action(const sp<Executor>& executor) {
            mExecutor = executor;
        }

        virtual void tryRun() {
            if (claim()) {
                mExecutor->execute(this);
            }
        }

        virtual void run() = 0;

    protected:
        bool claim() {
            return mClaim.compareAndSet(false, true);
        }

        sp<Executor> mExecutor;

    private:
        AtomicBoolean mClaim;
    };

    void setException(const sp<Exception>& exception) {
        AutoLock autoLock(mLock);
        if (!mIsDone) {
            mException = exception;
            mIsDone = true;
        }
    }

    sp<Exception> getException() {
        AutoLock autoLock(mLock);
        return mException;
    }

    void addAction(const sp<Action>& action) {
        AutoLock autoLock(mLock);
        if (mActions == nullptr) {
            mActions = new LinkedList<sp<Action>>();
        }
        mActions->add(action);
        if (mIsDone) {
            mActions->remove(action);
            action->tryRun();
        }
    }

    void removeAction(const sp<Action>& action) {
        AutoLock autoLock(mLock);
        if (mActions != nullptr) {
            mActions->remove(action);
        }
    }

    void runActions() {
        if (mActions != nullptr) {
            auto itr = mActions->iterator();
            while (itr.hasNext()) {
                sp<Action> action = itr.next();
                itr.remove();
                action->tryRun();
            }
        }
    }

    mutable sp<Lock> mLock;
    mutable sp<Condition> mCondition;
    sp<Exception> mException;
    bool mIsDone = false;
    sp<Executor> mExecutor;

private:
    static void logException(const sp<Exception>& exception);

    // TODO: Lock-free ConcurrentLinkedQueue.
    sp<LinkedList<sp<Action>>> mActions;

    template <typename> friend class Promise;
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_CONCURRENT_THENABLE_H_ */
