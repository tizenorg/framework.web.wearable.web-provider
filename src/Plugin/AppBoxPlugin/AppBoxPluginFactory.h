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
 * @file    AppBoxPluginFactory.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef APP_BOX_PLUGIN_FACTORY_H
#define APP_BOX_PLUGIN_FACTORY_H

#include <string>
#include <memory>
#include <Plugin/IBoxPluginFactory.h>
#include <Core/View/IRenderView.h>
#include <Core/Buffer/IRenderBuffer.h>
#include <EWebKit.h>
#include <EWebKit_internal.h>
#include <Evas.h>

// forward declaration
struct BoxInfo;

class AppBoxPluginFactory: public IBoxPluginFactory {
    public:
        IRenderViewPtr createRenderView(
                std::shared_ptr<BoxInfo> boxInfo,
                std::shared_ptr<Ewk_Context> ewkContext);

        AppBoxPluginFactory() {};
        ~AppBoxPluginFactory() {};
};

#endif //APP_BOX_PLUGIN_FACTORY_H
