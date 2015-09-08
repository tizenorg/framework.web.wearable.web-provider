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
 * @file    BoxRenderBuffer.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include <string>
#include <Ecore.h>
#include <Evas.h>
#include <widget_provider.h>
#include <widget_provider_buffer.h>
#include <Core/Util/Log.h>
#include <Core/Util/Util.h>
#include <API/web_provider_widget_info.h>
#include "RenderBuffer.h"
#include "BoxRenderBuffer.h"

BoxRenderBuffer::BoxRenderBuffer(BoxInfoPtr boxInfo, TouchViewCallback touchCallback)
    : m_boxId(boxInfo->boxId)
    , m_instanceId(boxInfo->instanceId)
    , m_contentInfo(boxInfo->contentInfo)
    , m_width(boxInfo->boxWidth)
    , m_height(boxInfo->boxHeight)
    , m_touchCallback(touchCallback)
{
}

BoxRenderBuffer::~BoxRenderBuffer()
{
}

BufferInfoPtr BoxRenderBuffer::acquireBuffer()
{
    LogD("enter");

    bool touchable = true;
    char* type = web_provider_widget_get_box_type(m_boxId.c_str());
    const char* defaultType = web_provider_widget_get_default_type();
    if (type) {
        touchable = web_provider_widget_get_mouse_event(m_boxId.c_str()) ||
        strcmp(type, defaultType);
    }

    std::free(type);

    BufferInfoPtr bufferInfo =
        widget_provider_buffer_create(
                WIDGET_TYPE_WIDGET,
                m_boxId.c_str(),
                m_instanceId.c_str(),
                1,
                touchable ? handleTouchEventCallback : NULL,
                this);

    if( bufferInfo == NULL )
        return NULL;

    if (widget_provider_buffer_acquire(
                bufferInfo,
                m_width,
                m_height,
                sizeof(int))) {
        widget_provider_buffer_destroy(bufferInfo);
		bufferInfo = NULL;
    }

    return bufferInfo;
}

void BoxRenderBuffer::updateBuffer()
{
    widget_damage_region_s region;
    region.x = 0;
    region.y = 0;
    region.w = m_width;
    region.h = m_height;

    widget_provider_send_updated(
        m_boxId.c_str(),
        m_instanceId.c_str(),
        WIDGET_PRIMARY_BUFFER,
        &region,
        0,
        NULL);
}

int BoxRenderBuffer::handleTouchEventCallback(
        BufferInfoPtr bufferInfo,
        struct widget_buffer_event_data *evt,
        void* data)
{
    LogD("enter");
    UNUSED_PARAM(bufferInfo);

    BoxRenderBuffer* This = static_cast<BoxRenderBuffer*>(data);
    if( This == NULL )
        return 0;

    char* boxType = web_provider_widget_get_box_type(This->m_boxId.c_str());
    TouchType type;
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
		std::free(boxType);
        return -1;
    }

    if(boxType) {
        This->didHandleTouchEvent(type, evt->timestamp, evt->info.pointer.x, evt->info.pointer.y);
    }

    // call touch callback of renderView
    This->m_touchCallback(evt->info.pointer.x, evt->info.pointer.y);

	std::free(boxType);

    return 0;
}

void BoxRenderBuffer::didHandleTouchEvent(
        TouchType type, double timestamp, int x, int y)
{
    UNUSED_PARAM(timestamp);
    unsigned int flags;

    // timestamp format sent by viewer is not same to the timestamp format used by webkit-efl
    // so web-provider should get timestamp using ecore_time_get()
    // and then feed event with the timestamp to webkit

    LogD("enter");
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
