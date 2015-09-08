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
 * @file    web_provider_service.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */

#include <fcntl.h>
#include <string>
#include <sstream>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/smack.h>
#include <string.h>
#include <unistd.h>
#include <app_control.h>

#include "web_provider_widget_info.h"
#include "web_provider_service.h"

static std::string getFifoFile(const char* appId)
{
    int boxCount = 0;
    char** boxList = web_provider_widget_get_box_id_list(appId, &boxCount);
    if (!boxList) {
        return std::string();
    }
    web_provider_widget_release_box_id_list(boxList, boxCount);

    std::string fifoFile = "/tmp/";
    fifoFile += appId;

    return fifoFile;
}

static bool requestRemoveBoxesByAppId(const char* appId)
{
    if (!appId) {
        return false;
    }

    app_control_h handle = NULL;
    int ret = APP_CONTROL_ERROR_NONE;

    ret = app_control_create(&handle);
    if (ret != APP_CONTROL_ERROR_NONE && !handle) {
        return false;
    }

    ret = app_control_set_app_id(handle, APP_CONTROL_WEB_PROVIDER_APPID);
    if (ret != APP_CONTROL_ERROR_NONE) {
        app_control_destroy(handle);
        return false;
    }

    ret = app_control_set_operation(handle, APP_CONTROL_OPERATION_BOX_REMOVE);
    if (ret != APP_CONTROL_ERROR_NONE) {
        app_control_destroy(handle);
        return false;
    }

    // set special key as extra data for removing boxes by web-provider
    app_control_add_extra_data(handle, APP_CONTROL_REMOVE_APPID_KEY, appId);

    ret = app_control_send_launch_request(handle, NULL, NULL);
    if (ret != APP_CONTROL_ERROR_NONE) {
        app_control_destroy(handle);
        return false;
    }

    app_control_destroy(handle);
    return true;
}

int web_provider_service_wait_boxes_removed(const char* app_id)
{
    if (!app_id) {
       return -1;
    }

    // 1. request service for removing boxes to web-provider
    // 2. wait using epoll_wait while fifo file is changed
    if(!requestRemoveBoxesByAppId(app_id)) {
        return -1;
    }

    // create fifo file
    std::string fifoFile = getFifoFile(app_id);
    if( fifoFile.empty() )
        return -1;
    int ret = mkfifo(fifoFile.c_str(), 0666);
    if (ret < 0) {
        if (errno != EEXIST) {
            return -1;
        }
    }
    if (chown(fifoFile.c_str(), 5000, 5000) < 0) {
       return -1;
    }

    ret = smack_setlabel(fifoFile.c_str(), "*", SMACK_LABEL_ACCESS);
    if (ret < 0) {
        return -1;
    }

    int epollMaxSize = 1;
    int epollFd = epoll_create(epollMaxSize);
    int fifoFd = open(fifoFile.c_str(), O_RDONLY | O_NONBLOCK);
    if (fifoFd < 0) {
        return -1;
    }

    struct epoll_event fifoEvent;
    fifoEvent.events = EPOLLIN | EPOLLOUT;
    fifoEvent.data.fd = fifoFd;

    struct epoll_event events;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fifoFd, &fifoEvent) < 0) {
        close(fifoFd);
        return -1;
    }

    int epollTimeOut = 10000; // 10 seconds
    int status = epoll_wait(epollFd, &events, epollMaxSize, epollTimeOut);
    // epoll is unblocked!

    int result = 0;
    if (status < 0) {
        // epoll failed
    } else if (status == 0) {
        // epoll timeout
    } else {
        int buffMaxSize = 3;
        char buff[buffMaxSize];
        memset(buff, 0x0, buffMaxSize);
        int size = read(fifoFd, buff, buffMaxSize);
        if (size > 0) {
            std::string status(buff);
            std::istringstream inputStream(status);
            inputStream >> result;
        }
    }

    close(fifoFd);
    close(epollFd);
    unlink(fifoFile.c_str());
    return result;
}

int web_provider_service_wakeup_installer(const char* app_id)
{
    if (!app_id) {
       return -1;
    }

    std::string fifoFile = getFifoFile(app_id);
    if(fifoFile.empty())
        return -1;
    int fifoFd = open(fifoFile.c_str(), O_WRONLY | O_NONBLOCK);
    if (fifoFd < 0) {
        return -1;
    }

    std::string result("1");
    if (write(fifoFd, result.c_str(), result.size() + 1) < 0) {
        close(fifoFd);
        return -1;
    }
    close(fifoFd);
    return 0;
}
