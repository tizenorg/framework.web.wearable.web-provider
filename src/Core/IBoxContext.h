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
 * @file    IBoxContext.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef I_BOX_CONTEXT
#define I_BOX_CONTEXT

#include "IBoxState.h"

class IBoxContext {
    public:
        virtual void setState(IBoxStatePtr state) = 0;
        virtual ~IBoxContext() {};
};

typedef std::shared_ptr<IBoxContext> IBoxContextPtr;

#endif // I_BOX_CONTEXT
