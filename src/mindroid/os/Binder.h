/*
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (C) 2013 Daniel Himmelein
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

#ifndef MINDROID_OS_BINDER_H_
#define MINDROID_OS_BINDER_H_

#include <mindroid/lang/NoSuchMethodException.h>
#include <mindroid/lang/Runnable.h>
#include <mindroid/lang/String.h>
#include <mindroid/os/IBinder.h>
#include <mindroid/os/IInterface.h>
#include <mindroid/os/Handler.h>
#include <mindroid/os/Looper.h>
#include <mindroid/os/RemoteException.h>
#include <mindroid/net/URI.h>
#include <mindroid/util/concurrent/Executor.h>
#include <mindroid/util/Log.h>
#include <mindroid/util/concurrent/Promise.h>

namespace mindroid {

class Runtime;

/**
 * Base class for a remotable object, the core part of a lightweight remote procedure call mechanism
 * defined by {@link IBinder}. This class is an implementation of IBinder that provides the standard
 * support creating a local implementation of such an object.
 *
 * You can derive directly from Binder to implement your own custom RPC protocol or simply
 * instantiate a raw Binder object directly to use as a token that can be shared across processes.
 *
 * @see IBinder
 */
class Binder :
        public IBinder {
private:
    static const char* const TAG;
    static const int32_t TRANSACTION = 1;
    static const int32_t LIGHTWEIGHT_TRANSACTION = 2;
    static const sp<String> EXCEPTION_MESSAGE;

    class IMessenger : public Object {
    public:
        virtual bool isCurrentThread() = 0;
        virtual bool send(const sp<Message>& message) = 0;
    };

    class Messenger : public IMessenger {
    public:
        Messenger(Binder& binder) : Messenger(binder, Looper::myLooper()) {
        }

        Messenger(Binder& binder, const sp<Looper>& looper) {
            class BinderHandler : public Handler {
            public:
                BinderHandler(Binder& binder, const sp<Looper>& looper) : Handler(looper), mBinder(binder) {
                }

                void handleMessage(const sp<Message>& message) override {
                    mBinder.onTransact(message);
                }

            private:
                Binder& mBinder;
            };

            mHandler = new BinderHandler(binder, looper);
        }

        bool isCurrentThread() override {
            return mHandler->getLooper()->isCurrentThread();
        }

        bool send(const sp<Message>& message) override {
            return mHandler->sendMessage(message);
        }

    private:
        sp<Handler> mHandler;

        friend class Binder;
    };

    class ExecutorMessenger : public IMessenger {
    public:
        ExecutorMessenger(Binder& binder, const sp<Executor>& executor) : mBinder(binder), mExecutor(executor) {
        }

        bool isCurrentThread() override {
            return false;
        }

        bool send(const sp<Message>& message) override {
            mExecutor->execute(new Runnable([=] {
                mBinder.onTransact(message);
            }));
            return true;
        }

    private:
        Binder& mBinder;
        sp<Executor> mExecutor;
    };

public:
    Binder();
    Binder(const sp<Looper>& looper);
    Binder(const sp<Executor>& executor);
    /** @hide */
    Binder(const sp<Binder>& binder);
    virtual ~Binder();

    uint64_t getId() const override {
        return mId & 0xFFFFFFFFL;
    }

    sp<URI> getUri() const override {
        return mUri;
    }

    /**
     * Convenience method for associating a specific interface with the Binder. After calling,
     * queryInterface() will be implemented for you to return the given owner IInterface when the
     * corresponding descriptor is requested.
     */
    void attachInterface(const wp<IInterface>& owner, const sp<String>& descriptor);

    /**
     * Default implementation returns an empty interface name.
     */
    sp<String> getInterfaceDescriptor() const override {
        return mDescriptor;
    }

    /**
     * Use information supplied to attachInterface() to return the associated IInterface if it
     * matches the requested descriptor.
     */
    sp<IInterface> queryLocalInterface(const char* descriptor) override {
        return queryLocalInterface(String::valueOf(descriptor));
    }
    sp<IInterface> queryLocalInterface(const sp<String>& descriptor) override {
        if (mDescriptor->equals(descriptor)) {
            return mOwner.get();
        }
        return nullptr;
    }

    /**
     * Returns true if the current thread is this binder's thread.
     */
    bool isCurrentThread() {
        return mTarget->isCurrentThread();
    }

    /**
     * Default implementations rewinds the parcels and calls onTransact. On the remote side,
     * transact calls into the binder to do the IPC.
     */
    sp<Promise<sp<Parcel>>> transact(int32_t what, const sp<Parcel>& data, int32_t flags) override;

    void transact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Thenable>& result, int32_t flags) override;

    void link(const sp<Supervisor>& supervisor, const sp<Bundle>& extras) override;

    bool unlink(const sp<Supervisor>& supervisor, const sp<Bundle>& extras) override;

    bool equals(const sp<Object>& obj) const override;

    size_t hashCode() const override;

    template<typename T>
    static T get(const sp<Promise<T>>& result) {
        try {
            return result->get();
        } catch (const CancellationException& e) {
            throw RemoteException(EXCEPTION_MESSAGE);
        } catch (const ExecutionException& e) {
            if (e.getCause() != nullptr) {
                throw RemoteException(e.getCause()->getMessage());
            }
            throw RemoteException(EXCEPTION_MESSAGE);
        }
    }

    template<typename T>
    static T get(const sp<Promise<T>>& result, uint64_t timeout) {
        try {
            return result->get(timeout);
        } catch (const TimeoutException& e) {
            throw RemoteException(EXCEPTION_MESSAGE);
        } catch (const CancellationException& e) {
            throw RemoteException(EXCEPTION_MESSAGE);
        } catch (const ExecutionException& e) {
            if (e.getCause() != nullptr) {
                throw RemoteException(e.getCause()->getMessage());
            }
            throw RemoteException(EXCEPTION_MESSAGE);
        }
    }

protected:
    /**
     * Default implementation is a stub that returns null. You will want to override this to do the
     * appropriate unmarshalling of transactions.
     *
     * <p>
     * If you want to call this, call transact().
     */
    virtual void onTransact(int32_t what, const sp<Parcel>& data, const sp<Promise<sp<Parcel>>>& result) {
        throw RemoteException(NoSuchMethodException());
    }

    virtual void onTransact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Thenable>& result) {
        throw RemoteException(NoSuchMethodException());
    }

private:
    void onTransact(const sp<Message>& message);
    /** @hide */
    void setId(uint64_t id);

    uint64_t mId;
    sp<Runtime> mRuntime;
    sp<IMessenger> mTarget;
    wp<IInterface> mOwner;
    sp<String> mDescriptor;
    sp<URI> mUri;

    friend class Runtime;

public:
    class Proxy final : public IBinder {
    public:
        static sp<Proxy> create(const sp<URI>& uri);

        virtual ~Proxy();

        uint64_t getId() const override {
            return mId;
        }

        sp<URI> getUri() const override {
            return mUri;
        }

        sp<String> getInterfaceDescriptor() const override {
            return mDescriptor;
        }

        sp<IInterface> queryLocalInterface(const char* descriptor) override {
            return nullptr;
        }
        sp<IInterface> queryLocalInterface(const sp<String>& descriptor) override {
            return nullptr;
        }

        sp<Promise<sp<Parcel>>> transact(int32_t what, const sp<Parcel>& data, int32_t flags) override;

        void transact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Thenable>& result, int32_t flags) override;

        void link(const sp<Supervisor>& supervisor, const sp<Bundle>& extras) override;

        bool unlink(const sp<Supervisor>& supervisor, const sp<Bundle>& extras) override;

        bool equals(const sp<Object>& obj) const override;

        size_t hashCode() const override;

    private:
        Proxy(const sp<URI>& uri);

        static const sp<String> EXCEPTION_MESSAGE;

        uint64_t mProxyId = 0;
        uint64_t mId;
        sp<Runtime> mRuntime;
        sp<String> mDescriptor;
        sp<URI> mUri;
    };
};

} /* namespace mindroid */

#endif /* MINDROID_OS_BINDER_H_ */
