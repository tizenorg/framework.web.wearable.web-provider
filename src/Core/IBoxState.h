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
 * @file    IBoxState.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef I_BOX_STATE
#define I_BOX_STATE

#include <memory>

class IBoxState;
typedef std::shared_ptr<IBoxState> IBoxStatePtr;

class IBoxState {
    public:
        virtual IBoxStatePtr permitShow() = 0;
        virtual IBoxStatePtr permitHide() = 0;
        virtual IBoxStatePtr permitResume() = 0;
        virtual IBoxStatePtr permitPause() = 0;
        virtual IBoxStatePtr permitOpenGbar() = 0;
        virtual IBoxStatePtr permitCloseGbar() = 0;
        virtual IBoxStatePtr permitShutdown() = 0;
        virtual void switchState() = 0;
        virtual ~IBoxState() {};
};

#endif // I_BOX_STATE
