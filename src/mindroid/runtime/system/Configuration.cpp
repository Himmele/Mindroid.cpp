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

#include <mindroid/runtime/system/Configuration.h>
#include <mindroid/io/File.h>
#include <mindroid/util/Log.h>
#include <tinyxml2/tinyxml2.h>

using namespace tinyxml2;

namespace mindroid {

const char* const Configuration::TAG = "Configuration";
const char* const Configuration::ROOT_TAG = "runtime";
const char* const Configuration::PLUGINS_TAG = "plugins";
const char* const Configuration::PLUGIN_TAG = "plugin";
const char* const Configuration::PLUGIN_NAME_ATTR = "name";
const char* const Configuration::PLUGIN_SCHEME_ATTR = "scheme";
const char* const Configuration::PLUGIN_CLASS_ATTR = "class";
const char* const Configuration::PLUGIN_ENABLED_ATTR = "enabled";
const char* const Configuration::SERVICE_DISCOVERY_TAG = "serviceDiscovery";
const char* const Configuration::NODE_TAG = "node";
const char* const Configuration::NODE_ID_ATTR = "id";
const char* const Configuration::NODE_URI_ATTR = "uri";
const char* const Configuration::SERVICE_TAG = "service";
const char* const Configuration::SERVICE_ID_ATTR = "id";
const char* const Configuration::SERVICE_NAME_ATTR = "name";
const char* const Configuration::SERVICE_INTERFACE_ATTR = "interface";

sp<Configuration> Configuration::read(const sp<File>& configurationFile) {
    XMLDocument doc;
    if (doc.LoadFile(configurationFile->getPath()->c_str()) != XML_SUCCESS) {
        return nullptr;
    }

    sp<Configuration> configuration = new Configuration();
    XMLElement* rootNode = doc.FirstChildElement(ROOT_TAG);
    if (rootNode != nullptr) {
        XMLElement* element;
        for (element = rootNode->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
            if (XMLUtil::StringEqual(PLUGINS_TAG, element->Name())) {
                parsePlugins(element, configuration);
            }
        }
        return configuration;
    }
    return nullptr;
}

void Configuration::parsePlugins(const XMLElement* rootNode, const sp<Configuration>& configuration) {
    const XMLElement* element;
    for (element = rootNode->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
        if (XMLUtil::StringEqual(PLUGIN_TAG, element->Name())) {
            sp<Plugin> plugin = parsePlugin(element);
            if (plugin != nullptr) {
                configuration->plugins->put(plugin->scheme, plugin);
            }
        }
    }
}

sp<Configuration::Plugin> Configuration::parsePlugin(const XMLElement* pluginNode) {
    sp<Configuration::Plugin> plugin = new Configuration::Plugin();

    const XMLAttribute* attribute = pluginNode->FindAttribute(PLUGIN_NAME_ATTR);
    if (attribute != nullptr) {
        plugin->name = String::valueOf(attribute->Value());
    }
    attribute = pluginNode->FindAttribute(PLUGIN_SCHEME_ATTR);
    if (attribute != nullptr) {
        plugin->scheme = String::valueOf(attribute->Value());
    }
    attribute = pluginNode->FindAttribute(PLUGIN_CLASS_ATTR);
    if (attribute != nullptr) {
        plugin->clazz = String::valueOf(attribute->Value());
    }
    attribute = pluginNode->FindAttribute(PLUGIN_ENABLED_ATTR);
    if (attribute != nullptr) {
        plugin->enabled = attribute->BoolValue();
    }

    if (plugin->name == nullptr || plugin->name->isEmpty()
            || plugin->scheme == nullptr || plugin->scheme->isEmpty()
            || plugin->clazz == nullptr || plugin->clazz->isEmpty()) {
        Log::e(TAG, "Invalid plugin: %s", (plugin->name != nullptr) ? plugin->name->c_str() : "");
        return nullptr;
    }

    const XMLElement* element;
    for (element = pluginNode->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
        if (XMLUtil::StringEqual(SERVICE_DISCOVERY_TAG, element->Name())) {
            parseServiceDiscovery(element, plugin);
        }
    }

    return plugin;
}

void Configuration::parseServiceDiscovery(const XMLElement* sdNode, const sp<Plugin>& plugin) {
    plugin->services = new HashMap<sp<String>, sp<Configuration::Service>>();
    sp<HashMap<uint32_t, sp<Configuration::Node>>> nodes = new HashMap<uint32_t, sp<Configuration::Node>>();

    const XMLElement* element;
    for (element = sdNode->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
        if (XMLUtil::StringEqual(NODE_TAG, element->Name())) {
            sp<Configuration::Node> node = parseNode(element);
            if (node != nullptr) {
                nodes->put(node->id, node);
                auto itr = node->services->iterator();
                while (itr.hasNext()) {
                    sp<Configuration::Service> service = itr.next();
                    plugin->services->put(service->name, service);
                }
            }
        }
    }
    plugin->nodes = nodes;
}

sp<Configuration::Node> Configuration::parseNode(const XMLElement* nodeNode) {
    sp<Configuration::Node> node = new Configuration::Node();

    const XMLAttribute* attribute = nodeNode->FindAttribute(NODE_ID_ATTR);
    if (attribute != nullptr) {
        node->id = attribute->IntValue();
    }
    attribute = nodeNode->FindAttribute(NODE_URI_ATTR);
    if (attribute != nullptr) {
        node->uri = String::valueOf(attribute->Value());
    }

    if (node->id == 0) {
        Log::e(TAG, "Invalid node: %u", node->id);
        return nullptr;
    }

    sp<ArrayList<sp<Configuration::Service>>> services = new ArrayList<sp<Configuration::Service>>();
    const XMLElement* element;
    for (element = nodeNode->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
        if (XMLUtil::StringEqual(SERVICE_TAG, element->Name())) {
            sp<Configuration::Service> service = parseService(element);
            if (service != nullptr) {
                service->node = node;
                services->add(service);
            }
        }
    }
    node->services = services;

    return node;
}

sp<Configuration::Service> Configuration::parseService(const XMLElement* serviceNode) {
    sp<Configuration::Service> service = new Configuration::Service();

    const XMLAttribute* attribute = serviceNode->FindAttribute(SERVICE_ID_ATTR);
    if (attribute != nullptr) {
        service->id = attribute->IntValue();
    }
    attribute = serviceNode->FindAttribute(SERVICE_NAME_ATTR);
    if (attribute != nullptr) {
        service->name = String::valueOf(attribute->Value());
    }
    attribute = serviceNode->FindAttribute(SERVICE_INTERFACE_ATTR);
    if (attribute != nullptr) {
        service->interfaceDescriptor = String::valueOf(attribute->Value());
    }

    if (service->id == 0 || service->name == nullptr || service->name->isEmpty()
            || service->interfaceDescriptor == nullptr || service->interfaceDescriptor->isEmpty()) {
        Log::e(TAG, "Invalid service: %u", service->id);
        return nullptr;
    }

    return service;
}

} /* namespace mindroid */
