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
 * @file    GbarHelper.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include <string>
#include <Evas.h>
#include <ewk_view.h>
#include <Core/Util/Log.h>
#include <Core/Util/Util.h>
#include "IRenderView.h"
#include "IGbarHelper.h"
#include "GbarHelper.h"

GbarHelper::GbarHelper(RenderInfoPtr gbarRenderInfo, std::string gbarStartUrl)
    : m_boxWebView()
    , m_gbarWebView()
    , m_gbarRenderInfo(gbarRenderInfo)
    , m_startUrl(gbarStartUrl)
    , m_opened(false)
{
}

GbarHelper::~GbarHelper()
{
}

void GbarHelper::startOpen()
{
    LogD("enter");
    if (!m_boxWebView) {
        return;
    }

    //make javascript string for gbar
    std::string script = "var gbarWindow = window.open(\"";
    script += validateUrl(m_startUrl);
    script += "\", \"_blank\");";

    // execute javascript for opening new webview for gbar
    LogD("executed script: %s", script.c_str());
    ewk_view_script_execute(
            m_boxWebView, script.c_str(), executeScriptCallback, this);
}

void GbarHelper::finishOpen(Evas_Object* child)
{
    LogD("enter");

    // gbar webview set and resize
    m_gbarWebView = child;
    evas_object_resize(m_gbarWebView, m_gbarRenderInfo->width, m_gbarRenderInfo->height);
    m_opened = true;
}

void GbarHelper::close()
{
    LogD("enter");
}

void GbarHelper::setBoxWebView(Evas_Object* webview)
{
    LogD("enter");
    m_boxWebView = webview;
}

void GbarHelper::setGbarWebView(Evas_Object* webview)
{
    LogD("enter");
    m_gbarWebView = webview;
}

Evas_Object* GbarHelper::getBoxWebView() const
{
    LogD("enter");
    return m_boxWebView;
}

Evas_Object* GbarHelper::getGbarWebView() const
{
    LogD("enter");
    return m_gbarWebView;
}

Evas* GbarHelper::getGbarCanvas() const
{
    LogD("enter");
    return evas_object_evas_get(m_gbarRenderInfo->window);
}

bool GbarHelper::isGbarOpened() const
{
    LogD("enter");
    return m_opened;
}

void GbarHelper::didExecuteScript(Evas_Object* webview, std::string& result)
{
    LogD("enter");
    UNUSED_PARAM(webview);
    LogD("javascript execution result: %s", result.c_str());
}

std::string GbarHelper::validateUrl(std::string& url)
{
    LogD("enter");

    if (url.empty()) {
        return std::string();
    }

    if((!url.compare(0, 4, "http")) ||
            (!url.compare(0, 5, "https")) ||
            (!url.compare(0, 4, "file")))
    {
        return url;
    }

    std::string newUrl("file://");
    newUrl += url;
    return newUrl;
}

void GbarHelper::executeScriptCallback(
        Evas_Object* webview, const char* result, void* data)
{
    LogD("enter");

    GbarHelper* This = static_cast<GbarHelper*>(data);
    std::string resultStr(result ? result : "null");
    This->didExecuteScript(webview, resultStr);
}