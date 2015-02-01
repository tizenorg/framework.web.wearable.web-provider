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
 * @file    web_provider_livebox_info.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */


#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <list>
#include <glib.h>
#include <glib-object.h>
#include <json-glib/json-glib.h>
#include <Core/Util/Log.h>
#include "web_provider_livebox_info.h"
#include "web_provider_plugin_info.h"
#include <memory>

// static functions
static web_provider_plugin_info* get_parsed_json_data(const std::string& configPath);
static bool web_provider_plugin_release_info(web_provider_plugin_info* info);

namespace{
    static const std::string kInstalledPluginDirPath("/usr/lib/web-provider/");
}
// Json's content for plugin is the following (example)
// {
//      "type" : "clip",
//      "path" : "/usr/lib/web-provider/libweb-provider-plugin-clipbox.so",
//      "service_boxid" : "org.tizen.browser"
// }
//
// "service_boxid" is only optional member in json file

namespace{
static const std::string kJsonMemberType("type");
static const std::string kJsonMemberPath("path");
static const std::string kJsonMemberBoxId("service_boxid");
static const std::string kJsonMemberBoxScrollable("box_scrollable");
static const std::string kJsonMemberBoxSize("supported_size");

static const std::string kJsonValueBoolTrue("true");
static const std::string kJsonValueBoolFalse("false");
static const std::string kJsonFileExtension(".json");
static const std::string kMandatoryBoxSize("1x1");
}
web_provider_plugin_info** web_provider_plugin_get_installed_list(int* count)
{
    LogD("enter");

    // open directory of web provider plugin
    DIR* dir = opendir(kInstalledPluginDirPath.c_str());
    if (!dir) {
        LogD("failed to open directory for web livebox plugins");
        *count = 0;
        return NULL;
    }

    // read plugin directory and store plugin config path
    std::list<std::string> configList;
    struct dirent* entry;

    while ((entry = readdir(dir))) {
        if ((!strcmp(entry->d_name, ".")) || (!strcmp(entry->d_name, ".."))) {
            continue;
        }

        std::string configPath = kInstalledPluginDirPath + entry->d_name;
        struct stat configStat;
        if (stat(configPath.c_str(), &configStat) < 0) {
            LogD("Failed to open file");
            continue;
        }

        if (S_ISDIR(configStat.st_mode)) {
            continue;
        }

        if (configPath.substr(configPath.find_last_of(".")) == kJsonFileExtension) {
            LogD("config file: %s", configPath.c_str());
            configList.push_back(configPath);
        }
    }
    // close directory of web provider plugin
    closedir(dir);

    if (configList.size() == 0) {
        *count = 0;
        return NULL;
    }

    // parse available each plugin json file
    std::list<web_provider_plugin_info*> pluginList;
    for (auto it = configList.begin();
            it != configList.end(); it++) {
        web_provider_plugin_info* info = get_parsed_json_data(*it) ;
        if (!info) {
            continue;
        }

        pluginList.push_back(info);
    }
    *count = pluginList.size();
    LogD("read plugin count: %d", *count);

    // c style array allocation for return of result
    web_provider_plugin_info** info_list =
        static_cast<web_provider_plugin_info**>(
                malloc((*count) * sizeof(web_provider_plugin_info*)));

    // copy from members in std::list to one in c style array
    int idx = 0;
    for (auto it = pluginList.begin();
            it != pluginList.end(); it++) {
        LogD("type: %s", (*it)->type);
        LogD("path: %s", (*it)->path);
        if ((*it)->service_boxid) {
            LogD("service_boxid: %s", (*it)->service_boxid);
        }
        info_list[idx] = *it;
        idx++;
    }

    LogD("success to return plugin information");
    return info_list;
}

void web_provider_plugin_release_installed_list(
        web_provider_plugin_info** info_list,
        int count)
{
    if (!info_list) {
        return;
    }

    for (int i = 0; i < count; i++) {
        web_provider_plugin_release_info(info_list[i]);
    }
    free(info_list);
}

int web_provider_plugin_get_box_scrollable(const char* plugin_type)
{
    if (!plugin_type) {
        return -1;
    }

    std::string configPath;
    configPath = kInstalledPluginDirPath;
    configPath += plugin_type;
    configPath += kJsonFileExtension;
    web_provider_plugin_info* info = get_parsed_json_data(configPath);

    if (!info) {
        return -1;
    }
    int ret = info->box_scrollable;
    web_provider_plugin_release_info(info);

    LogD("box_scrollable: %d", ret);
    return ret;
}

static web_provider_plugin_info* get_parsed_json_data(const std::string& configPath)
{
#if !GLIB_CHECK_VERSION(2, 35, 0)
	g_type_init();
#endif

    web_provider_plugin_info* info;
    JsonParser* parser = json_parser_new();
    GError* error = NULL;

    if (!json_parser_load_from_file(parser, configPath.c_str(), &error)) {
        LogD("failed to parse json file: %s -> %s", configPath.c_str(), error->message);
        g_error_free(error);
        g_object_unref(parser);
        return NULL;
    }

    JsonNode* root = json_parser_get_root(parser);
    JsonObject* object = json_node_get_object(root);

    // check if type member exists on this json file
    const char* type =
        static_cast<const char*>(
            json_object_get_string_member(object, kJsonMemberType.c_str()));

    const char* path =
        static_cast<const char*>(
            json_object_get_string_member(object, kJsonMemberPath.c_str()));

    JsonArray* size =
        json_object_get_array_member(object, kJsonMemberBoxSize.c_str());
    int sizeCount = static_cast<int>(json_array_get_length(size));

    if (!type || !path || !sizeCount) {
        LogD("mandatory members don't exist");
        g_error_free(error);
        g_object_unref(parser);
        return NULL;
    }

    // allocate instance of plugin info struct
    info = static_cast<web_provider_plugin_info*>(
            malloc(sizeof(web_provider_plugin_info)));
    memset(info, 0, sizeof(web_provider_plugin_info));

    info->type = strdup(type);
    info->path = strdup(path);
    info->box_size = static_cast<char**>(malloc(sizeof(char*) * sizeCount));

    for (int i = 0; i < sizeCount; i++) {
        info->box_size[i] =
            strdup(static_cast<const char*>(json_array_get_string_element(size, i)));
    }
    info->box_size_count = sizeCount;

    gboolean hasBoxId = json_object_has_member(object, kJsonMemberBoxId.c_str());
    if (hasBoxId == TRUE) {
        const char* boxId =
            static_cast<const char*>(
                json_object_get_string_member(object, kJsonMemberBoxId.c_str()));
        if (boxId) {
            info->service_boxid = strdup(boxId);
        }
    }

    gboolean hasBoxScrollable =
        json_object_has_member(object, kJsonMemberBoxScrollable.c_str());
    if (hasBoxScrollable == TRUE) {
        const char* boxScrollable =
            static_cast<const char*>(
                json_object_get_string_member(object, kJsonMemberBoxScrollable.c_str()));
        if (boxScrollable && (kJsonValueBoolTrue == boxScrollable)) {
            info->box_scrollable = 1;
        } else {
            info->box_scrollable = 0;
        }
    }

    LogD("type: %s", info->type);
    LogD("path: %s", info->path);
    if (info->service_boxid) {
        LogD("service_boxid: %s", info->service_boxid);
    }
    LogD("box_scrollable: %d", info->box_scrollable);

    g_error_free(error);
    g_object_unref(parser);

    return info;
}

bool web_provider_plugin_release_info(web_provider_plugin_info* info)
{
    LogD("enter");
    if (!info) {
        LogD("empty struct");
        return false;
    }

    // only members with buffer are released
    free(info->type);
    free(info->path);
    free(info->service_boxid);
    for(int i = 0; i < info->box_size_count; i++) {
        free(info->box_size[i]);
    }
    free(info->box_size);
    free(info);

    return true;
}

bool web_provider_plugin_check_supported_size(
    const char* plugin_type, char** size, int sizeCount)
{
    // read plugin directory and store plugin config path
    std::string configPath;
    configPath = kInstalledPluginDirPath;
    configPath += plugin_type;
    configPath += kJsonFileExtension;

    // get the json datas
    web_provider_plugin_info* jsonData = get_parsed_json_data(configPath);
    if (!jsonData) {
        LogD("failed to get the json file");
        return false;
    }

    // check if this type is default type
    bool isDefaultType = false;
    const char* defaultType = web_provider_livebox_get_default_type();
    if (!defaultType) {
        LogD("can't get default type");
        web_provider_plugin_release_info(jsonData);
        return false;
    }
    if (!strcmp(plugin_type, defaultType)) {
        isDefaultType = true;
    }

    // compare the parsed config data with the parsed json data
    bool mandatoryCheck = false;
    for (int configCnt = 0; configCnt < sizeCount; configCnt++) {
        bool supportedSizeCheck = false;
        for (int jsonCnt = 0; jsonCnt < jsonData->box_size_count; jsonCnt++) {

            // check mandatory size
            if (isDefaultType && !strcmp(kMandatoryBoxSize.c_str(), size[configCnt])) {
                mandatoryCheck = true;
            }

            // check supported size
            if (!strcmp(jsonData->box_size[jsonCnt], size[configCnt])) {
                supportedSizeCheck = true;
                break;
            }
        }

        if (!supportedSizeCheck) {
            LogD("Not supported size: %s", size[configCnt]);
            web_provider_plugin_release_info(jsonData);
            return false;
        }
    }

    //release the jsonData
    web_provider_plugin_release_info(jsonData);
    if (isDefaultType && !mandatoryCheck) {
        LogD("Mandatory members don't exist ");
        return false;
    }

    return true;
}
