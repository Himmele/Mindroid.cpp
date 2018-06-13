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

#include <mindroid/runtime/system/Runtime.h>
#include <mindroid/runtime/system/Configuration.h>
#include <mindroid/runtime/system/Plugin.h>
#include <mindroid/runtime/system/Mindroid.h>
#include <mindroid/lang/Integer.h>
#include <mindroid/lang/IllegalArgumentException.h>
#include <mindroid/lang/NullPointerException.h>
#include <mindroid/lang/NumberFormatException.h>
#include <mindroid/net/URI.h>
#include <mindroid/net/URISyntaxException.h>
#include <mindroid/util/concurrent/locks/ReentrantLock.h>

namespace mindroid {

const char* const Runtime::TAG = "Runtime";
std::mutex Runtime::sLock;
sp<Runtime> Runtime::sRuntime;
const sp<String> Runtime::MINDROID_SCHEME = String::valueOf("mindroid");
const sp<String> Runtime::MINDROID_SCHEME_WITH_SEPARATOR = String::valueOf("mindroid://");

CLASS(mindroid, Mindroid);

Runtime::Runtime(uint32_t nodeId, const sp<File>& configuration) :
        mNodeId(nodeId),
        mLock(new ReentrantLock()),
        mPlugins(new HashMap<sp<String>, sp<Plugin>>()),
        mBinderIds(new HashMap<uint64_t, wp<Binder>>()),
        mBinderUris(new HashMap<sp<String>, wp<Binder>>()),
        mServices(new HashMap<sp<String>, sp<IBinder>>()),
        mBinderIdGenerator(new AtomicInteger(1)),
        mProxyIdGenerator(new AtomicInteger(1)),
        mIds(new HashSet<uint64_t>()) {
    if (nodeId == 0) {
        throw IllegalArgumentException("Mindroid runtime system node id must not be 0");
    }
    Log::println('I', TAG, "Mindroid runtime system node id: %u", mNodeId);
    if (configuration != nullptr) {
        try {
            mConfiguration = Configuration::read(configuration);
        } catch (const Exception& e) {
            Log::println('E', TAG, "Failed to read Mindroid runtime system configuration");
        }
    }
    if (mConfiguration != nullptr) {
        auto itr = mConfiguration->plugins->iterator();
        while (itr.hasNext()) {
            auto entry = itr.next();
            sp<Configuration::Plugin> plugin = entry.getValue();
            if (plugin->enabled) {
                sp<Class<Plugin>> clazz = Class<Plugin>::forName(plugin->clazz);
                sp<Plugin> p = clazz->newInstance();
                if (p != nullptr) {
                    mPlugins->put(plugin->scheme, p);
                } else {
                    Log::println('E', TAG, "Cannot find class '%s'", plugin->clazz->c_str());
                }
            }
        }
    }
}

void Runtime::start(int32_t nodeId, const sp<File>& configuration) {
    bool start = false;
    sLock.lock();
    if (sRuntime == nullptr) {
        sRuntime = new Runtime(nodeId, configuration);
        start = true;
    }
    sLock.unlock();
    if (start) {
        AutoLock autoLock(sRuntime->mLock);
        auto itr = sRuntime->mPlugins->iterator();
        while (itr.hasNext()) {
            auto entry = itr.next();
            sp<Plugin> plugin = entry.getValue();
            plugin->setUp(sRuntime);
            plugin->start();
        }
    }
}

void Runtime::shutdown() {
    bool shutdown = false;
    sp<Runtime> runtime = sRuntime;
    sLock.lock();
    if (sRuntime != nullptr) {
        sRuntime = nullptr;
        shutdown = true;
    }
    sLock.unlock();
    if (shutdown) {
        AutoLock autoLock(runtime->mLock);
        auto itr = runtime->mPlugins->iterator();
        while (itr.hasNext()) {
            auto entry = itr.next();
            sp<Plugin> plugin = entry.getValue();
            plugin->stop();
            plugin->tearDown();
        }
    }
}

sp<Configuration> Runtime::getConfiguration() const {
    return mConfiguration;
}

void Runtime::addIds(const sp<HashSet<uint64_t>>& ids) {
    mIds->addAll(ids);
}

uint64_t Runtime::attachBinder(const sp<Binder>& binder) {
    if (binder == nullptr) {
        throw NullPointerException();
    }
    AutoLock autoLock(mLock);
    uint64_t id;
    do {
        id = ((uint64_t) mNodeId << 32) | (mBinderIdGenerator->getAndIncrement() & 0xFFFFFFFFL);
    } while (mIds->contains(id));
    if (!mBinderIds->containsKey(id)) {
        mBinderIds->put(id, binder);
        mIds->add(id);
    } else {
        Log::wtf(TAG, "Invalid binder id: %" PRIu64, id);
    }
    return id;
}

void Runtime::attachBinder(const sp<URI>& uri, const sp<Binder>& binder) {
    if (uri == nullptr || binder == nullptr) {
        throw NullPointerException();
    }
    if (binder->getUri() == nullptr) {
        throw IllegalArgumentException("Binder URI must not be null");
    }
    AutoLock autoLock(mLock);
    if (!mBinderUris->containsKey(uri->toString())) {
        mBinderUris->put(uri->toString(), binder);

        sp<Plugin> plugin = mPlugins->get(binder->getUri()->getScheme());
        if (plugin != nullptr) {
            plugin->attachBinder(binder);
        }
    } else {
        Log::wtf(TAG, "Multiple Binder registration for URI: %s", uri->toString()->c_str());
    }
}

void Runtime::detachBinder(uint64_t id, const sp<URI>& uri) {
    AutoLock autoLock(mLock);
    mIds->remove(id);
    mBinderIds->remove(id);
    if (uri != nullptr) {
        mBinderUris->remove(uri->toString());
        sp<Plugin> plugin = mPlugins->get(uri->getScheme());
        if (plugin != nullptr) {
            plugin->detachBinder(id);
        }
    }
}

sp<Binder> Runtime::getBinder(uint64_t id) {
    AutoLock autoLock(mLock);
    uint32_t nodeId = (uint32_t) ((id >> 32) & 0xFFFFFFFFL);
    if (nodeId == 0 || mNodeId == nodeId) {
        wp<Binder> binder = mBinderIds->get(((uint64_t) mNodeId << 32) | id);
        if (binder != nullptr) {
            return binder.get();
        } else {
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

sp<IBinder> Runtime::getBinder(const sp<URI>& uri) {
    AutoLock autoLock(mLock);
    if (uri != nullptr) {
        uint32_t nodeId;
        sp<String> authority = uri->getAuthority();
        sp<StringArray> parts = authority->split(".");
        if (parts->size() == 2) {
            try {
                nodeId = Integer::valueOf(parts->get(0))->intValue();
            } catch (const NumberFormatException& e) {
                throw IllegalArgumentException(String::format("Invalid URI: %s", uri->toString()->c_str()));
            }
        } else {
            throw IllegalArgumentException(String::format("Invalid URI: %s", uri->toString()->c_str()));
        }
        if (mNodeId == nodeId) {
            sp<String> key = String::format("%s://%s", uri->getScheme()->c_str(), uri->getAuthority()->c_str());
            wp<Binder> b = mBinderUris->get(key);
            sp<IBinder> binder;
            if (MINDROID_SCHEME->equals(uri->getScheme())) {
                if (b != nullptr && (binder = b.get()) != nullptr) {
                    return binder;
                } else {
                    mBinderUris->remove(key);
                    return nullptr;
                }
            } else {
                if (b != nullptr && (binder = b.get()) != nullptr) {
                    return binder;
                } else {
                    mBinderUris->remove(key);
                    key = String::format("%s%s", MINDROID_SCHEME_WITH_SEPARATOR->c_str(), uri->getAuthority()->c_str());
                    b = mBinderUris->get(key);
                    if (b != nullptr && (binder = b.get()) != nullptr) {
                        sp<Plugin> plugin = mPlugins->get(uri->getScheme());
                        if (plugin != nullptr) {
                            sp<Binder> stub = plugin->getStub(object_cast<Binder>(binder));
                            if (stub != nullptr) {
                                mBinderUris->put(stub->getUri()->toString(), stub);
                            }
                            return stub;
                        } else {
                            return nullptr;
                        }
                    } else {
                        return nullptr;
                    }
                }
            }
        } else {
            return Binder::Proxy::create(uri);
        }
    } else {
        return nullptr;
    }
}

void Runtime::addService(const sp<URI>& uri, const sp<IBinder>& service) {
    if (uri == nullptr || service == nullptr) {
        throw NullPointerException();
    }
    if (service->getUri() == nullptr) {
        throw IllegalArgumentException("Service URI must not be null");
    }
    if (uri->getScheme() == nullptr || !uri->getScheme()->equals(service->getUri()->getScheme())) {
        throw IllegalArgumentException(String::format("Binder scheme mismatch: %s != %s", uri->toString()->c_str(), service->getUri()->toString()->c_str()));
    }
    AutoLock autoLock(mLock);
    if (!mServices->containsKey(uri->toString())) {
        if (Class<Binder>::isInstance(service)) {
            if (mConfiguration != nullptr) {
                sp<Configuration::Plugin> plugin = mConfiguration->plugins->get(MINDROID_SCHEME);
                if (plugin != nullptr) {
                    sp<Configuration::Service> s = plugin->services->get(uri->getAuthority());
                    if (s != nullptr) {
                        uint64_t oldId = ((uint64_t) mNodeId << 32) | (service->getId() & 0xFFFFFFFFL);
                        mIds->remove(oldId);
                        mBinderIds->remove(oldId);
                        mBinderUris->remove(service->getUri()->toString());
                        uint64_t newId = ((uint64_t) mNodeId << 32) | (s->id & 0xFFFFFFFFL);
                        object_cast<Binder>(service)->setId(newId);
                        mIds->add(newId);
                        mBinderIds->put(newId, object_cast<Binder>(service));
                        mBinderUris->put(service->getUri()->toString(), object_cast<Binder>(service));
                    }
                }
            }
        }
        mServices->put(uri->toString(), service);
    }
}

void Runtime::removeService(const sp<IBinder>& service) {
    if (service != nullptr) {
        AutoLock autoLock(mLock);
        auto itr = mServices->iterator();
        while (itr.hasNext()) {
            auto entry = itr.next();
            if (entry.getValue()->getId() == service->getId()) {
                itr.remove();
            }
        }
    }
}

sp<IBinder> Runtime::getService(const sp<URI>& uri) {
    AutoLock autoLock(mLock);
    sp<IBinder> binder = mServices->get(uri->toString());
    if (binder != nullptr) {
        return binder;
    } else {
        try {
            if (MINDROID_SCHEME->equals(uri->getScheme())) {
                return nullptr;
            } else {
                binder = mServices->get(String::format("%s%s", MINDROID_SCHEME_WITH_SEPARATOR->c_str(), uri->getAuthority()->c_str()));
                if (binder != nullptr) {
                    if (Class<Binder>::isInstance(binder)) {
                        sp<Plugin> plugin = mPlugins->get(uri->getScheme());
                        if (plugin != nullptr) {
                            sp<Binder> stub = plugin->getStub(object_cast<Binder>(binder));
                            if (stub != nullptr) {
                                mServices->put(uri->toString(), stub);
                            }
                            return stub;
                        } else {
                            return nullptr;
                        }
                    } else {
                        sp<URI> descriptor = new URI(binder->getInterfaceDescriptor());
                        sp<IBinder> proxy = Binder::Proxy::create(new URI(uri->getScheme(),
                                binder->getUri()->getAuthority(),
                                String::format("/if=%s", descriptor->getPath()->substring(1)->c_str()), nullptr, nullptr));
                        mServices->put(uri->toString(), proxy);
                        return proxy;
                    }
                } else {
                    return nullptr;
                }
            }
        } catch (const URISyntaxException& e) {
            return nullptr;
        }
        return nullptr;
    }
}

uint64_t Runtime::attachProxy(const sp<Binder::Proxy>& proxy) {
    const uint64_t proxyId = mProxyIdGenerator->getAndIncrement();
    sp<Plugin> plugin;
    {
        AutoLock autoLock(mLock);
        plugin = mPlugins->get(proxy->getUri()->getScheme());
    }
    if (plugin != nullptr) {
        plugin->attachProxy(proxyId, proxy);
    }
    return proxyId;
}

void Runtime::detachProxy(uint64_t id, const sp<URI>& uri, uint64_t proxyId) {
    sp<Plugin> plugin;
    {
        AutoLock autoLock(mLock);
        plugin = mPlugins->get(uri->getScheme());
    }
    if (plugin != nullptr) {
        plugin->detachProxy(proxyId, id);
    }
}

sp<IInterface> Runtime::getProxy(const sp<IBinder>& binder) {
    sp<Plugin> plugin;
    {
        AutoLock autoLock(mLock);
        plugin = mPlugins->get(binder->getUri()->getScheme());
    }
    if (plugin != nullptr) {
        return plugin->getProxy(binder);
    } else {
        return nullptr;
    }
}

sp<Promise<sp<Parcel>>> Runtime::transact(const sp<IBinder>& binder, int32_t what, const sp<Parcel>& data, int32_t flags) {
    sp<Plugin> plugin;
    {
        AutoLock autoLock(mLock);
        plugin = mPlugins->get(binder->getUri()->getScheme());
    }
    if (plugin != nullptr) {
        sp<Promise<sp<Parcel>>> promise = plugin->transact(binder, what, data, flags);
        if (flags != Binder::FLAG_ONEWAY && promise == nullptr) {
            throw RemoteException("Binder transaction failure");
        }
        return promise;
    } else {
        throw RemoteException("Binder transaction failure");
    }
}

} /* namespace mindroid */
