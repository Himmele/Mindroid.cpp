/*
 * Copyright (C) 2013 Daniel Himmelein
 * Copyright (C) 2016 E.S.R.Labs
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
#include <mindroid/content/pm/PackageManager.h>
#include <mindroid/os/Environment.h>
#include <mindroid/os/ServiceManager.h>
#include <mindroid/io/File.h>
#include <mindroid/io/FilenameFilter.h>
#include <mindroid/lang/Class.h>
#include <mindroid/util/Log.h>
#include <tinyxml2/tinyxml2.h>

using namespace tinyxml2;

namespace mindroid {

const char* const PackageManagerService::TAG = "PackageManager";
const char* PackageManagerService::MANIFEST_TAG = "manifest";
const char* PackageManagerService::APPLICATION_TAG = "application";
const char* PackageManagerService::SERVICE_TAG = "service";

void PackageManagerService::onCreate() {
    class ManifestFilenameFilter : public FilenameFilter {
    public:
        bool accept(const sp<File>& dir, const sp<String>& filename) override {
            return filename->toLowerCase()->endsWith(".xml");
        }
    };
    sp<ArrayList<sp<File>>> apps = Environment::getAppsDirectory()->listFiles(new ManifestFilenameFilter());
    if (apps != nullptr) {
        auto itr = apps->iterator();
        while (itr.hasNext()) {
            sp<File> manifest = itr.next();
            sp<PackageInfo> packageInfo = parseManifest(manifest);
            if (packageInfo != nullptr) {
                mPackages->put(packageInfo->packageName, packageInfo);
                auto itr = packageInfo->services->iterator();
                while (itr.hasNext()) {
                    sp<ServiceInfo> si = itr.next();
                    mComponents->put(new ComponentName(si->packageName, si->name), si);
                }
            }
        }
    }

    ServiceManager::addService(Context::PACKAGE_MANAGER, mBinder);
}

int32_t PackageManagerService::onStartCommand(const sp<Intent>& intent, int32_t flags, int32_t startId) {
    sp<String> action = intent->getAction();
    if (action != nullptr) {
        if (action->equals(PackageManager::ACTION_START_APPLICATIONS)) {
            sp<ArrayList<sp<PackageInfo>>> packages = getInstalledPackages(PackageManager::GET_SERVICES);
            if (packages != nullptr) {
                sp<IServiceManager> serviceManager = ServiceManager::getServiceManager();
                auto packageItr = packages->iterator();
                while (packageItr.hasNext()) {
                    sp<PackageInfo> packageInfo = packageItr.next();
                    if (packageInfo->services != nullptr) {
                        sp<ArrayList<sp<ServiceInfo>>> services = packageInfo->services;
                        auto serviceItr = services->iterator();
                        while (serviceItr.hasNext()) {
                            sp<ServiceInfo> serviceInfo = serviceItr.next();
                            if (serviceInfo->isEnabled() && serviceInfo->hasFlag(ServiceInfo::FLAG_AUTO_START)) {
                                sp<Intent> service = new Intent();
                                service->setComponent(new ComponentName(serviceInfo->packageName, serviceInfo->name));
                                Log::d(TAG, "Starting service %s.%s [version: code=%d, name=%s]", serviceInfo->packageName->c_str(), serviceInfo->name->c_str(), packageInfo->versionCode, packageInfo->versionName->c_str());
                                serviceManager->startService(service);
                            }
                        }
                    }
                }
            }
        } else if (action->equals(PackageManager::ACTION_SHUTDOWN_APPLICATIONS)) {
            sp<ArrayList<sp<PackageInfo>>> packages = getInstalledPackages(PackageManager::GET_SERVICES);
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
                                serviceManager->stopService(service);
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}

void PackageManagerService::onDestroy() {
    ServiceManager::removeService(mBinder);
}

sp<ArrayList<sp<PackageInfo>>> PackageManagerService::getInstalledPackages(int32_t flags) {
    if ((flags & PackageManager::GET_SERVICES) == PackageManager::GET_SERVICES) {
        sp<ArrayList<sp<PackageInfo>>> packages = new ArrayList<sp<PackageInfo>>();
        auto itr = mPackages->iterator();
        while (itr.hasNext()) {
            auto entry = itr.next();
            sp<PackageInfo> p = entry.getValue();
            packages->add(p);
        }
        return packages->isEmpty() ? nullptr : packages;
    } else {
        return nullptr;
    }
}

sp<ResolveInfo> PackageManagerService::resolveService(const sp<Intent>& intent, int32_t flags) {
    sp<ResolveInfo> resolveInfo = nullptr;
    if (mComponents->containsKey(intent->getComponent())) {
        sp<ComponentInfo> componentInfo = mComponents->get(intent->getComponent());
        if (Class<ServiceInfo>::isInstance(componentInfo)) {
            resolveInfo = new ResolveInfo();
            resolveInfo->serviceInfo = object_cast<ServiceInfo>(componentInfo);
        }
    }
    return resolveInfo;
}

sp<PackageInfo> PackageManagerService::parseManifest(const sp<File>& file) {
    XMLDocument doc;
    if (doc.LoadFile(file->getPath()->c_str()) == XML_SUCCESS) {
        const XMLElement* rootNode = doc.FirstChildElement(MANIFEST_TAG);
        if (rootNode != nullptr) {
            sp<PackageInfo> pi = new PackageInfo();
            sp<ApplicationInfo> ai = new ApplicationInfo();
            pi->applicationInfo = ai;

            sp<String> packageName;
            int32_t versionCode = 0;
            sp<String> versionName = String::EMPTY_STRING;
            const XMLAttribute* attribute = rootNode->FindAttribute("package");
            if (attribute != nullptr) {
                packageName = String::valueOf(attribute->Value());
            }
            if (packageName == nullptr || packageName->length() == 0) {
                Log::e(TAG, "Manifest is missing a package name");
                return nullptr;
            }
            attribute = rootNode->FindAttribute("mindroid:versionCode");
            if (attribute != nullptr) {
                versionCode = attribute->IntValue();
            }
            attribute = rootNode->FindAttribute("mindroid:versionName");
            if (attribute != nullptr) {
                versionName = String::valueOf(attribute->Value());
            }

            pi->packageName = packageName;
            pi->versionCode = versionCode;
            pi->versionName = versionName;
            ai->packageName = pi->packageName;

            const XMLElement* element;
            for (element = rootNode->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
                if (XMLUtil::StringEqual(APPLICATION_TAG, element->Name())) {
                    sp<ArrayList<sp<ServiceInfo>>> services = parseApplication(ai, element);
                    pi->services = services;
                }
            }

            return pi;
        } else {
            return nullptr;
        }
    } else {
        Log::w(TAG, "Cannot read %s", file->getPath()->c_str());
        return nullptr;
    }
}

sp<ArrayList<sp<ServiceInfo>>> PackageManagerService::parseApplication(sp<ApplicationInfo>& ai, const XMLElement* applicationNode) {
    sp<ArrayList<sp<ServiceInfo>>> services = new ArrayList<sp<ServiceInfo>>();

    const XMLAttribute* attribute = applicationNode->FindAttribute("process");
    if (attribute != nullptr) {
        ai->processName = String::valueOf(attribute->Value());
    }
    attribute = applicationNode->FindAttribute("enabled");
    if (attribute != nullptr) {
        ai->enabled = attribute->BoolValue();
    }

    const XMLElement* element;
    for (element = applicationNode->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
        if (XMLUtil::StringEqual(SERVICE_TAG, element->Name())) {
            sp<ServiceInfo> si = parseService(ai, element);
            if (si != nullptr) {
                services->add(si);
            }
        }
    }

    return services;
}

sp<ServiceInfo> PackageManagerService::parseService(sp<ApplicationInfo>& ai, const XMLElement* serviceNode) {
    sp<String> name;
    sp<String> processName = ai->packageName;
    bool enabled = true;
    bool autostart = false;

    const XMLAttribute* attribute = serviceNode->FindAttribute("mindroid:name");
    if (attribute != nullptr) {
        name = String::valueOf(attribute->Value());
        if (name->startsWith(".")) {
            name = name->substring(1);
        } else {
            if (name->startsWith(ai->packageName)) {
                name = name->substring(ai->packageName->length() + 1);
            } else {
                name = nullptr;
            }
        }
    }
    attribute = serviceNode->FindAttribute("mindroid:process");
    if (attribute != nullptr) {
        processName = String::valueOf(attribute->Value());
    }
    attribute = serviceNode->FindAttribute("mindroid:enabled");
    if (attribute != nullptr) {
        enabled = attribute->BoolValue();
    }
    attribute = serviceNode->FindAttribute("mindroid:autostart");
    if (attribute != nullptr) {
        autostart = attribute->BoolValue();
    }

    if (name == nullptr || name->length() == 0) {
        return nullptr;
    }

    sp<ServiceInfo> si = new ServiceInfo();
    si->name = name;
    si->packageName = ai->packageName;
    si->applicationInfo = ai;
    si->processName = processName;
    si->enabled = enabled;
    if (autostart) {
        si->flags |= ServiceInfo::FLAG_AUTO_START;
    }

    return si;
}

} /* namespace mindroid */
