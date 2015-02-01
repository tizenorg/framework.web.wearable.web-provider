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
 * @file    PdHelper.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include <string>
#include <Evas.h>
#include <ewk_view.h>
#include <Core/Util/Log.h>
#include <Core/Util/Util.h>
#include "IRenderView.h"
#include "IPdHelper.h"
#include "PdHelper.h"

PdHelper::PdHelper(RenderInfoPtr pdRenderInfo, std::string pdStartUrl)
    : m_boxWebView()
    , m_pdWebView()
    , m_pdRenderInfo(pdRenderInfo)
    , m_startUrl(pdStartUrl)
    , m_opened(false)
{
}

PdHelper::~PdHelper()
{
}

void PdHelper::startOpen()
{
    LogD("enter");
    if (!m_boxWebView) {
        return;
    }

    //make javascript string for pd
    std::string script = "var pdWindow = window.open(\"";
    script += validateUrl(m_startUrl);
    script += "\", \"_blank\");";

    // execute javascript for opening new webview for pd
    LogD("executed script: %s", script.c_str());
    ewk_view_script_execute(
            m_boxWebView, script.c_str(), executeScriptCallback, this);
}

void PdHelper::finishOpen(Evas_Object* child)
{
    LogD("enter");

    // pd webview set and resize
    m_pdWebView = child;
    evas_object_resize(m_pdWebView, m_pdRenderInfo->width, m_pdRenderInfo->height);
    m_opened = true;
}

void PdHelper::close()
{
    LogD("enter");
}

void PdHelper::setBoxWebView(Evas_Object* webview)
{
    LogD("enter");
    m_boxWebView = webview;
}

void PdHelper::setPdWebView(Evas_Object* webview)
{
    LogD("enter");
    m_pdWebView = webview;
}

Evas_Object* PdHelper::getBoxWebView() const
{
    LogD("enter");
    return m_boxWebView;
}

Evas_Object* PdHelper::getPdWebView() const
{
    LogD("enter");
    return m_pdWebView;
}

Evas* PdHelper::getPdCanvas() const
{
    LogD("enter");
    return evas_object_evas_get(m_pdRenderInfo->window);
}

bool PdHelper::isPdOpened() const
{
    LogD("enter");
    return m_opened;
}

void PdHelper::didExecuteScript(Evas_Object* webview, std::string& result)
{
    LogD("enter");
    UNUSED_PARAM(webview);
    LogD("javascript execution result: %s", result.c_str());
}

std::string PdHelper::validateUrl(std::string& url)
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

void PdHelper::executeScriptCallback(
        Evas_Object* webview, const char* result, void* data)
{
    LogD("enter");

    PdHelper* This = static_cast<PdHelper*>(data);
    std::string resultStr(result ? result : "null");
    This->didExecuteScript(webview, resultStr);
}

