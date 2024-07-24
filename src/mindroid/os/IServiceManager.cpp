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

#include <mindroid/os/IServiceManager.h>
#include <mindroid/os/IRemoteCallback.h>
#include <mindroid/net/URI.h>
#include <mindroid/runtime/system/Runtime.h>

namespace mindroid {
namespace binder {

const char* const ServiceManager::Stub::DESCRIPTOR = "mindroid://interfaces/mindroid/os/IServiceManager";

void ServiceManager::Stub::onTransact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Thenable>& result) {
    switch (what) {
    case MSG_START_SERVICE: {
        sp<Intent> service = object_cast<Intent>(obj);
        object_cast<Promise<sp<ComponentName>>, Thenable>(result)->completeWith(startService(service));
        break;
    }
    case MSG_STOP_SERVICE: {
        sp<Intent> service = object_cast<Intent>(obj);
        object_cast<Promise<sp<Boolean>>, Thenable>(result)->completeWith(stopService(service));
        break;
    }
    case MSG_BIND_SERVICE: {
        sp<Intent> service = object_cast<Intent>(data->getObject("intent"));
        sp<ServiceConnection> conn = object_cast<ServiceConnection>(data->getObject("conn"));
        int32_t flags = data->getInt("flags");
        sp<IBinder> binder = data->getBinder("binder");
        object_cast<Promise<sp<Boolean>>, Thenable>(result)->completeWith(bindService(service, conn, flags, binder::RemoteCallback::Stub::asInterface(binder)));
        break;
    }
    case MSG_UNBIND_SERVICE: {
        sp<Intent> intent = object_cast<Intent>(data->getObject("intent"));
        sp<ServiceConnection> conn = object_cast<ServiceConnection>(data->getObject("conn"));
        sp<IBinder> binder = nullptr;
        if (binder == nullptr) {
            unbindService(intent, conn);
        } else {
            unbindService(intent, conn, binder::RemoteCallback::Stub::asInterface(binder));
        }
        break;
    }
    case MSG_START_SYSTEM_SERVICE: {
        sp<Intent> service = object_cast<Intent>(obj);
        object_cast<Promise<sp<ComponentName>>, Thenable>(result)->completeWith(startSystemService(service));
        break;
    }
    case MSG_STOP_SYSTEM_SERVICE: {
        sp<Intent> service = object_cast<Intent>(obj);
        object_cast<Promise<sp<Boolean>>, Thenable>(result)->completeWith(stopSystemService(service));
        break;
    }
    default:
        Binder::onTransact(what, num, obj, data, result);
    }
}

sp<Promise<sp<ComponentName>>> ServiceManager::Stub::Proxy::startService(const sp<Intent>& service) {
    sp<Promise<sp<ComponentName>>> promise = new Promise<sp<ComponentName>>();
    mRemote->transact(MSG_START_SERVICE, 0, service, nullptr, promise, 0);
    return promise;
}

sp<Promise<sp<Boolean>>> ServiceManager::Stub::Proxy::stopService(const sp<Intent>& service) {
    sp<Promise<sp<Boolean>>> promise = new Promise<sp<Boolean>>();
    mRemote->transact(MSG_STOP_SERVICE, 0, service, nullptr, promise, 0);
    return promise;
}

sp<Promise<sp<Boolean>>> ServiceManager::Stub::Proxy::bindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) {
    sp<Promise<sp<Boolean>>> promise = new Promise<sp<Boolean>>();
    sp<Bundle> data = new Bundle();
    data->putObject("intent", service);
    data->putObject("conn", conn);
    data->putInt("flags", flags);
    data->putBinder("binder", callback->asBinder());
    mRemote->transact(MSG_BIND_SERVICE, 0, nullptr, data, promise, 0);
    return promise;
}

void ServiceManager::Stub::Proxy::unbindService(const sp<Intent>& intent, const sp<ServiceConnection>& conn) {
    sp<Bundle> data = new Bundle();
    data->putObject("intent", intent);
    data->putObject("conn", conn);
    mRemote->transact(MSG_UNBIND_SERVICE, 0, nullptr, data, nullptr, FLAG_ONEWAY);
}

void ServiceManager::Stub::Proxy::unbindService(const sp<Intent>& intent, const sp<ServiceConnection>& conn, const sp<IRemoteCallback>& callback) {
    sp<Bundle> data = new Bundle();
    data->putObject("intent", intent);
    data->putObject("conn", conn);
    data->putBinder("binder", callback->asBinder());
    mRemote->transact(MSG_UNBIND_SERVICE, 0, nullptr, data, nullptr, FLAG_ONEWAY);
}

sp<Promise<sp<ComponentName>>> ServiceManager::Stub::Proxy::startSystemService(const sp<Intent>& service) {
    sp<Promise<sp<ComponentName>>> promise = new Promise<sp<ComponentName>>();
    mRemote->transact(MSG_START_SYSTEM_SERVICE, 0, service, nullptr, promise, 0);
    return promise;
}

sp<Promise<sp<Boolean>>> ServiceManager::Stub::Proxy::stopSystemService(const sp<Intent>& service) {
    sp<Promise<sp<Boolean>>> promise = new Promise<sp<Boolean>>();
    mRemote->transact(MSG_STOP_SYSTEM_SERVICE, 0, service, nullptr, promise, 0);
    return promise;
}

} /* namespace binder */
} /* namespace mindroid */
