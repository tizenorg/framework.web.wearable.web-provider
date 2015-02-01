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
 * @file    IBoxManager.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef I_BOX_MANAGER_H
#define I_BOX_MANAGER_H

#include <string>
#include <Util/Noncopyable.h>
#include <Plugin/box_plugin_interface.h>
#include "BoxData.h"

class IBoxManager: Noncopyable {
    public:
        virtual bool doCommand(const request_cmd_type, const BoxInfoPtr&) = 0;
        virtual ~IBoxManager() {};
};

typedef std::shared_ptr<IBoxManager> IBoxManagerPtr;

#endif // I_BOX_MANAGER_H
