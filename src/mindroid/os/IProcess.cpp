/*
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

#include <mindroid/os/IProcess.h>
#include <mindroid/os/IRemoteCallback.h>
#include <mindroid/content/Intent.h>
#include <mindroid/content/ServiceConnection.h>
#include <mindroid/net/URI.h>
#include <mindroid/runtime/system/Runtime.h>

namespace mindroid {
namespace binder {

const char* const Process::Stub::DESCRIPTOR = "mindroid://interfaces/mindroid/os/IProcess";

void Process::Stub::onTransact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Thenable>& result) {
    switch (what) {
    case MSG_CREATE_SERVICE: {
        sp<Intent> intent = object_cast<Intent>(data->getObject("intent"));
        sp<IBinder> binder = data->getBinder("binder");
        createService(intent, RemoteCallback::Stub::asInterface(binder));
        break;
    }
    case MSG_START_SERVICE: {
        sp<Intent> intent = object_cast<Intent>(data->getObject("intent"));
        int32_t flags = data->getInt("flags");
        int32_t startId = data->getInt("startId");
        sp<IBinder> binder = data->getBinder("binder");
        startService(intent, flags, startId, RemoteCallback::Stub::asInterface(binder));
        break;
    }
    case MSG_STOP_SERVICE: {
        sp<Intent> intent = object_cast<Intent>(data->getObject("intent"));
        sp<IBinder> binder = data->getBinder("binder");
        if (binder == nullptr) {
            stopService(intent);
        } else {
            stopService(intent, RemoteCallback::Stub::asInterface(binder));
        }
        break;
    }
    case MSG_BIND_SERVICE: {
        sp<Intent> intent = object_cast<Intent>(data->getObject("intent"));
        sp<ServiceConnection> conn = object_cast<ServiceConnection>(data->getObject("conn"));
        int32_t flags = data->getInt("flags");
        sp<IBinder> binder = data->getBinder("binder");
        bindService(intent, conn, flags, RemoteCallback::Stub::asInterface(binder));
        break;
    }
    case MSG_UNBIND_SERVICE: {
        sp<Intent> intent = object_cast<Intent>(data->getObject("intent"));
        sp<IBinder> binder = data->getBinder("binder");
        if (binder == nullptr) {
            unbindService(intent);
        } else {
            unbindService(intent, RemoteCallback::Stub::asInterface(binder));
        }
        break;
    }
    default:
        Binder::onTransact(what, num, obj, data, result);
        break;
    }
}

void Process::Stub::Proxy::createService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) {
    sp<Bundle> data = new Bundle();
    data->putObject("intent", intent);
    data->putBinder("binder", callback->asBinder());
    mRemote->transact(MSG_CREATE_SERVICE, 0, nullptr, data, nullptr, FLAG_ONEWAY);
}

void Process::Stub::Proxy::startService(const sp<Intent>& intent, int32_t flags, int32_t startId, const sp<IRemoteCallback>& callback) {
    sp<Bundle> data = new Bundle();
    data->putObject("intent", intent);
    data->putInt("flags", flags);
    data->putInt("startId", startId);
    data->putBinder("binder", callback->asBinder());
    mRemote->transact(MSG_START_SERVICE, 0, nullptr, data, nullptr, FLAG_ONEWAY);
}

void Process::Stub::Proxy::stopService(const sp<Intent>& intent) {
    sp<Bundle> data = new Bundle();
    data->putObject("intent", intent);
    mRemote->transact(MSG_STOP_SERVICE, 0, nullptr, data, nullptr, FLAG_ONEWAY);
}

void Process::Stub::Proxy::stopService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) {
    sp<Bundle> data = new Bundle();
    data->putObject("intent", intent);
    data->putBinder("binder", callback->asBinder());
    mRemote->transact(MSG_STOP_SERVICE, 0, nullptr, data, nullptr, FLAG_ONEWAY);
}

void Process::Stub::Proxy::bindService(const sp<Intent>& intent, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) {
    sp<Bundle> data = new Bundle();
    data->putObject("intent", intent);
    data->putObject("conn", conn);
    data->putInt("flags", flags);
    data->putBinder("binder", callback->asBinder());
    mRemote->transact(MSG_BIND_SERVICE, 0, nullptr, data, nullptr, FLAG_ONEWAY);
}

void Process::Stub::Proxy::unbindService(const sp<Intent>& intent) {
    sp<Bundle> data = new Bundle();
    data->putObject("intent", intent);
    mRemote->transact(MSG_UNBIND_SERVICE, 0, nullptr, data, nullptr, FLAG_ONEWAY);
}

void Process::Stub::Proxy::unbindService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) {
    sp<Bundle> data = new Bundle();
    data->putObject("intent", intent);
    data->putBinder("binder", callback->asBinder());
    mRemote->transact(MSG_UNBIND_SERVICE, 0, nullptr, data, nullptr, FLAG_ONEWAY);
}

Process::Proxy::Proxy(const sp<IBinder>& binder) {
    mBinder = binder;
    if (binder->getUri()->getScheme()->equals("mindroid")) {
        mStub = object_cast<Process::Stub>(binder->queryLocalInterface(Process::Stub::DESCRIPTOR));
        mProxy = new Process::Stub::Proxy(binder);
    } else {
        sp<Runtime> runtime = Runtime::getRuntime();
        mStub = object_cast<Process::Stub>(runtime->getBinder(binder->getId()));
        mProxy = object_cast<IProcess>(runtime->getProxy(binder));
    }
}

void Process::Proxy::createService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        mStub->createService(intent, callback);
    } else {
        mProxy->createService(intent, callback);
    }
}

void Process::Proxy::startService(const sp<Intent>& intent, int32_t flags, int32_t startId, const sp<IRemoteCallback>& callback) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        mStub->startService(intent, flags, startId, RemoteCallback::Stub::asInterface(callback->asBinder()));
    } else {
        mProxy->startService(intent, flags, startId, callback);
    }
}

void Process::Proxy::stopService(const sp<Intent>& intent) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        mStub->stopService(intent);
    } else {
        mProxy->stopService(intent);
    }
}

void Process::Proxy::stopService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        mStub->stopService(intent, RemoteCallback::Stub::asInterface(callback->asBinder()));
    } else {
        mProxy->stopService(intent, callback);
    }
}

void Process::Proxy::bindService(const sp<Intent>& intent, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        mStub->bindService(intent, conn, flags, RemoteCallback::Stub::asInterface(callback->asBinder()));
    } else {
        mProxy->bindService(intent, conn, flags, callback);
    }
}

void Process::Proxy::unbindService(const sp<Intent>& intent) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        mStub->unbindService(intent);
    } else {
        mProxy->unbindService(intent);
    }
}

void Process::Proxy::unbindService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        mStub->unbindService(intent, RemoteCallback::Stub::asInterface(callback->asBinder()));
    } else {
        mProxy->unbindService(intent, callback);
    }
}

} /* namespace binder */
} /* namespace mindroid */
