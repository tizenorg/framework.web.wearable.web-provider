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
 * @file    AppBoxManager.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include <map>
#include <pkgmgr-info.h>
#include <EWebKit.h>
#include <EWebKit_internal.h>
#include <core_module.h>
#include <Plugin/IBoxPluginFactory.h>
#include <Core/BoxData.h>
#include <Core/BoxManager.h>
#include <Core/IBox.h>
#include <Core/Util/Log.h>
#include <API/web_provider_widget_info.h>
#include "AppBoxManager.h"

static const std::string bundlePath("/usr/lib/libwrt-injected-bundle.so");

AppBoxManager::AppBoxManager(IBoxPluginFactoryPtr factory)
    : BoxManager(factory)
{
    bool ret = WRT::CoreModuleSingleton::Instance().Init();
    if (!ret) {
        throw; // throw exeception
    }

    vconf_notify_key_changed(VCONFKEY_NETWORK_PROXY,proxyChangedCallback,this);
}

AppBoxManager::~AppBoxManager()
{
    vconf_ignore_key_changed(VCONFKEY_NETWORK_PROXY,proxyChangedCallback);
}

bool AppBoxManager::requestAddBox(BoxInfoPtr boxInfo, EwkContextPtr ewkContext)
{
    char* appId =
        web_provider_widget_get_app_id(boxInfo->boxId.c_str());

    if (!appId) {
        LogD("no appid of %s", boxInfo->boxId.c_str());
        return false;
    }

    std::string appIdStr(appId);
    free(appId);

    auto it = m_ewkContextMap.find(appIdStr);
    if (it == m_ewkContextMap.end()) {
        ewkContext = getAvailableEwkContext(appIdStr);
        if( ewkContext == NULL ){
            LogE("Can't create ewkContext (not allowed appid)");
            return false;
        }
        insertContextMap(appIdStr, ewkContext);
    } else {
        ewkContext = it->second;
    }

    if (!BoxManager::requestAddBox(boxInfo, ewkContext)) {
        return false;
    }

    return true;
}

void AppBoxManager::updateEwkContext(std::string& boxId)
{
    LogD("enter");
    char* appId = web_provider_widget_get_app_id(boxId.c_str());
    if (!appId) {
        LogD("no appid of %s", boxId.c_str());
        return;
    }

    std::string appIdStr(appId);
    free(appId);

    int count = BoxManager::getBoxCount(appIdStr);
    LogD("count: %d", count);
    if (!count) {
        LogD("%s's ewk context removed", appIdStr.c_str());
        // remove ewk context
        eraseContextMap(appIdStr);
    }
}

EwkContextPtr AppBoxManager::getAvailableEwkContext(const std::string& appId)
{

    // get the base executable path
    std::string baseExecutablePath = getBaseExecutablePath(appId);
    if (baseExecutablePath.empty()) {
        return EwkContextPtr();
    }

    // get web process path for this box
    std::string webProcessPath = baseExecutablePath + ".d-box";

    // get plugin process path for this box
    std::string pluginProcessPath = baseExecutablePath + ".npruntime";

    // box manager should set webprocess path as value of 'WEB_PROCESS_PATH'
    //  before calling ewk_context_new_with_injected_bundle_path().
    setenv("WEB_PROCESS_EXECUTABLE_PATH", webProcessPath.c_str(), 1);
    setenv("PLUGIN_PROCESS_EXECUTABLE_PATH", pluginProcessPath.c_str(), 1);

    EwkContextPtr newEwkContext(
            ewk_context_new_with_injected_bundle_path(bundlePath.c_str()),
            BoxManager::EwkContextDeleter());

    //find package root path
    std::string pkgpath = baseExecutablePath.substr(0, baseExecutablePath.rfind("/bin/"));
    pkgpath = pkgpath + "/data/.webkit/localStorage/";
    //setting for localStorage database path to share with webapp
    ewk_context_web_storage_path_set(newEwkContext.get(), pkgpath.c_str());

    // proxy server setting
    char *proxy = vconf_get_str(VCONFKEY_NETWORK_PROXY);
    LogD("proxy address changed %s", proxy);
    std::string dummyProxy("0.0.0.0");
    if (proxy && strlen(proxy) && (dummyProxy != proxy)) {
        free(proxy);
        proxy = NULL;
    }
    ewk_context_proxy_uri_set(newEwkContext.get(), proxy);

    // We can't unset due to Webprocess was forked in other thread.
    // unset the following env variables not to affect other ewk context creation
    //unsetenv("WEB_PROCESS_EXECUTABLE_PATH");
    //unsetenv("PLUGIN_PROCESS_EXECUTABLE_PATH");

    return newEwkContext;
}

void AppBoxManager::insertContextMap(std::string& appId, EwkContextPtr ewkContext)
{
    m_ewkContextMap.insert(EwkContextMapPair(appId, ewkContext));
}

void AppBoxManager::eraseContextMap(std::string& appId)
{
    m_ewkContextMap.erase(appId);
}

std::string AppBoxManager::getBaseExecutablePath(const std::string& appId)
{
    pkgmgrinfo_appinfo_h handle = NULL;
    char* retStr = NULL;

    int ret = pkgmgrinfo_appinfo_get_appinfo(appId.c_str(), &handle);
    if (ret != PMINFO_R_OK) {
        LogE("pkgmgrinfo_appinfo_get_appinfo failed");
        return std::string();
    }

    ret = pkgmgrinfo_appinfo_get_exec(handle, &retStr);
    if (ret != PMINFO_R_OK || !retStr) {
        LogE("pkgmgrinfo_appinfo_get_exec failed");

        ret = pkgmgrinfo_appinfo_destroy_appinfo(handle);
        if (ret != PMINFO_R_OK) {
            LogE("pkgmgrinfo_appinfo_destroy_appinfo failed");
        }
        return std::string();
    } else {
        std::string basePath(retStr);

        ret = pkgmgrinfo_appinfo_destroy_appinfo(handle);
        if (ret != PMINFO_R_OK) {
            LogE("pkgmgrinfo_appinfo_destroy_appinfo failed");
            return std::string();
        }
        return basePath;
    }
}


void AppBoxManager::proxyChangedCallback(keynode_t* /* keynode */, void* data){
    LogD("enter");
    AppBoxManager *This = static_cast<AppBoxManager*>(data);
    if( This == NULL )
        return;

    // proxy server setting
    char *proxy = vconf_get_str(VCONFKEY_NETWORK_PROXY);
    LogD("proxy address changed %s", proxy);
    std::string dummyProxy("0.0.0.0");
    if (proxy && strlen(proxy) && (dummyProxy != proxy)) {
        free(proxy);
        proxy = NULL;
    }
    for( auto it = This->m_ewkContextMap.begin(); it != This->m_ewkContextMap.end(); ++it){
        ewk_context_proxy_uri_set(it->second.get(), proxy);
    }
    free(proxy);
}
