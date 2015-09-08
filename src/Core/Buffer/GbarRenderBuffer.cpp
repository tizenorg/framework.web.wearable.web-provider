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
 * @file    GbarRenderBuffer.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */

#include "GbarRenderBuffer.h"

#include "config.h"

#include <string>
#include <Ecore.h>
#include <Evas.h>
#include <EWebKit.h>
#include <EWebKit_internal.h>
#include <Elementary.h>
#include <widget_service.h>
#include <widget_provider.h>
#include <widget_provider_buffer.h>
#include <Core/Util/Log.h>
#include <Core/Util/Util.h>
#include "RenderBuffer.h"

GbarRenderBuffer::GbarRenderBuffer(BoxInfoPtr boxInfo, TouchViewCallback touchCallback)
    : m_boxId(boxInfo->boxId)
    , m_instanceId(boxInfo->instanceId)
    , m_width(boxInfo->gbarWidth)
    , m_height(boxInfo->gbarHeight)
    , m_touchCallback(touchCallback)
    , m_accessibiilty(false)
{
}

GbarRenderBuffer::~GbarRenderBuffer()
{
}

BufferInfoPtr GbarRenderBuffer::acquireBuffer()
{
   BufferInfoPtr bufferInfo =
        widget_provider_buffer_create(
                WIDGET_TYPE_GBAR,
                m_boxId.c_str(),
                m_instanceId.c_str(),
                1,
                handleTouchEventCallback,
                this);

    if( bufferInfo == NULL )
        return NULL;

    if (widget_provider_buffer_acquire(
                bufferInfo,
                m_width,
                m_height,
                sizeof(int))){
        widget_provider_buffer_destroy(bufferInfo);
        bufferInfo = NULL;
    }

   m_accessibiilty = false;
   return bufferInfo;
}

void GbarRenderBuffer::updateBuffer()
{
    LogD("enter");
    //TODO: check proper signature and arguments
    /*
    widget_provider_send_desc_updated(
            m_boxId.c_str(),
            m_instanceId.c_str(),
            NULL);
    */
}

int GbarRenderBuffer::handleTouchEventCallback(
        BufferInfoPtr bufferInfo,
        struct widget_buffer_event_data *evt,
        void* data)
{
    LogD("enter");
    UNUSED_PARAM(bufferInfo);

    GbarRenderBuffer* This = static_cast<GbarRenderBuffer*>(data);

    if( This == NULL )
        return 0;

    TouchType type;

#if !ENABLE(WEBKIT_UPVERSION)

    Elm_Access_Action_Info ActionInfo;

    switch (evt->type) {
    case WIDGET_BUFFER_EVENT_ACCESS_HIGHLIGHT:
    case WIDGET_BUFFER_EVENT_ACCESS_HIGHLIGHT_NEXT:
    case WIDGET_BUFFER_EVENT_ACCESS_HIGHLIGHT_PREV:
    case WIDGET_BUFFER_EVENT_ACCESS_ACTIVATE:
    case WIDGET_BUFFER_EVENT_ACCESS_ACTION_UP:
    case WIDGET_BUFFER_EVENT_ACCESS_ACTION_DOWN:
    case WIDGET_BUFFER_EVENT_ACCESS_SCROLL_UP:
    case WIDGET_BUFFER_EVENT_ACCESS_SCROLL_MOVE:
    case WIDGET_BUFFER_EVENT_ACCESS_SCROLL_DOWN:
    case WIDGET_BUFFER_EVENT_ACCESS_UNHIGHLIGHT:
    {
        LogD("handleTouchEventCallback event %d x,y %f,%f", evt->type, evt->info.pointer.x, evt->info.pointer.y);
        ActionInfo.highlight_cycle = EINA_FALSE;

        if (!This->getWebView())
            return 0;

        Ewk_View_Smart_Data* pSmartData = (Ewk_View_Smart_Data*) evas_object_smart_data_get(This->getWebView());

        switch (evt->type) {
        case WIDGET_BUFFER_EVENT_ACCESS_HIGHLIGHT:
            {
                ActionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT;
                ActionInfo.action_by = ELM_ACCESS_ACTION_HIGHLIGHT;
                if (pSmartData->api->screen_reader_action_execute(pSmartData, &ActionInfo)) {
                    widget_provider_send_access_status(This->m_boxId.c_str(), This->m_instanceId.c_str(), WIDGET_ACCESS_STATUS_DONE);
                } else {
                    widget_provider_send_access_status(This->m_boxId.c_str(), This->m_instanceId.c_str(), WIDGET_ACCESS_STATUS_DONE);
                }
                ActionInfo.action_type = ELM_ACCESS_ACTION_READ;
                break;
            }
        case WIDGET_BUFFER_EVENT_ACCESS_HIGHLIGHT_NEXT:
            if (!This->m_accessibiilty) {
                ActionInfo.action_by = ELM_ACCESS_ACTION_HIGHLIGHT;
                ActionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT;
                if (pSmartData->api->screen_reader_action_execute(pSmartData, &ActionInfo)) {
                    widget_provider_send_access_status(This->m_boxId.c_str(), This->m_instanceId.c_str(), WIDGET_ACCESS_STATUS_DONE);
                } else {
                    widget_provider_send_access_status(This->m_boxId.c_str(), This->m_instanceId.c_str(), WIDGET_ACCESS_STATUS_DONE);
                }
                ActionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT_NEXT;
                This->m_accessibiilty = true;
            } else {
                ActionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT_NEXT;
            }
            break;
        case WIDGET_BUFFER_EVENT_ACCESS_HIGHLIGHT_PREV:
            if (!This->m_accessibiilty) {
                ActionInfo.action_by = ELM_ACCESS_ACTION_HIGHLIGHT;
                ActionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT;
                if (pSmartData->api->screen_reader_action_execute(pSmartData, &ActionInfo)) {
                    widget_provider_send_access_status(This->m_boxId.c_str(), This->m_instanceId.c_str(), WIDGET_ACCESS_STATUS_DONE);
                } else {
                    widget_provider_send_access_status(This->m_boxId.c_str(), This->m_instanceId.c_str(), WIDGET_ACCESS_STATUS_DONE);
                }
                ActionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT_PREV;
                This->m_accessibiilty = true;
            } else {
                ActionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT_PREV;
            }
            break;
        case WIDGET_BUFFER_EVENT_ACCESS_ACTIVATE:
            ActionInfo.action_type = ELM_ACCESS_ACTION_ACTIVATE;
            break;
        case WIDGET_BUFFER_EVENT_ACCESS_UNHIGHLIGHT:
            ActionInfo.action_type = ELM_ACCESS_ACTION_UNHIGHLIGHT;
            break;
        case WIDGET_BUFFER_EVENT_ACCESS_ACTION_UP:
        case WIDGET_BUFFER_EVENT_ACCESS_ACTION_DOWN:
        case WIDGET_BUFFER_EVENT_ACCESS_SCROLL_UP:
        case WIDGET_BUFFER_EVENT_ACCESS_SCROLL_MOVE:
        case WIDGET_BUFFER_EVENT_ACCESS_SCROLL_DOWN:
        default:
            break;
        }
        int xWebview,yWebview,w,h;
        evas_object_geometry_get(This->getWebView(), &xWebview, &yWebview, &w, &h);

        ActionInfo.x = xWebview + static_cast<int>(w*evt->info.pointer.x);
        ActionInfo.y = yWebview + static_cast<int>(h*evt->info.pointer.y);

        if (pSmartData->api->screen_reader_action_execute(pSmartData, &ActionInfo)) {
            widget_provider_send_access_status(This->m_boxId.c_str(), This->m_instanceId.c_str(), WIDGET_ACCESS_STATUS_DONE);
        } else {
            widget_provider_send_access_status(This->m_boxId.c_str(), This->m_instanceId.c_str(), WIDGET_ACCESS_STATUS_LAST);
            This->m_accessibiilty = false;
        }
        }
        return 0;
    default:
        break;
    }
#endif // WEBKIT_UPVERSION

    switch (evt->type) {
    case WIDGET_BUFFER_EVENT_MOVE:
        type = TOUCH_EVENT_MOVE;
        break;
    case WIDGET_BUFFER_EVENT_DOWN:
        type = TOUCH_EVENT_DOWN;
        break;
    case WIDGET_BUFFER_EVENT_UP:
        type = TOUCH_EVENT_UP;
        break;
    case WIDGET_BUFFER_EVENT_ON_HOLD:
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

void GbarRenderBuffer::didHandleTouchEvent(
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