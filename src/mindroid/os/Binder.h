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

#ifndef MINDROID_BINDER_H_
#define MINDROID_BINDER_H_

#include "mindroid/lang/String.h"
#include "mindroid/os/IBinder.h"
#include "mindroid/os/IInterface.h"
#include "mindroid/os/Handler.h"
#include "mindroid/os/Looper.h"
#include "mindroid/os/RemoteException.h"

namespace mindroid {

class Awaitable;

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
	static const sp<String> EXCEPTION_MESSAGE;

	class Messenger : public Handler {
	public:
		Messenger(const sp<Binder>& binder) : Handler(binder->mLooper), mBinder(binder) {
		}

		virtual void handleMessage(const sp<Message>& message);

	private:
		sp<Binder> mBinder;
	};

public:
    Binder() = default;
    
    /**
	 * Convenience method for associating a specific interface with the Binder. After calling,
	 * queryInterface() will be implemented for you to return the given owner IInterface when the
	 * corresponding descriptor is requested.
	 */
    void attachInterface(const sp<IInterface>& owner, const sp<String>& descriptor) {
        mOwner = owner;
        mDescriptor = descriptor;
        mLooper = Looper::myLooper();
    	mMessenger = new Messenger(this);
    }

    void attachInterface(const sp<IInterface>& owner, const sp<String>& descriptor, const sp<Looper>& looper) {
        mOwner = owner;
        mDescriptor = descriptor;
        mLooper = looper;
    	mMessenger = new Messenger(this);
    }
	
    /**
	 * Default implementation returns an empty interface name.
	 */
	virtual sp<String> getInterfaceDescriptor() {
		return mDescriptor;
	}

	/**
	 * Use information supplied to attachInterface() to return the associated IInterface if it
	 * matches the requested descriptor.
	 */
    virtual sp<IInterface> queryLocalInterface(const char* descriptor) {
    	return queryLocalInterface(String::valueOf(descriptor));
    }
    virtual sp<IInterface> queryLocalInterface(const sp<String>& descriptor) {
        if (mDescriptor->equals(descriptor)) {
            return mOwner;
        }
        return nullptr;
    }
	
    /**
	 * Default implementations rewinds the parcels and calls onTransact. On the remote side,
	 * transact calls into the binder to do the IPC.
	 */
    virtual void transact(int32_t what, const sp<Awaitable>& result, int32_t flags);
	virtual void transact(int32_t what, const sp<Object>& obj, const sp<Awaitable>& result, int32_t flags);
	virtual void transact(int32_t what, int32_t arg1, int32_t arg2, const sp<Awaitable>& result, int32_t flags);
	virtual void transact(int32_t what, int32_t arg1, int32_t arg2, const sp<Object>& obj, const sp<Awaitable>& result, int32_t flags);
	virtual void transact(int32_t what, const sp<Bundle>& data, const sp<Awaitable>& result, int32_t flags);
	virtual void transact(int32_t what, int32_t arg1, int32_t arg2, const sp<Bundle>& data, const sp<Awaitable>& result, int32_t flags);

	virtual bool runsOnSameThread() {
		return mLooper == Looper::myLooper();
    }

protected:
	/**
	 * Default implementation is a stub that does nothing.  You will want
	 * to override this to do the appropriate unmarshalling of transactions.
	 *
	 * <p>If you want to call this, call transact().
	 */
    virtual void onTransact(int32_t what, int32_t arg1, int32_t arg2, const sp<Object>& obj, const sp<Bundle>& data, const sp<Object>& result) {
	}

private:
    static const char* const TAG;

	sp<IInterface> mOwner;
	sp<String> mDescriptor;
	sp<Messenger> mMessenger;
	sp<Looper> mLooper;
};

} /* namespace mindroid */

#endif /* MINDROID_BINDER_H_ */
