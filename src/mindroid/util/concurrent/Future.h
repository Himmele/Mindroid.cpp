/*
 * Copyright (C) 2016 Daniel Himmelein
 * Copyright (C) 2016 E.S.R.Labs
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

/*
 * Portions of this file are modified versions of
 * http://gee.cs.oswego.edu/cgi-bin/viewcvs.cgi/jsr166/src/main/java/util/concurrent/CompletionStage.java?revision=1.39
 * which contained the following notice:
 *
 * Written by Doug Lea with assistance from members of JCP JSR-166
 * Expert Group and released to the public domain, as explained at
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#ifndef MINDROID_UTIL_CONCURRENT_FUTURE_H_
#define MINDROID_UTIL_CONCURRENT_FUTURE_H_

#include <mindroid/util/concurrent/Thenable.h>

namespace mindroid {

template<typename T>
class Future :
        public Thenable {
public:
    Future() : Thenable() {
    }

    Future(const sp<Handler>& handler) : Thenable(handler) {
    }

    Future(const sp<Executor>& executor) : Thenable(executor) {
    }

    virtual ~Future() = default;

    /**
     * Returns {@code true} if completed in any fashion: normally,
     * exceptionally, or via cancellation.
     *
     * @return {@code true} if completed
     */
    virtual bool isDone() const = 0;

    /**
     * Waits if necessary for this Future to complete, and then
     * returns its result.
     *
     * @return the result value
     * @throws CancellationException if this future was cancelled
     * @throws ExecutionException if this future completed exceptionally
     * @throws InterruptedException if the current thread was interrupted
     * while waiting
     */
    virtual T get() const = 0;

    /**
     * Waits if necessary for at most the given time for this Future
     * to complete, and then returns its result, if available.
     *
     * @param timeout the maximum time to wait in milliseconds
     * @return the result value
     * @throws CancellationException if this future was cancelled
     * @throws ExecutionException if this future completed exceptionally
     * @throws InterruptedException if the current thread was interrupted
     * while waiting
     * @throws TimeoutException if the wait timed out
     */
    virtual T get(uint64_t timeout) const = 0;

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
};

} /* namespace mindroid */

#endif /* MINDROID_UTIL_CONCURRENT_FUTURE_H_ */
