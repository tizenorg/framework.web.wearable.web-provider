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
 * @file    web_provider_service.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef WEB_PROVIDER_SERVICE_H
#define WEB_PROVIDER_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EXPORT_API
#define EXPORT_API      __attribute__((visibility("default")))
#endif

// These are string for removing widget from wrt-installer
#define APP_CONTROL_OPERATION_BOX_REMOVE    "http://tizen.org/appcontrol/operation/dynamicbox/web/remove"
#define APP_CONTROL_WEB_PROVIDER_APPID      "widget.web-provider"
#define APP_CONTROL_REMOVE_APPID_KEY        "app_id"

// These are string for updating widget from web app
#define APP_CONTROL_OPERATION_BOX_UPDATE    "http://tizen.org/appcontrol/operation/dynamicbox/web/update"
#define APP_CONTROL_BOX_SERVICE_SCHEME      "box-service://"
#define APP_CONTROL_CONTENT_INFO_KEY        "content-info"
#define APP_CONTROL_ALARM_CALLER_KEY        "__ALARM_MGR_CALLER_APPID"

EXPORT_API int web_provider_service_wait_boxes_removed(const char* app_id);
EXPORT_API int web_provider_service_wakeup_installer(const char* app_id);

#ifdef __cplusplus
}
#endif
#endif //WEB_PROVIDER_SERVICE_H
