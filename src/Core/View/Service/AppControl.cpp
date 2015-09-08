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
 * @file    LaunchBrowser.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */

#include <string>
#include <app_control.h>
#include <Core/Util/Log.h>
#include "AppControl.h"

namespace Service {
namespace AppControl {

bool launchBrowser(std::string& url)
{
    LogD("enter");

    app_control_h handle = NULL;
    int ret = APP_CONTROL_ERROR_NONE;

    ret = app_control_create(&handle);
    if (ret != APP_CONTROL_ERROR_NONE) {
        LogD("failed to create app_control");
        return false;
    }

    ret = app_control_set_operation(handle, APP_CONTROL_OPERATION_VIEW);
    if (ret != APP_CONTROL_ERROR_NONE) {
        LogD("failed to set operation");
        app_control_destroy(handle);
        return false;
    }

    ret = app_control_set_uri(handle, url.c_str());
    if (ret != APP_CONTROL_ERROR_NONE) {
        LogD("failed to set url");
        app_control_destroy(handle);
        return false;
    }

    ret = app_control_send_launch_request(handle, NULL, NULL);
    if (ret != APP_CONTROL_ERROR_NONE) {
        LogD("failed to request launch");
        app_control_destroy(handle);
        return false;
    }

    LogD("success to launch browser: %s", url.c_str());
    app_control_destroy(handle);

    return true;
}

bool launchDownloader(std::string& url, std::string& cookie)
{
    LogD("enter");

    app_control_h handle = NULL;
    int ret = APP_CONTROL_ERROR_NONE;

    if (url.empty()) {
        LogD("invalid arguments");
        return false;
    }

    ret = app_control_create(&handle);
    if (ret != APP_CONTROL_ERROR_NONE) {
        LogD("failed to create app_control");
        return false;
    }

    ret = app_control_set_operation(handle, APP_CONTROL_OPERATION_DOWNLOAD);
    if (ret != APP_CONTROL_ERROR_NONE) {
        LogD("failed to set operation");
        app_control_destroy(handle);
        return false;
    }

    ret = app_control_set_uri(handle, url.c_str());
    if (ret != APP_CONTROL_ERROR_NONE) {
        LogD("failed to set url");
        app_control_destroy(handle);
        return false;
    }

    if (!cookie.empty()) {
        ret = app_control_add_extra_data(handle, "cookie", cookie.c_str());
        if (ret != APP_CONTROL_ERROR_NONE) {
            LogD("failed to set cookie");
            app_control_destroy(handle);
            return false;
        }
    }

    ret = app_control_send_launch_request(handle, NULL, NULL);
    if (ret != APP_CONTROL_ERROR_NONE) {
        LogD("failed to request launch");
        app_control_destroy(handle);
        return false;
    }

    LogD("success to launch downloader");
    app_control_destroy(handle);

    return true;
}

} // AppControl
} // Service
