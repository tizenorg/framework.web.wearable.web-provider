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
 * @file    box_plugin_interface.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include <memory>
#include <Core/BoxData.h>
#include <Core/Util/Log.h>
#include <Plugin/box_plugin_interface.h>
#include "AppBoxManager.h"
#include "AppBoxPluginFactory.h"

static std::shared_ptr<IBoxManager> g_manager;

int web_provider_plugin_interface_initialize()
{
    LogD("enter");
    IBoxPluginFactoryPtr factory(new AppBoxPluginFactory());
    g_manager = AppBoxManager::create(factory);

    return 0;
}

int web_provider_plugin_interface_command(const request_cmd_type type, const BoxInfoPtr& boxInfo)
{
    LogD("enter");
    int ret = g_manager->doCommand(type, boxInfo);   

    if (!ret) {
        return -1;
    }

    return 0;
}

int web_provider_plugin_interface_shutdown()
{
    LogD("enter");
    g_manager.reset();
    return 0;
}
