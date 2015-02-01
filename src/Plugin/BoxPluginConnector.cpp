/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Flora License, Version 1.1 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://floralicense.org/license/
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/**
 * @file    BoxPluginConnector.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include <map>
#include <dlfcn.h>
#include <Core/Util/Log.h>
#include <Core/BoxData.h>
#include <API/web_provider_plugin_info.h>
#include "box_plugin_interface.h"
#include "BoxPluginConnector.h"

BoxPluginConnector::BoxPluginConnector()
{
}

BoxPluginConnector::~BoxPluginConnector()
{
}

bool BoxPluginConnector::initialize()
{
    LogD("enter");

    int count;
    web_provider_plugin_info** pluginList = NULL;
    pluginList = web_provider_plugin_get_installed_list(&count);

    if (!pluginList) {
        LogD("failed to get installed plugin's information");
        return false;
    }

    if (count <= 0) {
        LogD("There is no available livebox plugins");
        return false;
    }

    m_pluginMap.clear();

    // get information of installed plugin
    LogD("get information of installed plugin");
    for (int i = 0; i < count; i++) {
        if (!pluginList[i]) {
            continue;
        }

        LogD("plugin path: %s", pluginList[i]->path);
        void* handle = dlopen(pluginList[i]->path, RTLD_LAZY);
        if (!handle) {
            LogD("failed to load plugin so: %s", dlerror());
            continue;
        }

        std::shared_ptr<plugin_interfaces> pluginInfo(new plugin_interfaces);
        
        pluginInfo->handle = handle;
        pluginInfo->service_boxid = NULL;
        if (pluginList[i]->service_boxid) {
            pluginInfo->service_boxid = strdup(pluginList[i]->service_boxid);
        }

        pluginInfo->initialize = 
            reinterpret_cast<plugin_interface_func_initialize>(
                    dlsym(handle, WEB_PROVIDER_PLUGIN_INTERFACE_SYM_INITIALIZE));
        pluginInfo->command = 
            reinterpret_cast<plugin_interface_func_command>(
                    dlsym(handle, WEB_PROVIDER_PLUGIN_INTERFACE_SYM_COMMAND));
        pluginInfo->shutdown = 
            reinterpret_cast<plugin_interface_func_shutdown>(
                dlsym(handle, WEB_PROVIDER_PLUGIN_INTERFACE_SYM_SHUTDOWN));

        if (!pluginInfo->initialize || !pluginInfo->command ||
                !pluginInfo->shutdown) 
        {
            LogD("symbol for plugin interface is not found");
            continue;
        }
        
        m_pluginMap[std::string(pluginList[i]->type)] = pluginInfo;
    }

    // initialize plugins 
    for (auto it = m_pluginMap.begin(); 
            it != m_pluginMap.end(); ++it) 
    {
        if (it->second) {
            // TODO add exception or abnormal action on loading plugin
            if (it->second->initialize() < 0) {
                LogD("fail to intialize plugin");
                continue;
            }
        }
    }

    // release information
    LogD("release json data of plugins"); 
    web_provider_plugin_release_installed_list(pluginList, count);

    return true;
}

bool BoxPluginConnector::shutdown()
{
    LogD("enter");
    // if needed, unload each plugin's DSO.
    for (auto it = m_pluginMap.begin(); 
            it != m_pluginMap.end(); ++it) 
    {
        if (it->second) {
            it->second->shutdown();
            dlclose(it->second->handle);
        }
    }

    return true;
}

bool BoxPluginConnector::requestCommand(
        const request_cmd_type type, const BoxInfoPtr& boxInfo)
{
    LogD("enter");

    // in case of request of resume all or pause all, all plugins should handle that.
    if (type == REQUEST_CMD_RESUME_ALL ||
        type == REQUEST_CMD_PAUSE_ALL ||
        type == REQUEST_CMD_UPDATE_ALL ||
        type == REQUEST_CMD_UPDATE_APPBOX) {
        for (auto it = m_pluginMap.begin(); 
                it != m_pluginMap.end(); ++it) 
        {
            if (it->second) {
                it->second->command(type, boxInfo);
            }
        }
        return true;
    }

    const std::shared_ptr<plugin_interfaces> plugin = m_pluginMap[boxInfo->boxType];
    if (!plugin) {
        LogD("not available livebox type");
        return false;
    }

    int ret = plugin->command(type, boxInfo); 
    if (ret < 0) {
        LogD("failed to request command");
        return false;
    }

    return true;
}

std::string BoxPluginConnector::getBoxType(const std::string& serviceBoxId)
{
    LogD("enter");

    std::string type; 
    for (auto it = m_pluginMap.begin(); 
            it != m_pluginMap.end(); ++it) 
    {
        if (it->second && it->second->service_boxid) {
            if (serviceBoxId == it->second->service_boxid) {
                LogD("service box id is matched!: %s", it->first.c_str());
                type = it->first;
                break;
            }
        }
    }

    return type;
}
