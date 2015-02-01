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
 * @file    BoxDaemonImpl.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include <cstring>
#include <sys/types.h>
#include <string>
#include <unistd.h>
#include <vector>

#include <app.h>
#include <appcore-efl.h>
#include <aul.h>
#include <Ecore_X.h>
#include <Elementary.h>
#include <dynamicbox_service.h>
#include <provider.h>
#include <vconf.h>

#include <API/web_provider_livebox_info.h>
#include <API/web_provider_service.h>
#include <Core/Util/Log.h>
#include <Core/Util/Util.h>
#include <Plugin/IBoxPluginConnector.h>
#include <Plugin/BoxPluginConnector.h>

#include "BoxDaemonUtil.h"
#include "BoxDaemonImpl.h"

#define MASTER_PROVIDER_PING_TIME  120.0f

BoxDaemonImpl::BoxDaemonImpl()
    : m_pluginConnector(BoxPluginConnector::create())
{
    appcore_set_event_callback(APPCORE_EVENT_LANG_CHANGE, requestUpdateLangCallback, this);
    appcore_set_event_callback(APPCORE_EVENT_LOW_MEMORY, requestLowMemoryCallback, this);
    appcore_set_event_callback(APPCORE_EVENT_REGION_CHANGE, requestUpdateLangCallback, this);
    // reserved
    appcore_set_event_callback(APPCORE_EVENT_LOW_BATTERY, NULL, NULL);

    if (vconf_notify_key_changed("db/setting/accessibility/font_name", requestUpdateAllCallback, this) != 0) {
        LogD("vconf_notify_key_changed returned FALSE!");
    }
    if (vconf_notify_key_changed(VCONFKEY_SYSTEM_TIME_CHANGED, requestUpdateAppBoxCallback,  this) != 0) {
        LogD("vconf_notify_key_changed returned FALSE!");
    }
    if (vconf_notify_key_changed(VCONFKEY_NETWORK_WIFI_STATE, requestUpdateAppBoxCallback,  this) != 0) {
        LogD("vconf_notify_key_changed returned FALSE!");
    }
    if (vconf_notify_key_changed(VCONFKEY_NETWORK_CELLULAR_STATE, requestUpdateAppBoxCallback,  this) != 0) {
        LogD("vconf_notify_key_changed returned FALSE!");
    }
}

BoxDaemonImpl::~BoxDaemonImpl()
{
}

bool BoxDaemonImpl::start(std::string& name)
{
    LogD("enter");

    // initialize existing plugins for web livebox
    if (!m_pluginConnector->initialize()) {
        LogD("failed to initialize plugins");
        return false;
    }

    // set name
    m_daemonName = name;

    // provider init
    ProviderCallbacks callbacks;
    setProviderCallbacks(callbacks);

    int ret = provider_init_with_options(ecore_x_display_get(),
                            m_daemonName.c_str(),
                            &callbacks,
                            this, 1, 1);
    if (ret < 0) {
        LogD("failed to initialize provider");
        return false;
    }

    return true;
}

bool BoxDaemonImpl::stop()
{
    LogD("enter");

    // deinitialize existing plugins for web livebox
    if (!m_pluginConnector->shutdown()) {
        LogD("failed to shutdown plugins");
        return false;
    }

    // deinitialize provider
    provider_fini();

    return true;
}

bool BoxDaemonImpl::handleAppControl(app_control_h app_control)
{
    LogD("enter");
    bool result = false;
    char* operation = NULL;

    // app_control_get_operation allocates memory using malloc() to operation
    if (app_control_get_operation(app_control, &operation) != APP_CONTROL_ERROR_NONE) {
        LogD("App Control error");
        return false;
    }

    // checks for scenario when APP_CONTROL_ERROR_NONE returned but operation is NULL
    if (!operation) {
        LogD("no operation");
        return false;
    }

    if (!strcmp(APP_CONTROL_OPERATION_BOX_UPDATE, operation)) {
        BoxInfoPtr info = handleOperationUpdate(app_control);
        if (info) {
            JobInfo* jobInfo = new JobInfo(REQUEST_CMD_UPDATE_BOX, info, this);
            if (ecore_job_add(requestBoxJobCallback, jobInfo)) {
                result = true;
            }
        }
    } else if (!strcmp(APP_CONTROL_OPERATION_BOX_REMOVE, operation)) {
        std::vector<BoxInfoPtr> container = handleOperationRemove(app_control);
        for (unsigned i = 0; i < container.size(); i++) {
            // directly remove box not using ecore job,
            // because removing box is timing-senstive job
            m_pluginConnector->requestCommand(REQUEST_CMD_REMOVE_BOX, container[i]);
        }

        // wake up pended process like installer
        char* appId = NULL;
        app_control_get_extra_data(app_control, APP_CONTROL_REMOVE_APPID_KEY, &appId);
        web_provider_service_wakeup_installer(appId);
        result = true;
        free(appId);
    } else {
        LogD("unknown operation: %s", operation);
    }

    free(operation);
    operation = NULL;

    return result;
}

int BoxDaemonImpl::connectedCallback(ProviderEventArgPtr arg, void* data)
{
    LogD("enter");
    UNUSED_PARAM(arg);

    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    if (!provider_send_hello()) {
        This->m_pingTimer =
            ecore_timer_add(MASTER_PROVIDER_PING_TIME, pingToMasterCallback, This);
    }

    return 0;
}

int BoxDaemonImpl::disconnectedCallback(ProviderEventArgPtr arg, void* data)
{
    LogD("enter");
    UNUSED_PARAM(arg);
    UNUSED_PARAM(data);

    return 0;
}

int BoxDaemonImpl::boxCreateCallback(
        ProviderEventArgPtr arg,
        int* width, int* height,
        double* priority, void* data)
{
    LogD("enter");
    UNUSED_PARAM(priority);

    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    if( This == NULL )
        return -1;

    BoxInfoPtr info = This->initializeBoxInfo(arg);
    if (!info) {
        return -1;
    }

    if ((arg->info.lb_create.width == 0) || (arg->info.lb_create.height == 0)) {
        dynamicbox_service_get_size(DBOX_SIZE_TYPE_1x1, width, height);
    } else {
        *width = arg->info.lb_create.width;
        *height = arg->info.lb_create.height;
    }

    info->boxWidth = *width;
    info->boxHeight = *height;
    info->priority = 1.0f;
    info->period = arg->info.lb_create.period;
    if (arg->info.lb_create.content && strlen(arg->info.lb_create.content)) {
        info->contentInfo = std::string(arg->info.lb_create.content);
    } else {
        // we generate our instance id
        info->contentInfo = BoxDaemonUtil::createWebInstanceId(info->boxId);
    }

    // box information
    LogD("--------------------------------------------");
    LogD("boxId: %s", info->boxId.c_str());
    LogD("InstanceId: %s", info->instanceId.c_str());
    LogD("width: %d", info->boxWidth);
    LogD("height: %d", info->boxHeight);
    LogD("update period: %f", info->period);
    LogD("content info: %s", info->contentInfo.c_str());
    LogD("--------------------------------------------");

    bool ret = This->m_pluginConnector->requestCommand(REQUEST_CMD_ADD_BOX, info);
    return ret ? 0 : -1;
}

int BoxDaemonImpl::boxReCreateCallback(ProviderEventArgPtr arg, void* data)
{
    LogD("enter");

    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    if( This == NULL )
        return -1;

    BoxInfoPtr info = This->initializeBoxInfo(arg);
    if (!info) {
        return -1;
    }

    if ((arg->info.lb_recreate.width == 0) || (arg->info.lb_recreate.height == 0)) {
        dynamicbox_service_get_size(DBOX_SIZE_TYPE_1x1,
                                 &arg->info.lb_recreate.width,
                                 &arg->info.lb_recreate.height);
    }

    info->boxWidth = arg->info.lb_recreate.width;
    info->boxHeight = arg->info.lb_recreate.height;
    info->priority = 1.0f;
    info->period = arg->info.lb_recreate.period;
    if (arg->info.lb_recreate.content) {
        info->contentInfo = std::string(arg->info.lb_recreate.content);
    }

    // box information
    LogD("--------------------------------------------");
    LogD("boxId: %s", info->boxId.c_str());
    LogD("InstanceId: %s", info->instanceId.c_str());
    LogD("width: %d", info->boxWidth);
    LogD("height: %d", info->boxHeight);
    LogD("update period: %f", info->period);
    LogD("content info: %s", info->contentInfo.c_str());
    LogD("--------------------------------------------");

    bool ret = This->m_pluginConnector->requestCommand(REQUEST_CMD_ADD_BOX, info);
    return ret ? 0 : -1;
}

int BoxDaemonImpl::boxDestroyCallback(ProviderEventArgPtr arg, void* data)
{
    LogD("enter");

    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    if( This == NULL )
        return -1;

    BoxInfoPtr info = This->initializeBoxInfo(arg);
    if (!info) {
        return -1;
    }

    LogD("--------------------------------------------");
    LogD("boxId: %s", info->boxId.c_str());
    LogD("InstanceId: %s", info->instanceId.c_str());
    LogD("--------------------------------------------");

    This->m_pluginConnector->requestCommand(REQUEST_CMD_REMOVE_BOX, info);

    return 0;
}

int BoxDaemonImpl::pdCreateCallback(ProviderEventArgPtr arg, void* data)
{
    LogD("enter");

    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    if( This == NULL )
        return -1;

    BoxInfoPtr info = This->initializeBoxInfo(arg);
    if (!info) {
        return -1;
    }
    if (arg->info.pd_create.w == 0 || arg->info.pd_create.h == 0) {
        return -1;
    }

    //Use the screen width to fix the device width
    ecore_x_window_size_get(0, &info->pdWidth, NULL);
    info->pdHeight = arg->info.pd_create.h;
    info->pdX = arg->info.pd_create.x;
    info->pdY = arg->info.pd_create.y;

    LogD("--------------------------------------------");
    LogD("boxId: %s", info->boxId.c_str());
    LogD("InstanceId: %s", info->instanceId.c_str());
    LogD("width: %d", info->pdWidth);
    LogD("height: %d", info->pdHeight);
    LogD("x: %f", info->pdX);
    LogD("y: %f", info->pdY);
    LogD("--------------------------------------------");

    JobInfo* jobInfo = new JobInfo(REQUEST_CMD_OPEN_PD, info, This);
    Ecore_Job* ret = ecore_job_add(requestBoxJobCallback, jobInfo);

    return ret ? 0 : -1;
}

int BoxDaemonImpl::pdDestroyCallback(ProviderEventArgPtr arg, void* data)
{
    LogD("enter");

    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    if( This == NULL )
        return -1;

    BoxInfoPtr info = This->initializeBoxInfo(arg);
    if (!info) {
        return -1;
    }

    LogD("--------------------------------------------");
    LogD("boxId: %s", info->boxId.c_str());
    LogD("InstanceId: %s", info->instanceId.c_str());
    LogD("--------------------------------------------");

    JobInfo* jobInfo = new JobInfo(REQUEST_CMD_CLOSE_PD, info, This);
    Ecore_Job* ret = ecore_job_add(requestBoxJobCallback, jobInfo);

    return ret ? 0 : -1;
}

int BoxDaemonImpl::clickedCallback(ProviderEventArgPtr arg, void* data)
{
    LogD("enter");

    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    if( This == NULL )
        return -1;

    BoxInfoPtr info = This->initializeBoxInfo(arg);
    if (!info) {
        return -1;
    }

    int flag = web_provider_livebox_get_auto_launch(info->boxId.c_str());
    if (!flag) {
        return -1;
    }

    BoxDaemonUtil::launchApplication(info->boxId, info->instanceId);
    return 0;
}

int BoxDaemonImpl::boxResizeCallback(ProviderEventArgPtr arg, void* data)
{
    LogD("enter");

    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    if( This == NULL )
        return -1;

    BoxInfoPtr info = This->initializeBoxInfo(arg);
    if (!info) {
        return -1;
    }
    if (arg->info.resize.w == 0 || arg->info.resize.h == 0) {
        return -1;
    }

    info->boxWidth = arg->info.resize.w;
    info->boxHeight = arg->info.resize.h;

    // box information
    LogD("--------------------------------------------");
    LogD("boxId: %s", info->boxId.c_str());
    LogD("InstanceId: %s", info->instanceId.c_str());
    LogD("width: %d", info->boxWidth);
    LogD("height: %d", info->boxHeight);
    LogD("--------------------------------------------");

    JobInfo* jobInfo = new JobInfo(REQUEST_CMD_RESIZE_BOX, info, This);
    Ecore_Job* ret = ecore_job_add(requestBoxJobCallback, jobInfo);

    return ret ? 0 : -1;
}

int BoxDaemonImpl::boxPauseCallback(ProviderEventArgPtr arg, void* data)
{
    LogD("enter");

    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    if( This == NULL )
        return -1;

    BoxInfoPtr info = This->initializeBoxInfo(arg);
    if (!info) {
        return -1;
    }

    // box information
    LogD("--------------------------------------------");
    LogD("boxId: %s", info->boxId.c_str());
    LogD("InstanceId: %s", info->instanceId.c_str());
    LogD("--------------------------------------------");

    JobInfo* jobInfo = new JobInfo(REQUEST_CMD_PAUSE_BOX, info, This);
    Ecore_Job* ret = ecore_job_add(requestBoxJobCallback, jobInfo);

    return ret ? 0 : -1;
}

int BoxDaemonImpl::boxResumeCallback(ProviderEventArgPtr arg, void* data)
{
    LogD("enter");

    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    if( This == NULL )
        return -1;

    BoxInfoPtr info = This->initializeBoxInfo(arg);
    if (!info) {
        return -1;
    }

    // box information
    LogD("--------------------------------------------");
    LogD("boxId: %s", info->boxId.c_str());
    LogD("InstanceId: %s", info->instanceId.c_str());
    LogD("--------------------------------------------");

    JobInfo* jobInfo = new JobInfo(REQUEST_CMD_RESUME_BOX, info, This);
    Ecore_Job* ret = ecore_job_add(requestBoxJobCallback, jobInfo);

    return ret ? 0 : -1;
}

int BoxDaemonImpl::pauseCallback(ProviderEventArgPtr arg, void* data)
{
    LogD("enter");
    UNUSED_PARAM(arg);

    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    if( This == NULL )
        return -1;

    LogD("--------------------------------------------");
    LogD("web-provider is paused");
    LogD("--------------------------------------------");

    JobInfo* jobInfo = new JobInfo(REQUEST_CMD_PAUSE_ALL, BoxInfoPtr(), This);
    Ecore_Job* ret = ecore_job_add(requestBoxJobCallback, jobInfo);

    return ret ? 0 : -1;
}

int BoxDaemonImpl::resumeCallback(ProviderEventArgPtr arg, void* data)
{
    LogD("enter");
    UNUSED_PARAM(arg);

    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    if( This == NULL )
        return -1;

    LogD("--------------------------------------------");
    LogD("web-provider is resumed");
    LogD("--------------------------------------------");

    JobInfo* jobInfo = new JobInfo(REQUEST_CMD_RESUME_ALL, BoxInfoPtr(), This);
    Ecore_Job* ret = ecore_job_add(requestBoxJobCallback, jobInfo);

    return ret ? 0 : -1;
}

int BoxDaemonImpl::boxPeriodChangeCallback(ProviderEventArgPtr arg, void* data)
{
    LogD("enter");
    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    if( This == NULL )
        return -1;

    BoxInfoPtr info = This->initializeBoxInfo(arg);
    if (!info) {
        return -1;
    }
    info->period = arg->info.set_period.period;

    LogD("--------------------------------------------");
    LogD("boxId: %s", info->boxId.c_str());
    LogD("InstanceId: %s", info->instanceId.c_str());
    LogD("period: %f", info->period);
    LogD("--------------------------------------------");

    JobInfo* jobInfo = new JobInfo(REQUEST_CMD_CHANGE_PERIOD, info, This);
    Ecore_Job* ret = ecore_job_add(requestBoxJobCallback, jobInfo);

    return ret ? 0 : -1;
}

int BoxDaemonImpl::boxUpdateCallback(ProviderEventArgPtr arg, void* data)
{
    LogD("enter");

    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    if( This == NULL )
        return -1;

    BoxInfoPtr info = This->initializeBoxInfo(arg);
    if (!info) {
        return -1;
    }

    LogD("--------------------------------------------");
    LogD("boxId: %s", info->boxId.c_str());
    LogD("InstanceId: %s", info->instanceId.c_str());
    LogD("--------------------------------------------");

    JobInfo* jobInfo = new JobInfo(REQUEST_CMD_UPDATE_BOX, info, This);
    Ecore_Job* ret = ecore_job_add(requestBoxJobCallback, jobInfo);

    return ret ? 0 : -1;
}

void BoxDaemonImpl::setProviderCallbacks(ProviderCallbacks& callbacks)
{
    LogD("enter");

    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.connected = BoxDaemonImpl::connectedCallback;
    callbacks.disconnected = BoxDaemonImpl::disconnectedCallback;
    callbacks.pause = BoxDaemonImpl::pauseCallback;
    callbacks.resume = BoxDaemonImpl::resumeCallback;
    callbacks.lb_create = BoxDaemonImpl::boxCreateCallback;
    callbacks.lb_recreate = BoxDaemonImpl::boxReCreateCallback;
    callbacks.lb_destroy = BoxDaemonImpl::boxDestroyCallback;
    callbacks.lb_pause = BoxDaemonImpl::boxPauseCallback;
    callbacks.lb_resume = BoxDaemonImpl::boxResumeCallback;
    callbacks.pd_create = BoxDaemonImpl::pdCreateCallback;
    callbacks.pd_destroy = BoxDaemonImpl::pdDestroyCallback;
    callbacks.clicked = BoxDaemonImpl::clickedCallback;
    callbacks.resize = BoxDaemonImpl::boxResizeCallback;
    callbacks.update_content = BoxDaemonImpl::boxUpdateCallback;
    callbacks.set_period = BoxDaemonImpl::boxPeriodChangeCallback;
}

std::string BoxDaemonImpl::getBoxType(const char* boxId)
{
    LogD("enter");

    if (!boxId) {
        return std::string();
    }

    char* type = web_provider_livebox_get_box_type(boxId);
    if (!type) {
        std::string boxType = m_pluginConnector->getBoxType(boxId);
        if (boxType.empty()) {
            LogD("unrecognized box id");
        } else {
            LogD("box id: %s, type: %s", boxId, boxType.c_str());
        }
        return boxType;
    }

    LogD("box id: %s, type: %s", boxId, type);
    std::string result{type};
    free(type);
    return result;
}

BoxInfoPtr BoxDaemonImpl::initializeBoxInfo(ProviderEventArgPtr arg)
{
    LogD("enter");

    if (!arg) {
        return BoxInfoPtr();
    }

    if (!arg->pkgname || !arg->id) {
        LogD("pkgname or id don't exist");
        return BoxInfoPtr();
    }

    std::string type = getBoxType(arg->pkgname);
    if (type.empty()) {
        return BoxInfoPtr();
    }
    BoxInfoPtr infoPtr = BoxInfoPtr(new BoxInfo(type, arg->pkgname, arg->id));

    return infoPtr;
}

std::string BoxDaemonImpl::getBoxIdFromAppControl(app_control_h app_control)
{
    LogD("enter");

    int ret;
    char* appControlUri = NULL;
    ret = app_control_get_uri(app_control, &appControlUri);
    if (ret != APP_CONTROL_ERROR_NONE) {
        LogD("no box uri");
        return std::string();
    }

    std::string uri(appControlUri);
    free(appControlUri);

    if(uri.compare(0, strlen(APP_CONTROL_BOX_SERVICE_SCHEME), APP_CONTROL_BOX_SERVICE_SCHEME)) {
        // uri is not box-service scheme
        return std::string();
    }

    std::string boxId = uri.substr(strlen(APP_CONTROL_BOX_SERVICE_SCHEME));
    return boxId;
}

bool BoxDaemonImpl::isAppControlCallerBoxOwner(app_control_h app_control)
{
    LogD("enter");

    int ret;

    std::string boxId = getBoxIdFromAppControl(app_control);
    if (boxId.empty()) {
        LogD("error box-id");
        return false;
    }

    // check if caller is owner of this box
    char* appId = web_provider_livebox_get_app_id(boxId.c_str());
    if (!appId) {
        return false;
    }
    std::string ownerAppId(appId);
    free(appId);

    char* caller = NULL;
    ret = app_control_get_caller(app_control, &caller);
    if (ret != APP_CONTROL_ERROR_NONE) {
        ret = app_control_get_extra_data(
                app_control, APP_CONTROL_ALARM_CALLER_KEY, &caller);
        if (ret != APP_CONTROL_ERROR_NONE) {
            LogD("failed to get caller's appid from app_control");
            return false;
        }
    }
    std::string callerAppId(caller);

    // release strings
    free(caller);

    if (ownerAppId != callerAppId) {
        LogD("caller is not matched with owner of requested box");
        return false;
    }

    return true;
}

BoxInfoPtr BoxDaemonImpl::handleOperationUpdate(app_control_h app_control)
{
    LogD("enter");

    if (!isAppControlCallerBoxOwner(app_control)) {
        return BoxInfoPtr();
    }

    std::string boxId = getBoxIdFromAppControl(app_control);
    if (boxId.empty()) {
        LogD("error box-id");
        return BoxInfoPtr();
    }

    char* appContentInfo = NULL;
    app_control_get_extra_data(app_control, APP_CONTROL_CONTENT_INFO_KEY, &appContentInfo);

    std::string type(getBoxType(boxId.c_str()));
    if (type.empty()) {
        LogD("no type for this box");
        free(appContentInfo);
        return BoxInfoPtr();
    }

    BoxInfoPtr info = BoxInfoPtr(new BoxInfo(type, boxId, ""));
    if (appContentInfo) {
        LogD("app_control content info: %s", appContentInfo);
        info->appContentInfo = std::string(appContentInfo);
    }

    // release string
    free(appContentInfo);

    return info;
}

int BoxDaemonImpl::requestUpdateAll(void)
{
    LogD("enter");

    BoxInfoPtr info = BoxInfoPtr(new BoxInfo());
    JobInfo* jobInfo = new JobInfo(REQUEST_CMD_UPDATE_ALL,  info, this);

    ecore_job_add(requestBoxJobCallback, jobInfo);
    return 0;
}

std::vector<BoxInfoPtr> BoxDaemonImpl::handleOperationRemove(app_control_h app_control)
{
    LogD("enter");

    char* appId = NULL;
    app_control_get_extra_data(app_control, APP_CONTROL_REMOVE_APPID_KEY, &appId);
    LogD("request appid: %s", appId);

    // check if this appId has dynamic boxes
    int boxCount = 0;
    char** boxIdList = web_provider_livebox_get_box_id_list(appId, &boxCount);
    free(appId);
    if (!boxIdList) {
        return std::vector<BoxInfoPtr>();
    }

    std::vector<BoxInfoPtr> boxInfoPtrContainer;
    for (int i = 0; i < boxCount; i++) {
        // get plugin type of each box id
        std::string type(getBoxType(boxIdList[i]));
        if (type.empty()) {
            LogD("no type for this box: %s", boxIdList[i]);
            continue;
        }

        BoxInfoPtr info = BoxInfoPtr(new BoxInfo(type, boxIdList[i], ""));
        boxInfoPtrContainer.push_back(info);
    }

    web_provider_livebox_release_box_id_list(boxIdList, boxCount);
    return boxInfoPtrContainer;
}

Eina_Bool BoxDaemonImpl::pingToMasterCallback(void* data)
{
    LogD("enter");
    UNUSED_PARAM(data);

    provider_send_ping();

    return ECORE_CALLBACK_RENEW;
}

void BoxDaemonImpl::requestBoxJobCallback(void* data)
{
    JobInfo* jobInfo = static_cast<JobInfo*>(data);
    if (!jobInfo) {
        LogD("no information for job");
        return;
    }

    // just for debugging
    //volatile int flag = 0;
    //while(flag == 0) {;}

    // request box job!
    jobInfo->daemonImpl->m_pluginConnector->requestCommand(
            jobInfo->cmdType, jobInfo->boxInfo);

    delete jobInfo;
}

int BoxDaemonImpl::requestUpdateLangCallback(void* data, void* tmp)
{
    LogD("enter");
    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    return This->requestUpdateAll();
}

int BoxDaemonImpl::requestLowMemoryCallback(void* data, void* tmp)
{
    LogD("enter");
    UNUSED_PARAM(data);

    // terminate box daemon and revive
    elm_exit();
    return 0;
}

void BoxDaemonImpl::requestUpdateAllCallback(keynode_t* key, void* data)
{
    LogD("enter");
    UNUSED_PARAM(key);

    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    if( This == NULL )
        return;
    This->requestUpdateAll();
}

void BoxDaemonImpl::requestUpdateAppBoxCallback(keynode_t* key, void* data)
{
    LogD("enter");
    UNUSED_PARAM(key);

    BoxDaemonImpl* This = static_cast<BoxDaemonImpl*>(data);
    if( This == NULL )
        return;
    BoxInfoPtr info = BoxInfoPtr(new BoxInfo());
    JobInfo* jobInfo = new JobInfo(REQUEST_CMD_UPDATE_APPBOX,  info, This);

    ecore_job_add(requestBoxJobCallback, jobInfo);
}
