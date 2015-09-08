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
 * @file    AppBoxGbarHelper.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include <string>
#include <Evas.h>
#include <Core/Util/Log.h>
#include <EWebKit.h>
#include <EWebKit_internal.h>
#include "AppBoxGbarHelper.h"

AppBoxGbarHelper::AppBoxGbarHelper(Evas_Object* gbarWin)
    : m_win(gbarWin)
    , m_boxWebView()
    , m_GbarWebView()
    , m_opened(false)
{
}

AppBoxGbarHelper::~AppBoxGbarHelper()
{
}

void AppBoxGbarHelper::startOpen()
{
    LogD("enter");
}

void AppBoxGbarHelper::finishOpen(Evas_Object* child)
{
    LogD("enter");
    m_opened = true;
    setGbarWebView(child);
}

void AppBoxGbarHelper::close()
{
    LogD("enter");
}

void AppBoxGbarHelper::setBoxWebView(Evas_Object* webview)
{
    LogD("enter");
    m_boxWebView = webview;
}

void AppBoxGbarHelper::setGbarWebView(Evas_Object* webview)
{
    LogD("enter");
    m_GbarWebView = webview;
}

Evas_Object* AppBoxGbarHelper::getBoxWebView() const
{
    LogD("enter");
    return m_boxWebView;
}

Evas_Object* AppBoxGbarHelper::getGbarWebView() const
{
    LogD("enter");
    return m_GbarWebView;
}

Evas* AppBoxGbarHelper::getGbarCanvas() const
{
    LogD("enter");
    return evas_object_evas_get(m_win);
}

bool AppBoxGbarHelper::isGbarOpened() const
{
    LogD("enter");
    return m_opened;
}