/*
 * Copyright (C) 2018 Daniel Himmelein
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

#include <mindroid/content/pm/PackageManagerService.h>
#include <mindroid/os/ServiceManager.h>
#include <mindroid/os/Environment.h>
#include <mindroid/lang/Class.h>
#include <mindroid/util/logging/Logger.h>
#include "examples/Services/src/ServiceExample1.h"
#include "examples/Services/src/ServiceExample2.h"
#include "examples/Concurrency/src/PromiseExample.h"
#include "examples/Concurrency/src/AsyncTaskExample.h"
#include "examples/Concurrency/src/HandlerExample.h"

using namespace mindroid;

CLASS(mindroid, Logger);
CLASS(mindroid, PackageManagerService);
CLASS(examples, ServiceExample1);
CLASS(examples, ServiceExample2);
CLASS(examples, HandlerExample);
CLASS(examples, PromiseExample);
CLASS(examples, AsyncTaskExample);

void startSystemSerices() {
    sp<IServiceManager> serviceManager = ServiceManager::getServiceManager();

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

void startServices() {
    sp<IServiceManager> serviceManager = ServiceManager::getServiceManager();

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

void shutdownSystemSerices() {
    sp<IServiceManager> serviceManager = ServiceManager::getServiceManager();

    sp<Intent> packageManager = new Intent();
    packageManager->setComponent(new ComponentName("mindroid", "PackageManagerService"));
    serviceManager->stopSystemService(packageManager);
    ServiceManager::waitForSystemServiceShutdown(Context::PACKAGE_MANAGER);

    sp<Intent> logger = new Intent();
    logger->setComponent(new ComponentName("mindroid", "Logger"));
    serviceManager->stopSystemService(logger);
}

void shutdownServices() {
    sp<IServiceManager> serviceManager = ServiceManager::getServiceManager();

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

int32_t main() {
    Environment::setRootDirectory(".");

    sp<ServiceManager> serviceManager = new ServiceManager();
    serviceManager->start();

    startSystemSerices();
    startServices();

    Thread::sleep(10000);

    shutdownServices();
    shutdownSystemSerices();

    serviceManager->shutdown();

    return 0;
}
