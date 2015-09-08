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
 * @file    BoxManager.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include "config.h"
#include "BoxManager.h"

#include <string>
#include <map>
#include <ctime>
#include <EWebKit.h>
#include <EWebKit_internal.h>
#include <Plugin/IBoxPluginFactory.h>
#include <Plugin/box_plugin_interface.h>
#include "Util/Log.h"
#include "Util/Util.h"
#include "IBox.h"
#include "Box.h"
#include "BoxData.h"

namespace{
    static const std::string kDefaultBoxType("app");
}

BoxManager::BoxManager(IBoxPluginFactoryPtr factory)
    : m_boxFactory(factory)
    , m_boxLoadBalancer()
{
    LogD("enter");
}

BoxManager::~BoxManager()
{
    LogD("enter");

    vconf_ignore_key_changed(VCONFKEY_NETWORK_PROXY,proxyChangedCallback);
}

bool BoxManager::doCommand(const request_cmd_type type, const BoxInfoPtr& boxInfo)
{
    if( boxInfo == NULL )
        return false;

    bool result = false;
    switch (type) {
    case REQUEST_CMD_ADD_BOX:
        result = requestAddBox(boxInfo, m_defaultContext);
        break;
    case REQUEST_CMD_REMOVE_BOX:
        if (!boxInfo->instanceId.empty()) {
            result = requestRemoveBoxByInstanceId(boxInfo);
            break;
        }
        if (!boxInfo->boxId.empty()) {
            result = requestRemoveBoxByBoxId(boxInfo);
            break;
        }
    case REQUEST_CMD_RESIZE_BOX:
        result = requestResizeBox(boxInfo->instanceId, boxInfo->boxWidth, boxInfo->boxHeight);
        break;
    case REQUEST_CMD_RESUME_BOX:
        result = requestResumeBox(boxInfo->instanceId);
        break;
    case REQUEST_CMD_PAUSE_BOX:
        result = requestPauseBox(boxInfo->instanceId);
        break;
    case REQUEST_CMD_RESUME_ALL:
        result = requestResumeAll();
        break;
    case REQUEST_CMD_PAUSE_ALL:
        result = requestPauseAll();
        break;
    case REQUEST_CMD_OPEN_GBAR:
        result = requestOpenGbar(boxInfo->instanceId,
                    boxInfo->gbarWidth, boxInfo->gbarHeight,
                    boxInfo->gbarX, boxInfo->gbarY);
        break;
    case REQUEST_CMD_CLOSE_GBAR:
        result = requestCloseGbar(boxInfo->instanceId);
        break;
    case REQUEST_CMD_CHANGE_PERIOD:
        result = requestChangePeriod(boxInfo->instanceId, boxInfo->period);
        break;
    case REQUEST_CMD_UPDATE_BOX:
        result = requestUpdateBox(boxInfo->boxId, boxInfo->appContentInfo);
        break;
    case REQUEST_CMD_UPDATE_ALL:
        result = requestUpdateAll(boxInfo->instanceId);
        break;
    case REQUEST_CMD_UPDATE_APPBOX:
         result = requestUpdateAppBox();
        break;
    default:
        LogD("not available request type");
        break;
    }

    return result;
}

bool BoxManager::requestAddBox(BoxInfoPtr boxInfo, EwkContextPtr ewkContext)
{
    LogD("enter");

    if( boxInfo == NULL ){
        LogE("boxInfo is NULL");
        return false;
    }

    IBoxPtr box;
    // create new box
    try {
        if (!ewkContext) {
            if (!m_defaultContext) {
                m_defaultContext = EwkContextPtr(ewk_context_new(), EwkContextDeleter());
            }
            ewkContext = m_defaultContext;
        }

        initEwkContextSetting(ewkContext);
        box = Box::create(boxInfo, m_boxFactory, ewkContext);
    } catch (...) {
        LogD("exection occurs during adding box");
        return false;
    }

    m_boxLoadBalancer.pushBoxAsyncCommandIntoTop(BOX_AYNC_REQUEST_CMD_SHOW, box, boxInfo);

    insertBoxMap(boxInfo->instanceId, box);
    return true;
}

bool BoxManager::requestRemoveBox(std::string& instanceId)
{
    LogD("enter");
    IBoxPtr box = searchBoxMap(instanceId);
    if (!box) {
        return false;
    }
    m_boxMap.erase(instanceId);

    if (!box->hide()) {
        return false;
    }
    return true;
}

bool BoxManager::requestRemoveBoxByInstanceId(BoxInfoPtr boxInfo)
{
    LogD("enter");
    if( boxInfo == NULL ){
        LogE("boxInfo is NULL");
        return false;
    }

    IBoxPtr box = searchBoxMap(boxInfo->instanceId);
    if(box == NULL){
        LogE("Can't find box %s", boxInfo->instanceId.c_str());
        return false;
    }

    m_boxLoadBalancer.eraseBoxAsyncCommandFromQueue(box);
    requestRemoveBox(boxInfo->instanceId);
    updateEwkContext(boxInfo->boxId);
    return true;
}

bool BoxManager::requestRemoveBoxByBoxId(BoxInfoPtr boxInfo)
{
    LogD("enter");
    if( boxInfo == NULL ){
        LogE("boxInfo is NULL");
        return false;
    }

    bool found = false;
    for (auto it = m_boxMap.begin(); it != m_boxMap.end(); ) {
        if (it->first.find(boxInfo->boxId) == std::string::npos) {
            ++it;
            continue;
        }
        auto to_deleted = it++;
        if( to_deleted->second && to_deleted->second->getBoxInfo() )
            requestRemoveBoxByInstanceId(to_deleted->second->getBoxInfo());
        found = true;
    }

    return found;
}

bool BoxManager::requestResizeBox(std::string& instanceId, int width, int height)
{
    LogD("enter");
    IBoxPtr box = searchBoxMap(instanceId);
    if (!box) {
        return false;
    }

    m_boxLoadBalancer.eraseBoxAsyncCommandFromQueue(box);
    return box->resize(width, height);
}

bool BoxManager::requestResumeBox(std::string& instanceId)
{
    LogD("enter");
    IBoxPtr box = searchBoxMap(instanceId);
    if (!box) {
        return false;
    }
    BoxInfoPtr boxInfo;
    box->setCurrent();

    if (box->needToUpdate()) {
        m_boxLoadBalancer.pushBoxAsyncCommandIntoQueue(BOX_AYNC_REQUEST_CMD_RESUME, box, boxInfo);
    }
    return true;
}

bool BoxManager::requestPauseBox(std::string& instanceId)
{
    LogD("enter");
    IBoxPtr box = searchBoxMap(instanceId);
    if (!box) {
        return false;
    }
    m_boxLoadBalancer.eraseBoxAsyncCommandFromQueue(box);

    return box->pause(false);
}

bool BoxManager::requestResumeAll()
{
    LogD("enter");
    m_boxLoadBalancer.stopLoadBalancer();

    for (auto it = m_boxMap.begin(); it != m_boxMap.end(); it++) {
        if (it->second->isCurrentTab()) {
            std::string strBox = it->first;
            requestResumeBox(strBox);
        }
    }

    return true;
}

bool BoxManager::requestPauseAll()
{
    LogD("enter");
    for (auto it = m_boxMap.begin(); it != m_boxMap.end(); it++) {
        if (it->second->isCurrentTab()) {
            // paused by entering background
            it->second->pause(true);
        }
    }
    m_boxLoadBalancer.stopLoadBalancer();
    return true;
}

bool BoxManager::requestOpenGbar(
        std::string& instanceId,
        int width, int height, int x, int y)
{
    LogD("enter");
    IBoxPtr box = searchBoxMap(instanceId);
    if (!box) {
        return false;
    }

    m_boxLoadBalancer.eraseBoxAsyncCommandFromQueue(box);

    return box->openGbar(width, height, x, y);
}

bool BoxManager::requestCloseGbar(std::string& instanceId)
{
    LogD("enter");
    IBoxPtr box = searchBoxMap(instanceId);
    if (!box) {
        return false;
    }

    return box->closeGbar();
}

bool BoxManager::requestChangePeriod(std::string& instanceId, float period)
{
    LogD("enter");
    IBoxPtr box = searchBoxMap(instanceId);
    if (!box) {
        return false;
    }

    return box->changePeriod(period);
}

bool BoxManager::requestUpdateBox(std::string& boxId, std::string& appContentInfo)
{
    LogD("enter");

    for (auto it = m_boxMap.begin(); it != m_boxMap.end(); ++it) {
        if (it->first.find(boxId) == std::string::npos) {
            continue;
        }
        IBoxPtr box = it->second;
        box->setNeedToUpdate();
        m_boxLoadBalancer.eraseBoxAsyncCommandFromQueue(box);
        m_boxLoadBalancer.pushBoxAsyncCommandIntoQueue(BOX_AYNC_REQUEST_CMD_UPDATE, box, appContentInfo);
    }

    return true;
}

bool BoxManager::requestUpdateAll(std::string& instanceId)
{
    LogD("enter");
    UNUSED_PARAM(instanceId);

    m_boxLoadBalancer.stopLoadBalancer();

    for (auto it = m_boxMap.begin(); it != m_boxMap.end(); ++it) {
        if (it->second) {
            IBoxPtr box = it->second;
            box->setNeedToUpdate();

            // I'll remove below if statement, 
            // because in box::update method, if box was pause state, actually does not execute update process,
            // just mark to m_updateNeeded, so, later if box was resume, this box will updated.
            //if (!box->isPaused())
            m_boxLoadBalancer.pushBoxAsyncCommandIntoQueue(BOX_AYNC_REQUEST_CMD_UPDATE, box, box->getBoxInfo()->appContentInfo);
        }
    }
    return true;
}

bool BoxManager::requestUpdateAppBox()
{
    LogD("enter");

    for (auto it = m_boxMap.begin(); it != m_boxMap.end(); ++it) {
        if (it->second) {
            IBoxPtr box = it->second;

            if(box->getBoxInfo()->boxType.find(kDefaultBoxType) == std::string::npos) {
                continue;
            }
            m_boxLoadBalancer.eraseBoxAsyncCommandFromQueue(box);
            box->setNeedToUpdate();

            // I'll remove below if statement, 
            // because in box::update method, if box was pause state, actually does not execute update process,
            // just mark to m_updateNeeded, so, later if box was resume, this box will updated.
            //if (!box->isPaused())
            m_boxLoadBalancer.pushBoxAsyncCommandIntoQueue(BOX_AYNC_REQUEST_CMD_UPDATE, box, box->getBoxInfo()->appContentInfo);
        }
    }
    return true;
}

void BoxManager::updateEwkContext(std::string& boxId)
{
    // this should be implemented by derived class
    UNUSED_PARAM(boxId);
}

int BoxManager::getBoxCount(std::string appId)
{
    int count = 0;
    for (auto it = m_boxMap.begin(); it != m_boxMap.end(); it++) {
        if (it->first.find(appId) == std::string::npos) {
            continue;
        }
        count++;
    }
    return count;
}

void BoxManager::insertBoxMap(std::string& instanceId, IBoxPtr box)
{
    if (!searchBoxMap(instanceId)) {
        LogD("insert box to map: %s", instanceId.c_str());
        m_boxMap.insert(BoxMapPair(instanceId, box));
    } else {
        LogD("this box was already inserted!");
    }
}

void BoxManager::eraseBoxMap(std::string& instanceId)
{
    LogD("erase box to map");
    if (!searchBoxMap(instanceId)) {
        LogD("not available box");
        return;
    }

    m_boxMap.erase(instanceId);
}

void BoxManager::updateBoxMap(std::string& instanceId, IBoxPtr box)
{
    if (searchBoxMap(instanceId)) {
        eraseBoxMap(instanceId);
    }

    insertBoxMap(instanceId, box);
}

IBoxPtr BoxManager::searchBoxMap(std::string& instanceId)
{
    LogD("enter");
    IBoxPtr box;
    auto it = m_boxMap.find(instanceId);
    if (it != m_boxMap.end()) {
        LogD("found box: %s (%p)", it->first.c_str(), it->second.get());
        box = it->second;
    }

    return box;
}

void BoxManager::clearBoxMap()
{
    m_boxMap.clear();
}

void BoxManager::EwkContextDeleter::operator()(Ewk_Context* ptr)
{
    LogD("ewk context delete");
    if (ptr) {
        ewk_object_unref(ptr);
    }
}

void BoxManager::proxyChangedCallback(keynode_t* /*keynode*/, void* data){
    LogD("enter");
    BoxManager *This = static_cast<BoxManager*>(data);

    // proxy server setting
    char *proxy = vconf_get_str(VCONFKEY_NETWORK_PROXY);
    LogD("proxy address changed %s", proxy);
    std::string dummyProxy("0.0.0.0");
    if (proxy && strlen(proxy) && (dummyProxy != proxy)) {
        free(proxy);
        proxy = NULL;
    }
    if( This->m_defaultContext.get() != NULL )
        ewk_context_proxy_uri_set(This->m_defaultContext.get(), proxy);
    free(proxy);
}

void BoxManager::initEwkContextSetting(EwkContextPtr ewkContext)
{
    LogD("enter");

    // proxy server setting
    char *proxy = vconf_get_str(VCONFKEY_NETWORK_PROXY);
    std::string dummyProxy("0.0.0.0");
    if (proxy && strlen(proxy) && (dummyProxy != proxy)) {
        LogD("proxy address: %s", proxy);
        ewk_context_proxy_uri_set(ewkContext.get(),  proxy);
    } else {
        LogD("proxy address is empty");
        ewk_context_proxy_uri_set(ewkContext.get(), NULL);
    }

    if (proxy) {
        free(proxy);
    }
    vconf_notify_key_changed(VCONFKEY_NETWORK_PROXY,proxyChangedCallback,this);

    Ewk_Cookie_Manager *ewkCookieManager;
    ewkCookieManager =
        ewk_context_cookie_manager_get(ewkContext.get());
    ewk_cookie_manager_accept_policy_set(ewkCookieManager,
                                         EWK_COOKIE_ACCEPT_POLICY_ALWAYS);
#if !ENABLE(WEBKIT_UPVERSION)
    ewk_context_tizen_extensible_api_set(ewkContext.get(), EWK_EXTENSIBLE_API_SUPPORT_MULTIMEDIA, EINA_FALSE);
#endif
}
