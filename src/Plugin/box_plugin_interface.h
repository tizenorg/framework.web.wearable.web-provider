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
 * @file    box_plugin_interface.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef BOX_PLUGIN_INTERFACE_H
#define BOX_PLUGIN_INTERFACE_H

#include <string>
#include <Core/BoxData.h>

#ifndef EXPORT_API
#define EXPORT_API  __attribute__((visibility("default")))
#endif

#define WEB_PROVIDER_PLUGIN_INTERFACE_SYM_INITIALIZE "web_provider_plugin_interface_initialize"
#define WEB_PROVIDER_PLUGIN_INTERFACE_SYM_COMMAND    "web_provider_plugin_interface_command"
#define WEB_PROVIDER_PLUGIN_INTERFACE_SYM_SHUTDOWN   "web_provider_plugin_interface_shutdown"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    REQUEST_CMD_ADD_BOX,
    REQUEST_CMD_REMOVE_BOX,
    REQUEST_CMD_OPEN_GBAR,
    REQUEST_CMD_CLOSE_GBAR,
    REQUEST_CMD_RESIZE_BOX,
    REQUEST_CMD_RESUME_BOX,
    REQUEST_CMD_PAUSE_BOX,
    REQUEST_CMD_RESUME_ALL,
    REQUEST_CMD_PAUSE_ALL,
    REQUEST_CMD_CHANGE_PERIOD,
    REQUEST_CMD_UPDATE_BOX,
    REQUEST_CMD_UPDATE_ALL,
    REQUEST_CMD_UPDATE_APPBOX
} request_cmd_type;

// definition of interface function type
typedef int (*plugin_interface_func_initialize)(void);
typedef int (*plugin_interface_func_command)(
        const request_cmd_type type, const BoxInfoPtr& boxInfo);
typedef int (*plugin_interface_func_shutdown)(void);

typedef struct {
    void* handle;
    const char* service_boxid;
    plugin_interface_func_initialize initialize;
    plugin_interface_func_command command;
    plugin_interface_func_shutdown shutdown;
} plugin_interfaces;

// inteface functions that should be implemented by each plugin
EXPORT_API int web_provider_plugin_interface_initialize();
EXPORT_API int web_provider_plugin_interface_command(
        const request_cmd_type type, const BoxInfoPtr& boxInfo);
EXPORT_API int web_provider_plugin_interface_shutdown();

#ifdef __cplusplus
}
#endif

#endif // BOX_PLUGIN_INTERFACE_H
