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
 * @file    IBox.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef I_BOX_H
#define I_BOX_H

#include <string>
#include <memory>
#include <ctime>
#include <EWebKit.h>
#include <EWebKit_internal.h>

class IBox {
    public:
        // functions for lifecycle
        virtual bool show() = 0;
        virtual bool hide() = 0;
        virtual bool resize(int width, int height) = 0;
        virtual bool resume() = 0;
        virtual bool pause(bool background) = 0;
        virtual bool openGbar(int width, int height, int x, int y) = 0;
        virtual bool closeGbar() = 0;
        virtual bool update(std::string& contentInfo) = 0;
        virtual bool needToUpdate() = 0;
        virtual void setNeedToUpdate() = 0;
        virtual bool isPaused() = 0;

        // functions for getting/setting box's data by BoxManager
        virtual bool changePeriod(float period) = 0;
        virtual bool isCurrentTab() = 0;
        virtual void setCurrent() = 0;
        virtual BoxInfoPtr getBoxInfo() = 0;

        //virtual IBox& operator=(const IBox& rhs) = 0;
        //virtual bool operator==(const IBox& rhs) const = 0;
        //virtual bool operator!=(const IBox& rhs) const = 0;
        virtual ~IBox() {};
};

typedef std::shared_ptr<IBox> IBoxPtr;
typedef std::shared_ptr<Ewk_Context> EwkContextPtr;

#endif //I_BOX_H
