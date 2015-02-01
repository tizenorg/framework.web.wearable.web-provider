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
 * @file    AppBoxPdHelper.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include <string>
#include <Evas.h>
#include <Core/Util/Log.h>
#include <EWebKit.h>
#include <EWebKit_internal.h>
#include "AppBoxPdHelper.h"

AppBoxPdHelper::AppBoxPdHelper(Evas_Object* pdWin)
    : m_win(pdWin)
    , m_boxWebView()
    , m_pdWebView()
    , m_opened(false)
{
}

AppBoxPdHelper::~AppBoxPdHelper()
{
}

void AppBoxPdHelper::startOpen()
{
    LogD("enter");
}

void AppBoxPdHelper::finishOpen(Evas_Object* child)
{
    LogD("enter");
    m_opened = true;
    setPdWebView(child);
}

void AppBoxPdHelper::close()
{
    LogD("enter");
}

void AppBoxPdHelper::setBoxWebView(Evas_Object* webview)
{
    LogD("enter");
    m_boxWebView = webview;
}

void AppBoxPdHelper::setPdWebView(Evas_Object* webview)
{
    LogD("enter");
    m_pdWebView = webview;
}

Evas_Object* AppBoxPdHelper::getBoxWebView() const
{
    LogD("enter");
    return m_boxWebView;
}

Evas_Object* AppBoxPdHelper::getPdWebView() const
{
    LogD("enter");
    return m_pdWebView;
}

Evas* AppBoxPdHelper::getPdCanvas() const
{
    LogD("enter");
    return evas_object_evas_get(m_win);
}

bool AppBoxPdHelper::isPdOpened() const
{
    LogD("enter");
    return m_opened;
}
