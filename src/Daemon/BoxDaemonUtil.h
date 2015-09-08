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
 * @file    BoxDaemonUtil.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef BOX_DAEMON_UTIL_H
#define BOX_DAEMON_UTIL_H

#include <string>

class BoxDaemonUtil {
    public:
        static bool launchApplication(std::string& boxId, std::string& instanceId);
        static std::string createWebInstanceId(std::string& boxId);

    private:
        static const std::string boxIdKey;
        static const std::string instanceIdKey;

};

#endif // BOX_DAEMON_UTIL_H
