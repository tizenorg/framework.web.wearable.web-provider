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
 * @file    IBoxPluginFactory.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef I_BOX_PLUGIN_FACTORY_H
#define I_BOX_PLUGIN_FACTORY_H

#include <string>
#include <memory>
#include <Evas.h>
#include <EWebKit.h>
#include <EWebKit_internal.h>

// forward declaration
class IRenderView;
struct BoxInfo;

class IBoxPluginFactory {
    public:
        virtual std::shared_ptr<IRenderView> createRenderView(
                std::shared_ptr<BoxInfo> boxInfo,
                std::shared_ptr<Ewk_Context> ewkContext) = 0;
        virtual ~IBoxPluginFactory() {};
};

typedef std::shared_ptr<IBoxPluginFactory> IBoxPluginFactoryPtr;

#endif //I_BOX_PLUGIN_FACTORY_H
