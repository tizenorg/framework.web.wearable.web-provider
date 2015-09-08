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
 * @file    BoxDaemon.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef BOX_DAEMON_H
#define BOX_DAEMON_H

#include <string>
#include <memory>
#include <app.h>
#include <Core/Util/Noncopyable.h>

class BoxDaemonImpl;

class BoxDaemon: Noncopyable {
    public:
        bool start(std::string& name, app_control_h app_control);
        bool stop();
        bool handleAppControl(app_control_h app_control);

        explicit BoxDaemon();
        ~BoxDaemon();

    private:
        std::shared_ptr<BoxDaemonImpl> m_impl;
};

#endif //BOX_DAEMON_H
