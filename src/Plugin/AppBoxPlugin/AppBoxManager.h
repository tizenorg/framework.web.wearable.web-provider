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
 * @file    AppBoxManager.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef APP_BOX_MANAGER_H
#define APP_BOX_MANAGER_H

#include <vconf.h>

#include <Core/IBox.h>
#include <Core/IBoxManager.h>
#include <Core/BoxManager.h>
#include <Core/BoxData.h>
#include <Plugin/IBoxPluginFactory.h>


class AppBoxManager: public BoxManager {
    public:
        static IBoxManagerPtr create(IBoxPluginFactoryPtr factory) 
        { 
            return IBoxManagerPtr(new AppBoxManager(factory)); 
        };
        ~AppBoxManager();

    private:
        // BoxManager implementation
        bool requestAddBox(BoxInfoPtr boxInfo, EwkContextPtr ewkContext);
        void updateEwkContext(std::string& boxId);

        EwkContextPtr getAvailableEwkContext(const std::string& appId);
        void insertContextMap(std::string& appId, EwkContextPtr ewkContext);
        void eraseContextMap(std::string& appId);
        std::string getBaseExecutablePath(const std::string& appId);
        explicit AppBoxManager(IBoxPluginFactoryPtr factory);
        static void proxyChangedCallback(keynode_t* keynode, void* data);


        // members
        typedef std::map<std::string, EwkContextPtr> EwkContextMap;
        typedef std::pair<std::string, EwkContextPtr> EwkContextMapPair;
        EwkContextMap m_ewkContextMap;
};


#endif //  APP_BOX_MANAGER_H
