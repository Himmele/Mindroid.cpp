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

#include "mindroid/os/Binder.h"
#include "mindroid/util/Log.h"
#include "mindroid/util/concurrent/Awaitable.h"

namespace mindroid {

const char* const Binder::LOG_TAG = "Binder";
const sp<String> Binder::EXCEPTION_MESSAGE = String::valueOf("Binder transaction failure");

void Binder::Messenger::handleMessage(const sp<Message>& message) {
	mBinder->onTransact(message->what, message->arg1, message->arg2, message->obj, message->peekData(), message->result);
}

void Binder::transact(int32_t what, const sp<Awaitable>& result, int32_t flags) {
	sp<Message> message = mMessenger->obtainMessage(what);
	if (flags == FLAG_ONEWAY) {
		message->sendToTarget();
	} else {
		message->result = result;
		message->sendToTarget();
		try {
			result->await();
		} catch (const CancellationException& e) {
			throw RemoteException(EXCEPTION_MESSAGE);
		} catch (const ExecutionException& e) {
			throw RemoteException(EXCEPTION_MESSAGE);
		}
	}
}

void Binder::transact(int32_t what, const sp<Object>& obj, const sp<Awaitable>& result, int32_t flags) {
	sp<Message> message = mMessenger->obtainMessage(what, obj);
	if (flags == FLAG_ONEWAY) {
		message->sendToTarget();
	} else {
		message->result = result;
		message->sendToTarget();
		try {
			result->await();
		} catch (const CancellationException& e) {
			throw RemoteException(EXCEPTION_MESSAGE);
		} catch (const ExecutionException& e) {
			throw RemoteException(EXCEPTION_MESSAGE);
		}
	}
}

void Binder::transact(int32_t what, int32_t arg1, int32_t arg2, const sp<Awaitable>& result, int32_t flags) {
	sp<Message> message = mMessenger->obtainMessage(what, arg1, arg2);
	if (flags == FLAG_ONEWAY) {
		message->sendToTarget();
	} else {
		message->result = result;
		message->sendToTarget();
		try {
			result->await();
		} catch (const CancellationException& e) {
			throw RemoteException(EXCEPTION_MESSAGE);
		} catch (const ExecutionException& e) {
			throw RemoteException(EXCEPTION_MESSAGE);
		}
	}
}

void Binder::transact(int32_t what, int32_t arg1, int32_t arg2, const sp<Object>& obj, const sp<Awaitable>& result, int32_t flags) {
	sp<Message> message = mMessenger->obtainMessage(what, arg1, arg2, obj);
	if (flags == FLAG_ONEWAY) {
		message->sendToTarget();
	} else {
		message->result = result;
		message->sendToTarget();
		try {
			result->await();
		} catch (const CancellationException& e) {
			throw RemoteException(EXCEPTION_MESSAGE);
		} catch (const ExecutionException& e) {
			throw RemoteException(EXCEPTION_MESSAGE);
		}
	}
}

void Binder::transact(int32_t what, const sp<Bundle>& data, const sp<Awaitable>& result, int32_t flags) {
	sp<Message> message = mMessenger->obtainMessage(what);
	message->setData(data);
	if (flags == FLAG_ONEWAY) {
		message->sendToTarget();
	} else {
		message->result = result;
		message->sendToTarget();
		try {
			result->await();
		} catch (const CancellationException& e) {
			throw RemoteException(EXCEPTION_MESSAGE);
		} catch (const ExecutionException& e) {
			throw RemoteException(EXCEPTION_MESSAGE);
		}
	}
}

void Binder::transact(int32_t what, int32_t arg1, int32_t arg2, const sp<Bundle>& data, const sp<Awaitable>& result, int32_t flags) {
	sp<Message> message = mMessenger->obtainMessage(what, arg1, arg2);
	message->setData(data);
	if (flags == FLAG_ONEWAY) {
		message->sendToTarget();
	} else {
		message->result = result;
		message->sendToTarget();
		try {
			result->await();
		} catch (const CancellationException& e) {
			throw RemoteException(EXCEPTION_MESSAGE);
		} catch (const ExecutionException& e) {
			throw RemoteException(EXCEPTION_MESSAGE);
		}
	}
}

} /* namespace mindroid */
