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

#include <mindroid/content/Intent.h>
#include <mindroid/content/pm/IPackageManager.h>
#include <mindroid/net/URI.h>
#include <mindroid/runtime/system/Runtime.h>
#include <mindroid/util/concurrent/Promise.h>

namespace mindroid {
namespace binder {

const char* const PackageManager::Stub::DESCRIPTOR = "mindroid://interfaces/mindroid/content/pm/IPackageManager";

void PackageManager::Stub::onTransact(int32_t what, int32_t num, const sp<Object>& obj, const sp<Bundle>& data, const sp<Promise<sp<Object>>>& result) {
    switch (what) {
    case MSG_GET_INSTALLED_PACKAGES: {
        sp<ArrayList<sp<PackageInfo>>> packages = getInstalledPackages(num);
        object_cast<Promise<sp<ArrayList<sp<PackageInfo>>>>, Object>(result)->complete(packages);
        break;
    }
    case MSG_RESOLVE_SERVICE: {
        sp<Intent> intent = object_cast<Intent>(obj);
        sp<ResolveInfo> serviceInfo = resolveService(intent, num);
        object_cast<Promise<sp<ResolveInfo>>, Object>(result)->complete(serviceInfo);
        break;
    }
    default:
        Binder::onTransact(what, num, obj, data, result);
    }
}

sp<ArrayList<sp<PackageInfo>>> PackageManager::Stub::Proxy::getInstalledPackages(int32_t flags) {
    sp<Promise<sp<ArrayList<sp<PackageInfo>>>>> promise = new Promise<sp<ArrayList<sp<PackageInfo>>>>();
    mRemote->transact(MSG_GET_INSTALLED_PACKAGES, flags, nullptr, nullptr, object_cast<Promise<sp<Object>>, Object>(promise), 0);
    return Binder::get(promise);
}

sp<ResolveInfo> PackageManager::Stub::Proxy::resolveService(const sp<Intent>& intent, int32_t flags) {
    sp<Promise<sp<ResolveInfo>>> promise = new Promise<sp<ResolveInfo>>();
    mRemote->transact(MSG_RESOLVE_SERVICE, flags, intent, nullptr, object_cast<Promise<sp<Object>>, Object>(promise), 0);
    return Binder::get(promise);
}

PackageManager::Proxy::Proxy(const sp<IBinder>& binder) {
    mBinder = binder;
    if (binder->getUri()->getScheme()->equals("mindroid")) {
        mStub = object_cast<PackageManager::Stub>(binder->queryLocalInterface(PackageManager::Stub::DESCRIPTOR));
        mProxy = new PackageManager::Stub::Proxy(binder);
    } else {
        sp<Runtime> runtime = Runtime::getRuntime();
        mStub = object_cast<PackageManager::Stub>(runtime->getBinder(binder->getId()));
        mProxy = object_cast<IPackageManager>(runtime->getProxy(binder));
    }
}

sp<ArrayList<sp<PackageInfo>>> PackageManager::Proxy::getInstalledPackages(int32_t flags) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->getInstalledPackages(flags);
    } else {
        return mProxy->getInstalledPackages(flags);
    }
}

sp<ResolveInfo> PackageManager::Proxy::resolveService(const sp<Intent>& intent, int32_t flags) {
    if (mStub != nullptr && mStub->isCurrentThread()) {
        return mStub->resolveService(intent, flags);
    } else {
        return mProxy->resolveService(intent, flags);
    }
}

} /* namespace binder */
} /* namespace mindroid */
