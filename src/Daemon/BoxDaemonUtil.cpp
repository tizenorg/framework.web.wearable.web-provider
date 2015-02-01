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
 * @file    BoxDaemonUtil.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include <sstream>
#include <string>
#include <memory>
#include <time.h>
#include <app_control.h>
#include <Core/Util/Log.h>
#include <API/web_provider_livebox_info.h>
#include "BoxDaemonUtil.h"

const std::string BoxDaemonUtil::boxIdKey("box-id");
const std::string BoxDaemonUtil::instanceIdKey("instance-id");

bool BoxDaemonUtil::launchApplication(std::string& boxId, std::string& instanceId)
{
    LogD("enter");

    char* appId = web_provider_livebox_get_app_id(boxId.c_str());
    if (!appId) {
        LogD("no appid of %s", boxId.c_str());
        return false;
    }

    app_control_h handle = NULL;
    int ret = APP_CONTROL_ERROR_NONE;

    ret = app_control_create(&handle);
    if (ret != APP_CONTROL_ERROR_NONE && !handle) {
        LogD("failed to create app_control");
        free(appId);
        return false;
    }

    ret = app_control_set_app_id(handle, appId);
    free(appId);
    if (ret != APP_CONTROL_ERROR_NONE) {
        LogD("failed to set package");
        app_control_destroy(handle);
        return false;
    }

    app_control_add_extra_data(handle, boxIdKey.c_str(), boxId.c_str());
    app_control_add_extra_data(handle, instanceIdKey.c_str(), instanceId.c_str());

    ret = app_control_send_launch_request(handle, NULL, NULL);
    if (ret != APP_CONTROL_ERROR_NONE) {
        LogD("failed to launch package");
        app_control_destroy(handle);
        return false;
    }

    app_control_destroy(handle);
    LogD("success to launch app of %s", boxId.c_str());

    return true;
}

std::string BoxDaemonUtil::createWebInstanceId(std::string& boxId)
{
    LogD("enter");
    std::ostringstream webInstanceId;

    webInstanceId << boxId;

    struct timespec timeSpec;
    if (clock_gettime(CLOCK_REALTIME, &timeSpec) < 0) {
        return std::string();
    }

    webInstanceId << "." << timeSpec.tv_sec << timeSpec.tv_nsec;
    LogD("Web instance id: %s", webInstanceId.str().c_str());
    return webInstanceId.str();
}
