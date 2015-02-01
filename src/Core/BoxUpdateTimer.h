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
 * @file    BoxUpdateTimer.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef BOX_UPDATE_TIMER_H
#define BOX_UPDATE_TIMER_H

#include <ctime>
#include <Ecore.h>
#include "Util/ITimer.h"

class BoxUpdateTimer: public ITimer {
    public:
        static ITimerPtr create(float period, Ecore_Task_Cb callback, void* data)
        {
            return ITimerPtr(new BoxUpdateTimer(period, callback, data));
        };
        void start();
        // instant timer for web-provider box contorl system
        void continueTimer(float period);
        void stop();
        void resume();
        void pause();
        void restart();
        void setPeriod(float period);
        float getPeriod() { return m_period; }
        time_t getStartTime() { return m_startTime; }
        bool isRunning() { return m_bTimerRunning; }
        ~BoxUpdateTimer();

    private:
        explicit BoxUpdateTimer(float period, Ecore_Task_Cb callback, void* data);
        float m_period;
        Ecore_Task_Cb m_callback;
        void* m_data;
        Ecore_Timer* m_timer;
        time_t m_startTime;
        bool m_bTimerRunning;
};

#endif // BOX_UPDATE_TIMER_H
