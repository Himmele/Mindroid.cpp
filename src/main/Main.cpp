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
#include <mindroid/lang/Integer.h>
#include <mindroid/lang/NumberFormatException.h>
#include <mindroid/lang/System.h>
#include <mindroid/io/File.h>
#include <mindroid/runtime/inspection/ConsoleService.h>
#include <mindroid/runtime/system/Runtime.h>
#include <mindroid/util/logging/LoggerService.h>
#include "examples/Services/src/ServiceExample1.h"
#include "examples/Services/src/ServiceExample2.h"
#include "examples/Concurrency/src/PromiseExample.h"
#include "examples/Concurrency/src/AsyncTaskExample.h"
#include "examples/Concurrency/src/HandlerExample.h"
#include "examples/Eliza/src/ElizaService.h"
#include "examples/Eliza/src/You.h"
#include <signal.h>

using namespace mindroid;

CLASS(mindroid, PackageManagerService);
CLASS(mindroid, LoggerService);
CLASS(mindroid, ConsoleService);
CLASS(examples, ServiceExample1);
CLASS(examples, ServiceExample2);
CLASS(examples, HandlerExample);
CLASS(examples, PromiseExample);
CLASS(examples, AsyncTaskExample);
CLASS(examples, ElizaService);
CLASS(examples, You);

void startSystemSerices() {
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
    serviceManager->startSystemService(logger);
    ServiceManager::waitForSystemService(Context::LOGGER_SERVICE);

    sp<Intent> console = new Intent();
    console->setComponent(new ComponentName("mindroid", "ConsoleService"))
            ->putExtra("name", Context::CONSOLE_SERVICE->toString())
            ->putExtra("process", "main");
    serviceManager->startSystemService(console);
    ServiceManager::waitForSystemService(Context::CONSOLE_SERVICE);

    sp<Intent> packageManager = new Intent();
    packageManager->setComponent(new ComponentName("mindroid", "PackageManagerService"))
            ->putExtra("name", Context::PACKAGE_MANAGER->toString())
            ->putExtra("process", "main");
    serviceManager->startSystemService(packageManager);
    ServiceManager::waitForSystemService(Context::PACKAGE_MANAGER);
}

void startServices() {
    sp<IServiceManager> serviceManager = ServiceManager::getServiceManager();
    sp<Intent> intent = new Intent(PackageManager::ACTION_START_APPLICATIONS);
    intent->setComponent(new ComponentName("mindroid", "PackageManagerService"));
    serviceManager->startSystemService(intent);
}

void shutdownSystemSerices() {
    sp<IServiceManager> serviceManager = ServiceManager::getServiceManager();

    sp<Intent> packageManager = new Intent();
    packageManager->setComponent(new ComponentName("mindroid", "PackageManagerService"));
    serviceManager->stopSystemService(packageManager);
    ServiceManager::waitForSystemServiceShutdown(Context::PACKAGE_MANAGER);

    sp<Intent> console = new Intent();
    console->setComponent(new ComponentName("mindroid", "ConsoleService"));
    serviceManager->stopSystemService(console);
    ServiceManager::waitForSystemServiceShutdown(Context::CONSOLE_SERVICE);

    sp<Intent> logger = new Intent();
    logger->setComponent(new ComponentName("mindroid", "LoggerService"));
    serviceManager->stopSystemService(logger);
    ServiceManager::waitForSystemServiceShutdown(Context::LOGGER_SERVICE);
}

void shutdownServices() {
    sp<IServiceManager> serviceManager = ServiceManager::getServiceManager();
    sp<Intent> intent = new Intent(PackageManager::ACTION_SHUTDOWN_APPLICATIONS);
    intent->setComponent(new ComponentName("mindroid", "PackageManagerService"));
    serviceManager->startSystemService(intent);
}

static sp<Promise<int32_t>> sShutdownHandler = new Promise<int32_t>();

static void signalHandler(int32_t signal) {
    sShutdownHandler->complete(signal);
}

int32_t main(int32_t argc, char* argv[]) {
    ::signal(SIGINT, signalHandler);

    const sp<String> ID_ARG = String::valueOf("id=");
    const sp<String> ROOT_DIR_ARG = String::valueOf("rootDir=");

    uint32_t nodeId = 1;
    sp<String> rootDir = String::valueOf(".");
    for (int32_t i = 0; i < argc; ++i) {
        sp<String> arg = String::valueOf(argv[i]);
        if (arg->startsWith(ID_ARG)) {
            try {
                nodeId = Integer::valueOf(arg->substring(ID_ARG->length()))->intValue();
            } catch (const NumberFormatException& e) {
                Log::println('E', "Main", "Invalid node id: %s", arg->c_str());
                System::exit(-1);
            }
        } else if (arg->startsWith(ROOT_DIR_ARG)) {
            rootDir = arg->substring(ROOT_DIR_ARG->length());
        }
    }

    Environment::setRootDirectory(rootDir);

    sp<File> file = new File(Environment::getRootDirectory(), "res/MindroidRuntimeSystem.xml");
    Runtime::start(nodeId, file->exists() ? file : nullptr);

    sp<ServiceManager> serviceManager = new ServiceManager();
    serviceManager->start();

    startSystemSerices();
    startServices();

    sShutdownHandler->get();
    Log::println('D', "Main", "Shutting down...");

    shutdownServices();
    shutdownSystemSerices();

    serviceManager->shutdown();

    Runtime::shutdown();

    return 0;
}
