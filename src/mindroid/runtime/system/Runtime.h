/*
 * Copyright (C) 2018 E.S.R.Labs
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

#ifndef MINDROID_RUNTIME_SYSTEM_RUNTIME_H_
#define MINDROID_RUNTIME_SYSTEM_RUNTIME_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/os/IBinder.h>
#include <mindroid/os/IInterface.h>
#include <mindroid/os/Binder.h>
#include <mindroid/os/RemoteException.h>
#include <mindroid/util/Log.h>
#include <mindroid/util/HashMap.h>
#include <mindroid/util/HashSet.h>
#include <mindroid/util/concurrent/atomic/AtomicInteger.h>
#include <mindroid/util/concurrent/locks/Lock.h>
#include <mindroid/util/concurrent/Executor.h>
#include <mindroid/util/concurrent/Promise.h>
#include <mutex>

namespace mindroid {

class File;
class URI;
class Parcel;
class Plugin;
class Configuration;

class Runtime : public Object {
public:
    static sp<Runtime> getRuntime() {
        return sRuntime;
    }

    static void start(int32_t nodeId, const sp<File>& configuration);

    static void shutdown();

    uint32_t getNodeId() const {
        return mNodeId;
    }

    sp<Configuration> getConfiguration() const;

    void addIds(const sp<HashSet<uint64_t>>& ids);

    uint64_t attachBinder(const sp<Binder>& binder);

    void attachBinder(const sp<URI>& uri, const sp<Binder>& binder);

    void detachBinder(uint64_t id, const sp<URI>& uri);

    sp<Binder> getBinder(uint64_t id);

    sp<IBinder> getBinder(const sp<URI>& uri);

    void addService(const sp<URI>& uri, const sp<IBinder>& service);

    void removeService(const sp<IBinder>& service);

    sp<IBinder> getService(const sp<URI>& uri);

    uint64_t attachProxy(const sp<Binder::Proxy>& proxy);

    void detachProxy(uint64_t id, const sp<URI>& uri, uint64_t proxyId);

    sp<IInterface> getProxy(const sp<IBinder>& binder);

    sp<Promise<sp<Parcel>>> transact(const sp<IBinder>& binder, int32_t what, const sp<Parcel>& data, int32_t flags);

    void link(const sp<IBinder>& binder, const sp<IBinder::Supervisor>& supervisor, const sp<Bundle>& extras);

    bool unlink(const sp<IBinder>& binder, const sp<IBinder::Supervisor>& supervisor, const sp<Bundle>& extras);

    void removeProxy(const sp<IBinder>& proxy);

private:
    Runtime(uint32_t nodeId, const sp<File>& configuration);
    sp<Binder::Proxy> getProxy(const sp<URI>& uri);

    static const char* const TAG;
    static std::mutex sLock;
    static sp<Runtime> sRuntime;
    static const sp<String> MINDROID_SCHEME;
    static const sp<String> MINDROID_SCHEME_WITH_SEPARATOR;

    uint32_t mNodeId;
    sp<Lock> mLock;
    sp<HashMap<sp<String>, sp<Plugin>>> mPlugins;
    sp<HashMap<uint64_t, wp<Binder>>> mBinderIds;
    sp<HashMap<sp<String>, wp<Binder>>> mBinderUris;
    sp<HashMap<sp<String>, sp<Binder>>> mServices;
    sp<HashMap<sp<String>, wp<Binder::Proxy>>> mProxies;
    sp<AtomicInteger> mBinderIdGenerator;
    sp<AtomicInteger> mProxyIdGenerator;
    sp<HashSet<uint64_t>> mIds;
    sp<Configuration> mConfiguration;
};

} /* namespace mindroid */

#endif /* MINDROID_RUNTIME_SYSTEM_RUNTIME_H_ */
