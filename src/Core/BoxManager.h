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
 * @file    BoxManager.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef BOX_MANAGER_H
#define BOX_MANAGER_H

#include <map>
#include <string>
#include <memory>
#include <vconf.h>
#include <Plugin/box_plugin_interface.h>
#include <Plugin/IBoxPluginFactory.h>
#include "BoxData.h"
#include "BoxLoadBalancer.h"
#include "IBox.h"
#include "IBoxManager.h"

#define EXPORT_CLASS    __attribute__ ((visibility("default")))

class EXPORT_CLASS BoxManager: public IBoxManager {
    public:
        static IBoxManagerPtr create(IBoxPluginFactoryPtr factory)
        {
            return IBoxManagerPtr(new BoxManager(factory));
        };
        virtual bool doCommand(const request_cmd_type type, const BoxInfoPtr& boxInfo);
        virtual ~BoxManager();

    protected:
        virtual bool requestAddBox(BoxInfoPtr boxInfo, EwkContextPtr ewkContext);
        virtual bool requestRemoveBox(std::string& instanceId); // deprecated
        virtual bool requestRemoveBoxByInstanceId(BoxInfoPtr boxInfo);
        virtual bool requestRemoveBoxByBoxId(BoxInfoPtr boxInfo);
        virtual bool requestResizeBox(std::string& instanceId, int width, int height);
        virtual bool requestResumeBox(std::string& instanceId);
        virtual bool requestPauseBox(std::string& instanceId);
        virtual bool requestResumeAll();
        virtual bool requestPauseAll();
        virtual bool requestOpenGbar(
                        std::string& instanceId,
                        int width, int height, int x, int y);
        virtual bool requestCloseGbar(std::string& instanceId);
        virtual bool requestChangePeriod(std::string& instanceId, float period);
        virtual bool requestUpdateBox(std::string& boxId, std::string& appContentInfo);
        virtual bool requestUpdateAll(std::string& instanceId);
        virtual bool requestUpdateAppBox();

        virtual void updateEwkContext(std::string& boxId);
        int getBoxCount(std::string appId);

        // ewk context deleter
        struct EwkContextDeleter {
            void operator()(Ewk_Context* ptr);
        };

        explicit BoxManager(IBoxPluginFactoryPtr factory);
        IBoxPtr searchBoxMap(std::string& instanceId);

    private:
        void initEwkContextSetting(EwkContextPtr ewkContext);
        // map operations
        void insertBoxMap(std::string& instanceId, IBoxPtr box);
        void eraseBoxMap(std::string& instanceId);
        void updateBoxMap(std::string& instanceId, IBoxPtr box);
        void clearBoxMap();
        static void proxyChangedCallback(keynode_t* keynode, void* data);


        typedef std::map<std::string, IBoxPtr> BoxMap;
        typedef std::pair<std::string, IBoxPtr> BoxMapPair;
        BoxMap m_boxMap;
        IBoxPluginFactoryPtr m_boxFactory;
        EwkContextPtr m_defaultContext;

        BoxLoadBalancer m_boxLoadBalancer;
};

#endif // BOX_MANAGER_H
