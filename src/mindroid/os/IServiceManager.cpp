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
#include <mindroid/lang/Boolean.h>
#include <mindroid/net/URI.h>
#include <mindroid/runtime/system/Runtime.h>

namespace mindroid {
namespace binder {

const char* const ServiceManager::Stub::DESCRIPTOR = "mindroid://interfaces/mindroid/os/IServiceManager";

void ServiceManager::Stub::onTransact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Promise<sp<Object>>>& result) {
    switch (what) {
    case MSG_START_SERVICE: {
        sp<Intent> service = object_cast<Intent>(obj);
        sp<ComponentName> component = startService(service);
        object_cast<Promise<sp<ComponentName>>, Object>(result)->complete(component);
        break;
    }
    case MSG_STOP_SERVICE: {
        sp<Intent> service = object_cast<Intent>(obj);
        object_cast<Promise<sp<Boolean>>, Object>(result)->complete(new Boolean(stopService(service)));
        break;
    }
    case MSG_BIND_SERVICE: {
        sp<Intent> service = object_cast<Intent>(data->getObject("intent"));
        sp<ServiceConnection> conn = object_cast<ServiceConnection>(data->getObject("conn"));
        int32_t flags = data->getInt("flags");
        sp<IBinder> binder = data->getBinder("binder");
        object_cast<Promise<sp<Boolean>>, Object>(result)->complete(new Boolean(bindService(service, conn, flags, binder::RemoteCallback::Stub::asInterface(binder))));
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
        sp<ComponentName> component = startSystemService(service);
        object_cast<Promise<sp<ComponentName>>, Object>(result)->complete(component);
        break;
    }
    case MSG_STOP_SYSTEM_SERVICE: {
        sp<Intent> service = object_cast<Intent>(obj);
        object_cast<Promise<sp<Boolean>>, Object>(result)->complete(new Boolean(stopSystemService(service)));
        break;
    }
    default:
        Binder::onTransact(what, num, obj, data, result);
    }
}

sp<ComponentName> ServiceManager::Stub::Proxy::startService(const sp<Intent>& service) {
    sp<Promise<sp<ComponentName>>> promise = new Promise<sp<ComponentName>>();
    mRemote->transact(MSG_START_SERVICE, 0, service, nullptr, object_cast<Promise<sp<Object>>, Object>(promise), 0);
    return Binder::get(promise);
}

bool ServiceManager::Stub::Proxy::stopService(const sp<Intent>& service) {
    sp<Promise<sp<Boolean>>> promise = new Promise<sp<Boolean>>();
    mRemote->transact(MSG_STOP_SERVICE, 0, service, nullptr, object_cast<Promise<sp<Object>>, Object>(promise), 0);
    return Binder::get(promise)->booleanValue();
}

bool ServiceManager::Stub::Proxy::bindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) {
    sp<Promise<sp<Boolean>>> promise = new Promise<sp<Boolean>>();
    sp<Bundle> data = new Bundle();
    data->putObject("intent", service);
    data->putObject("conn", conn);
    data->putInt("flags", flags);
    data->putBinder("binder", callback->asBinder());
    mRemote->transact(MSG_BIND_SERVICE, 0, nullptr, data, object_cast<Promise<sp<Object>>, Object>(promise), 0);
    return Binder::get(promise)->booleanValue();
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

sp<ComponentName> ServiceManager::Stub::Proxy::startSystemService(const sp<Intent>& service) {
    sp<Promise<sp<ComponentName>>> promise = new Promise<sp<ComponentName>>();
    mRemote->transact(MSG_START_SYSTEM_SERVICE, 0, service, nullptr, object_cast<Promise<sp<Object>>, Object>(promise), 0);
    return Binder::get(promise);
}

bool ServiceManager::Stub::Proxy::stopSystemService(const sp<Intent>& service) {
    sp<Promise<sp<Boolean>>> promise = new Promise<sp<Boolean>>();
    mRemote->transact(MSG_STOP_SYSTEM_SERVICE, 0, service, nullptr, object_cast<Promise<sp<Object>>, Object>(promise), 0);
    return Binder::get(promise)->booleanValue();
}

ServiceManager::Proxy::Proxy(const sp<IBinder>& binder) {
    mBinder = binder;
    if (binder->getUri()->getScheme()->equals("mindroid")) {
        mStub = object_cast<ServiceManager::Stub>(binder->queryLocalInterface(ServiceManager::Stub::DESCRIPTOR));
        mProxy = new ServiceManager::Stub::Proxy(binder);
    } else {
        sp<Runtime> runtime = Runtime::getRuntime();
        mStub = object_cast<ServiceManager::Stub>(runtime->getBinder(binder->getId()));
        mProxy = object_cast<IServiceManager>(runtime->getProxy(binder));
    }
}

sp<ComponentName> ServiceManager::Proxy::startService(const sp<Intent>& service) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->startService(service);
    } else {
        return mProxy->startService(service);
    }
}

bool ServiceManager::Proxy::stopService(const sp<Intent>& service) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->stopService(service);
    } else {
        return mProxy->stopService(service);
    }
}

bool ServiceManager::Proxy::bindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->bindService(service, conn, flags, callback);
    } else {
        return mProxy->bindService(service, conn, flags, callback);
    }
}

void ServiceManager::Proxy::unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        mStub->unbindService(service, conn);
    } else {
        mProxy->unbindService(service, conn);
    }
}

void ServiceManager::Proxy::unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, const sp<IRemoteCallback>& callback) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        mStub->unbindService(service, conn, callback);
    } else {
        mProxy->unbindService(service, conn, callback);
    }
}

sp<ComponentName> ServiceManager::Proxy::startSystemService(const sp<Intent>& service) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->startSystemService(service);
    } else {
        return mProxy->startSystemService(service);
    }
}

bool ServiceManager::Proxy::stopSystemService(const sp<Intent>& service) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->stopSystemService(service);
    } else {
        return mProxy->stopSystemService(service);
    }
}

} /* namespace binder */
} /* namespace mindroid */
