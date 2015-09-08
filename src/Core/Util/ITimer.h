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
 * @file    ITimer.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef I_TIMER_H
#define I_TIMER_H

#include <memory>

class ITimer {
    public:
        virtual void start() = 0;
        virtual void continueTimer(float period) = 0;
        virtual void stop() = 0;
        virtual void resume() = 0;
        virtual void pause() = 0;
        virtual void restart() = 0;
        virtual void setPeriod(float period) = 0;
        virtual float getPeriod() = 0;
        virtual time_t getStartTime() = 0;
        virtual bool isRunning() = 0;

        virtual ~ITimer() {};
};

typedef std::shared_ptr<ITimer> ITimerPtr;

#endif // I_TIMER_H
