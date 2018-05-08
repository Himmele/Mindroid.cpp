/*
 * Copyright (C) 2013 Daniel Himmelein
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
 * http://gee.cs.oswego.edu/cgi-bin/viewcvs.cgi/jsr166/src/main/java/util/concurrent/CompletableFuture.java?revision=1.211
 * which contained the following notice:
 *
 * Written by Doug Lea with assistance from members of JCP JSR-166
 * Expert Group and released to the public domain, as explained at
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#ifndef MINDROID_UTIL_CONCURRENT_PROMISE_H_
#define MINDROID_UTIL_CONCURRENT_PROMISE_H_

#include <mindroid/util/concurrent/Future.h>
#include <mindroid/util/concurrent/CancellationException.h>
#include <mindroid/util/concurrent/CompletionException.h>
#include <mindroid/util/concurrent/ExecutionException.h>
#include <mindroid/util/concurrent/TimeoutException.h>
#include <mindroid/util/ArrayList.h>
#include <mindroid/lang/Class.h>
#include <mindroid/lang/Void.h>
#include <mindroid/lang/NullPointerException.h>
#include <mindroid/os/Looper.h>
#include <mindroid/os/Handler.h>
#include <mindroid/os/HandlerThread.h>
#include <mindroid/os/SystemClock.h>
#include <functional>
#include <vector>
#include <mutex>

namespace mindroid {

/**
 * A Promise of a possibly asynchronous computation, that performs an
 * action or computes a value when another Promise completes.
 * A Promise completes upon termination of its computation, but this may
 * in turn trigger other dependent Promises.
 *
 * <ul>
 *
 * <li>The computation performed by a Promise may be expressed as a
 * Function, Consumer, or Runnable (using methods with names including
 * <em>apply</em> and <em>compose</em>, <em>accept</em>, or <em>run</em>,
 * respectively) depending on whether it requires arguments and/or
 * produces results.
 * For example:
 * <pre> {@code
 * promise.thenApply(x -> square(x))
 *      .thenAccept(x -> System.out.print(x))
 *      .thenRun(() -> System.out.println());}</pre>
 *
 * <p>Any argument to a Promise's computation is the outcome of a
 * triggering Promise's computation.
 *
 * <li>Dependencies among Promises control the triggering of
 * computations, but do not otherwise guarantee any particular
 * ordering. Additionally, execution of a new Promise's computations may
 * be arranged in any of two ways: default asynchronous execution,
 * or custom (via a supplied {@link Handler} or {@link Executor}).
 *
 * <li>Two method forms ({@link #thenApply(BiFunction) thenApply(BiFunction)}
 * and {@link #thenAccept(BiConsumer) thenAccept(BiConsumer)}) support
 * unconditional computation whether the triggering Promise completed normally
 * or exceptionally.
 * Method {@link #catchException catchException} supports computation
 * only when the triggering Promise completes exceptionally, computing a
 * replacement result, similarly to the Java {@code catch} keyword.
 * In all other cases, if a Promise's computation terminates abruptly
 * with an (unchecked) exception or error, then all dependent Promises
 * requiring its completion complete exceptionally as well, with a
 * {@link CompletionException} holding the exception as its cause.
 * In the case of method {@code thenApply(BiFunction)} or
 * {@code thenAccept(BiFunction)}, when the supplied action itself
 * encounters an exception, then the Promise completes exceptionally
 * with this exception even when the source Promise also completed exceptionally.
 *
 * </ul>
 *
 * <p>All methods adhere to the above triggering, execution, and
 * exceptional completion specifications (which are not repeated in
 * individual method specifications). Additionally, while arguments
 * used to pass a completion result (that is, for parameters of type
 * {@code T}) for methods accepting them may be null, passing a null
 * value for any other parameter will result in a {@link
 * NullPointerException} being thrown.
 *
 * <p>Method form {@link #thenApply(BiFunction) thenApply(BiFunction)}
 * is the most general way of creating a continuation Promise,
 * unconditionally performing a computation that is given both
 * the result and exception (if any) of the triggering Promise,
 * and computing an arbitrary result.
 * Method {@link #thenAccept(BiConsumer) thenAccept(BiConsumer)} is similar,
 * but preserves the result of the triggering Promise instead of computing
 * a new one.
 * Because a Promise's normal result may be {@code null}, both methods
 * should have a computation structured thus:
 *
 * <pre>{@code (result, exception) -> {
 *   if (exception == null) {
 *     // triggering Promise completed normally
 *   } else {
 *     // triggering Promise completed exceptionally
 *   }
 * }}</pre>
 */
template<typename T>
class Promise :
        public Future<T> {
public:
    /**
     * Creates a new incomplete Promise.
     */
    Promise() :
            Future<T>() {
    }

    /**
     * Creates a new Promise that is already completed with
     * the given value.
     */
    Promise(T result) : Promise() {
        complete(result);
    }

    /**
     * Creates a new Promise that is already completed with
     * the given throwable.
     */
    Promise(const sp<Exception>& exception) : Promise() {
        completeWith(exception);
    }

    Promise(const sp<Handler>& handler) :
            Future<T>(handler) {
    }

    Promise(const sp<Executor>& executor) :
            Future<T>(executor) {
    }

    Promise(const sp<Promise<T>>& supplier) : Promise() {
        completeWith(supplier);
    }

    virtual ~Promise() = default;

    /**
     * Returns a new Promise that is completed when all of
     * the given Promises complete and is executed using
     * this Promise's default asynchronous execution facility.
     * If any of the given Promises complete exceptionally,
     * then the returned Promise also does so, with a
     * CompletionException holding this exception as its cause.
     * Otherwise, the results, if any, of the given Promises
     * are not reflected in the returned Promise, but may be
     * obtained by inspecting them individually. If no
     * Promises are provided, returns a Promise completed
     * with the value {@code null}.
     *
     * <p>Among the applications of this method is to await completion
     * of a set of independent Promise before continuing a
     * program, as in: {@code Promise.allOf(p1, p2, p3).get();}.
     *
     * @param promises the Promises
     * @return a new Promise that is completed when all of the
     * given Promise complete
     * @throws NullPointerException if the array or any of its elements are
     * {@code null}
     */
    static sp<Promise<sp<Void>>> allOf(const std::vector<sp<Thenable>>& promises) {
        if (Looper::myLooper() != nullptr) {
            sp<Handler> handler = new Handler();
            return allOf(handler->asExecutor(), promises);
        } else {
            return allOf(sp<Executor>(nullptr), promises);
        }
    }

    /**
     * Returns a new Promise that is completed when all of
     * the given Promises complete and is executed using
     * the supplied Handler.
     * If any of the given Promises complete exceptionally,
     * then the returned Promise also does so, with a
     * CompletionException holding this exception as its cause.
     * Otherwise, the results, if any, of the given Promises
     * are not reflected in the returned Promise, but may be
     * obtained by inspecting them individually. If no
     * Promises are provided, returns a Promise completed
     * with the value {@code null}.
     *
     * <p>Among the applications of this method is to await completion
     * of a set of independent Promise before continuing a
     * program, as in: {@code Promise.allOf(p1, p2, p3).get();}.
     *
     * @param handler the Handler to use for asynchronous execution
     * @param promises the Promises
     * @return a new Promise that is completed when all of the
     * given Promise complete
     * @throws NullPointerException if the array or any of its elements are
     * {@code null}
     */
    static sp<Promise<sp<Void>>> allOf(const sp<Handler>& handler, const std::vector<sp<Thenable>>& promises) {
        return allOf(handler->asExecutor(), promises);
    }

    /**
     * Returns a new Promise that is completed when all of
     * the given Promises complete and is executed using
     * the supplied Executor.
     * If any of the given Promises complete exceptionally,
     * then the returned Promise also does so, with a
     * CompletionException holding this exception as its cause.
     * Otherwise, the results, if any, of the given Promises
     * are not reflected in the returned Promise, but may be
     * obtained by inspecting them individually. If no
     * Promises are provided, returns a Promise completed
     * with the value {@code null}.
     *
     * <p>Among the applications of this method is to await completion
     * of a set of independent Promise before continuing a
     * program, as in: {@code Promise.allOf(p1, p2, p3).get();}.
     *
     * @param executor the Executor to use for asynchronous execution
     * @param promises the Promises
     * @return a new Promise that is completed when all of the
     * given Promise complete
     * @throws NullPointerException if the array or any of its elements are
     * {@code null}
     */
    static sp<Promise<sp<Void>>> allOf(const sp<Executor>& executor, const std::vector<sp<Thenable>>& promises) {
        if (promises.size() == 0) {
            sp<Promise<sp<Void>>> p = new Promise<sp<Void>>(executor);
            p->complete(nullptr);
            return p;
        }
        return andTree(executor, promises, 0, promises.size() - 1);
    }

    /**
     * Returns a new Promise that is completed when any of
     * the given Promises complete, with the same result
     * and is executed using this Promise's default asynchronous
     * execution facility.
     * Otherwise, if it completed exceptionally, the returned
     * Promise also does so, with a CompletionException
     * holding this exception as its cause.  If no Promises
     * are provided, returns an incomplete Promise.
     *
     * @param promises the Promises
     * @return a new Promise that is completed with the
     * result or exception of any of the given Promises when
     * one completes
     * @throws NullPointerException if the array or any of its elements are
     * {@code null}
     */
    static sp<Promise<T>> anyOf(const std::vector<sp<Promise<T>>>& promises) {
        if (Looper::myLooper() != nullptr) {
            sp<Handler> handler = new Handler();
            return anyOf(handler->asExecutor(), promises);
        } else {
            return anyOf(sp<Executor>(nullptr), promises);
        }
    }

    /**
     * Returns a new Promise that is completed when any of
     * the given Promises complete, with the same result
     * and is executed using the supplied Handler.
     * Otherwise, if it completed exceptionally, the returned
     * Promise also does so, with a CompletionException
     * holding this exception as its cause.  If no Promises
     * are provided, returns an incomplete Promise.
     *
     * @param handler the Handler to use for asynchronous execution
     * @param promises the Promises
     * @return a new Promise that is completed with the
     * result or exception of any of the given Promises when
     * one completes
     * @throws NullPointerException if the array or any of its elements are
     * {@code null}
     */
    static sp<Promise<T>> anyOf(const sp<Handler>& handler, const std::vector<sp<Promise<T>>>& promises) {
        return anyOf(handler->asExecutor(), promises);
    }

    /**
     * Returns a new Promise that is completed when any of
     * the given Promises complete, with the same result
     * and is executed using the supplied Executor.
     * Otherwise, if it completed exceptionally, the returned
     * Promise also does so, with a CompletionException
     * holding this exception as its cause.  If no Promises
     * are provided, returns an incomplete Promise.
     *
     * @param executor the Executor to use for asynchronous execution
     * @param promises the Promises
     * @return a new Promise that is completed with the
     * result or exception of any of the given Promises when
     * one completes
     * @throws NullPointerException if the array or any of its elements are
     * {@code null}
     */
    static sp<Promise<T>> anyOf(const sp<Executor>& executor, const std::vector<sp<Promise<T>>>& promises) {
        if (promises.size() == 0) {
            return new Promise<T>(executor);
        } else {
            sp<Promise<T>> consumer = new Promise<T>(executor);
            for (auto const& p: promises) {
                if (p == nullptr) {
                    throw NullPointerException("Promise must not be null");
                }
                if (p->isDone()) {
                    if (p->getException() == nullptr) {
                        consumer->setResult(p->getResult());
                    } else {
                        consumer->setException(toCompletionException(p->getException()));
                    }
                    return consumer;
                }
            }

            sp<ArrayList<sp<AnyOfAction>>> actions = new ArrayList<sp<AnyOfAction>>(promises.size());
            sp<ArrayList<wp<AnyOfAction>>> list = new ArrayList<wp<AnyOfAction>>(promises.size());
            for (size_t i = 0; i < promises.size(); i++) {
                sp<AnyOfAction> a = new AnyOfAction(promises[i], consumer, list);
                actions->add(a);
                list->add(a);
            }
            for (size_t i = 0; i < promises.size(); i++) {
                promises[i]->addAction(actions->get(i));
            }

            // Clean up all promises if the consumer completed while adding the AnyOfAction actions.
            if (consumer->isDone()) {
                for (size_t i = 0; i < promises.size(); i++) {
                    promises[i]->removeAction(actions->get(i));
                }
            }

            return consumer;
        }
    }

    sp<Promise<T>> onHandler(const sp<Handler>& handler) {
        if (handler == nullptr) {
            throw NullPointerException("Handler must not be null");
        }
        mExecutor = handler->asExecutor();
        return this;
    }

    sp<Promise<T>> onExecutor(const sp<Executor>& executor) {
        if (executor == nullptr) {
            throw NullPointerException("Executor must not be null");
        }
        mExecutor = executor;
        return this;
    }

    virtual bool isDone() const override {
        AutoLock autoLock(mLock);
        return mIsDone;
    }

    virtual T get() const override {
        AutoLock autoLock(mLock);
        while (!mIsDone) {
            mCondition->await();
        }
        if (mException != nullptr) {
            if (Class<CancellationException>::isInstance(mException)) {
                throw Class<CancellationException>::cast(mException);
            }
            sp<Exception> exception = mException;
            sp<Exception> cause;
            if (Class<CompletionException>::isInstance(exception) &&
                    (cause = exception->getCause()) != nullptr) {
                exception = cause;
            }
            throw ExecutionException(exception);
        }
        return mResult;
    }

    virtual T get(uint64_t timeout) const override {
        AutoLock autoLock(mLock);
        int64_t duration = timeout;
        uint64_t start = SystemClock::uptimeMillis();
        while (!mIsDone && (duration > 0)) {
            mCondition->await(duration);
            duration = start + timeout - SystemClock::uptimeMillis();
        }
        if (!mIsDone) {
            throw TimeoutException("Promise timed out");
        }
        if (mException != nullptr) {
            if (Class<CancellationException>::isInstance(mException)) {
                throw Class<CancellationException>::cast(mException);
            }
            sp<Exception> exception = mException;
            sp<Exception> cause;
            if (Class<CompletionException>::isInstance(exception) &&
                    (cause = exception->getCause()) != nullptr) {
                exception = cause;
            }
            throw ExecutionException(exception);
        }
        return mResult;
    }

    bool complete(const T& result) {
        AutoLock autoLock(mLock);
        if (!mIsDone) {
            mResult = result;
            mIsDone = true;
            onComplete();
            return true;
        } else {
            return false;
        }
    }

    bool completeWith(const sp<Exception>& exception) {
        if (exception == nullptr) {
            throw NullPointerException("Exception must not be null");
        }
        AutoLock autoLock(mLock);
        if (!mIsDone) {
            mException = exception;
            mIsDone = true;
            onComplete();
            return true;
        } else {
            return false;
        }
    }

    bool completeWith(const Exception& exception) {
        return completeWith(exception.clone());
    }

    bool completeWith(const sp<Promise<T>>& supplier) {
        if (supplier == nullptr) {
            throw NullPointerException("Supplier must not be null");
        }
        sp<Thenable::Action> a = new RelayAction<T>(supplier, this);
        if (supplier->isDone()) {
            a->tryRun();
        } else {
            supplier->addAction(a);
        }
        return true;
    }

    virtual bool cancel() override {
        bool cancelled = completeWith(sp<Exception>(new CancellationException("Cancellation exception")));
        return cancelled || isCancelled();
    }

    virtual bool isCancelled() const override {
        AutoLock autoLock(mLock);
        if (mException != nullptr) {
            return Class<CancellationException>::isInstance(mException);
        }
        return false;
    }

    virtual bool isCompletedExceptionally() const override {
        AutoLock autoLock(mLock);
        return (mException != nullptr);
    }

    /**
     * Returns a new Future that, when this Future completes
     * normally, is executed using this Future's default asynchronous
     * execution facility, with this Future's result as the argument to
     * the supplied function.
     *
     * See the {@link Future} documentation for rules
     * covering exceptional completion.
     *
     * @param function the Function to use to compute the value of the
     * returned Future
     * @param <U> the function's return type
     * @return the new Future
     */
    template<typename U>
    sp<Promise<U>> thenApply(const std::function<U (T)>& function) {
        return thenApply(mExecutor, function);
    }

    /**
     * @see #thenApply(Function)
     */
    template<typename U>
    sp<Promise<U>> then(const std::function<U (T)>& function) {
        return thenApply(mExecutor, function);
    }

    /**
     * Returns a new Future that, when this Future completes
     * normally, is executed using the supplied Handler, with this
     * Future's result as the argument to the supplied function.
     *
     * See the {@link Future} documentation for rules
     * covering exceptional completion.
     *
     * @param handler the Handler to use for asynchronous execution
     * @param function the Function to use to compute the value of the
     * returned Future
     * @param <U> the function's return type
     * @return the new Future
     */
    template<typename U>
    sp<Promise<U>> thenApply(const sp<Handler>& handler, const std::function<U (T)>& function) {
        return thenApply(handler->asExecutor(), function);
    }

    /**
     * @see #thenApply(Handler, Function)
     */
    template<typename U>
    sp<Promise<U>> then(const sp<Handler>& handler, const std::function<U (T)>& function) {
        return thenApply(handler->asExecutor(), function);
    }

    /**
     * Returns a new Future that, when this Future completes
     * normally, is executed using the supplied Executor, with this
     * Future's result as the argument to the supplied function.
     *
     * See the {@link Future} documentation for rules
     * covering exceptional completion.
     *
     * @param executor the Executor to use for asynchronous execution
     * @param function the Function to use to compute the value of the
     * returned Future
     * @param <U> the function's return type
     * @return the new Future
     */
    template<typename U>
    sp<Promise<U>> thenApply(const sp<Executor>& executor, const std::function<U (T)>& function) {
        if (executor == nullptr) {
            throw NullPointerException("Executor must not be null");
        }
        sp<Promise<U>> p = new Promise<U>(mExecutor);
        sp<Thenable::Action> a = new FunctionAction<U>(executor, this, p, function);
        if (isDone()) {
            a->tryRun();
        } else {
            addAction(a);
        }
        return p;
    }

    /**
     * @see #thenApply(Executor, Function)
     */
    template<typename U>
    sp<Promise<U>> then(const sp<Executor>& executor, const std::function<U (T)>& function) {
        return thenApply(executor, function);
    }

    /**
     * Returns a new Future that, when this Future completes
     * either normally or exceptionally, is executed using this Future's
     * default asynchronous execution facility, with this Future's
     * result and exception as arguments to the supplied function.
     *
     * <p>When this Future is complete, the given function is invoked
     * with the result (or {@code null} if none) and the exception (or
     * {@code null} if none) of this Future as arguments, and the
     * function's result is used to complete the returned Future.
     *
     * @param function the Function to use to compute the value of the
     * returned Future
     * @param <U> the function's return type
     * @return the new Future
     */
    template<typename U>
    sp<Promise<U>> thenApply(const std::function<U (T, const sp<Exception>&)>& function) {
        return thenApply(mExecutor, function);
    }

    /**
     * @see #thenApply(BiFunction)
     */
    template<typename U>
    sp<Promise<U>> then(const std::function<U (T, const sp<Exception>&)>& function) {
        return thenApply(mExecutor, function);
    }

    /**
     * Returns a new Future that, when this Future completes
     * either normally or exceptionally, is executed using the
     * supplied Handler, with this Future's result and exception as
     * arguments to the supplied function.
     *
     * <p>When this Future is complete, the given function is invoked
     * with the result (or {@code null} if none) and the exception (or
     * {@code null} if none) of this Future as arguments, and the
     * function's result is used to complete the returned Future.
     *
     * @param handler the Handler to use for asynchronous execution
     * @param function the Function to use to compute the value of the
     * returned Future
     * @param <U> the function's return type
     * @return the new Future
     */
    template<typename U>
    sp<Promise<U>> thenApply(const sp<Handler>& handler, const std::function<U (T, const sp<Exception>&)>& function) {
        return thenApply(handler->asExecutor(), function);
    }

    /**
     * @see #thenApply(Handler, BiFunction)
     */
    template<typename U>
    sp<Promise<U>> then(const sp<Handler>& handler, const std::function<U (T, const sp<Exception>&)>& function) {
        return thenApply(handler->asExecutor(), function);
    }

    /**
     * Returns a new Future that, when this Future completes
     * either normally or exceptionally, is executed using the
     * supplied Executor, with this Future's result and exception as
     * arguments to the supplied function.
     *
     * <p>When this Future is complete, the given function is invoked
     * with the result (or {@code null} if none) and the exception (or
     * {@code null} if none) of this Future as arguments, and the
     * function's result is used to complete the returned Future.
     *
     * @param executor the Executor to use for asynchronous execution
     * @param function the Function to use to compute the value of the
     * returned Future
     * @param <U> the function's return type
     * @return the new Future
     */
    template<typename U>
    sp<Promise<U>> thenApply(const sp<Executor>& executor, const std::function<U (T, const sp<Exception>&)>& function) {
        if (executor == nullptr) {
            throw NullPointerException("Executor must not be null");
        }
        sp<Promise<U>> p = new Promise<U>(mExecutor);
        sp<Thenable::Action> a = new BiFunctionAction<U>(executor, this, p, function);
        if (isDone()) {
            a->tryRun();
        } else {
            addAction(a);
        }
        return p;
    }

    /**
     * @see #thenApply(Executor, BiFunction)
     */
    template<typename U>
    sp<Promise<U>> then(const sp<Executor>& executor, const std::function<U (T, const sp<Exception>&)>& function) {
        return thenApply(executor, function);
    }

    /**
     * Returns a new Future that is completed with the same
     * value as the Future returned by the given function,
     * executed using this Future's default asynchronous execution
     * facility.
     *
     * <p>When this Future completes normally, the given function is
     * invoked with this Future's result as the argument, returning
     * another Future.  When that Future completes normally,
     * the Future returned by this method is completed with
     * the same value.
     *
     * <p>To ensure progress, the supplied function must arrange
     * eventual completion of its result.
     *
     * <p>See the {@link Future} documentation for rules
     * covering exceptional completion.
     *
     * @param function the Function to use to compute another Future
     * @param <U> the type of the returned Future's result
     * @return the new Future
     */
    template<typename U>
    sp<Promise<U>> thenCompose(const std::function<sp<Promise<U>> (T)>& function) {
        return thenCompose(mExecutor, function);
    }

    /**
     * Returns a new Future that is completed with the same
     * value as the Future returned by the given function,
     * executed using the supplied Handler.
     *
     * <p>When this Future completes normally, the given function is
     * invoked with this Future's result as the argument, returning
     * another Future.  When that Future completes normally,
     * the Future returned by this method is completed with
     * the same value.
     *
     * <p>To ensure progress, the supplied function must arrange
     * eventual completion of its result.
     *
     * <p>See the {@link Future} documentation for rules
     * covering exceptional completion.
     *
     * @param handler the Handler to use for asynchronous execution
     * @param function the Function to use to compute another Future
     * @param <U> the type of the returned Future's result
     * @return the new Future
     */
    template<typename U>
    sp<Promise<U>> thenCompose(const sp<Handler>& handler, const std::function<sp<Promise<U>> (T)>& function) {
        return thenCompose(handler->asExecutor(), function);
    }

    /**
     * Returns a new Future that is completed with the same
     * value as the Future returned by the given function,
     * executed using the supplied Executor.
     *
     * <p>When this Future completes normally, the given function is
     * invoked with this Future's result as the argument, returning
     * another Future.  When that Future completes normally,
     * the Future returned by this method is completed with
     * the same value.
     *
     * <p>To ensure progress, the supplied function must arrange
     * eventual completion of its result.
     *
     * <p>See the {@link Future} documentation for rules
     * covering exceptional completion.
     *
     * @param executor the Executor to use for asynchronous execution
     * @param function the Function to use to compute another Future
     * @param <U> the type of the returned Future's result
     * @return the new Future
     */
    template<typename U>
    sp<Promise<U>> thenCompose(const sp<Executor>& executor, const std::function<sp<Promise<U>> (T)>& function) {
        if (executor == nullptr) {
            throw NullPointerException("Executor must not be null");
        }
        sp<Promise<U>> p = new Promise<U>(mExecutor);
        sp<Thenable::Action> a = new CompositionFunctionAction<U>(executor, this, p, function);
        if (isDone()) {
            a->tryRun();
        } else {
            addAction(a);
        }
        return p;
    }

    /**
     * Returns a new Future that is completed with the same
     * value as the Future returned by the given function,
     * executed using this Future's default asynchronous execution
     * facility.
     *
     * <p>When this Future is complete, the given function is invoked
     * with the result (or {@code null} if none) and the exception (or
     * {@code null} if none) of this Future as arguments, returning
     * another Future.  When that Future completes normally,
     * the Future returned by this method is completed with
     * the same value.
     *
     * <p>To ensure progress, the supplied function must arrange
     * eventual completion of its result.
     *
     * <p>See the {@link Future} documentation for rules
     * covering exceptional completion.
     *
     * @param function the Function to use to compute another Future
     * @param <U> the type of the returned Future's result
     * @return the new Future
     */
    template<typename U>
    sp<Promise<U>> thenCompose(const std::function<sp<Promise<U>> (T, const sp<Exception>&)>& function) {
        return thenCompose(mExecutor, function);
    }

    /**
     * Returns a new Future that is completed with the same
     * value as the Future returned by the given function,
     * executed using the supplied Handler.
     *
     * <p>When this Future is complete, the given function is invoked
     * with the result (or {@code null} if none) and the exception (or
     * {@code null} if none) of this Future as arguments, returning
     * another Future.  When that Future completes normally,
     * the Future returned by this method is completed with
     * the same value.
     *
     * <p>To ensure progress, the supplied function must arrange
     * eventual completion of its result.
     *
     * <p>See the {@link Future} documentation for rules
     * covering exceptional completion.
     *
     * @param handler the Handler to use for asynchronous execution
     * @param function the Function to use to compute another Future
     * @param <U> the type of the returned Future's result
     * @return the new Future
     */
    template<typename U>
    sp<Promise<U>> thenCompose(const sp<Handler>& handler, const std::function<sp<Promise<U>> (T, const sp<Exception>&)>& function) {
        return thenCompose(handler->asExecutor(), function);
    }

    /**
     * Returns a new Future that is completed with the same
     * value as the Future returned by the given function,
     * executed using the supplied Executor.
     *
     * <p>When this Future is complete, the given function is invoked
     * with the result (or {@code null} if none) and the exception (or
     * {@code null} if none) of this Future as arguments, returning
     * another Future.  When that Future completes normally,
     * the Future returned by this method is completed with
     * the same value.
     *
     * <p>To ensure progress, the supplied function must arrange
     * eventual completion of its result.
     *
     * <p>See the {@link Future} documentation for rules
     * covering exceptional completion.
     *
     * @param executor the Executor to use for asynchronous execution
     * @param function the Function to use to compute another Future
     * @param <U> the type of the returned Future's result
     * @return the new Future
     */
    template<typename U>
    sp<Promise<U>> thenCompose(const sp<Executor>& executor, const std::function<sp<Promise<U>> (T, const sp<Exception>&)>& function) {
        if (executor == nullptr) {
            throw NullPointerException("Executor must not be null");
        }
        sp<Promise<U>> p = new Promise<U>(mExecutor);
        sp<Thenable::Action> a = new BiCompositionFunctionAction<U>(executor, this, p, function);
        if (isDone()) {
            a->tryRun();
        } else {
            addAction(a);
        }
        return p;
    }

    /**
     * Returns a new Future that, when this Future completes
     * normally, is executed using this Future's default asynchronous
     * execution facility, with this Future's result as the argument to
     * the supplied action.
     *
     * See the {@link Future} documentation for rules
     * covering exceptional completion.
     *
     * @param action the action to perform before completing the
     * returned Future
     * @return the new Future
     */
    sp<Promise<T>> thenAccept(const std::function<void (T)>& function) {
        return thenAccept(mExecutor, function);
    }

    /**
     * @see #thenAccept(Consumer)
     */
    sp<Promise<T>> then(const std::function<void (T)>& function) {
        return thenAccept(mExecutor, function);
    }

    /**
     * Returns a new Future that, when this Future completes
     * normally, is executed using the supplied Handler, with this
     * Future's result as the argument to the supplied action.
     *
     * See the {@link Future} documentation for rules
     * covering exceptional completion.
     *
     * @param handler the Handler to use for asynchronous execution
     * @param action the action to perform before completing the
     * returned Future
     * @return the new Future
     */
    sp<Promise<T>> thenAccept(const sp<Handler>& handler, const std::function<void (T)>& function) {
        return thenAccept(handler->asExecutor(), function);
    }

    /**
     * @see #thenAccept(Handler, Consumer)
     */
    sp<Promise<T>> then(const sp<Handler>& handler, const std::function<void (T)>& function) {
        return thenAccept(handler->asExecutor(), function);
    }

    /**
     * Returns a new Future that, when this Future completes
     * normally, is executed using the supplied Executor, with this
     * Future's result as the argument to the supplied action.
     *
     * See the {@link Future} documentation for rules
     * covering exceptional completion.
     *
     * @param executor the Executor to use for asynchronous execution
     * @param action the action to perform before completing the
     * returned Future
     * @return the new Future
     */
    sp<Promise<T>> thenAccept(const sp<Executor>& executor, const std::function<void (T)>& function) {
        if (executor == nullptr) {
            throw NullPointerException("Executor must not be null");
        }
        sp<Promise<T>> p = new Promise<T>(mExecutor);
        sp<Thenable::Action> a = new ConsumerAction(executor, this, p, function);
        if (isDone()) {
            a->tryRun();
        } else {
            addAction(a);
        }
        return p;
    }

    /**
     * @see #thenAccept(Executor, Consumer)
     */
    sp<Promise<T>> then(const sp<Executor>& executor, const std::function<void (T)>& function) {
        return thenAccept(executor, function);
    }

    /**
     * Returns a new Future with the same result or exception as
     * this Future, that executes the given action using this Future's
     * default asynchronous execution facility when this Future completes.
     *
     * <p>When this Future is complete, the given action is invoked with the
     * result (or {@code null} if none) and the exception (or {@code null}
     * if none) of this Future as arguments.  The returned Future is completed
     * when the action returns.
     *
     * @param action the action to perform
     * @return the new Future
     */
    sp<Promise<T>> thenAccept(const std::function<void (T, const sp<Exception>&)>& function) {
        return thenAccept(mExecutor, function);
    }

    /**
     * @see #thenAccept(BiConsumer)
     */
    sp<Promise<T>> then(const std::function<void (T, const sp<Exception>&)>& function) {
        return thenAccept(mExecutor, function);
    }

    /**
     * Returns a new Future with the same result or exception as
     * this Future, that executes the given action using the supplied
     * Handler when this Future completes.
     *
     * <p>When this Future is complete, the given action is invoked with the
     * result (or {@code null} if none) and the exception (or {@code null}
     * if none) of this Future as arguments.  The returned Future is completed
     * when the action returns.
     *
     * @param handler the Handler to use for asynchronous execution
     * @param action the action to perform
     * @return the new Future
     */
    sp<Promise<T>> thenAccept(const sp<Handler>& handler, const std::function<void (T, const sp<Exception>&)>& function) {
        return thenAccept(handler->asExecutor(), function);
    }

    /**
     * @see #thenAccept(Handler, BiConsumer)
     */
    sp<Promise<T>> then(const sp<Handler>& handler, const std::function<void (T, const sp<Exception>&)>& function) {
        return thenAccept(handler->asExecutor(), function);
    }

    /**
     * Returns a new Future with the same result or exception as
     * this Future, that executes the given action using the supplied
     * Executor when this Future completes.
     *
     * <p>When this Future is complete, the given action is invoked with the
     * result (or {@code null} if none) and the exception (or {@code null}
     * if none) of this Future as arguments.  The returned Future is completed
     * when the action returns.
     *
     * @param executor the Executor to use for asynchronous execution
     * @param action the action to perform
     * @return the new Future
     */
    sp<Promise<T>> thenAccept(const sp<Executor>& executor, const std::function<void (T, const sp<Exception>&)>& function) {
        if (executor == nullptr) {
            throw NullPointerException("Executor must not be null");
        }
        sp<Promise<T>> p = new Promise<T>(mExecutor);
        sp<Thenable::Action> a = new BiConsumerAction(executor, this, p, function);
        if (isDone()) {
            a->tryRun();
        } else {
            addAction(a);
        }
        return p;
    }

    /**
     * Returns a new Future that, when this Future completes
     * normally, executes the given action using this Future's default
     * asynchronous execution facility.
     *
     * See the {@link Future} documentation for rules
     * covering exceptional completion.
     *
     * @param action the action to perform before completing the
     * returned Future
     * @return the new Future
     */
    sp<Promise<T>> then(const sp<Executor>& executor, const std::function<void (T, const sp<Exception>&)>& function) {
        return thenAccept(executor, function);
    }

    /**
     * Returns a new Future that, when this Future completes
     * normally, executes the given action using this Future's default
     * asynchronous execution facility.
     *
     * See the {@link Future} documentation for rules
     * covering exceptional completion.
     *
     * @param action the action to perform before completing the
     * returned Future
     * @return the new Future
     */
    sp<Promise<T>> thenRun(const sp<Runnable>& action) {
        return thenRun(mExecutor, action);
    }

    sp<Promise<T>> thenRun(const std::function<void (void)>& function) {
        return thenRun(mExecutor, new Runnable(function));
    }

    /**
     * @see #thenRun(Runnable)
     */
    sp<Promise<T>> then(const sp<Runnable>& action) {
        return thenRun(mExecutor, action);
    }

    sp<Promise<T>> then(const std::function<void (void)>& function) {
        return thenRun(mExecutor, new Runnable(function));
    }

    /**
     * Returns a new Future that, when this Future completes
     * normally, executes the given action using the supplied Handler.
     *
     * See the {@link Future} documentation for rules
     * covering exceptional completion.
     *
     * @param handler the Handler to use for asynchronous execution
     * @param action the action to perform before completing the
     * returned Future
     * @return the new Future
     */
    sp<Promise<T>> thenRun(const sp<Handler>& handler, const sp<Runnable>& action) {
        return thenRun(handler->asExecutor(), action);
    }

    sp<Promise<T>> thenRun(const sp<Handler>& handler, const std::function<void (void)>& function) {
        return thenRun(handler->asExecutor(), new Runnable(function));
    }

    /**
     * @see #thenRun(Handler, Runnable)
     */
    sp<Promise<T>> then(const sp<Handler>& handler, const sp<Runnable>& action) {
        return thenRun(handler->asExecutor(), action);
    }

    sp<Promise<T>> then(const sp<Handler>& handler, const std::function<void (void)>& function) {
        return thenRun(handler->asExecutor(), new Runnable(function));
    }

    /**
     * Returns a new Future that, when this Future completes
     * normally, executes the given action using the supplied Executor.
     *
     * See the {@link Future} documentation for rules
     * covering exceptional completion.
     *
     * @param executor the executor to use for asynchronous execution
     * @param action the action to perform before completing the
     * returned Future
     * @return the new Future
     */
    sp<Promise<T>> thenRun(const sp<Executor>& executor, const sp<Runnable>& action) {
        if (executor == nullptr) {
            throw NullPointerException("Executor must not be null");
        }
        if (action == nullptr) {
            throw NullPointerException("Action must not be null");
        }
        sp<Promise<T>> p = new Promise<T>(mExecutor);
        sp<Thenable::Action> a = new RunAction(executor, this, p, action);
        if (isDone()) {
            a->tryRun();
        } else {
            addAction(a);
        }
        return p;
    }

    sp<Promise<T>> thenRun(const sp<Executor>& executor, const std::function<void (void)>& function) {
        return thenRun(executor, new Runnable(function));
    }

    /**
     * @see #thenRun(Executor, Runnable)
     */
    sp<Promise<T>> then(const sp<Executor>& executor, const sp<Runnable>& action) {
        return thenRun(executor, action);
    }

    sp<Promise<T>> then(const sp<Executor>& executor, const std::function<void (void)>& function) {
        return thenRun(executor, new Runnable(function));
    }

    /**
     * Returns a new Future that, when this Future completes
     * exceptionally, is executed with this Future's exception as the
     * argument to the supplied function using this Future's
     * default asynchronous execution.  Otherwise, if this Future
     * completes normally, then the returned Future also completes
     * normally with the same value.
     *
     * @param function the Function to use to compute the value of the
     * returned Future if this Future completed
     * exceptionally
     * @return the new Future
     */
    template<typename U>
    sp<Promise<U>> catchException(const std::function<U (const sp<Exception>&)>& function) {
        return catchException(mExecutor, function);
    }

    /**
     * Returns a new Future that, when this Future completes
     * exceptionally, is executed with this Future's exception as the
     * argument to the supplied function using the supplied
     * Handler.  Otherwise, if this Future completes normally,
     * then the returned Future also completes normally with the same value.
     *
     * @param handler the Handler to use for asynchronous execution
     * @param function the Function to use to compute the value of the
     * returned Future if this Future completed
     * exceptionally
     * @return the new Future
     */
    template<typename U>
    sp<Promise<U>> catchException(const sp<Handler>& handler, const std::function<U (const sp<Exception>&)>& function) {
        return catchException(handler->asExecutor(), function);
    }

    /**
     * Returns a new Future that, when this Future completes
     * exceptionally, is executed with this Future's exception as the
     * argument to the supplied function using the supplied
     * Executor.  Otherwise, if this Future completes normally,
     * then the returned Future also completes normally with the same value.
     *
     * @param executor the Executor to use for asynchronous execution
     * @param function the Function to use to compute the value of the
     * returned Future if this Future completed
     * exceptionally
     * @return the new Future
     */
    template<typename U>
    sp<Promise<U>> catchException(const sp<Executor>& executor, const std::function<U (const sp<Exception>&)>& function) {
        if (executor == nullptr) {
            throw NullPointerException("Executor must not be null");
        }
        sp<Promise<U>> p = new Promise<U>(mExecutor);
        sp<Thenable::Action> a = new ErrorFunctionAction<U>(executor, this, p, function);
        if (isDone()) {
            a->tryRun();
        } else {
            addAction(a);
        }
        return p;
    }

    sp<Promise<T>> catchException(const std::function<void (const sp<Exception>&)>& function) {
        return catchException(mExecutor, function);
    }

    sp<Promise<T>> catchException(const sp<Handler>& handler, const std::function<void (const sp<Exception>&)>& function) {
        return catchException(handler->asExecutor(), function);
    }

    sp<Promise<T>> catchException(const sp<Executor>& executor, const std::function<void (const sp<Exception>&)>& function) {
        if (executor == nullptr) {
            throw NullPointerException("Executor must not be null");
        }
        sp<Promise<T>> p = new Promise<T>(mExecutor);
        sp<Thenable::Action> a = new ErrorConsumerAction(executor, this, p, function);
        if (isDone()) {
            a->tryRun();
        } else {
            addAction(a);
        }
        return p;
    }

    sp<Promise<T>> orTimeout(uint64_t timeout) {
        if (!isDone()) {
            return then(Timeout::add(new typename Promise<T>::Timeout::Exception(this), timeout));
        }
        return this;
    }

    sp<Promise<T>> completeOnTimeout(T value, uint64_t timeout) {
        if (!isDone()) {
            return then(Timeout::add(new typename Promise<T>::Timeout::Completion(this, value), timeout));
        }
        return this;
    }

    sp<Promise<T>> await(uint64_t delay) {
        sp<Promise<T>> p = new Promise<T>(mExecutor);
        sp<Thenable::Action> a = new DelayAction(this, p, delay);
        if (isDone()) {
            a->tryRun();
        } else {
            addAction(a);
        }
        return p;
    }

private:
    template<typename U>
    class RelayAction : public Thenable::Action {
    public:
        RelayAction(const sp<Promise<T>>& supplier, const sp<Promise<U>>& consumer) :
                Action(nullptr), mSupplier(supplier), mConsumer(consumer) {
        }

        virtual void tryRun() override final {
            if (claim()) {
                run();
            }
        }

        virtual void run() override final {
            if (mSupplier->getException() == nullptr) {
                mConsumer->setResult(mSupplier->getResult());
            } else {
                mConsumer->setException(toCompletionException(mSupplier->getException()));
            }

            mConsumer->onComplete();
        }

    private:
        sp<Promise<T>> mSupplier;
        sp<Promise<U>> mConsumer;
    };

    class BiRelayAction : public Thenable::Action {
    public:
        BiRelayAction(const sp<Thenable>& supplier1, const sp<Thenable>& supplier2, const sp<Promise<sp<Void>>>& consumer) :
            Action(nullptr), mSupplier1(supplier1), mSupplier2(supplier2), mConsumer(consumer) {
        }

        virtual void tryRun() override final {
            if (mSupplier1->isDone() && mSupplier2->isDone()) {
                if (claim()) {
                    run();
                }
            }
        }

        virtual void run() override final {
            if (mSupplier1->getException() != nullptr) {
                mConsumer->setException(toCompletionException(mSupplier1->getException()));
            } else if (mSupplier2->getException() != nullptr) {
                mConsumer->setException(toCompletionException(mSupplier2->getException()));
            } else {
                mConsumer->setResult(T());
            }

            mConsumer->onComplete();
        }

    private:
        sp<Thenable> mSupplier1;
        sp<Thenable> mSupplier2;
        sp<Promise<sp<Void>>> mConsumer;
    };

    template<typename U>
    class FunctionAction : public Thenable::Action {
    public:
        FunctionAction(const sp<Executor>& executor, const sp<Promise<T>>& supplier, const sp<Promise<U>>& consumer,
                const std::function<U (T)>& function) : Action(executor),
                mSupplier(supplier), mConsumer(consumer), mFunction(function) {
        }

        virtual void run() override final {
            if (mSupplier->getException() == nullptr) {
                try {
                    U u = mFunction(mSupplier->getResult());
                    mConsumer->setResult(u);
                } catch (const Exception& e) {
                    mConsumer->setException(toCompletionException(e));
                } catch (const std::exception& e) {
                    mConsumer->setException(new CompletionException(e));
                } catch (...) {
                    mConsumer->setException(new CompletionException("Unknown exception"));
                }
            } else {
                mConsumer->setException(toCompletionException(mSupplier->getException()));
            }

            mConsumer->onComplete();
        }

    private:
        sp<Promise<T>> mSupplier;
        sp<Promise<U>> mConsumer;
        std::function<U (T)> mFunction;
    };

    template<typename U>
    class BiFunctionAction : public Thenable::Action {
    public:
        BiFunctionAction(const sp<Executor>& executor, const sp<Promise<T>>& supplier, const sp<Promise<U>>& consumer,
                const std::function<U (T, const sp<Exception>&)>& function) : Action(executor),
                mSupplier(supplier), mConsumer(consumer), mFunction(function) {
        }

        virtual void run() override final {
            try {
                U u;
                if (mSupplier->getException() == nullptr) {
                    u = mFunction(mSupplier->getResult(), nullptr);
                } else {
                    u = mFunction(T(), mSupplier->getException());
                }
                mConsumer->setResult(u);
            } catch (const Exception& e) {
                mConsumer->setException(toCompletionException(e));
            } catch (const std::exception& e) {
                mConsumer->setException(new CompletionException(e));
            } catch (...) {
                mConsumer->setException(new CompletionException("Unknown exception"));
            }

            mConsumer->onComplete();
        }

    private:
        sp<Promise<T>> mSupplier;
        sp<Promise<U>> mConsumer;
        std::function<U (T, const sp<Exception>&)> mFunction;
    };

    template<typename U>
    class CompositionFunctionAction: public Thenable::Action {
    public:
        CompositionFunctionAction(const sp<Executor>& executor, const sp<Promise<T>>& supplier, const sp<Promise<U>>& consumer,
                const std::function<sp<Promise<U>> (T)>& function) : Action(executor),
                mSupplier(supplier), mConsumer(consumer), mFunction(function) {
        }

        virtual void run() override final {
            if (mSupplier->getException() == nullptr) {
                try {
                    sp<Promise<T>> u = mFunction(mSupplier->getResult());
                    mConsumer->completeWith(u);
                    return;
                } catch (const Exception& e) {
                    mConsumer->setException(toCompletionException(e));
                } catch (const std::exception& e) {
                    mConsumer->setException(new CompletionException(e));
                } catch (...) {
                    mConsumer->setException(new CompletionException("Unknown exception"));
                }
            } else {
                mConsumer->setException(toCompletionException(mSupplier->getException()));
            }

            mConsumer->onComplete();
        }

    private:
        sp<Promise<T>> mSupplier;
        sp<Promise<U>> mConsumer;
        std::function<sp<Promise<U>> (T)> mFunction;
    };

    template<typename U>
    class BiCompositionFunctionAction : public Thenable::Action {
    public:
        BiCompositionFunctionAction(const sp<Executor>& executor, const sp<Promise<T>>& supplier, const sp<Promise<U>>& consumer,
                const std::function<sp<Promise<U>> (T, const sp<Exception>&)>& function) : Action(executor),
                mSupplier(supplier), mConsumer(consumer), mFunction(function) {
        }

        virtual void run() override final {
            try {
                sp<Promise<T>> u;
                if (mSupplier->getException() == nullptr) {
                    u = mFunction(mSupplier->getResult(), nullptr);
                } else {
                    u = mFunction(T(), mSupplier->getException());
                }
                mConsumer->completeWith(u);
                return;
            } catch (const Exception& e) {
                mConsumer->setException(toCompletionException(e));
            } catch (const std::exception& e) {
                mConsumer->setException(new CompletionException(e));
            } catch (...) {
                mConsumer->setException(new CompletionException("Unknown exception"));
            }

            mConsumer->onComplete();
        }

    private:
        sp<Promise<T>> mSupplier;
        sp<Promise<U>> mConsumer;
        std::function<sp<Promise<U>> (T, const sp<Exception>&)> mFunction;
    };

    class ConsumerAction : public Thenable::Action {
    public:
        ConsumerAction(const sp<Executor>& executor, const sp<Promise<T>>& supplier, const sp<Promise<T>>& consumer,
                const std::function<void (T)>& function) : Action(executor),
                mSupplier(supplier), mConsumer(consumer), mFunction(function) {
        }

        virtual void run() override final {
            if (mSupplier->getException() == nullptr) {
                try {
                    mFunction(mSupplier->getResult());
                    mConsumer->setResult(mSupplier->getResult());
                } catch (const Exception& e) {
                    mConsumer->setException(toCompletionException(e));
                } catch (const std::exception& e) {
                    mConsumer->setException(new CompletionException(e));
                } catch (...) {
                    mConsumer->setException(new CompletionException("Unknown exception"));
                }
            } else {
                mConsumer->setException(toCompletionException(mSupplier->getException()));
            }

            mConsumer->onComplete();
        }

    private:
        sp<Promise<T>> mSupplier;
        sp<Promise<T>> mConsumer;
        std::function<void (T)> mFunction;
    };

    class BiConsumerAction : public Thenable::Action {
    public:
        BiConsumerAction(const sp<Executor>& executor, const sp<Promise<T>>& supplier, const sp<Promise<T>>& consumer,
                const std::function<void (T, const sp<Exception>&)>& function) : Action(executor),
                mSupplier(supplier), mConsumer(consumer), mFunction(function) {
        }

        virtual void run() override final {
            try {
                if (mSupplier->getException() == nullptr) {
                    mFunction(mSupplier->getResult(), nullptr);
                    mConsumer->setResult(mSupplier->getResult());
                } else {
                    mFunction(T(), mSupplier->getException());
                    mConsumer->setException(mSupplier->getException());
                }
            } catch (const Exception& e) {
                mConsumer->setException(toCompletionException(e));
            } catch (const std::exception& e) {
                mConsumer->setException(new CompletionException(e));
            } catch (...) {
                mConsumer->setException(new CompletionException("Unknown exception"));
            }

            mConsumer->onComplete();
        }

    private:
        sp<Promise<T>> mSupplier;
        sp<Promise<T>> mConsumer;
        std::function<void (T, const sp<Exception>&)> mFunction;
    };

    class RunAction : public Thenable::Action {
    public:
        RunAction(const sp<Executor>& executor, const sp<Promise<T>>& supplier, const sp<Promise<T>>& consumer, const sp<Runnable>& action) :
                Action(executor),
                mSupplier(supplier), mConsumer(consumer), mAction(action) {
        }

        virtual void run() override final {
            if (mSupplier->getException() == nullptr) {
                try {
                    mAction->run();
                    mConsumer->setResult(mSupplier->getResult());
                } catch (const Exception& e) {
                    mConsumer->setException(toCompletionException(e));
                } catch (const std::exception& e) {
                    mConsumer->setException(new CompletionException(e));
                } catch (...) {
                    mConsumer->setException(new CompletionException("Unknown exception"));
                }
            } else {
                mConsumer->setException(toCompletionException(mSupplier->getException()));
            }

            mConsumer->onComplete();
        }

    private:
        sp<Promise<T>> mSupplier;
        sp<Promise<T>> mConsumer;
        sp<Runnable> mAction;
    };

    class AnyOfAction : public Thenable::Action {
    public:
        AnyOfAction(const sp<Promise<T>>& supplier, const sp<Promise<T>>& consumer, const sp<ArrayList<wp<AnyOfAction>>>& actions) :
                Action(nullptr),
                mSupplier(supplier), mConsumer(consumer), mActions(actions) {
        }

        virtual void tryRun() override final {
            if (claim()) {
                run();
            }
        }

        virtual void run() override final {
            if (mSupplier->getException() == nullptr) {
                mConsumer->setResult(mSupplier->getResult());
            } else {
                mConsumer->setException(toCompletionException(mSupplier->getException()));
            }

            mConsumer->onComplete();

            // TODO: Calling mSupplier->removeAction here may lead to concurrent modification problems.
            // while mConsumer->onComplete iterates over Thenable::mActions.
            //auto itr = mActions->iterator();
            //while (itr.hasNext()) {
            //    sp<AnyOfAction> a = itr.next().lock();
            //    if (a != nullptr && a != this) {
            //        a->mSupplier->removeAction(a);
            //    }
            //}
        }

    private:
        sp<Promise<T>> mSupplier;
        sp<Promise<T>> mConsumer;
        sp<ArrayList<wp<AnyOfAction>>> mActions;
    };

    template<typename U>
    class ErrorFunctionAction : public Thenable::Action {
    public:
        ErrorFunctionAction(const sp<Executor>& executor, const sp<Promise<T>>& supplier, const sp<Promise<U>>& consumer,
                const std::function<U (const sp<Exception>&)>& function) : Action(executor),
                mSupplier(supplier), mConsumer(consumer), mFunction(function) {
        }

        virtual void run() override final {
            if (mSupplier->getException() != nullptr) {
                try {
                    U u = mFunction(mSupplier->getException());
                    mConsumer->setResult(u);
                } catch (const Exception& e) {
                    mConsumer->setException(toCompletionException(e));
                } catch (const std::exception& e) {
                    mConsumer->setException(new CompletionException(e));
                } catch (...) {
                    mConsumer->setException(new CompletionException("Unknown exception"));
                }
            } else {
                mConsumer->setResult(mSupplier->getResult());
            }

            mConsumer->onComplete();
        }

    private:
        sp<Promise<T>> mSupplier;
        sp<Promise<U>> mConsumer;
        std::function<U (const sp<Exception>&)> mFunction;
    };

    class ErrorConsumerAction : public Thenable::Action {
    public:
        ErrorConsumerAction(const sp<Executor>& executor, const sp<Promise<T>>& supplier, const sp<Promise<T>>& consumer,
                const std::function<void (const sp<Exception>&)>& function) : Action(executor),
                mSupplier(supplier), mConsumer(consumer), mFunction(function) {
        }

        virtual void run() override final {
            if (mSupplier->getException() != nullptr) {
                try {
                    mFunction(mSupplier->getException());
                    mConsumer->setException(mSupplier->getException());
                } catch (const Exception& e) {
                    mConsumer->setException(toCompletionException(e));
                } catch (const std::exception& e) {
                    mConsumer->setException(new CompletionException(e));
                } catch (...) {
                    mConsumer->setException(new CompletionException("Unknown exception"));
                }
            } else {
                mConsumer->setResult(mSupplier->getResult());
            }

            mConsumer->onComplete();
        }

    private:
        sp<Promise<T>> mSupplier;
        sp<Promise<T>> mConsumer;
        std::function<void (const sp<Exception>&)> mFunction;
    };

    class DelayAction : public Thenable::Action {
    public:
        DelayAction(const sp<Promise<T>>& supplier, const sp<Promise<T>>& consumer, uint64_t delay) :
            Action(nullptr), mSupplier(supplier), mConsumer(consumer), mDelay(delay) {
        }

        virtual void tryRun() override final {
            if (claim()) {
                run();
            }
        }

        virtual void run() override final {
            if (mSupplier->getException() == nullptr) {
                try {
                    mConsumer->completeOnTimeout(mSupplier->getResult(), mDelay);
                } catch (const Exception& e) {
                    mConsumer->setException(toCompletionException(e));
                } catch (const std::exception& e) {
                    mConsumer->setException(new CompletionException(e));
                } catch (...) {
                    mConsumer->setException(new CompletionException("Unknown exception"));
                }
            } else {
                mConsumer->setException(toCompletionException(mSupplier->getException()));
            }
        }

    private:
        sp<Promise<T>> mSupplier;
        sp<Promise<T>> mConsumer;
        uint64_t mDelay;
    };

    /// @private
    class Timeout {
    public:
        static std::function<void (T, const sp<mindroid::Exception>&)> add(const sp<Runnable>& command, uint64_t delay) {
            sLock.lock();
            if (sThread == nullptr) {
                sThread = new HandlerThread("TimeoutExecutorDaemon");
                sThread->start();
                sHandler = new Handler(sThread->getLooper());
            }
            sLock.unlock();
            sHandler->postDelayed(command, delay);
            return [=] (T ignore, const sp<mindroid::Exception>& exception) {
                sHandler->removeCallbacks(command);
            };
        }

        /// @private
        class Completion : public Runnable {
        public:
            Completion(const sp<Promise<T>>& consumer, T value) : mConsumer(consumer), mValue(value) {
            }

            virtual void run() override final {
                if (mConsumer != nullptr && !mConsumer->isDone()) {
                    mConsumer->complete(mValue);
                }
            }

        private:
            sp<Promise<T>> mConsumer;
            T mValue;
        };

        /// @private
        class Exception : public Runnable {
        public:
            Exception(const sp<Promise<T>>& consumer) : mConsumer(consumer) {
            }

            virtual void run() override final {
                if (mConsumer != nullptr && !mConsumer->isDone()) {
                    mConsumer->completeWith(sp<mindroid::Exception>(new TimeoutException("Timeout exception")));
                }
            }

        private:
            sp<Promise<T>> mConsumer;
        };

    private:
        static std::mutex sLock;
        static sp<HandlerThread> sThread;
        static sp<Handler> sHandler;
    };

    void onComplete() {
        AutoLock autoLock(mLock);
        mCondition->signalAll();
        runActions();
    }

    static sp<CompletionException> toCompletionException(const Exception& exception) {
        return toCompletionException(exception.clone());
    }

    static sp<CompletionException> toCompletionException(const sp<Exception>& exception) {
        return Class<CompletionException>::isInstance(exception) ? Class<CompletionException>::cast(exception) : new CompletionException(exception);
    }

    void setResult(T result) {
        AutoLock autoLock(mLock);
        if (!mIsDone) {
            mResult = result;
            mIsDone = true;
        }
    }

    T getResult() {
        AutoLock autoLock(mLock);
        return mResult;
    }

    static sp<Promise<sp<Void>>> andTree(const sp<Executor>& executor, const std::vector<sp<Thenable>>& promises, size_t start, size_t end) {
        sp<Thenable> supplier1;
        sp<Thenable> supplier2;
        sp<Promise<sp<Void>>> consumer = new Promise<sp<Void>>(executor);
        sp<Exception> exception;
        size_t mid = (start + end) / 2;
        if ((supplier1 = (start == mid ? promises[start] :
                object_cast<Thenable>(andTree(executor, promises, start, mid)))) == nullptr ||
            (supplier2 = (start == end ? supplier1 : (end == mid + 1) ? promises[end] :
                    object_cast<Thenable>(andTree(executor, promises, mid + 1, end)))) == nullptr) {
            if (supplier1 == nullptr || supplier2 == nullptr) {
                throw NullPointerException("Supplier must not be null");
            }
        }
        if (!supplier1->isDone() || !supplier2->isDone()) {
            sp<Promise<sp<Void>>> p = new Promise<sp<Void>>(executor);
            sp<Thenable::Action> action = new BiRelayAction(supplier1, supplier2, consumer);
            if (supplier1->isDone()) {
                action->tryRun();
            } else {
                supplier1->addAction(action);
            }
            if (supplier2->isDone()) {
                action->tryRun();
            } else {
                supplier2->addAction(action);
            }
        } else if (((exception = supplier1->getException()) != nullptr) || ((exception = supplier2->getException()) != nullptr)) {
            consumer->setException(toCompletionException(exception));
        } else {
            consumer->setResult(nullptr);
        }
        return consumer;
    }

    using Thenable::setException;
    using Thenable::getException;

    using Thenable::addAction;
    using Thenable::removeAction;
    using Thenable::runActions;

private:
    using Thenable::mLock;
    using Thenable::mCondition;
    T mResult;
    using Thenable::mException;
    using Thenable::mIsDone;
    using Thenable::mExecutor;

    template <typename> friend class Promise;
};

template <typename T>
std::mutex Promise<T>::Timeout::sLock;
template <typename T>
sp<HandlerThread> Promise<T>::Timeout::sThread = nullptr;
template <typename T>
sp<Handler> Promise<T>::Timeout::sHandler = nullptr;

} /* namespace mindroid */

#endif /* MINDROID_UTIL_CONCURRENT_PROMISE_H_ */
