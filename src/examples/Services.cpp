#include "mindroid/app/Service.h"
#include "mindroid/content/pm/PackageManagerService.h"
#include "mindroid/os/ServiceManager.h"
#include "mindroid/os/Environment.h"
#include "mindroid/lang/Class.h"
#include "mindroid/util/Log.h"

using namespace mindroid;

namespace test {

class Service1 :
		public Service {
public:
	Service1() {
		Log::d(LOG_TAG, "Service1::ctor");
	}

	virtual ~Service1() {
		Log::d(LOG_TAG, "Service1::dtor");
	}

	virtual void onCreate() {
		mHandler = new Handler();
		Log::d(LOG_TAG, "Service1::onCreate");
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
		Log::d(LOG_TAG, "Service1::onStartCommand");
		return 0;
	}

	virtual void onDestroy() {
		Log::d(LOG_TAG, "Service1::onDestroy");
	}

	virtual sp<IBinder> onBind(const sp<Intent>& intent) { return nullptr; }

	void test() {
		Log::d(LOG_TAG, "Test");
		mHandler->postDelayed([this] { test(); }, 1000);
	}

	class ServiceConnection1 :
			public ServiceConnection {
	public:
		virtual void onServiceConnected(const sp<ComponentName>& name, const sp<IBinder>& service) {
			Log::d(LOG_TAG, "Service1::onServiceConnected: %s", name->toShortString()->c_str());
		}

		virtual void onServiceDisconnected(const sp<ComponentName>& name) {
			Log::d(LOG_TAG, "Service1::onServiceDisconnected: %s", name->toShortString()->c_str());
		}
	};

private:
	static const char* const LOG_TAG;

	sp<Handler> mHandler;
	sp<ServiceConnection> mServiceConnection;
};

const char* const Service1::LOG_TAG = "Service1";

class Service2 :
		public Service {
public:
	Service2() {
		Log::d(LOG_TAG, "Service2::ctor");
	}

	virtual ~Service2() {
		Log::d(LOG_TAG, "Service2::dtor");
	}

	virtual void onCreate() {
		Log::d(LOG_TAG, "Service2::onCreate");
		sp<SharedPreferences> sp = getSharedPreferences("Test", 0);
		int32_t value = sp->getInt("Test", 0);
		Log::d(LOG_TAG, "Value: %d", value);
		sp->edit()->putInt("Test", 123)->commit();
	}

	virtual void onDestroy() {
		Log::d(LOG_TAG, "Service2::onDestroy");
	}

	virtual sp<IBinder> onBind(const sp<Intent>& intent) {
		Log::d(LOG_TAG, "Service2::onBind: %s", intent->getComponent()->toShortString()->c_str());
		return nullptr;
	}

	bool onUnbind(const sp<Intent>& intent) {
		Log::d(LOG_TAG, "Service2::onUnbind: %s", intent->getComponent()->toShortString()->c_str());
		return true;
	}

private:
	static const char* const LOG_TAG;
};

const char* const Service2::LOG_TAG = "Service2";

} /* namespace test */

CLASS(mindroid, PackageManagerService);
CLASS(test, Service1);
CLASS(test, Service2);

void startSystemSerices(sp<IServiceManager> serviceManager) {
	sp<Intent> packageManager = new Intent();
	packageManager->setComponent(new ComponentName("mindroid", "PackageManagerService"))
			->putExtra("process", "main")
			->putExtra("service", "PackageManager");
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
	packageManager->setComponent(new ComponentName("mindroid", "PackageManagerService"))
			->putExtra("process", "main")
			->putExtra("service", "PackageManager");
	serviceManager->stopSystemService(packageManager);
	ServiceManager::waitForSystemServiceShutdown(Context::PACKAGE_MANAGER);
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

sp<ServiceManager> sServiceManager;

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
