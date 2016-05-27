/*
 * Copyright (C) 2010 The Android Open Source Project
 * Copyright (C) 2016 Daniel Himmelein
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

#ifndef MINDROID_REMOTECALLBACK_H_
#define MINDROID_REMOTECALLBACK_H_

#include "mindroid/lang/Object.h"
#include "mindroid/os/IRemoteCallback.h"

namespace mindroid {

/**
 * @hide
 */
class RemoteCallback :
		public Object {
public:
	RemoteCallback(const sp<Handler> handler) :
			mHandler(handler),
			mCallback(new Stub(this)) {
	}

	virtual ~RemoteCallback() = default;

	sp<IRemoteCallback> asInterface() {
		return mCallback;
	}

protected:
	virtual void onResult(const sp<Bundle>& data) = 0;

	sp<Handler> getHandler() {
		return mHandler;
	}

private:
	class Stub : public binder::RemoteCallback::Stub {
	public:
		Stub(const sp<RemoteCallback>& remoteCallback) :
				mRemoteCallback(remoteCallback) {
		}

		virtual void sendResult(const sp<Bundle>& data) {
			sp<RemoteCallback> remoteCallback = mRemoteCallback;
			remoteCallback->mHandler->post([=] { remoteCallback->onResult(data); });
			mRemoteCallback = nullptr;
		};

	private:
		sp<RemoteCallback> mRemoteCallback;
	};

	sp<Handler> mHandler;
	sp<IRemoteCallback> mCallback;
};

} /* namespace mindroid */

#endif /* MINDROID_REMOTECALLBACK_H_ */
