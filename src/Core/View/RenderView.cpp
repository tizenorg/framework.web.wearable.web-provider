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
 * @file    RenderView.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include "config.h"
#include "RenderView.h"

#include <string>
#include <sstream>
#include <functional>
#include <Ecore.h>
#include <EWebKit.h>
#include <EWebKit_internal.h>
#include <Core/Buffer/IRenderBuffer.h>
#include <Core/Buffer/BoxRenderBuffer.h>
#include <Core/Buffer/GbarRenderBuffer.h>
#include <Core/Util/Log.h>
#include <Core/Util/Util.h>
#include "JsInterface.h"
#include <sys/stat.h>

using namespace std::placeholders;

// This is used for informing context of box to web content as value of url parameter
static const std::string renderTypeCreate("create");
static const std::string renderTypeResize("resize");
static const std::string renderTypeOpenGbar("gbaropen");
static const std::string renderTypeUpdate("update");

RenderView::RenderView(BoxInfoPtr boxInfo, bool hwEnable)
    : m_boxInfo(boxInfo)
    , m_boxBuffer()
    , m_gbarBuffer()
    , m_jsInterface()
    , m_lastShowBoxTime()
{

    auto touchCallback = std::bind(RenderView::touchBoxCallback, _1, _2, this);
    m_boxBuffer = BoxRenderBuffer::create(m_boxInfo, touchCallback);
    m_boxBuffer->allocate(hwEnable);
    m_jsInterface = JsInterface::create(this, m_boxInfo);
}

RenderView::~RenderView()
{
}

void RenderView::show()
{
    LogD("enter");
    showInternal(renderTypeCreate);
}

void RenderView::hide()
{
    LogD("enter");
    if (m_gbarBuffer) {
        closeGbar();
    }
    m_boxBuffer->stopCanvasUpdate();
    hideBox();
    m_boxBuffer->free();
    m_boxBuffer.reset();
}

void RenderView::resize()
{
    LogD("enter");
    m_boxBuffer->reallocate(
            m_boxInfo->boxWidth, m_boxInfo->boxHeight);
    showInternal(renderTypeResize);
}

void RenderView::update()
{
    LogD("enter");
    showInternal(renderTypeUpdate);
}

void RenderView::resume()
{
    LogD("enter");
    resumeBox();
}

void RenderView::pause()
{
    LogD("enter");
    pauseBox();
}

void RenderView::openGbar()
{
    LogD("enter");

    auto touchCallback = std::bind(RenderView::touchGbarCallback, _1, _2, this);
    m_gbarBuffer = GbarRenderBuffer::create(m_boxInfo, touchCallback);
    m_gbarBuffer->allocate();
    m_gbarBuffer->stopCanvasUpdate();
    showInternal(renderTypeOpenGbar);
    ecore_idler_add(startUpdateRenderBufferIdlerCallback, m_gbarBuffer.get());
}

void RenderView::closeGbar()
{
    LogD("enter");
    if (m_gbarBuffer) {
        m_gbarBuffer->stopCanvasUpdate();
        hideGbar();
        m_gbarBuffer->free();
        m_gbarBuffer.reset();
    }
}

Evas_Object* RenderView::getBoxWebView()
{
    LogD("enter");
    // this will be implemented by derived class
    return NULL;
}

Evas_Object* RenderView::getGbarWebView()
{
    LogD("enter");
    // this will be implemented by derived class
    return NULL;
}

void RenderView::showBox(RenderInfoPtr boxRenderInfo)
{
    LogD("enter");
    UNUSED_PARAM(boxRenderInfo);
    // this will be implemented by derived class
}

void RenderView::hideBox()
{
    LogD("enter");
    // this will be implemented by derived class
}

void RenderView::pauseBox()
{
    LogD("enter");
    // this will be implemented by derived class
}

void RenderView::resumeBox()
{
    LogD("enter");
    // this will be implemented by derived class
}

void RenderView::showGbar(RenderInfoPtr gbarRenderInfo, RenderInfoPtr boxRenderInfo)
{
    LogD("enter");
    UNUSED_PARAM(gbarRenderInfo);
    UNUSED_PARAM(boxRenderInfo);
    // this will be implemented by derived class
}

void RenderView::hideGbar()
{
    LogD("enter");
    // this will be implemented by derived class
}

IRenderBufferPtr RenderView::getBoxBuffer() const
{
    return m_boxBuffer;
}

IRenderBufferPtr RenderView::getGbarBuffer() const
{
    return m_gbarBuffer;
}

bool RenderView::processBoxScheme(std::string& uri)
{
    LogD("enter");
    return m_jsInterface->process(uri);
}

void RenderView::didBoxTouched(int x, int y)
{
    UNUSED_PARAM(x);
    UNUSED_PARAM(y);
    // this will be implemented by derived class
}

void RenderView::didGbarTouched(int x, int y)
{
    UNUSED_PARAM(x);
    UNUSED_PARAM(y);
    // this will be implemented by derived class
}

void RenderView::touchBoxCallback(int x, int y, RenderView* This)
{
    This->didBoxTouched(x, y);
}

void RenderView::touchGbarCallback(int x, int y, RenderView* This)
{
    This->didGbarTouched(x, y);
}

Eina_Bool RenderView::startUpdateRenderBufferIdlerCallback(void* data)
{
    LogD("enter");
    RenderBuffer* buffer = static_cast<RenderBuffer*>(data);
    if (!buffer) {
        LogD("no buffer");
    } else {
        buffer->startCanvasUpdate();
    }

    return ECORE_CALLBACK_CANCEL;
}

RenderInfoPtr RenderView::makeRenderInfo(const std::string& renderType, UrlType urlType) const
{
    LogD("enter");
    RenderInfoPtr renderInfo(new RenderInfo);

    std::ostringstream query;

    // add width, height, operation type
    query << "?type=" << renderType;

    // set width, height
    switch (urlType) {
    case URL_TYPE_BOX:
        renderInfo->window = m_boxBuffer->getWindow();
        renderInfo->width = m_boxInfo->boxWidth;
        renderInfo->height = m_boxInfo->boxHeight;
        break;
    case URL_TYPE_GBAR:
        renderInfo->window = m_gbarBuffer->getWindow();
        renderInfo->width = m_boxInfo->gbarWidth;
        renderInfo->height = m_boxInfo->gbarHeight;
        break;
    default:
        LogD("error url type");
        return RenderInfoPtr();
    }
    query << "&width=" << renderInfo->width << "&height=" << renderInfo->height;

    // if needed, set gbar information
    if (renderType == renderTypeOpenGbar) {
        // add position infomation of gbar
        query << "&gbaropen-direction=";
        if (m_boxInfo->gbarY == 0) {
            query << "down";
        } else {
            query << "up";
        }

        query << "&gbaropen-arrow-xpos=";
        query << static_cast<int>((m_boxInfo->gbarX) * (m_boxInfo->gbarWidth));

        // add last update time & box's width, height
        query << "&box-last-update-time=" << m_lastShowBoxTime;
        query << "&box-width=" << m_boxInfo->boxWidth;
        query << "&box-height=" << m_boxInfo->boxHeight;
    }

    // add service content info passed from application
    if (!m_boxInfo->appContentInfo.empty()) {
        query << "&" << m_boxInfo->appContentInfo;
    }

    // add content info passed from master provider
    // this value can be different per box type
    if (!m_boxInfo->contentInfo.empty()) {
        query << "&" << m_boxInfo->contentInfo;
    }

    LogD("default url param string: %s", query.str().c_str());
    renderInfo->defaultUrlParams = query.str();
    return renderInfo;
}

void RenderView::showInternal(const std::string& renderType)
{
    LogD("enter");

    if (renderType == renderTypeOpenGbar) {
        RenderInfoPtr gbarRenderInfo = makeRenderInfo(renderType, URL_TYPE_GBAR);
        RenderInfoPtr boxRenderInfo = makeRenderInfo(renderType, URL_TYPE_BOX);
        showGbar(gbarRenderInfo, boxRenderInfo);
    } else {
        // set current time to lastest time for showing box
        // this should be pre-executed before calling makeRenderInfo.
        m_lastShowBoxTime = time(NULL);
        RenderInfoPtr boxRenderInfo = makeRenderInfo(renderType, URL_TYPE_BOX);
        showBox(boxRenderInfo);
    }

    struct stat tmp;

    if (stat(WEB_PROVIDER_INSPECTOR_FILE_PATH, &tmp) == 0) {
        unsigned int portnum = ewk_view_inspector_server_start(
            getBoxWebView(), WEB_PROVIDER_INSPECTOR_PORT_NUMBER);
        LogD("WEB_PROVIDER_INSPECTOR enabled port:%d",portnum);
    }
}
