#include "mindroid/app/Service.h"
#include "mindroid/util/logging/Logger.h"
#include "mindroid/content/pm/PackageManagerService.h"
#include "mindroid/os/ServiceManager.h"
#include "mindroid/os/Environment.h"
#include "mindroid/os/AsyncTask.h"
#include "mindroid/lang/Class.h"
#include "mindroid/lang/Void.h"
#include "mindroid/lang/Boolean.h"
#include "mindroid/lang/Integer.h"
#include "mindroid/lang/System.h"
#include "mindroid/util/Log.h"
#include "mindroid/util/concurrent/Promise.h"
#include "mindroid/util/concurrent/SerialExecutor.h"
#include <vector>

using namespace mindroid;

namespace test {

class Service1 :
        public Service {
public:
    Service1() {
        Log::d(TAG, "Service1::ctor");
    }

    virtual ~Service1() {
        Log::d(TAG, "Service1::dtor");
    }

    virtual void onCreate() {
        mHandler = new Handler();
        Log::d(TAG, "Service1::onCreate");
        mHandler->postDelayed([this] { test(); }, 1000);

        sp<Intent> service2 = new Intent();
        service2->setComponent(new ComponentName("test", "Service2"));
        mServiceConnection = new ServiceConnection1();
        bindService(service2, mServiceConnection, 0);

        mHandler->postDelayed([=] {
            unbindService(mServiceConnection);
            mServiceConnection = nullptr;
        }, 2000);
    }

    virtual int32_t onStartCommand(const sp<Intent>& intent, int32_t flags, int32_t startId) {
        Log::d(TAG, "Service1::onStartCommand");
        return 0;
    }

    virtual void onDestroy() {
        Log::d(TAG, "Service1::onDestroy");
    }

    virtual sp<IBinder> onBind(const sp<Intent>& intent) { return nullptr; }

    void test() {
        Log::d(TAG, "Test");
        mHandler->postDelayed([this] { test(); }, 1000);
    }

    class ServiceConnection1 :
            public ServiceConnection {
    public:
        virtual void onServiceConnected(const sp<ComponentName>& name, const sp<IBinder>& service) {
            Log::d(TAG, "Service1::onServiceConnected: %s", name->toShortString()->c_str());
        }

        virtual void onServiceDisconnected(const sp<ComponentName>& name) {
            Log::d(TAG, "Service1::onServiceDisconnected: %s", name->toShortString()->c_str());
        }
    };

private:
    static const char* const TAG;

    sp<Handler> mHandler;
    sp<ServiceConnection> mServiceConnection;
};

const char* const Service1::TAG = "Service1";

class Service2 :
        public Service {
public:
    Service2() {
        Log::d(TAG, "Service2::ctor");
    }

    virtual ~Service2() {
        Log::d(TAG, "Service2::dtor");
    }

    virtual void onCreate() {
        Log::d(TAG, "Service2::onCreate");
        sp<SharedPreferences> preferences = getSharedPreferences("Test", 0);
        int32_t value = preferences->getInt("Test", 0);
        Log::d(TAG, "Value: %d", value);
        preferences->edit()->putInt("Test", 123)->commit();
    }

    virtual void onDestroy() {
        Log::d(TAG, "Service2::onDestroy");
    }

    virtual sp<IBinder> onBind(const sp<Intent>& intent) {
        Log::d(TAG, "Service2::onBind: %s", intent->getComponent()->toShortString()->c_str());
        return nullptr;
    }

    bool onUnbind(const sp<Intent>& intent) {
        Log::d(TAG, "Service2::onUnbind: %s", intent->getComponent()->toShortString()->c_str());
        return true;
    }

private:
    static const char* const TAG;
};

const char* const Service2::TAG = "Service2";

class Service3 :
        public Service {
public:
    Service3() {
        Log::d(TAG, "Service3::ctor");
    }

    virtual ~Service3() {
        Log::d(TAG, "Service3::dtor");
    }

    virtual void onCreate() {
        Log::d(TAG, "Service3::onCreate");
        mHandler = new Handler();
        mExecutor = new SerialExecutor("Executor");

        mPromise1->completeWith(mPromise2);
        mPromise1->orTimeout(10000)
        ->then<sp<Boolean>>([=] (const sp<Integer>& i) {
            Log::i(TAG, "Promise stage 1: %d", i->intValue());
            if (System::currentTimeMillis() % 2 == 0) {
                throw RuntimeException("Test");
            }
            return new Boolean(true);
        })->then<sp<Integer>>([=] (const sp<Boolean>& b) {
            Log::i(TAG, "Promise stage 2: %s", b->booleanValue() ? "true" : "false");
            return new Integer(17);
        })->catchException([=] (const sp<Exception>& exception) {
            Log::i(TAG, "Promise error stage 1: %s", exception->getMessage()->c_str());
        })->then(object_cast<Executor>(mExecutor), [=] (const sp<Integer>& i, const sp<Exception>& exception) {
            Thread::sleep(1000);
            if (exception != nullptr) {
                Log::i(TAG, "Promise error stage 2: %s", exception->getMessage()->c_str());
            } else {
                Log::i(TAG, "Promise stage 3: %d", i->intValue());
            }
        })->then<sp<Integer>>([=] (const sp<Integer>& i, const sp<Exception>& exception) {
            if (exception != nullptr) {
                Log::i(TAG, "Promise error stage 3: %s", exception->getMessage()->c_str());
            } else {
                Log::i(TAG, "Promise stage 4: %d", i->intValue());
            }
            return new Integer(12345);
        })->then([=] (const sp<Integer>& i) {
            Log::i(TAG, "Promise stage 5: %d", i->intValue());
        })->then([&] {
            Log::i(TAG, "Promise stage 6");
        });

        sp<Handler> h = new Handler();
        h->postDelayed([=] {
            mPromise2->complete(new Integer(42));
        }, 5000);


        action1(42)
            ->thenCompose<int>([=] (int value) { return action2(value); })
            ->thenCompose<int>(object_cast<Executor>(mExecutor), [=] (int value) { return action3(value); })
            ->thenCompose<int>([=] (int value) { return action4(value); })
            ->then([=] (int value) { Log::i(TAG, "Result: %d", value); });


        object_cast<Promise<int>>(new Promise<int>(42))
            ->thenApply<sp<String>>([=] (int value) { return String::valueOf(value); })
            ->thenAccept([=] (const sp<String>& value) { Log::i(TAG, "Result: %s", value->c_str()); });


        h->postDelayed([=] {
            mPromise3->complete(new Integer(17));
        }, 7500);

        h->postDelayed([=] {
            mPromise4->complete(new Integer(0));
        }, 8000);

        uint64_t startTime = SystemClock::uptimeMillis();
        sp<Promise<sp<Void>>> allOf = Promise<sp<Void>>::allOf({mPromise1, mPromise2, mPromise3, mPromise4});
        allOf->then([=] (const sp<Void>&, const sp<Exception>& exception) {
            uint64_t now = SystemClock::uptimeMillis();
            if (exception != nullptr) {
                Log::i(TAG, "AllOf exception after %lldms: %s", now - startTime, exception->getMessage()->c_str());
            } else {
                Log::i(TAG, "AllOf result after %lldms", now - startTime);
            }
        });

        sp<Promise<sp<Integer>>> anyOf = Promise<sp<Integer>>::anyOf({mPromise1, mPromise2, mPromise3, mPromise4});
        anyOf->then([=] (const sp<Integer>& value, const sp<Exception>& exception) {
            uint64_t now = SystemClock::uptimeMillis();
            if (exception != nullptr) {
                Log::i(TAG, "AnyOf exception after %lldms: %s", now - startTime, exception->getMessage()->c_str());
            } else {
                Log::i(TAG, "AnyOf result after %lldms: %d", now - startTime, value->intValue());
            }
        });
    }

    virtual void onDestroy() {
        Log::d(TAG, "Service3::onDestroy");
        mExecutor->shutdown();
    }

    virtual sp<IBinder> onBind(const sp<Intent>& intent) {
        Log::d(TAG, "Service3::onBind: %s", intent->getComponent()->toShortString()->c_str());
        return nullptr;
    }

    bool onUnbind(const sp<Intent>& intent) {
        Log::d(TAG, "Service3::onUnbind: %s", intent->getComponent()->toShortString()->c_str());
        return true;
    }

private:
    sp<Promise<int>> action1(int value) {
        Log::i(TAG, "Action 1: %d", value);
        sp<Promise<int>> promise = new Promise<int>();
        mHandler->postDelayed([=] { promise->complete(value + 1); }, 1000);
        return promise;
    }

    sp<Promise<int>> action2(int value) {
        Log::i(TAG, "Action 2: %d", value);
        sp<Promise<int>> promise = new Promise<int>();
        mHandler->postDelayed([=] { promise->complete(value + 2); }, 1000);
        return promise;
    }

    sp<Promise<int>> action3(int value) {
        Log::i(TAG, "Action 3: %d", value);
        Thread::sleep(1000);
        sp<Promise<int>> promise = new Promise<int>(value + 3);
        return promise;
    }

    sp<Promise<int>> action4(int value) {
        Log::i(TAG, "Action 4: %d", value);
        sp<Promise<int>> promise = new Promise<int>();
        mHandler->postDelayed([=] { promise->complete(value + 4); }, 1000);
        return promise;
    }

    sp<Handler> mHandler;
    sp<SerialExecutor> mExecutor;
    sp<Promise<sp<Integer>>> mPromise1 = new Promise<sp<Integer>>();
    sp<Promise<sp<Integer>>> mPromise2 = new Promise<sp<Integer>>();
    sp<Promise<sp<Integer>>> mPromise3 = new Promise<sp<Integer>>();
    sp<Promise<sp<Integer>>> mPromise4 = new Promise<sp<Integer>>();

    static const char* const TAG;
};

const char* const Service3::TAG = "Service3";

class Service4 :
        public Service {
public:
    Service4() {
        Log::d(TAG, "Service4::ctor");
    }

    virtual ~Service4() {
        Log::d(TAG, "Service4::dtor");
    }

    virtual void onCreate() {
        Log::d(TAG, "Service4::onCreate");
        sp<AsyncTask<std::vector<int32_t>, int32_t, int32_t>> asyncTask = new Task();
        asyncTask->execute({1, 2, 3});
    }

    virtual void onDestroy() {
        Log::d(TAG, "Service4::onDestroy");
    }

    virtual sp<IBinder> onBind(const sp<Intent>& intent) {
        Log::d(TAG, "Service4::onBind: %s", intent->getComponent()->toShortString()->c_str());
        return nullptr;
    }

    bool onUnbind(const sp<Intent>& intent) {
        Log::d(TAG, "Service4::onUnbind: %s", intent->getComponent()->toShortString()->c_str());
        return true;
    }

private:
    class Task : public AsyncTask<std::vector<int32_t>, int32_t, int32_t> {
    public:
        virtual void onPreExecute() override {
            Log::i(TAG, "onPreExecute");
        }

        virtual int32_t doInBackground(std::vector<int32_t> params) override {
            Log::i(TAG, "doInBackground");
            for (size_t i = 0; i < params.size(); i++) {
                Thread::sleep(1000);
                publishProgress(params[i]);
            }
            return 42;
        }

        virtual void onProgressUpdate(int32_t value) override {
            Log::i(TAG, "onProgressUpdate: %d", value);
        }

        virtual void onPostExecute(int32_t result) override {
            Log::i(TAG, "onPostExecute: %d", result);
        }

        virtual void onCancelled() override {
            Log::i(TAG, "onCancelled");
        }
    };

    static const char* const TAG;
};

const char* const Service4::TAG = "Service4";

} /* namespace test */

CLASS(mindroid, Logger);
CLASS(mindroid, PackageManagerService);
CLASS(test, Service1);
CLASS(test, Service2);
CLASS(test, Service3);
CLASS(test, Service4);

sp<ArrayList<sp<String>>> getLogBuffers(const sp<String>& lb);

void startSystemSerices(sp<IServiceManager> serviceManager) {
    sp<ArrayList<sp<String>>> logFlags = new ArrayList<sp<String>>();
    logFlags->add(String::valueOf("timestamp"));
    sp<Intent> logger = new Intent(Logger::ACTION_LOG);
    logger->setComponent(new ComponentName("mindroid", "Logger"))
            ->putExtra("name", Context::LOGGER_SERVICE)
            ->putExtra("process", "main")
            ->putExtra("logBuffer", Log::LOG_ID_MAIN)
            ->putExtra("logPriority", Log::DEBUG)
            ->putStringArrayListExtra("logFlags", logFlags)
            ->putExtra("consoleLogging", true);
    serviceManager->startSystemService(logger);

    sp<Intent> packageManager = new Intent();
    packageManager->setComponent(new ComponentName("mindroid", "PackageManagerService"))
            ->putExtra("name", "PackageManager")
            ->putExtra("process", "main");
    serviceManager->startSystemService(packageManager);
    ServiceManager::waitForSystemService(Context::PACKAGE_MANAGER);
}

void startServices(sp<IServiceManager> serviceManager) {
    sp<IPackageManager> packageManager = binder::PackageManager::Stub::asInterface(ServiceManager::getSystemService(Context::PACKAGE_MANAGER));
    sp<ArrayList<sp<PackageInfo>>> packages = packageManager->getInstalledPackages(PackageManager::GET_SERVICES);
    if (packages != nullptr) {
        auto packageItr = packages->iterator();
        while (packageItr.hasNext()) {
            sp<PackageInfo> package = packageItr.next();
            if (package->services != nullptr) {
                sp<ArrayList<sp<ServiceInfo>>> services = package->services;
                auto serviceItr = services->iterator();
                while (serviceItr.hasNext()) {
                    sp<ServiceInfo> service = serviceItr.next();
                    if (service->isEnabled() && service->hasFlag(ServiceInfo::FLAG_AUTO_START)) {
                        sp<Intent> intent = new Intent();
                        intent->setComponent(new ComponentName(service->packageName, service->name));
                        serviceManager->startService(intent);
                    }
                }
            }
        }
    }
}

void shutdownSystemSerices(sp<IServiceManager> serviceManager) {
    sp<Intent> packageManager = new Intent();
    packageManager->setComponent(new ComponentName("mindroid", "PackageManagerService"));
    serviceManager->stopSystemService(packageManager);
    ServiceManager::waitForSystemServiceShutdown(Context::PACKAGE_MANAGER);

    sp<Intent> logger = new Intent();
    logger->setComponent(new ComponentName("mindroid", "Logger"));
    serviceManager->stopSystemService(logger);
}

void shutdownServices(sp<IServiceManager> serviceManager) {
    sp<IPackageManager> packageManager = binder::PackageManager::Stub::asInterface(ServiceManager::getSystemService(Context::PACKAGE_MANAGER));
    sp<ArrayList<sp<PackageInfo>>> packages = packageManager->getInstalledPackages(PackageManager::GET_SERVICES);
    if (packages != nullptr) {
        auto packageItr = packages->iterator();
        while (packageItr.hasNext()) {
            sp<PackageInfo> package = packageItr.next();
            if (package->services != nullptr) {
                sp<ArrayList<sp<ServiceInfo>>> services = package->services;
                auto serviceItr = services->iterator();
                while (serviceItr.hasNext()) {
                    sp<ServiceInfo> service = serviceItr.next();
                    if (service->isEnabled()) {
                        sp<Intent> intent = new Intent();
                        intent->setComponent(new ComponentName(service->packageName, service->name));
                        serviceManager->stopService(intent);
                    }
                }
            }
        }
    }
}

static sp<ServiceManager> sServiceManager;

int main() {
    Environment::setRootDirectory(".");

    sServiceManager = new ServiceManager();
    sServiceManager->start();

    sp<IServiceManager> serviceManager = ServiceManager::getServiceManager();

    startSystemSerices(serviceManager);
    startServices(serviceManager);

    Thread::sleep(10000);

    shutdownServices(serviceManager);
    shutdownSystemSerices(serviceManager);

    sServiceManager->shutdown();

    return 0;
}
