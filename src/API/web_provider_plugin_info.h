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
 * @file    web_provider_plugin_info.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef WEB_PROVIDER_PLUGIN_INFO_H
#define WEB_PROVIDER_PLUGIN_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EXPORT_API
#define EXPORT_API __attribute__((visibility("default")))
#endif

struct _web_provider_plugin_info {
    char* type;
    char* path;
    char* service_boxid;
    char** box_size;
    int box_scrollable;
    int box_size_count;
};
typedef _web_provider_plugin_info web_provider_plugin_info;

EXPORT_API web_provider_plugin_info** web_provider_plugin_get_installed_list(
                int* count);
EXPORT_API void web_provider_plugin_release_installed_list(
                web_provider_plugin_info** info_list,
                int count);
EXPORT_API int web_provider_plugin_get_box_scrollable(const char* plugin_type);
EXPORT_API bool web_provider_plugin_check_supported_size(const char* plugin_type, char** size, int sizeCount);
#ifdef __cplusplus
}
#endif
#endif //WEB_PROVIDER_PROVIDER_INFO_H
