/*
 * Copyright (C) 2019 E.S.R.Labs
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

#include <gtest/gtest.h>
#include <mindroid/runtime/system/Runtime.h>
#include <mindroid/content/Context.h>
#include <mindroid/io/File.h>
#include <mindroid/os/ServiceManager.h>

using namespace mindroid;

/*
 * Mindroid runtime system defaults service URIs to "mindroid" scheme.
 */
TEST(Runtime, checkServiceUris) {
    Runtime::start(1, nullptr);
    sp<ServiceManager> serviceManager = new ServiceManager();
    serviceManager->start();

    sp<URI> relativeURI = new URI(Context::SERVICE_MANAGER->getSchemeSpecificPart());

    sp<IBinder> serviceA = Runtime::getRuntime()->getService(relativeURI);
    sp<IBinder> serviceB = Runtime::getRuntime()->getService(Context::SERVICE_MANAGER);

    EXPECT_EQ(serviceA, serviceB);

    serviceManager->shutdown();
    Runtime::shutdown();
}
