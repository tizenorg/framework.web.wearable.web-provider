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
 * @file    BoxPluginConnector.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef BOX_PLUGIN_CONNECTOR_H
#define BOX_PLUGIN_CONNECTOR_H

#include <map>
#include <Core/BoxData.h>
#include "IBoxPluginConnector.h"
#include "box_plugin_interface.h"

class BoxPluginConnector: public IBoxPluginConnector {
    public: // IBoxPluginConnector
        static IBoxPluginConnectorPtr create() 
        { 
            return IBoxPluginConnectorPtr(new BoxPluginConnector()); 
        };
        virtual bool initialize();
        virtual bool shutdown();
        virtual bool requestCommand(
                const request_cmd_type type, const BoxInfoPtr& boxInfo);
        virtual std::string getBoxType(const std::string& serviceBoxId);
        virtual ~BoxPluginConnector();

    private:
        BoxPluginConnector();
        
        // type definition
        typedef std::map<std::string, std::shared_ptr<plugin_interfaces> > pluginMap;

        pluginMap m_pluginMap;

};

#endif // BOX_PLUGIN_CONNECTOR_H
