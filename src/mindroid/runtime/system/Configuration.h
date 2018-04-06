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

#ifndef MINDROID_RUNTIME_SYSTEM_CONFIGURATION_H_
#define MINDROID_RUNTIME_SYSTEM_CONFIGURATION_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/util/ArrayList.h>
#include <mindroid/util/HashMap.h>

namespace tinyxml2 {
class XMLElement;
}

namespace mindroid {

class File;

class Configuration : public Object {
public:
    class Node;
    class Service;

    class Plugin : public Object {
    public:
        sp<String> name;
        sp<String> scheme;
        sp<String> clazz;
        bool enabled;
        sp<HashMap<uint32_t, sp<Node>>> nodes;
        sp<HashMap<sp<String>, sp<Service>>> services;
    };

    class Node : public Object {
    public:
        uint32_t id;
        sp<String> uri;
        sp<ArrayList<sp<Service>>> services;
    };

    class Service : public Object {
    public:
        sp<Node> node;
        uint32_t id;
        sp<String> name;
        sp<String> interfaceDescriptor;
    };

    sp<HashMap<sp<String>, sp<Plugin>>> plugins = new HashMap<sp<String>, sp<Plugin>>();

    static sp<Configuration> read(const sp<File>& configurationFile);

private:
    static void parsePlugins(const tinyxml2::XMLElement* rootNode, const sp<Configuration>& configuration);
    static sp<Configuration::Plugin> parsePlugin(const tinyxml2::XMLElement* pluginNode);
    static void parseServiceDiscovery(const tinyxml2::XMLElement* sdNode, const sp<Plugin>& plugin);
    static sp<Configuration::Node> parseNode(const tinyxml2::XMLElement* nodeNode);
    static sp<Configuration::Service> parseService(const tinyxml2::XMLElement* serviceNode);

    static const char* const TAG;
    static const char* const ROOT_TAG;
    static const char* const PLUGINS_TAG;
    static const char* const PLUGIN_TAG;
    static const char* const PLUGIN_NAME_ATTR;
    static const char* const PLUGIN_SCHEME_ATTR;
    static const char* const PLUGIN_CLASS_ATTR;
    static const char* const PLUGIN_ENABLED_ATTR;
    static const char* const SERVICE_DISCOVERY_TAG;
    static const char* const NODE_TAG;
    static const char* const NODE_ID_ATTR;
    static const char* const NODE_URI_ATTR;
    static const char* const SERVICE_TAG;
    static const char* const SERVICE_ID_ATTR;
    static const char* const SERVICE_NAME_ATTR;
    static const char* const SERVICE_INTERFACE_ATTR;
};

} /* namespace mindroid */

#endif /* MINDROID_RUNTIME_SYSTEM_CONFIGURATION_H_ */
