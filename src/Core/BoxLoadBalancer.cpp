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
 * @file    BoxLoadBalancer.cpp
 * @author  Minhyung Ko (minhyung.ko@samsung.com)
 */
#include "Util/Log.h"
#include "Util/Util.h"
#include "BoxLoadBalancer.h"

// boxLoadBalanceTimer period
#define BOX_UPDATE_TIME 0.1
#define BOX_UPDATE_TIME_INC 0.3
#define BOX_UPDATE_MAX_TIME 5.0f
#define BOX_UPDATE_QUEUE_SIZE_THRESHOLD 5

BoxAsyncCommandInfo::BoxAsyncCommandInfo(request_box_cmd_type type, IBoxPtr boxPtr, BoxInfoPtr boxInfoPtr)
{
    m_type = type;
    m_box = boxPtr;
    m_boxInfo = boxInfoPtr;
}

BoxAsyncCommandInfo::BoxAsyncCommandInfo(request_box_cmd_type type, IBoxPtr boxPtr, std::string& appContentInfo)
{
    m_type = type;
    m_box = boxPtr;
    m_appContentInfo = appContentInfo;
}

BoxLoadBalancer::BoxLoadBalancer()
{
    m_boxAsyncCommandList.clear();
    m_boxLoadBalanceTimer = NULL;
    m_updateTimeCount = 0.0f;
}

BoxLoadBalancer::~BoxLoadBalancer()
{

}

Eina_Bool BoxLoadBalancer::requestBoxLoadBalanceCallback(void* data)
{
    LogD("requestBoxLoadBalanceCallback");
    BoxLoadBalancer* This = static_cast<BoxLoadBalancer*>(data);

    if (This->m_boxAsyncCommandList.empty()) {
        LogD("m_boxAsyncCommandList.size() is empty");
        This->m_boxLoadBalanceTimer = NULL;
        return ECORE_CALLBACK_CANCEL;
    }

    BoxAsyncCommandInfoPtr boxCommandPtr = This->m_boxAsyncCommandList.front();
    IBoxPtr box = boxCommandPtr->getBoxPtr();
    if( box ){
        switch (boxCommandPtr->getType()) {
            case BOX_AYNC_REQUEST_CMD_SHOW:
                box->show();
                break;
            case BOX_AYNC_REQUEST_CMD_RESUME:
                box->resume();
                break;
            case BOX_AYNC_REQUEST_CMD_UPDATE:
            {
                std::string contentInfo = boxCommandPtr->getAppContentInfo();
                box->update(contentInfo);
                break;
            }
            default:
                LogW("Wrong command %d", boxCommandPtr->getType());
        }
    }

    This->m_boxAsyncCommandList.pop_front();

    int commandListNumber = This->m_boxAsyncCommandList.size();

    if (commandListNumber > BOX_UPDATE_QUEUE_SIZE_THRESHOLD) {
        float timecountbefore = BOX_UPDATE_TIME + BOX_UPDATE_TIME_INC * (commandListNumber - BOX_UPDATE_QUEUE_SIZE_THRESHOLD);

        if (timecountbefore > BOX_UPDATE_MAX_TIME) {
            timecountbefore = BOX_UPDATE_MAX_TIME;
        }
        ecore_timer_del(This->m_boxLoadBalanceTimer);

        This->m_updateTimeCount = timecountbefore;

        This->m_boxLoadBalanceTimer = ecore_timer_add( This->m_updateTimeCount, requestBoxLoadBalanceCallback, This);
        LogD("requestBoxLoadBalanceCallback launched pending until %f m_updateTimeCount %d size", This->m_updateTimeCount, commandListNumber);
        return ECORE_CALLBACK_CANCEL;
    }

    if (commandListNumber > 0 && (This->m_updateTimeCount > BOX_UPDATE_TIME)) {
        ecore_timer_del(This->m_boxLoadBalanceTimer);
        This->m_updateTimeCount = BOX_UPDATE_TIME;
        This->m_boxLoadBalanceTimer = ecore_timer_add( This->m_updateTimeCount, requestBoxLoadBalanceCallback, This);
        return ECORE_CALLBACK_CANCEL;
    }

    if (commandListNumber > 0) {
        LogD("m_boxAsyncCommandList.size() = %d", commandListNumber);
        return ECORE_CALLBACK_RENEW;
    }

    This->m_boxLoadBalanceTimer = NULL;
    return ECORE_CALLBACK_CANCEL;
}

void BoxLoadBalancer::eraseBoxAsyncCommandFromQueue(IBoxPtr boxPtr)
{
    if (m_boxAsyncCommandList.empty())
        return;
    auto it = m_boxAsyncCommandList.begin();
    while( it != m_boxAsyncCommandList.end() ){
        BoxAsyncCommandInfoPtr boxCommandPtr = *it;
        if ( boxCommandPtr->getBoxPtr() == boxPtr) {
            auto deleteIt = it++;
            m_boxAsyncCommandList.erase(deleteIt);
            LogD("m_boxAsyncCommandList.erase");
        }else
            ++it;
    }
}

void BoxLoadBalancer::pushBoxAsyncCommandIntoQueue(request_box_cmd_type cmdType, IBoxPtr boxPtr, BoxInfoPtr boxInfoPtr)
{
    BoxAsyncCommandInfoPtr boxAsyncInfo = BoxAsyncCommandInfoPtr(new BoxAsyncCommandInfo(cmdType, boxPtr, boxInfoPtr));

    m_boxAsyncCommandList.push_back(boxAsyncInfo);
    LogD("m_boxAsyncCommandList added = %d ",m_boxAsyncCommandList.size());

    if (!m_boxLoadBalanceTimer) {
        // start timer for clearing existing snapshot in case of only pd open
        m_boxLoadBalanceTimer = ecore_timer_add(BOX_UPDATE_TIME, requestBoxLoadBalanceCallback, this);
        m_updateTimeCount = BOX_UPDATE_TIME;
        LogD("timer launched");
    }
}

void BoxLoadBalancer::pushBoxAsyncCommandIntoQueue(request_box_cmd_type cmdType, IBoxPtr boxPtr, std::string& appContentInfo)
{
    BoxAsyncCommandInfoPtr boxAsyncInfo = BoxAsyncCommandInfoPtr(new BoxAsyncCommandInfo(cmdType, boxPtr, appContentInfo));

    m_boxAsyncCommandList.push_back(boxAsyncInfo);
    LogD("m_boxAsyncCommandList added = %d ",m_boxAsyncCommandList.size());

    if (!m_boxLoadBalanceTimer) {
        // start timer for clearing existing snapshot in case of only gbar open
        m_boxLoadBalanceTimer = ecore_timer_add(BOX_UPDATE_TIME, requestBoxLoadBalanceCallback, this);
        m_updateTimeCount = BOX_UPDATE_TIME;
        LogD("timer launched");
    }
}

void BoxLoadBalancer::pushBoxAsyncCommandIntoTop(request_box_cmd_type cmdType, IBoxPtr boxPtr, BoxInfoPtr boxInfoPtr)
{
    BoxAsyncCommandInfoPtr boxAsyncInfo = BoxAsyncCommandInfoPtr(new BoxAsyncCommandInfo(cmdType, boxPtr, boxInfoPtr));
    m_boxAsyncCommandList.push_front(boxAsyncInfo);
    LogD("m_boxAsyncCommandList added = %d ",m_boxAsyncCommandList.size());

    if (!m_boxLoadBalanceTimer) {
        // start timer for clearing existing snapshot in case of only gbar open
        m_boxLoadBalanceTimer = ecore_timer_add(BOX_UPDATE_TIME, requestBoxLoadBalanceCallback, this);
        m_updateTimeCount = BOX_UPDATE_TIME;
        LogD("timer launched");
    }
}

void BoxLoadBalancer::stopLoadBalancer()
{
    if (m_boxLoadBalanceTimer) {
        ecore_timer_del(m_boxLoadBalanceTimer);
        m_boxLoadBalanceTimer = NULL;
    }
    m_boxAsyncCommandList.clear();
}
