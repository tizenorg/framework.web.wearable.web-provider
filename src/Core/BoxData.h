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
 * @file    BoxData.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef BOX_DATA_H
#define BOX_DATA_H

#include <memory>
#include <string>

struct BoxInfo
{
    std::string boxType;
    std::string boxId;
    std::string instanceId;
    int boxWidth;
    int boxHeight;
    int pdWidth;
    int pdHeight;
    int pdX;
    int pdY;
    int priority;
    float period;
    // contentInfo is used differently per box type
    std::string contentInfo;
    // appContentInfo is used for saving custom data from box's app
    std::string appContentInfo;

    // initialization
    BoxInfo(std::string boxType,
            std::string boxId,
            std::string instanceId) :
        boxType(boxType),
        boxId(boxId),
        instanceId(instanceId),
        boxWidth(0),
        boxHeight(0),
        pdWidth(0),
        pdHeight(0),
        pdX(0),
        pdY(0),
        priority(0),
        period(0),
        contentInfo(),
        appContentInfo()
    {
    };

   BoxInfo() :
        boxType(),
        boxId(),
        instanceId(),
        boxWidth(0),
        boxHeight(0),
        pdWidth(0),
        pdHeight(0),
        pdX(0),
        pdY(0),
        priority(0),
        period(0),
        contentInfo(),
        appContentInfo()
    {
    };
};

typedef std::shared_ptr<struct BoxInfo> BoxInfoPtr;

#endif // BOX_DATA_H
