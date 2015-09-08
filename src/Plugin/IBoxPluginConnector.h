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
 * @file    IBoxPluginConnector.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef I_BOX_PLUGIN_CONNECTOR_H
#define I_BOX_PLUGIN_CONNECTOR_H

#include <memory>
#include <Core/BoxData.h>
#include <Core/Util/Noncopyable.h>
#include "box_plugin_interface.h"

class IBoxPluginConnector: Noncopyable {
    public:
        virtual bool initialize() = 0;
        virtual bool shutdown() = 0;
        virtual bool requestCommand(
                const request_cmd_type type, const BoxInfoPtr& boxInfo) = 0;
        virtual std::string getBoxType(const std::string& serviceBoxId) = 0;
        virtual ~IBoxPluginConnector() {};
};

typedef std::shared_ptr<IBoxPluginConnector> IBoxPluginConnectorPtr;

#endif // I_BOX_PLUGIN_CONNECTOR_H
