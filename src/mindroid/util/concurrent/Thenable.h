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

#ifndef MINDROID_THENABLE_H_
#define MINDROID_THENABLE_H_

#include "mindroid/lang/Runnable.h"
#include "mindroid/lang/Exception.h"
#include "mindroid/util/concurrent/Executor.h"
#include "mindroid/util/concurrent/atomic/AtomicBoolean.h"
#include "mindroid/util/concurrent/locks/ReentrantLock.h"
#include "mindroid/util/LinkedList.h"

namespace mindroid {

class Handler;

class Thenable :
        public Object {
public:
    Thenable();

    Thenable(const sp<Handler>& handler);

    Thenable(const sp<Executor>& executor);

    virtual bool isDone() const = 0;

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
    bool mIsDone;
    sp<Executor> mExecutor;

private:
    // TODO: Lock-free ConcurrentLinkedQueue.
    sp<LinkedList<sp<Action>>> mActions;

    template <typename> friend class Promise;
};

} /* namespace mindroid */

#endif /* MINDROID_THENABLE_H_ */
