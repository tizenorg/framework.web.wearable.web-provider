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
 * @file    Box.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include <sstream>
#include <Ecore.h>
#include <Plugin/IBoxPluginFactory.h>
#include "Util/Log.h"
#include "Util/Util.h"
#include "BoxData.h"
#include "IBoxState.h"
#include "BoxState.h"
#include "Util/ITimer.h"
#include "BoxUpdateTimer.h"
#include "Box.h"

Box::Box(BoxInfoPtr boxInfo, IBoxPluginFactoryPtr factory, EwkContextPtr ewkContext)
    : m_boxInfo(boxInfo)
    , m_factory(factory)
    , m_currentTab(true)
    , m_paused(false)
    , m_updateNeeded(false)
    , m_remainUpdateRequestTime()
    , m_showed(false)
{
    LogD("enter");
    if( boxInfo == NULL ){
        LogE("boxInfo is NULL");
        throw;
    }
    try {
        m_renderView = m_factory->createRenderView(m_boxInfo, ewkContext);
        m_updateTimer = BoxUpdateTimer::create(
                boxInfo->period, Box::updateCallback, this);
        // TODO code regarding state needs more testing
        //m_state = BoxInitState::create(
        //           IBoxContextPtr(dynamic_cast<IBoxContext*>(this)));
    } catch (...) {
        throw;
    }
}

Box::~Box()
{
    LogD("enter");
}

bool Box::show()
{
    LogD("enter");
    CHECK_BOX_STATE(m_state, permitShow);

    try {
        m_updateTimer->start();
        m_renderView->show();
    } catch (...) {
        return false;
    }
    m_showed = true;
    SWITCH_BOX_STATE();
    return true;
}

bool Box::hide()
{
    LogD("enter");
    CHECK_BOX_STATE(m_state, permitHide);

    try {
        m_updateTimer->stop();
        m_renderView->hide();
    } catch (...) {
        return false;
    }

    SWITCH_BOX_STATE();
    return true;
}

bool Box::resize(int width, int height)
{
    LogD("enter");
    CHECK_BOX_STATE(m_state, permitShow);

    // reset box info to new width, height
    m_boxInfo->boxWidth = width;
    m_boxInfo->boxHeight = height;

    try {
        m_updateTimer->restart();
        m_renderView->resize();
    } catch (...) {
        LogD("resize exception");
        return false;
    }
    m_showed = true;
    SWITCH_BOX_STATE();
    return true;
}

bool Box::resume()
{
    LogD("enter");
    CHECK_BOX_STATE(m_state, permitResume);

    m_currentTab = true;
    m_paused = false;

    if (m_updateNeeded) {
        m_updateNeeded = false;
        m_updateTimer->start();
        m_renderView->update();
    } else {
        m_renderView->resume();
    }

    m_showed = true;
    SWITCH_BOX_STATE();
    return true;
}

bool Box::needToUpdate()
{
    LogD("enter");

    if (m_updateNeeded) {
        return true;
    }

    if (m_updateTimer && m_updateTimer->getPeriod() > 0) {
        // check for need updateTimer
        time_t currentTime = time(NULL);
        float diffTime = static_cast<float>(difftime(currentTime, m_remainUpdateRequestTime));
        if (diffTime < m_updateTimer->getPeriod()) {
            if (!m_updateTimer->isRunning()) {
                // we need start timer only remained time from first timer launched
                m_updateTimer->continueTimer(m_updateTimer->getPeriod() - diffTime);
            }
        } else {
            return true;
        }
        return false;
    }

    if (!m_showed) {
        return true;
    }

    return false;
}

bool Box::pause(bool background)
{
    LogD("enter");
    CHECK_BOX_STATE(m_state, permitPause);

    try {
        if (!background) {
            m_currentTab = false;
        }
        m_paused = true;
        m_renderView->pause();

        if (m_updateTimer->isRunning()) {
            m_remainUpdateRequestTime = m_updateTimer->getStartTime();
            m_updateTimer->stop();
        }
    } catch (...) {
        return false;
    }

    SWITCH_BOX_STATE();
    return true;
}

bool Box::openPd(int width, int height, int x, int y)
{
    LogD("enter");
    CHECK_BOX_STATE(m_state, permitOpenPd);

    m_boxInfo->pdWidth = width;
    m_boxInfo->pdHeight = height;
    m_boxInfo->pdX = x;
    m_boxInfo->pdY = y;

    try {
        m_updateTimer->stop();
        m_renderView->openPd();
    } catch (...) {
        return false;
    }
    m_showed = true;
    SWITCH_BOX_STATE();
    return true;
}

bool Box::closePd()
{
    LogD("enter");
    CHECK_BOX_STATE(m_state, permitClosePd);

    try {
        m_renderView->closePd();
        m_updateTimer->restart();
    } catch (...) {
        return false;
    }

    SWITCH_BOX_STATE();
    return true;
}

bool Box::update(std::string& appContentInfo)
{
    LogD("enter");

    m_boxInfo->appContentInfo = appContentInfo;
    if (m_paused) {
        // update is dalayed
        m_updateNeeded = true;
        return true;
    }

    try {
        m_updateTimer->start();
        m_renderView->update();
    } catch (...) {
        return false;
    }
    m_showed = true;
    return true;
}

bool Box::changePeriod(float period)
{
    LogD("enter");

    // reset period
    m_boxInfo->period = period;

    if (m_updateTimer->isRunning()) {
        m_updateTimer->setPeriod(m_boxInfo->period);
    }

    return true;
}

bool Box::isCurrentTab()
{
    return m_currentTab;
}

void Box::setState(IBoxStatePtr state)
{
    UNUSED_PARAM(state);
    // assign new state
    //m_state = state;
}

Eina_Bool Box::updateCallback(void* data)
{
    LogD("enter");
    Box* This = static_cast<Box*>(data);
    This->update(This->m_boxInfo->appContentInfo);
    return ECORE_CALLBACK_RENEW;
}

BoxInfoPtr Box::getBoxInfo()
{
    LogD("enter");
    return m_boxInfo;
}

void Box::setNeedToUpdate()
{
    m_updateNeeded = true;
}

bool Box::isPaused()
{
    return m_paused;
}

void Box::setCurrent()
{
    m_currentTab = true;
    m_paused = false;
}
