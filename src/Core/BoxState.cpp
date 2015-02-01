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
 * @file    BoxState.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include "IBoxContext.h"
#include "IBoxState.h"
#include "BoxState.h"

// BoxState
void BoxState::switchState()
{
    // TODO this creation may be wrong..
    m_context->setState(IBoxStatePtr(this));
}

void BoxState::setContext(IBoxContextPtr context)
{
    m_context = context;
}

IBoxContextPtr BoxState::getContext()
{
    return m_context;
}

// BoxReadyState
IBoxStatePtr BoxInitState::permitShow()
{
    return IBoxStatePtr(BoxShowState::create(getContext()));
}

// BoxShowState
IBoxStatePtr BoxShowState::permitShow()
{
    // In this case, existing state needn't to be changed
    return IBoxStatePtr(this);
}

IBoxStatePtr BoxShowState::permitHide()
{
    return IBoxStatePtr(BoxHideState::create(getContext()));
}

IBoxStatePtr BoxShowState::permitOpenPd()
{
    return IBoxStatePtr(BoxOpenPdState::create(getContext()));
}

IBoxStatePtr BoxShowState::permitPause()
{
    return IBoxStatePtr(BoxPauseState::create(getContext()));
}

// BoxHideState
IBoxStatePtr BoxHideState::permitShutdown()
{
    // In this case, existing state needn't to be changed
    // because there is no state to be changed from Hide State
    return IBoxStatePtr(this);
}

// BoxOpenPdState
IBoxStatePtr BoxOpenPdState::permitClosePd()
{
    return IBoxStatePtr(BoxClosePdState::create(getContext()));
}

// BoxClosePdState
IBoxStatePtr BoxClosePdState::permitShow()
{
    return IBoxStatePtr(BoxShowState::create(getContext()));
}

// BoxPauseState
IBoxStatePtr BoxPauseState::permitResume()
{
    return IBoxStatePtr(BoxResumeState::create(getContext()));
}

IBoxStatePtr BoxPauseState::permitHide()
{
    return IBoxStatePtr(BoxHideState::create(getContext()));
}

// BoxResumeState
IBoxStatePtr BoxResumeState::permitShow()
{
    return IBoxStatePtr(BoxShowState::create(getContext()));
}

IBoxStatePtr BoxResumeState::permitHide()
{
    return IBoxStatePtr(BoxHideState::create(getContext()));
}
