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
#include <Core/Buffer/PdRenderBuffer.h>
#include <Core/Util/Log.h>
#include <Core/Util/Util.h>
#include "JsInterface.h"
#include <sys/stat.h>

using namespace std::placeholders;

// This is used for informing context of box to web content as value of url parameter
static const std::string renderTypeCreate("create");
static const std::string renderTypeResize("resize");
static const std::string renderTypeOpenPd("pdopen");
static const std::string renderTypeUpdate("update");

RenderView::RenderView(BoxInfoPtr boxInfo, bool hwEnable)
    : m_boxInfo(boxInfo)
    , m_boxBuffer()
    , m_pdBuffer()
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
    if (m_pdBuffer) {
        closePd();
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

void RenderView::openPd()
{
    LogD("enter");

    auto touchCallback = std::bind(RenderView::touchPdCallback, _1, _2, this);
    m_pdBuffer = PdRenderBuffer::create(m_boxInfo, touchCallback);
    m_pdBuffer->allocate();
    m_pdBuffer->stopCanvasUpdate();
    showInternal(renderTypeOpenPd);
    ecore_idler_add(startUpdateRenderBufferIdlerCallback, m_pdBuffer.get());
}

void RenderView::closePd()
{
    LogD("enter");
    if (m_pdBuffer) {
        m_pdBuffer->stopCanvasUpdate();
        hidePd();
        m_pdBuffer->free();
        m_pdBuffer.reset();
    }
}

Evas_Object* RenderView::getBoxWebView()
{
    LogD("enter");
    // this will be implemented by derived class
    return NULL;
}

Evas_Object* RenderView::getPdWebView()
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

void RenderView::showPd(RenderInfoPtr pdRenderInfo, RenderInfoPtr boxRenderInfo)
{
    LogD("enter");
    UNUSED_PARAM(pdRenderInfo);
    UNUSED_PARAM(boxRenderInfo);
    // this will be implemented by derived class
}

void RenderView::hidePd()
{
    LogD("enter");
    // this will be implemented by derived class
}

IRenderBufferPtr RenderView::getBoxBuffer() const
{
    return m_boxBuffer;
}

IRenderBufferPtr RenderView::getPdBuffer() const
{
    return m_pdBuffer;
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

void RenderView::didPdTouched(int x, int y)
{
    UNUSED_PARAM(x);
    UNUSED_PARAM(y);
    // this will be implemented by derived class
}

void RenderView::touchBoxCallback(int x, int y, RenderView* This)
{
    This->didBoxTouched(x, y);
}

void RenderView::touchPdCallback(int x, int y, RenderView* This)
{
    This->didPdTouched(x, y);
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
    case URL_TYPE_PD:
        renderInfo->window = m_pdBuffer->getWindow();
        renderInfo->width = m_boxInfo->pdWidth;
        renderInfo->height = m_boxInfo->pdHeight;
        break;
    default:
        LogD("error url type");
        return RenderInfoPtr();
    }
    query << "&width=" << renderInfo->width << "&height=" << renderInfo->height;

    // if needed, set pd information
    if (renderType == renderTypeOpenPd) {
        // add position infomation of pd
        query << "&pdopen-direction=";
        if (m_boxInfo->pdY == 0) {
            query << "down";
        } else {
            query << "up";
        }

        query << "&pdopen-arrow-xpos=";
        query << static_cast<int>((m_boxInfo->pdX) * (m_boxInfo->pdWidth));

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

    if (renderType == renderTypeOpenPd) {
        RenderInfoPtr pdRenderInfo = makeRenderInfo(renderType, URL_TYPE_PD);
        RenderInfoPtr boxRenderInfo = makeRenderInfo(renderType, URL_TYPE_BOX);
        showPd(pdRenderInfo, boxRenderInfo);
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
