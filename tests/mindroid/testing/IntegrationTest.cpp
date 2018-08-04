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

#include <mindroid/testing/IntegrationTest.h>
#include <mindroid/content/pm/PackageManager.h>
#include <mindroid/content/pm/PackageManagerService.h>
#include <mindroid/io/File.h>
#include <mindroid/lang/Class.h>
#include <mindroid/lang/Integer.h>
#include <mindroid/lang/NumberFormatException.h>
#include <mindroid/lang/System.h>
#include <mindroid/os/ServiceManager.h>
#include <mindroid/os/Environment.h>
#include <mindroid/runtime/inspection/ConsoleService.h>
#include <mindroid/runtime/system/Runtime.h>
#include <mindroid/util/Properties.h>
#include <mindroid/util/logging/Logger.h>
#include <mindroid/util/logging/LoggerService.h>

CLASS(mindroid, PackageManagerService);
CLASS(mindroid, LoggerService);
CLASS(mindroid, ConsoleService);

namespace mindroid {

const char* const IntegrationTest::TAG = "IntegrationTest";
sp<ServiceManager> IntegrationTest::sServiceManager = nullptr;

void IntegrationTest::SetUpTestCase() {
    const uint32_t nodeId = 1;
    const sp<String> rootDir = String::valueOf(".");

    System::setProperty(Properties::INTEGRATION_TESTING, "true");
    Log::setIntegrationTesting(true);

    Environment::setRootDirectory(rootDir);

    sp<File> file = new File(Environment::getRootDirectory(), "res/MindroidRuntimeSystem.xml");
    Runtime::start(nodeId, file->exists() ? file : nullptr);

    sServiceManager = new ServiceManager();
    sServiceManager->start();

    startSystemSerices();
    startServices();
}

void IntegrationTest::TearDownTestCase() {
    shutdownServices();
    shutdownSystemSerices();

    sServiceManager->shutdown();
    sServiceManager = nullptr;

    Runtime::shutdown();
}

void IntegrationTest::SetUp() {
    sp<Logger> logger = new Logger();
    logger->mark();
}

void IntegrationTest::TearDown() {
    sp<Logger> logger = new Logger();
    logger->reset();
}

void IntegrationTest::startSystemSerices() {
    sp<IServiceManager> serviceManager = ServiceManager::getServiceManager();

    sp<ArrayList<sp<String>>> logFlags = new ArrayList<sp<String>>();
    logFlags->add(String::valueOf("timestamp"));
    sp<Intent> logger = new Intent(Logger::ACTION_LOG);
    logger->setComponent(new ComponentName("mindroid", "LoggerService"))
            ->putExtra("name", Context::LOGGER_SERVICE->toString())
            ->putExtra("process", "main")
            ->putExtra("logBuffer", Log::LOG_ID_MAIN)
            ->putExtra("logPriority", Log::DEBUG)
            ->putStringArrayListExtra("logFlags", logFlags)
            ->putExtra("consoleLogging", true);
    serviceManager->startSystemService(logger)->get();

    sp<Intent> console = new Intent();
    console->setComponent(new ComponentName("mindroid", "ConsoleService"))
            ->putExtra("name", Context::CONSOLE_SERVICE->toString())
            ->putExtra("process", "main");
    serviceManager->startSystemService(console)->get();

    sp<Intent> packageManager = new Intent();
    packageManager->setComponent(new ComponentName("mindroid", "PackageManagerService"))
            ->putExtra("name", Context::PACKAGE_MANAGER->toString())
            ->putExtra("process", "main");
    serviceManager->startSystemService(packageManager)->get();
}

void IntegrationTest::startServices() {
    sp<PackageManager> packageManager = new PackageManager();
    sp<ArrayList<sp<PackageInfo>>> packages = packageManager->getInstalledPackages(PackageManager::GET_SERVICES);
    if (packages != nullptr) {
        sp<IServiceManager> serviceManager = ServiceManager::getServiceManager();
        auto packageItr = packages->iterator();
        while (packageItr.hasNext()) {
            sp<PackageInfo> package = packageItr.next();
            if (package->services != nullptr) {
                sp<ArrayList<sp<ServiceInfo>>> services = package->services;
                auto serviceItr = services->iterator();
                while (serviceItr.hasNext()) {
                    sp<ServiceInfo> serviceInfo = serviceItr.next();
                    if (serviceInfo->isEnabled() && serviceInfo->hasFlag(ServiceInfo::FLAG_AUTO_START)) {
                        sp<Intent> service = new Intent();
                        service->setComponent(new ComponentName(serviceInfo->packageName, serviceInfo->name));
                        try {
                            serviceManager->startService(service)->get(10000);
                        } catch (...) {
                            throw RuntimeException("System failure");
                        }
                    }
                }
            }
        }
    }
}

void IntegrationTest::shutdownSystemSerices() {
    sp<IServiceManager> serviceManager = ServiceManager::getServiceManager();

    sp<Intent> packageManager = new Intent();
    packageManager->setComponent(new ComponentName("mindroid", "PackageManagerService"));
    serviceManager->stopSystemService(packageManager)->get();

    sp<Intent> console = new Intent();
    console->setComponent(new ComponentName("mindroid", "ConsoleService"));
    serviceManager->stopSystemService(console)->get();

    sp<Intent> logger = new Intent();
    logger->setComponent(new ComponentName("mindroid", "LoggerService"));
    serviceManager->stopSystemService(logger)->get();
}

void IntegrationTest::shutdownServices() {
    sp<PackageManager> packageManager = new PackageManager();
    sp<ArrayList<sp<PackageInfo>>> packages = packageManager->getInstalledPackages(PackageManager::GET_SERVICES);
    if (packages != nullptr) {
        sp<IServiceManager> serviceManager = ServiceManager::getServiceManager();
        auto packageItr = packages->iterator();
        while (packageItr.hasNext()) {
            sp<PackageInfo> package = packageItr.next();
            if (package->services != nullptr) {
                sp<ArrayList<sp<ServiceInfo>>> services = package->services;
                auto serviceItr = services->iterator();
                while (serviceItr.hasNext()) {
                    sp<ServiceInfo> serviceInfo = serviceItr.next();
                    if (serviceInfo->isEnabled()) {
                        sp<Intent> service = new Intent();
                        service->setComponent(new ComponentName(serviceInfo->packageName, serviceInfo->name));
                        sp<String> name = String::format("%s::%s", serviceInfo->packageName->c_str(), serviceInfo->name->c_str());
                        uint64_t start = SystemClock::uptimeMillis();
                        try {
                            serviceManager->stopService(service)->get(10000);
                            uint64_t end = SystemClock::uptimeMillis();
                            if (end - start >= 1000) {
                                Log::println('W', TAG, String::format("Stopping service %s took very long: %" PRIu64 "ms", name->c_str(), end - start)->c_str());
                            }
                        } catch (const CancellationException& e) {
                        } catch (const ExecutionException& e) {
                        } catch (const TimeoutException& e) {
                        } catch (const RemoteException& e) {
                            Log::println('E', TAG, "Failed to stop service %s", name->c_str());
                        }
                    }
                }
            }
        }
    }
}

} /* namespace mindroid */
