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
 * @file    BoxUpdateTimer.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include <Ecore.h>
#include <Core/Util/Log.h>
#include "BoxUpdateTimer.h"

#define UPDATE_TIME_MIN 1800.0f

BoxUpdateTimer::BoxUpdateTimer(float period, Ecore_Task_Cb callback, void* data)
    : m_period(period)
    , m_callback(callback)
    , m_data(data)
    , m_timer()
    , m_startTime()
    , m_bTimerRunning(false)
{
    LogD("enter");
}

BoxUpdateTimer::~BoxUpdateTimer()
{
    LogD("enter");
}

void BoxUpdateTimer::start()
{
    if (m_period <= 0.0f ) {
        return;
    }

    if (m_period < UPDATE_TIME_MIN) {
        LogD("reset to minimum period(%f)", UPDATE_TIME_MIN);
        m_period = UPDATE_TIME_MIN;
    }

    if (m_timer) {
        stop();
    }

    m_timer = ecore_timer_add(m_period, m_callback, m_data);
    m_startTime = time(NULL);
    m_bTimerRunning  = true;
}

void BoxUpdateTimer::continueTimer(float period)
{
    if (period <= 0.0f ) {
        return;
    }

    if (m_timer) {
        stop();
    }

    m_timer = ecore_timer_add(period, m_callback, m_data);
    // this function does not need m_startTime beacuse this is concept for continue not really start
    // box.h update algorithm used boxupdatetimer
    m_bTimerRunning  = true;
}

void BoxUpdateTimer::stop()
{
    if (m_timer) {
        ecore_timer_del(m_timer);
        m_timer = NULL;
    }
    m_bTimerRunning = false;
}

void BoxUpdateTimer::resume()
{
    LogD("enter");
    ecore_timer_thaw(m_timer);
}

void BoxUpdateTimer::pause()
{
    LogD("enter");
    ecore_timer_freeze(m_timer);
}

void BoxUpdateTimer::restart()
{
    if (m_timer) {
        ecore_timer_reset(m_timer);
    } else {
        start();
    }
}

void BoxUpdateTimer::setPeriod(float period)
{
    m_period = period;
    start();
}
