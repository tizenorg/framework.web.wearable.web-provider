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
 * @file    ScrollHolder.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */

#include <string>
#include <widget_provider.h>
#include <Core/Util/Log.h>

namespace Service {
namespace ScrollHolder {

void holdHorizontalScroll(std::string& boxId, std::string& instanceId, bool start)
{
    LogD("enter"); 

    if (start) {
        LogD("scroll start");
        widget_provider_send_hold_scroll(boxId.c_str(), instanceId.c_str(), 1);
    } else {
        LogD("scroll stop");
        widget_provider_send_hold_scroll(boxId.c_str(), instanceId.c_str(), 0);
    }
}

} // AppControl
} // Service
