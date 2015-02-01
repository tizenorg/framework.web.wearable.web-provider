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
 * @file    BoxDaemon.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include <string>
#include <app.h>
#include "BoxDaemon.h"
#include "BoxDaemonImpl.h"

BoxDaemon::BoxDaemon()
    : m_impl(new BoxDaemonImpl())
{
}

BoxDaemon::~BoxDaemon()
{
}

bool BoxDaemon::start(std::string& name)
{
    return m_impl->start(name);
}

bool BoxDaemon::stop()
{
    return m_impl->stop();
}

bool BoxDaemon::handleAppControl(app_control_h app_control)
{
    return m_impl->handleAppControl(app_control);
}
