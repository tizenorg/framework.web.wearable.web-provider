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
 * @file    RenderBuffer.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include <string>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <provider.h>
#include <provider_buffer.h>
#include <Core/Util/Log.h>
#include <Core/Util/Util.h>
#include "IRenderBuffer.h"
#include "RenderBuffer.h"

const char * ecoreevasdataforrenderbuffer = "ecoreevasrenderbufferforwebprovider";

RenderBuffer::RenderBuffer()
    : m_bufferAddr(NULL)
    , m_bufferInfo(NULL)
    , m_hwEnable(false)
    , m_webview(NULL)
{
    LogD("enter");
}

RenderBuffer::~RenderBuffer()
{
    LogD("enter");
	free();
}

bool RenderBuffer::allocate(bool hwEnable)
{
    bool ret;

    LogD("enter");

    m_hwEnable = hwEnable;

    if (m_bufferAddr) {
        free();
        m_bufferAddr = NULL;
    }

    if (hwEnable) {
        ret = bufferAllocationForGLRendering();
    } else
        ret = bufferAllocationForSWRendering();

    startCanvasUpdate();
    return true;
}

bool RenderBuffer::reallocate(int width, int height)
{
    LogD("enter");
    stopCanvasUpdate();

    // TODO This function should be implemented due to box resize operation
    setWidth(width);
    setHeight(height);

    Ecore_Evas* ee = ecore_evas_ecore_evas_get(m_canvas);
    // resize function will invoke the freeCallback and allocateCallback again. (internally)
    ecore_evas_resize(ee, getWidth(), getHeight());
    evas_object_resize(m_win, getWidth(), getHeight());

    if (m_hwEnable)
    {
        bufferReAllocation(width, height);
    }

    startCanvasUpdate();
    return true;
}

bool RenderBuffer::free()
{
    if (!m_canvas) {
        return false;
    }

    stopCanvasUpdate();
    ecore_evas_free(ecore_evas_ecore_evas_get(m_canvas));
    m_canvas = NULL;
    m_win = NULL;

    return true;
}

void RenderBuffer::startCanvasUpdate()
{
    LogD("enter");
    evas_event_callback_del(
            m_canvas,
            EVAS_CALLBACK_RENDER_PRE,
            preRenderCallback);

    evas_event_callback_del(
            m_canvas,
            EVAS_CALLBACK_RENDER_POST,
            postRenderCallback);

    evas_event_callback_add(
            m_canvas,
            EVAS_CALLBACK_RENDER_PRE,
            preRenderCallback, this);

    evas_event_callback_add(
            m_canvas,
            EVAS_CALLBACK_RENDER_POST,
            postRenderCallback, this);

}

void RenderBuffer::stopCanvasUpdate()
{
    LogD("enter");
    evas_event_callback_del(
            m_canvas,
            EVAS_CALLBACK_RENDER_PRE,
            preRenderCallback);

    evas_event_callback_del(
            m_canvas,
            EVAS_CALLBACK_RENDER_POST,
            postRenderCallback);
}

Evas_Object* RenderBuffer::getWindow()
{
    return m_win;
}

void RenderBuffer::preRenderCallback(void* data, Evas* canvas, void *eventInfo)
{
    UNUSED_PARAM(eventInfo);

    RenderBuffer *buffer = static_cast<RenderBuffer*>(data);

    if ((buffer->m_hwEnable == true)) {
        return;
    }

     provider_buffer_pre_render(buffer->m_bufferInfo);
     //No need to re-draw after comment out evas_data_argb_unpremul function.
     //evas_damage_rectangle_add(canvas, 0, 0, buffer->getWidth(), buffer->getHeight());
}

void RenderBuffer::postRenderCallback(void* data, Evas* canvas, void* eventInfo)
{
    UNUSED_PARAM(canvas);
    UNUSED_PARAM(eventInfo);

    RenderBuffer* buffer = static_cast<RenderBuffer*>(data);

    if (buffer->m_hwEnable == true) {
        buffer->updateBuffer();
    } else {
        //To resolve font jaggies issue, comment out below line
        //evas_data_argb_unpremul(static_cast<unsigned int*>(buffer->m_bufferAddr), buffer->getWidth() * buffer->getHeight());
#ifdef RENDER_BUFFER_VERIFY_SHOT
        {
            FILE *fp;
            static int idx = 0;
            char filename[256];
            snprintf(filename, sizeof(filename) - 1, "/tmp/render%d-%dx%d.raw", idx++, buffer->getWidth(), buffer->getHeight());
            fp = fopen(filename, "w+");
            if (fp) {
                LogD("RenderShot: %s(%d)\n", filename, buffer->getWidth() * buffer->getHeight() * sizeof(int));
                fwrite(buffer->m_bufferAddr, buffer->getWidth() * buffer->getHeight() * sizeof(int), 1, fp);
                fclose(fp);
            } else {
                LogD("Failed to open a file: %s", filename);
            }
            LogD("/tmp/render-%dx%d.raw", buffer->getWidth(), buffer->getHeight());
        }
#endif

        if (!provider_buffer_pixmap_is_support_hw(buffer->m_bufferInfo)) {
            provider_buffer_sync(buffer->m_bufferInfo);
            buffer->updateBuffer();
        } else {
            provider_buffer_post_render(buffer->m_bufferInfo);
            buffer->updateBuffer();
        }
    }
}

void RenderBuffer::paintColor(unsigned int color)
{
    LogD("enter");

    if (!provider_buffer_pixmap_is_support_hw(m_bufferInfo)) {
        memset(m_bufferAddr, color, getWidth() * getHeight() * 4);
        provider_buffer_sync(m_bufferInfo);
        updateBuffer();
    } else {
        preRenderCallback(this, m_canvas, NULL);
        memset(m_bufferAddr, color, getWidth() * getHeight() * 4);
        postRenderCallback(this, m_canvas, NULL);
    }
}

Evas* RenderBuffer::getCanvas()
{
    return m_canvas;
}

void* RenderBuffer::allocateCallback(void* data, int size)
{
    LogD("enter");
    UNUSED_PARAM(size);

    RenderBuffer* buffer = static_cast<RenderBuffer*>(data);

    if (buffer->m_bufferInfo) {
        freeCallback(data, NULL);
    }

    buffer->m_bufferInfo = buffer->acquireBuffer();
    if (!buffer->m_bufferInfo) {
        return NULL;
    }

    // set buffer address
    if (!provider_buffer_pixmap_is_support_hw(buffer->m_bufferInfo)) {
        LogD("s/w evas backend");
        buffer->m_bufferAddr = provider_buffer_ref(buffer->m_bufferInfo);
    } else {
        LogD("h/w evas backend");
        int ret = provider_buffer_pixmap_create_hw(buffer->m_bufferInfo);
        if (ret < 0) {
            LogD("can't create hw pixmap");
        }
        buffer->m_bufferAddr = provider_buffer_pixmap_hw_addr(buffer->m_bufferInfo);
    }

    LogD("success to allocate buffer");
    return buffer->m_bufferAddr;
}

void RenderBuffer::freeCallback(void* data, void *pix)
{
    LogD("enter");
    UNUSED_PARAM(pix);

    RenderBuffer* buffer = static_cast<RenderBuffer*>(data);

    // destroy buffer
    if (!provider_buffer_pixmap_is_support_hw(buffer->m_bufferInfo)) {
        provider_buffer_unref(buffer->m_bufferAddr);
    } else {
        provider_buffer_pixmap_destroy_hw(buffer->m_bufferInfo);
    }

    provider_buffer_release(buffer->m_bufferInfo);

    buffer->m_bufferInfo = NULL;
    buffer->m_bufferAddr = NULL;

    LogD("success to free buffer");
    return;
}

Evas_Object *RenderBuffer::getSnapshot(void)
{
    LogD("enter");
    Evas_Object *snapshot;
    void *tmpBuffer;

    snapshot = evas_object_image_add(m_canvas);
    if (!snapshot)
        return NULL;
    evas_object_image_data_set(snapshot, NULL);
    evas_object_image_colorspace_set(snapshot, EVAS_COLORSPACE_ARGB8888);
    evas_object_image_alpha_set(snapshot, EINA_TRUE);
    evas_object_image_size_set(snapshot, getWidth(), getHeight());

    tmpBuffer = malloc(getWidth() * getHeight() * sizeof(int));
    if (tmpBuffer) {
        memcpy(tmpBuffer, m_bufferAddr, getWidth() * getHeight() * sizeof(int));
        evas_data_argb_premul(
                static_cast<unsigned int*>(tmpBuffer),
                getWidth() * getHeight());
        evas_object_image_data_set(snapshot, tmpBuffer);
    } else {
        LogD("Failed to allocate heap");
    }

    evas_object_image_data_update_add(snapshot, 0, 0, getWidth(), getHeight());
    evas_object_image_fill_set(snapshot, 0, 0, getWidth(), getHeight());
    evas_object_resize(snapshot, getWidth(), getHeight());

    return snapshot;
}

bool RenderBuffer::bufferAllocationForGLRendering()
{
    Ecore_Evas* ee = NULL;

    ee = ecore_evas_gl_x11_new(NULL, 0, 0, 0, getWidth(), getHeight());

    LogD("Using %s engine!", ecore_evas_engine_name_get(ee));

    if (!ee) {
        LogD("invalid ecore evas object");
        return false;
    }

    m_bufferInfo = acquireBuffer();
    if (!m_bufferInfo) {
        return NULL;
    }

    if (!provider_buffer_pixmap_is_support_hw(m_bufferInfo)) {
        LogD("s/w evas backend");
        m_bufferAddr = provider_buffer_ref(m_bufferInfo);
    } else {
        LogD("h/w evas backend");
        int ret = provider_buffer_pixmap_create_hw(m_bufferInfo);
        if (ret < 0) {
            LogD("can't create hw pixmap");
        }

        m_bufferAddr = (void *)provider_buffer_pixmap_id(m_bufferInfo);
        LogD("evas ecore provider_buffer_pixmap_id m_bufferAddr %lu", (unsigned long)m_bufferAddr);
    }

    ecore_evas_activate(ee);

    Evas* e = ecore_evas_get(ee);
    evas_image_cache_flush(e);
    Evas_Object *eo = evas_object_rectangle_add(e);
    evas_object_size_hint_weight_set(eo, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_color_set(eo, 0, 0, 0, 1);
    evas_object_resize(eo, getWidth(), getHeight());

    m_canvas = e;
    m_win = eo;

    return true;
}

bool RenderBuffer::bufferAllocationForSWRendering()
{
    Ecore_Evas* ee = NULL;

    ee = ecore_evas_buffer_allocfunc_new(
                getWidth(), getHeight(),
                allocateCallback, freeCallback,
                this);

    LogD("Using %s engine!", ecore_evas_engine_name_get(ee));

    if (!ee) {
        LogD("invalid ecore evas object");
        return false;
    }

    // alpha_set function access the canvas buffer directly,
    //  without pre/post render callback.
    provider_buffer_pre_render(m_bufferInfo);
    ecore_evas_alpha_set(ee, EINA_TRUE);
    provider_buffer_post_render(m_bufferInfo);
    ecore_evas_manual_render_set(ee, EINA_FALSE);

    // resize function will invoke the freeCallback and allocateCallback again. (internally)
    ecore_evas_resize(ee, getWidth(), getHeight());
    ecore_evas_show(ee);


    ecore_evas_activate(ee);

    Evas* e = ecore_evas_get(ee);
    evas_image_cache_flush(e);
    Evas_Object *eo = evas_object_rectangle_add(e);
    evas_object_size_hint_weight_set(eo, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_color_set(eo, 0, 0, 0, 1);
    evas_object_resize(eo, getWidth(), getHeight());

    m_canvas = e;
    m_win = eo;

    return true;
}

void RenderBuffer::bufferReAllocation(int width, int height)
{
    LogD("enter");
    UNUSED_PARAM(width);
    UNUSED_PARAM(height);

    provider_buffer_release(m_bufferInfo);

    m_bufferInfo = acquireBuffer();
    if (!m_bufferInfo) {
        return;
    }

    // set buffer address
    if (!provider_buffer_pixmap_is_support_hw(m_bufferInfo)) {
        LogD("s/w evas backend");
        m_bufferAddr = provider_buffer_ref(m_bufferInfo);
    } else {
        LogD("h/w evas backend");
        int ret = provider_buffer_pixmap_create_hw(m_bufferInfo);
        if (ret < 0) {
            LogD("can't create hw pixmap");
        }
        m_bufferAddr = (void *)provider_buffer_pixmap_id(m_bufferInfo);
        LogD("evas ecore provider_buffer_pixmap_id m_bufferAddr %lu", (unsigned long)m_bufferAddr);
     }
}

