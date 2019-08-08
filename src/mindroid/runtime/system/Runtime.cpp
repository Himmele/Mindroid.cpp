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
#include <mindroid/runtime/system/Plugin.h>
#include <mindroid/runtime/system/Mindroid.h>
#include <mindroid/lang/Integer.h>
#include <mindroid/lang/IllegalArgumentException.h>
#include <mindroid/lang/NullPointerException.h>
#include <mindroid/lang/NumberFormatException.h>
#include <mindroid/net/URI.h>
#include <mindroid/net/URISyntaxException.h>
#include <mindroid/runtime/system/ServiceDiscovery.h>
#include <mindroid/util/concurrent/locks/ReentrantLock.h>

namespace mindroid {

const char* const Runtime::TAG = "Runtime";
std::mutex Runtime::sLock;
sp<Runtime> Runtime::sRuntime;
const sp<String> Runtime::MINDROID_SCHEME = String::valueOf("mindroid");
const sp<String> Runtime::MINDROID_SCHEME_WITH_SEPARATOR = String::valueOf("mindroid://");

CLASS(mindroid, Mindroid);

Runtime::Runtime(uint32_t nodeId, const sp<File>& configurationFile) :
        mNodeId(nodeId),
        mLock(new ReentrantLock()),
        mPlugins(new HashMap<sp<String>, sp<Plugin>>()),
        mBinderIds(new HashMap<uint64_t, wp<Binder>>()),
        mBinderUris(new HashMap<sp<String>, wp<Binder>>()),
        mServices(new HashMap<sp<String>, sp<Binder>>()),
        mProxies(new HashMap<sp<String>, wp<Binder::Proxy>>()),
        mBinderIdGenerator(new AtomicInteger(1)),
        mProxyIdGenerator(new AtomicInteger(1)),
        mIds(new HashSet<uint64_t>()) {
    if (nodeId == 0) {
        throw IllegalArgumentException("Mindroid runtime system node id must not be 0");
    }
    Log::println('I', TAG, "Mindroid runtime system node id: %u", mNodeId);
    if (configurationFile != nullptr) {
        try {
            mConfiguration = ServiceDiscovery::read(configurationFile);
        } catch (const Exception& e) {
            Log::println('E', TAG, "Failed to read Mindroid runtime system configuration");
        }
    }
    if (mConfiguration != nullptr && mConfiguration->nodes->containsKey(mNodeId)) {
        auto pluginItr = mConfiguration->nodes->get(mNodeId)->plugins->iterator();
        while (pluginItr.hasNext()) {
            auto entry = pluginItr.next();
            sp<ServiceDiscovery::Configuration::Plugin> plugin = entry.getValue();
            sp<Class<Plugin>> clazz = Class<Plugin>::forName(plugin->clazz);
            sp<Plugin> p = clazz->newInstance();
            if (p != nullptr) {
                mPlugins->put(plugin->scheme, p);
            } else {
                Log::println('E', TAG, "Cannot find class '%s'", plugin->clazz->c_str());
            }
        }

        sp<HashSet<uint64_t>> ids = new HashSet<uint64_t>();
        auto serviceItr = mConfiguration->services->iterator();
        while (serviceItr.hasNext()) {
            auto entry = serviceItr.next();
            sp<ServiceDiscovery::Configuration::Service> service = entry.getValue();
            if (service->node->id == nodeId) {
                uint64_t id = ((uint64_t) nodeId << 32) | (service->id & 0xFFFFFFFFL);
                ids->add(id);
            }
        }
        mIds->addAll(ids);
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
            try {
                plugin->start(nullptr, nullptr)->get();
            } catch (const CancellationException& e) {
                Log::println('E', TAG, "Failed to start plugin");
            } catch (const ExecutionException& e) {
                Log::println('E', TAG, "Failed to start plugin");
            }
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
            try {
                plugin->stop(nullptr, nullptr)->get();
            } catch (const CancellationException& e) {
                Log::println('E', TAG, "Failed to stop plugin");
            } catch (const ExecutionException& e) {
                Log::println('E', TAG, "Failed to stop plugin");
            }
            plugin->tearDown();
        }
    }
}

sp<ServiceDiscovery::Configuration> Runtime::getConfiguration() const {
    return mConfiguration;
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
                    b = mBinderUris->get(String::format("%s%s", MINDROID_SCHEME_WITH_SEPARATOR->c_str(), uri->getAuthority()->c_str()));
                    if (b != nullptr && (binder = b.get()) != nullptr) {
                        sp<Plugin> plugin = mPlugins->get(uri->getScheme());
                        if (plugin != nullptr) {
                            sp<Binder> stub = plugin->getStub(object_cast<Binder>(binder));
                            if (stub != nullptr) {
                                mBinderUris->put(key, stub);
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
    if (Class<Binder>::isInstance(service)) {
        if (!mServices->containsKey(uri->toString())) {
            if (mConfiguration != nullptr) {
                sp<ServiceDiscovery::Configuration::Node> node = mConfiguration->nodes->get(mNodeId);
                if (node != nullptr) {
                    sp<ServiceDiscovery::Configuration::Service> s = node->services->get(uri->getAuthority());
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
            mServices->put(uri->toString(), object_cast<Binder>(service));
        }
    } else {
        throw IllegalArgumentException("Service must be of superclass mindroid::Binder");
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
    if (uri == nullptr) {
        throw NullPointerException();
    }
    AutoLock autoLock(mLock);
    sp<URI> serviceUri;
    if (uri->getScheme() != nullptr) {
        serviceUri = uri;
    } else {
        serviceUri = new URI(MINDROID_SCHEME,
                uri->getUserInfo(),
                uri->getHost(),
                uri->getPort(),
                uri->getPath(),
                uri->getQuery(),
                uri->getFragment());
    }

    // Services
    sp<IBinder> binder = mServices->get(serviceUri->toString());
    if (binder != nullptr) {
        return binder;
    } else {
        binder = mServices->get(String::format("%s%s", MINDROID_SCHEME_WITH_SEPARATOR->c_str(), serviceUri->getAuthority()->c_str()));
        if (binder != nullptr) {
            sp<Plugin> plugin = mPlugins->get(serviceUri->getScheme());
            if (plugin != nullptr) {
                sp<Binder> stub = plugin->getStub(object_cast<Binder>(binder));
                if (stub != nullptr) {
                    mServices->put(serviceUri->toString(), stub);
                }
                return stub;
            } else {
                return nullptr;
            }
        }
    }

    // Proxies
    return getProxy(serviceUri);
}

uint64_t Runtime::attachProxy(const sp<Binder::Proxy>& proxy) {
    if (proxy == nullptr) {
        throw NullPointerException();
    }
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
    if (uri == nullptr) {
        throw NullPointerException();
    }
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
    if (binder == nullptr) {
        throw NullPointerException();
    }
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
    if (binder == nullptr) {
        throw NullPointerException();
    }
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

void Runtime::link(const sp<IBinder>& binder, const sp<IBinder::Supervisor>& supervisor, const sp<Bundle>& extras) {
    if (binder == nullptr || supervisor == nullptr) {
        throw NullPointerException();
    }
    sp<Plugin> plugin;
    {
        AutoLock autoLock(mLock);
        plugin = mPlugins->get(binder->getUri()->getScheme());
    }
    if (plugin != nullptr) {
        plugin->link(binder, supervisor, extras);
    } else {
        throw RemoteException("Binder linking failure");
    }
}

bool Runtime::unlink(const sp<IBinder>& binder, const sp<IBinder::Supervisor>& supervisor, const sp<Bundle>& extras) {
    if (binder == nullptr || supervisor == nullptr) {
        throw NullPointerException();
    }
    sp<Plugin> plugin;
    {
        AutoLock autoLock(mLock);
        plugin = mPlugins->get(binder->getUri()->getScheme());
    }
    if (plugin != nullptr) {
        return plugin->unlink(binder, supervisor, extras);
    } else {
        return true;
    }
}

void Runtime::removeProxy(const sp<IBinder>& proxy) {
    if (proxy == nullptr) {
        throw NullPointerException();
    }
    AutoLock autoLock(mLock);
    auto itr = mProxies->iterator();
    while (itr.hasNext()) {
        auto entry = itr.next();
        wp<Binder::Proxy> p = entry.getValue();
        if (proxy->equals(p.get())) {
            itr.remove();
        }
    }
}

sp<Promise<sp<Void>>> Runtime::start(const sp<URI>& uri, const sp<Bundle>& extras) {
    sp<Plugin> plugin;
    {
        AutoLock autoLock(mLock);
        plugin = mPlugins->get(uri->getScheme());
    }
    if (plugin != nullptr) {
        return plugin->start(uri, extras);
    } else {
        return new Promise<sp<Void>>(sp<Exception>(new RemoteException("Plugin start failure")));
    }
}

sp<Promise<sp<Void>>> Runtime::stop(const sp<URI>& uri, const sp<Bundle>& extras) {
    sp<Plugin> plugin;
    {
        AutoLock autoLock(mLock);
        plugin = mPlugins->get(uri->getScheme());
    }
    if (plugin != nullptr) {
        return plugin->start(uri, extras);
    } else {
        return new Promise<sp<Void>>(sp<Exception>(new RemoteException("Plugin stop failure")));
    }
}

sp<Promise<sp<Void>>> Runtime::connect(const sp<URI>& node, const sp<Bundle>& extras) {
    sp<Plugin> plugin;
    {
        AutoLock autoLock(mLock);
        plugin = mPlugins->get(node->getScheme());
    }
    if (plugin != nullptr) {
        return plugin->connect(node, extras);
    } else {
        return new Promise<sp<Void>>(sp<Exception>(new RemoteException("Node connection failure")));
    }
}

sp<Promise<sp<Void>>> Runtime::disconnect(const sp<URI>& node, const sp<Bundle>& extras) {
    sp<Plugin> plugin;
    {
        AutoLock autoLock(mLock);
        plugin = mPlugins->get(node->getScheme());
    }
    if (plugin != nullptr) {
        return plugin->disconnect(node, extras);
    } else {
        return new Promise<sp<Void>>(sp<Exception>(new RemoteException("Node disconnection failure")));
    }
}

sp<Binder::Proxy> Runtime::getProxy(const sp<URI>& uri) {
    AutoLock autoLock(mLock);

    sp<String> key = uri->toString();
    wp<Binder::Proxy> wp = mProxies->get(key);
    sp<Binder::Proxy> p;
    if (wp != nullptr && (p = wp.get()) != nullptr) {
        return p;
    } else {
        mProxies->remove(key);
    }

    if (mConfiguration != nullptr) {
        sp<ServiceDiscovery::Configuration::Service> service = mConfiguration->services->get(uri->getAuthority());
        if (service == nullptr) {
            return nullptr;
        }

        try {
            sp<URI> interfaceDescriptor = new URI(service->announcements->get(uri->getScheme()));
            sp<URI> proxyUri = new URI(uri->getScheme(), String::format("%u.%u", service->node->id, service->id), String::format("/if=%s", interfaceDescriptor->getPath()->substring(1)->c_str()), interfaceDescriptor->getQuery(), nullptr);
            sp<Binder::Proxy> proxy = Binder::Proxy::create(proxyUri);
            mProxies->put(key, proxy);
            return proxy;
        } catch (const Exception& e) {
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

} /* namespace mindroid */
