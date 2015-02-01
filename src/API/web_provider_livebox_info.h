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
 * @file    web_provider_livebox_info.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef WEB_PROVIDER_LIVEBOX_INFO_H
#define WEB_PROVIDER_LIVEBOX_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EXPORT_API
#define EXPORT_API      __attribute__((visibility("default")))
#endif

#define DEPRECATED_API  __attribute__((visibility("default"))) __attribute__((deprecated))

/* TODO doxygen comments are needed to each exported API */

EXPORT_API const char* web_provider_livebox_get_default_type();
EXPORT_API char* web_provider_livebox_get_box_type(const char* box_id);
EXPORT_API char* web_provider_livebox_get_app_id(const char* box_id);
EXPORT_API int web_provider_livebox_get_auto_launch(const char* box_id);
EXPORT_API int web_provider_livebox_get_mouse_event(const char* box_id);
EXPORT_API bool web_provider_livebox_get_pd_fast_open(const char* box_id);
EXPORT_API int web_provider_livebox_insert_box_info(
        const char* box_id,
        const char* app_id,
        const char* box_type,
        int auto_launch,
        int mouse_event,
        int pd_fast_open);
EXPORT_API int web_provider_livebox_delete_by_box_id(const char* box_id);
EXPORT_API int web_provider_livebox_delete_by_app_id(const char* app_id);
EXPORT_API int web_provider_livebox_delete_by_type(const char* type);

EXPORT_API char** web_provider_livebox_get_box_id_list(const char* app_id, int* count);
EXPORT_API void web_provider_livebox_release_box_id_list(char** box_id_list, int count);
EXPORT_API bool web_provider_livebox_check_box_installed(const char* box_id);

#ifdef __cplusplus
}
#endif
#endif //WEB_PROVIDER_LIVEBOX_INFO_H
