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

#ifndef MINDROID_RUNTIME_SYSTEM_PLUGIN_H_
#define MINDROID_RUNTIME_SYSTEM_PLUGIN_H_

#include <mindroid/lang/Object.h>
#include <mindroid/os/Binder.h>

namespace mindroid {

class Parcel;
template<typename T> class Promise;
class Runtime;

class Plugin : public Object {
public:
    Plugin() = default;
    virtual ~Plugin() = default;

    void setUp(const sp<Runtime>& runtime);
    void tearDown();

    virtual void start() = 0;
    virtual void stop() = 0;

    virtual void attachBinder(const sp<Binder>& binder) = 0;
    virtual void detachBinder(uint64_t id) = 0;
    virtual void attachProxy(uint64_t proxyId, const sp<Binder::Proxy>& proxy) = 0;
    virtual void detachProxy(uint64_t proxyId, uint64_t binderId) = 0;

    virtual sp<Binder> getStub(const sp<Binder>& binder) = 0;
    virtual sp<IInterface> getProxy(const sp<IBinder>& binder) = 0;
    virtual sp<Promise<sp<Parcel>>> transact(const sp<IBinder>& binder, int32_t what, const sp<Parcel>& data, int32_t flags) = 0;

    class Observer : public Object {
        virtual void onEntry(int32_t nodeId) = 0;
        virtual void onExit(int32_t nodeId) = 0;
    };

protected:
    sp<Runtime> mRuntime;
};

} /* namespace mindroid */

#endif /* MINDROID_RUNTIME_SYSTEM_PLUGIN_H_ */
