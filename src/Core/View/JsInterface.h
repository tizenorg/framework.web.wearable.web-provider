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
 * @file    JsInterface.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef JS_INTERFACE_H
#define JS_INTERFACE_H

#include <string>
#include <map>
#include <Core/BoxData.h>
#include "Service/PeriodChanger.h"
#include "Service/MessageManager.h"
#include "RenderView.h"
#include "IJsInterface.h"

using namespace Service;

#define EXPORT_CLASS __attribute__ ((visibility("default")))

class EXPORT_CLASS JsInterface: public IJsInterface {
    public:
        static IJsInterfacePtr create(RenderView* renderView, BoxInfoPtr boxInfo)
        {
            return IJsInterfacePtr(new JsInterface(renderView, boxInfo));
        }
        bool process(std::string& uri);
        ~JsInterface();

    private:
        bool isBoxScheme(std::string& uri);
        bool handleReload();
        bool handleChangePeriod(float requestedPeriod = -1.0f);
        bool handleLaunchBrowser(std::string& url);
        bool handleSendMessage(
                MessageManager::ReceiverType receiver,
                std::string& message);
        bool handleScroll(bool start);
        std::string parse(std::string& uri, std::string& key);
        explicit JsInterface(RenderView* renderView, BoxInfoPtr boxInfo);

        // members for service
        std::shared_ptr<PeriodChanger> m_periodChanger;
        std::shared_ptr<MessageManager> m_messageManager;
        RenderView* m_renderView;
        BoxInfoPtr m_boxInfo;
};

#endif // JS_INTERFACE_H

