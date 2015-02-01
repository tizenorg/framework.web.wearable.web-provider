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
 * @file    main.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/capability.h>
#include <Elementary.h>
#include <aul.h>
#include <app.h>
#include <Core/Util/Log.h>
#include <Core/Util/Util.h>
#include "BoxDaemon.h"


// declarations
static BoxDaemon boxDaemon;
static bool isDaemonReleased = false;

static void atExitCallback()
{
    LogD("enter");
    if (isDaemonReleased) {
        return;
    }

    // this callback is called due to abnormal exit()
    LogD("release daemon resources explicitly");
    boxDaemon.stop();
}

static bool appCreateCallback(void *data)
{
    LogD("app create");
    UNUSED_PARAM(data);

    elm_config_preferred_engine_set("software_x11");
    return true;
}

static void appTerminateCallback(void *data)
{
    BoxDaemon *boxDaemon = static_cast<BoxDaemon *>(data);
    boxDaemon->stop();
    isDaemonReleased = true;
}

static void appPauseCallback(void *data)
{
    LogD("app pasue");
    UNUSED_PARAM(data);
}

static void appResumeCallback(void *data)
{
    LogD("app resume");
    UNUSED_PARAM(data);
}

static void appControlCallback(app_control_h app_control, void *data)
{
    LogD("app control");

    int ret;
    char* name = NULL;

    BoxDaemon *boxDaemon = static_cast<BoxDaemon*>(data);
    ret = app_control_get_extra_data(app_control, "name", &name);

    // check if web-provider is launched, or not
    if (ret == APP_CONTROL_ERROR_NONE) {
        std::string daemonName(name);
        if(!(boxDaemon->start(daemonName))) {
            LogD("daemon failed to start");
            elm_exit();
        }
        atexit(atExitCallback);
        free(name);
        return;
    }

    boxDaemon->handleAppControl(app_control);
    free(name);
}

static bool grantProcessCapability()
{
    cap_user_header_t header;
    cap_user_data_t data;

    header = static_cast<cap_user_header_t>(malloc(sizeof(*header)));
    data = static_cast<cap_user_data_t>(calloc(sizeof(*data), _LINUX_CAPABILITY_U32S_3));

    header->pid = getpid();
    header->version = _LINUX_CAPABILITY_VERSION_3;

    // read already granted capabilities of this process
    if (capget(header, data) < 0) {
        LogD("capget error");
        free(header);
        free(data);
        return false;
    }

    // set only inheritable bit for CAP_MAC_ADMIN to '1'
    data[CAP_TO_INDEX(CAP_MAC_ADMIN)].inheritable |= CAP_TO_MASK(CAP_MAC_ADMIN);

    // remove capabilities not needed any more
    data[CAP_TO_INDEX(CAP_MAC_ADMIN)].permitted &= ~CAP_TO_MASK(CAP_MAC_ADMIN);
    data[CAP_TO_INDEX(CAP_MAC_ADMIN)].effective &= ~CAP_TO_MASK(CAP_MAC_ADMIN);
    data[CAP_TO_INDEX(CAP_SETPCAP)].permitted &= ~CAP_TO_MASK(CAP_SETPCAP);
    data[CAP_TO_INDEX(CAP_SETPCAP)].effective &= ~CAP_TO_MASK(CAP_SETPCAP);

    bool ret = true;
    if (capset(header, data) < 0) {
        LogD("capset error");
        ret = false;
    }

    free(header);
    free(data);

    return ret;
}

int main (int argc, char *argv[])
{
    // set inheritable bit for CAP_MAC_ADMIN
    // so that WebProcess will have CAP_MAC_ADMIN capability
    if (!grantProcessCapability()) {
        return -1;
    }

    // set the appcore callbacks
    app_event_callback_s ops;
    memset(&ops, 0x00, sizeof(app_event_callback_s));
    ops.create = appCreateCallback;
    ops.terminate = appTerminateCallback;
    ops.pause = appPauseCallback;
    ops.resume = appResumeCallback;
    ops.app_control = appControlCallback;

    // start appcore
    int ret = app_efl_main(&argc, &argv, &ops, &boxDaemon);
    return ret;
}
