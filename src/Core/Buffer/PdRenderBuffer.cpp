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
 * @file    PdRenderBuffer.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */

#include "PdRenderBuffer.h"

#include "config.h"

#include <string>
#include <Ecore.h>
#include <Evas.h>
#include <EWebKit.h>
#include <EWebKit_internal.h>
#include <provider.h>
#include <Elementary.h>
#include <dynamicbox_service.h>
#include <provider_buffer.h>
#include <Core/Util/Log.h>
#include <Core/Util/Util.h>
#include "RenderBuffer.h"

PdRenderBuffer::PdRenderBuffer(BoxInfoPtr boxInfo, TouchViewCallback touchCallback)
    : m_boxId(boxInfo->boxId)
    , m_instanceId(boxInfo->instanceId)
    , m_width(boxInfo->pdWidth)
    , m_height(boxInfo->pdHeight)
    , m_touchCallback(touchCallback)
{
}

PdRenderBuffer::~PdRenderBuffer()
{
}

BufferInfoPtr PdRenderBuffer::acquireBuffer()
{
    BufferInfoPtr bufferInfo =
        provider_buffer_create(
                TYPE_PD,
                m_boxId.c_str(),
                m_instanceId.c_str(),
                1,
                handleTouchEventCallback,
                this);
    if( bufferInfo == NULL )
        return NULL;
    if (provider_buffer_acquire(
                bufferInfo,
                m_width,
                m_height,
                sizeof(int))) {
        provider_buffer_destroy(bufferInfo);
		bufferInfo = NULL;
    }

    m_accessibiilty = false;
    return bufferInfo;
}

void PdRenderBuffer::updateBuffer()
{
    LogD("enter");
    provider_send_desc_updated(
            m_boxId.c_str(),
            m_instanceId.c_str(),
            NULL);
}

int PdRenderBuffer::handleTouchEventCallback(
        BufferInfoPtr bufferInfo,
        struct buffer_event_data *evt,
        void* data)
{
    LogD("enter");
    UNUSED_PARAM(bufferInfo);

    PdRenderBuffer* This = static_cast<PdRenderBuffer*>(data);

    if( This == NULL )
        return 0;

    TouchType type;
    Elm_Access_Action_Info ActionInfo;

#if !ENABLE(WEBKIT_UPVERSION)
    switch (evt->type) {
    case BUFFER_EVENT_HIGHLIGHT:
    case BUFFER_EVENT_HIGHLIGHT_NEXT:
    case BUFFER_EVENT_HIGHLIGHT_PREV:
    case BUFFER_EVENT_ACTIVATE:
    case BUFFER_EVENT_ACTION_UP:
    case BUFFER_EVENT_ACTION_DOWN:
    case BUFFER_EVENT_SCROLL_UP:
    case BUFFER_EVENT_SCROLL_MOVE:
    case BUFFER_EVENT_SCROLL_DOWN:
    case BUFFER_EVENT_UNHIGHLIGHT:
    {
        LogD("handleTouchEventCallback event %d x,y %f,%f", event, x,y);
        ActionInfo.highlight_cycle = EINA_FALSE;

        if (!This->getWebView())
            return 0;

        Ewk_View_Smart_Data* pSmartData = (Ewk_View_Smart_Data*) evas_object_smart_data_get(This->getWebView());

        switch (evt->type) {
        case BUFFER_EVENT_HIGHLIGHT:
            {
                ActionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT;
                ActionInfo.action_by = ELM_ACCESS_ACTION_HIGHLIGHT;
                if (pSmartData->api->screen_reader_action_execute(pSmartData, &ActionInfo)) {
                    provider_send_access_status(This->m_boxId.c_str(), This->m_instanceId.c_str(), LB_ACCESS_STATUS_DONE);
                } else {
                    provider_send_access_status(This->m_boxId.c_str(), This->m_instanceId.c_str(), LB_ACCESS_STATUS_DONE);
                }
                ActionInfo.action_type = ELM_ACCESS_ACTION_READ;
                break;
            }
        case BUFFER_EVENT_HIGHLIGHT_NEXT:
            if (!This->m_accessibiilty) {
                ActionInfo.action_by = ELM_ACCESS_ACTION_HIGHLIGHT;
                ActionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT;
                if (pSmartData->api->screen_reader_action_execute(pSmartData, &ActionInfo)) {
                    provider_send_access_status(This->m_boxId.c_str(), This->m_instanceId.c_str(), LB_ACCESS_STATUS_DONE);
                } else {
                    provider_send_access_status(This->m_boxId.c_str(), This->m_instanceId.c_str(), LB_ACCESS_STATUS_DONE);
                }
                ActionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT_NEXT;
                This->m_accessibiilty = true;
            } else {
                ActionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT_NEXT;
            }
            break;
        case BUFFER_EVENT_HIGHLIGHT_PREV:
            if (!This->m_accessibiilty) {
                ActionInfo.action_by = ELM_ACCESS_ACTION_HIGHLIGHT;
                ActionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT;
                if (pSmartData->api->screen_reader_action_execute(pSmartData, &ActionInfo)) {
                    provider_send_access_status(This->m_boxId.c_str(), This->m_instanceId.c_str(), LB_ACCESS_STATUS_DONE);
                } else {
                    provider_send_access_status(This->m_boxId.c_str(), This->m_instanceId.c_str(), LB_ACCESS_STATUS_DONE);
                }
                ActionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT_PREV;
                This->m_accessibiilty = true;
            } else {
                ActionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT_PREV;
            }
            break;
        case BUFFER_EVENT_ACTIVATE:
            ActionInfo.action_type = ELM_ACCESS_ACTION_ACTIVATE;
            break;
        case BUFFER_EVENT_UNHIGHLIGHT:
            ActionInfo.action_type = ELM_ACCESS_ACTION_UNHIGHLIGHT;
            break;
        case BUFFER_EVENT_ACTION_UP:
        case BUFFER_EVENT_ACTION_DOWN:
        case BUFFER_EVENT_SCROLL_UP:
        case BUFFER_EVENT_SCROLL_MOVE:
        case BUFFER_EVENT_SCROLL_DOWN:
        default:
            break;
        }
        int xWebview,yWebview,w,h;
        evas_object_geometry_get(This->getWebView(), &xWebview, &yWebview, &w, &h);

        ActionInfo.x = xWebview + static_cast<int>(w*evt->info.pointer.x);
        ActionInfo.y = yWebview + static_cast<int>(h*evt->info.pointer.y);

        if (pSmartData->api->screen_reader_action_execute(pSmartData, &ActionInfo)) {
            provider_send_access_status(This->m_boxId.c_str(), This->m_instanceId.c_str(), LB_ACCESS_STATUS_DONE);
        } else {
            provider_send_access_status(This->m_boxId.c_str(), This->m_instanceId.c_str(), LB_ACCESS_STATUS_LAST);
            This->m_accessibiilty = false;
        }
        }
        return 0;
    default:
        break;
    }
#endif // WEBKIT_UPVERSION

    switch (evt->type) {
    case BUFFER_EVENT_MOVE:
        type = TOUCH_EVENT_MOVE;
        break;
    case BUFFER_EVENT_DOWN:
        type = TOUCH_EVENT_DOWN;
        break;
    case BUFFER_EVENT_UP:
        type = TOUCH_EVENT_UP;
        break;
    case BUFFER_EVENT_ON_HOLD:
        type = TOUCH_EVENT_ON_HOLD;
        break;
    default:
        type = TOUCH_EVENT_UNRECOGNIZED;
        break;
    }

    if (type == TOUCH_EVENT_UNRECOGNIZED) {
        return -1;
    }

    This->didHandleTouchEvent(type, evt->timestamp, evt->info.pointer.x, evt->info.pointer.y);

    // call touch callback of renderView
    This->m_touchCallback(evt->info.pointer.x, evt->info.pointer.y);

    return 0;
}

void PdRenderBuffer::didHandleTouchEvent(
        TouchType type, double timestamp, int x, int y)
{
    UNUSED_PARAM(timestamp);
    unsigned int flags;

    // timestamp format sent by viewer is not same to the timestamp format used by webkit-efl
    // so web-provider should get timestamp using ecore_time_get()
    // and then feed event with the timestamp to webkit

    switch (type) {
    case TOUCH_EVENT_MOVE:
        LogD("move event");
        evas_event_feed_mouse_move(
                getCanvas(), x, y, ecore_time_get() * 1000, NULL);
        break;
    case TOUCH_EVENT_DOWN:
        LogD("down event");
        flags = evas_event_default_flags_get(getCanvas());
        flags &= ~EVAS_EVENT_FLAG_ON_SCROLL;
        flags &= ~EVAS_EVENT_FLAG_ON_HOLD;
        evas_event_default_flags_set(getCanvas(), (Evas_Event_Flags)flags);

        evas_event_feed_mouse_move(
                getCanvas(), x, y, ecore_time_get() * 1000, NULL);
        evas_event_feed_mouse_down(
                getCanvas(), 1, EVAS_BUTTON_NONE, 0, NULL);
        break;
    case TOUCH_EVENT_UP:
        LogD("up event");
        evas_event_feed_mouse_up(
                getCanvas(), 1, EVAS_BUTTON_NONE, 0, NULL);
        break;
    case TOUCH_EVENT_ON_HOLD:
        LogD("onhold event");
        flags = (unsigned int)evas_event_default_flags_get(getCanvas());
        flags |= (unsigned int)EVAS_EVENT_FLAG_ON_HOLD;
        evas_event_default_flags_set(getCanvas(), (Evas_Event_Flags)flags);
        break;
    default:
        LogD("wrong event");
        break;
    }
}
