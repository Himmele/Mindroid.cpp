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

#ifndef MINDROID_RUNTIME_SYSTEM_SERVICEDISCOVERYCONFIGURATIONREADER_H_
#define MINDROID_RUNTIME_SYSTEM_SERVICEDISCOVERYCONFIGURATIONREADER_H_

#include <mindroid/lang/Object.h>
#include <mindroid/lang/String.h>
#include <mindroid/util/ArrayList.h>
#include <mindroid/util/HashMap.h>

namespace tinyxml2 {
class XMLElement;
}

namespace mindroid {

class File;
class URI;

class ServiceDiscoveryConfigurationReader : public Object {
public:
    class Node;
    class Service;

    class Configuration : public Object {
    public:
        class Plugin;
        class Server;
        class Service;

        class Node : public Object {
        public:
            uint32_t id;
            sp<HashMap<sp<String>, sp<Plugin>>> plugins = new HashMap<sp<String>, sp<Plugin>>();
            sp<HashMap<sp<String>, sp<Service>>> services = new HashMap<sp<String>, sp<Service>>();
        };

        class Plugin : public Object {
        public:
            sp<Node> node;
            sp<String> scheme;
            sp<String> clazz;
            sp<Server> server;
        };

        class Server : public Object {
        public:
            sp<String> uri;
        };

        class Service : public Object {
        public:
            sp<Node> node;
            int32_t id = -1;
            sp<String> name;
            sp<HashMap<sp<String>, sp<String>>> announcements = new HashMap<sp<String>, sp<String>>();
        };

        sp<HashMap<uint32_t, sp<Node>>> nodes = new HashMap<uint32_t, sp<Node>>();
        sp<HashMap<sp<String>, sp<Service>>> services = new HashMap<sp<String>, sp<Service>>();
    };

    static sp<Configuration> read(const sp<File>& file);

private:
    static void parseNodes(const tinyxml2::XMLElement* rootNode, const sp<Configuration>& configuration);
    static sp<ServiceDiscoveryConfigurationReader::Configuration::Node> parseNode(const tinyxml2::XMLElement* element);
    static sp<ServiceDiscoveryConfigurationReader::Configuration::Plugin> parsePlugin(const tinyxml2::XMLElement* element);
    static sp<ServiceDiscoveryConfigurationReader::Configuration::Server> parseServer(const tinyxml2::XMLElement* element);
    static void parseServiceDiscovery(const tinyxml2::XMLElement* sdNode, const sp<Configuration>& configuration);
    static void parseServiceDiscoveryNode(const tinyxml2::XMLElement* element, const sp<Configuration>& configuration);
    static sp<ServiceDiscoveryConfigurationReader::Configuration::Service> parseService(const tinyxml2::XMLElement* element);
    static sp<URI> parseAnnouncement(const tinyxml2::XMLElement* element);

    static const char* const TAG;
    static const char* const ROOT_TAG;
    static const char* const NODES_TAG;
    static const char* const NODE_TAG;
    static const char* const NODE_ID_ATTR;
    static const char* const PLUGIN_TAG;
    static const char* const PLUGIN_SCHEME_ATTR;
    static const char* const PLUGIN_CLASS_ATTR;
    static const char* const SERVER_TAG;
    static const char* const SERVER_URI_ATTR;
    static const char* const SERVICE_DISCOVERY_TAG;
    static const char* const SERVICE_TAG;
    static const char* const SERVICE_ID_ATTR;
    static const char* const SERVICE_NAME_ATTR;
    static const char* const ANNOUNCEMENT_TAG;
    static const char* const ANNOUNCEMENT_INTERFACE_DESCRIPTOR_ATTR;
};

} /* namespace mindroid */

#endif /* MINDROID_RUNTIME_SYSTEM_SERVICEDISCOVERYCONFIGURATIONREADER_H_ */
