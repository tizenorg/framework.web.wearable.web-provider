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
 * @file    Box.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef BOX_H
#define BOX_H

#include <string>
#include <memory>
#include <ctime>
#include <Ecore.h>
#include <Plugin/IBoxPluginFactory.h>
#include "Buffer/IRenderBuffer.h"
#include "View/IRenderView.h"
#include "Util/ITimer.h"
#include "BoxData.h"
#include "IBoxState.h"
#include "IBoxContext.h"
#include "IBox.h"
#include "Box.h"

class Box: public IBox, public IBoxContext {
    public:
        static IBoxPtr create(
                BoxInfoPtr boxInfo,
                IBoxPluginFactoryPtr factory,
                EwkContextPtr ewkContext)
        {
            return IBoxPtr(new Box(boxInfo, factory, ewkContext));
        };
        // IBox
        bool show();
        bool hide();
        bool resize(int width, int height);
        bool resume();
        bool pause(bool background);
        bool openGbar(int width, int height, int x, int y);
        bool closeGbar();
        bool update(std::string& contentInfo);
        bool changePeriod(float period);
        bool isCurrentTab();
        bool needToUpdate();
        void setNeedToUpdate();
        void setCurrent();
        bool isPaused();
        BoxInfoPtr getBoxInfo();

        ~Box();

    private:
        // IBoxContext
        void setState(IBoxStatePtr state);

        // static callbacks
        static Eina_Bool updateCallback(void* data);

        // constructor
        explicit Box(
                BoxInfoPtr boxInfo,
                IBoxPluginFactoryPtr factory,
                EwkContextPtr ewkContext);

        BoxInfoPtr m_boxInfo;
        IBoxPluginFactoryPtr m_factory;
        IRenderViewPtr m_renderView;
        ITimerPtr m_updateTimer;
        //IBoxStatePtr m_state;
        // flag for knowing this box is on current tab
        bool m_currentTab;
        // flag for knowing this box has been already paused
        bool m_paused;
        // flag for knowing this box should be updated when the box is resumed
        bool m_updateNeeded;
        // timestamp for updateTimer
        time_t m_remainUpdateRequestTime;
        bool m_showed;
};

#endif //BOX_H
