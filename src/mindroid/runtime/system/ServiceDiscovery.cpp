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

#include <mindroid/io/File.h>
#include <mindroid/lang/IllegalArgumentException.h>
#include <mindroid/net/URI.h>
#include <mindroid/runtime/system/ServiceDiscovery.h>
#include <mindroid/util/Log.h>
#include <tinyxml2/tinyxml2.h>

using namespace tinyxml2;

namespace mindroid {

const char* const ServiceDiscovery::TAG = "ServiceDiscovery";
const char* const ServiceDiscovery::ROOT_TAG = "runtime";
const char* const ServiceDiscovery::NODES_TAG = "nodes";
const char* const ServiceDiscovery::NODE_TAG = "node";
const char* const ServiceDiscovery::NODE_ID_ATTR = "id";
const char* const ServiceDiscovery::PLUGIN_TAG = "plugin";
const char* const ServiceDiscovery::PLUGIN_SCHEME_ATTR = "scheme";
const char* const ServiceDiscovery::PLUGIN_CLASS_ATTR = "class";
const char* const ServiceDiscovery::SERVER_TAG = "server";
const char* const ServiceDiscovery::SERVER_URI_ATTR = "uri";
const char* const ServiceDiscovery::SERVICE_DISCOVERY_TAG = "serviceDiscovery";
const char* const ServiceDiscovery::SERVICE_TAG = "service";
const char* const ServiceDiscovery::SERVICE_ID_ATTR = "id";
const char* const ServiceDiscovery::SERVICE_NAME_ATTR = "name";
const char* const ServiceDiscovery::ANNOUNCEMENT_TAG = "announcement";
const char* const ServiceDiscovery::ANNOUNCEMENT_INTERFACE_DESCRIPTOR_ATTR = "interfaceDescriptor";

sp<ServiceDiscovery::Configuration> ServiceDiscovery::read(const sp<File>& file) {
    XMLDocument doc;
    if (doc.LoadFile(file->getPath()->c_str()) != XML_SUCCESS) {
        return nullptr;
    }

    sp<Configuration> configuration = new Configuration();
    XMLElement* rootNode = doc.FirstChildElement(ROOT_TAG);
    if (rootNode != nullptr) {
        XMLElement* element;
        for (element = rootNode->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
            if (XMLUtil::StringEqual(NODES_TAG, element->Name())) {
                parseNodes(element, configuration);
            } else if (XMLUtil::StringEqual(SERVICE_DISCOVERY_TAG, element->Name())) {
                parseServiceDiscovery(element, configuration);
            }
        }
        return configuration;
    }
    return nullptr;
}

void ServiceDiscovery::parseNodes(const XMLElement* curElement, const sp<Configuration>& configuration) {
    const XMLElement* element;
    for (element = curElement->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
        if (XMLUtil::StringEqual(NODE_TAG, element->Name())) {
            sp<Configuration::Node> node = parseNode(element);
            if (node != nullptr) {
                configuration->nodes->put(node->id, node);
            }
        }
    }
}

sp<ServiceDiscovery::Configuration::Node> ServiceDiscovery::parseNode(const XMLElement* curElement) {
    sp<Configuration::Node> node = new Configuration::Node();
    const XMLAttribute* attribute = curElement->FindAttribute(NODE_ID_ATTR);
    if (attribute != nullptr) {
        node->id = attribute->UnsignedValue();
    }
    if (node->id == 0) {
        Log::e(TAG, "Invalid node: %u", node->id);
        return nullptr;
    }

    const XMLElement* element;
    for (element = curElement->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
        if (XMLUtil::StringEqual(PLUGIN_TAG, element->Name())) {
            sp<Configuration::Plugin> plugin = parsePlugin(element);
            if (plugin != nullptr) {
                plugin->node = node;
                node->plugins->put(plugin->scheme, plugin);
            }
        }
    }

    return node;
}

sp<ServiceDiscovery::Configuration::Plugin> ServiceDiscovery::parsePlugin(const XMLElement* curElement) {
    sp<Configuration::Plugin> plugin = new Configuration::Plugin();
    const XMLAttribute* attribute = curElement->FindAttribute(PLUGIN_SCHEME_ATTR);
    if (attribute != nullptr) {
        plugin->scheme = String::valueOf(attribute->Value());
    }
    attribute = curElement->FindAttribute(PLUGIN_CLASS_ATTR);
    if (attribute != nullptr) {
        plugin->clazz = String::valueOf(attribute->Value());
    }
    if (plugin->scheme == nullptr || plugin->scheme->isEmpty()
            || plugin->clazz == nullptr || plugin->clazz->isEmpty()) {
        Log::e(TAG, "Invalid plugin: %s", (plugin->clazz) != nullptr ? plugin->clazz->c_str() : "");
        return nullptr;
    }

    const XMLElement* element;
    for (element = curElement->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
        if (XMLUtil::StringEqual(SERVER_TAG, element->Name())) {
            plugin->server = parseServer(element);
        }
    }

    return plugin;
}

sp<ServiceDiscovery::Configuration::Server> ServiceDiscovery::parseServer(const XMLElement* curElement) {
    sp<Configuration::Server> server = new Configuration::Server();
    const XMLAttribute* attribute = curElement->FindAttribute(SERVER_URI_ATTR);
    if (attribute != nullptr) {
        server->uri = String::valueOf(attribute->Value());
    }
    if (server->uri == nullptr || server->uri->isEmpty()) {
        Log::e(TAG, "Invalid server: %s", (server->uri) != nullptr ? server->uri->c_str() : "");
        return nullptr;
    }

    return server;
}

void ServiceDiscovery::parseServiceDiscovery(const XMLElement* sdNode, const sp<Configuration>& configuration) {
    const XMLElement* element;
    for (element = sdNode->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
        if (XMLUtil::StringEqual(NODE_TAG, element->Name())) {
            parseServiceDiscoveryNode(element, configuration);
        }
    }
}

void ServiceDiscovery::parseServiceDiscoveryNode(const XMLElement* curElement, const sp<Configuration>& configuration) {
    uint32_t nodeId = 0;
    const XMLAttribute* attribute = curElement->FindAttribute(NODE_ID_ATTR);
    if (attribute != nullptr) {
        nodeId = attribute->UnsignedValue();
    }
    if (!configuration->nodes->containsKey(nodeId)) {
        return;
    }

    sp<Configuration::Node> node = configuration->nodes->get(nodeId);
    const XMLElement* element;
    for (element = curElement->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
        if (XMLUtil::StringEqual(SERVICE_TAG, element->Name())) {
            sp<Configuration::Service> service = parseService(element);
            if (service != nullptr) {
                service->node = node;
                node->services->put(service->name, service);
                configuration->services->put(service->name, service);
            }
        }
    }
}

sp<ServiceDiscovery::Configuration::Service> ServiceDiscovery::parseService(const XMLElement* curElement) {
    sp<Configuration::Service> service = new Configuration::Service();
    const XMLAttribute* attribute = curElement->FindAttribute(SERVICE_ID_ATTR);
    if (attribute != nullptr) {
        service->id = attribute->IntValue();
    }
    attribute = curElement->FindAttribute(SERVICE_NAME_ATTR);
    if (attribute != nullptr) {
        service->name = String::valueOf(attribute->Value());
    }
    if (service->id == 0 || service->name == nullptr || service->name->isEmpty()) {
        Log::e(TAG, "Invalid service: %u", service->id);
        return nullptr;
    }

    const XMLElement* element;
    for (element = curElement->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
        if (XMLUtil::StringEqual(ANNOUNCEMENT_TAG, element->Name())) {
            sp<URI> interfaceDescriptor = parseAnnouncement(element);
            if (interfaceDescriptor != nullptr) {
                service->announcements->put(interfaceDescriptor->getScheme(), interfaceDescriptor->toString());
            }
        }
    }

    return service;
}

sp<URI> ServiceDiscovery::parseAnnouncement(const XMLElement* curElement) {
    sp<String> interfaceDescriptor = nullptr;
    const XMLAttribute* attribute = curElement->FindAttribute(ANNOUNCEMENT_INTERFACE_DESCRIPTOR_ATTR);
    if (attribute != nullptr) {
        interfaceDescriptor = String::valueOf(attribute->Value());
    }
    if (interfaceDescriptor == nullptr || interfaceDescriptor->isEmpty()) {
        Log::e(TAG, "Invalid announcement: %u", (interfaceDescriptor != nullptr) ? interfaceDescriptor->c_str() : "");
        return nullptr;
    }

    try {
        return URI::create(interfaceDescriptor);
    } catch (const IllegalArgumentException& e) {
        return nullptr;
    }
}

} /* namespace mindroid */
