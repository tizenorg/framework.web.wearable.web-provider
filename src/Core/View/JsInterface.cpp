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
 * @file    JsInterface.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */

#include <string.h>
#include <ctime>
#include <Core/BoxData.h>
#include <Core/Util/Log.h>
#include <Core/Util/Util.h>
#include "Service/AppControl.h"
#include "Service/PeriodChanger.h"
#include "Service/ScrollHolder.h"
#include "Service/MessageManager.h"
#include "JsInterface.h"

using namespace Service;

namespace {
static const std::string BOX_SCHEME("box://");
static const std::string BOX_SCHEME_RELOAD("box://reload");
static const std::string BOX_SCHEME_CHANGE_PERIOD("box://change-period");
static const std::string BOX_SCHEME_LAUNCH_BROWSER("box://launch-browser");
static const std::string BOX_SCHEME_SEND_MESSAGE_TO_PD("box://send-message-to-pd");
static const std::string BOX_SCHEME_SEND_MESSAGE_TO_BOX("box://send-message-to-box");
static const std::string BOX_SCHEME_SCROLL_START("box://scroll-start");
static const std::string BOX_SCHEME_SCROLL_STOP("box://scroll-stop");

static const std::string HTTP_SCHEME("http://");
static const std::string HTTPS_SCHEME("https://");
}
JsInterface::JsInterface(RenderView* renderView, BoxInfoPtr boxInfo)
    : m_renderView(renderView)
    , m_boxInfo(boxInfo)
{
    LogD("enter");
    if( renderView == NULL ){
        LogE("render view is NULL");
        throw;
    }
}

JsInterface::~JsInterface()
{
    LogD("enter");
}

bool JsInterface::process(std::string& uri)
{
    LogD("enter");

    if (!isBoxScheme(uri)) {
        return false;
    }

    if (!uri.compare(BOX_SCHEME_RELOAD)) {
       return handleReload();
    }

    if (!uri.compare(
                0,
                BOX_SCHEME_CHANGE_PERIOD.size(),
                BOX_SCHEME_CHANGE_PERIOD))
    {
        std::string key("period");
        std::string period = parse(uri, key);
        if (period.empty()) {
            return handleChangePeriod();
        }

        return handleChangePeriod(std::atof(period.c_str()));
    }

    if (!uri.compare(
                0,
                BOX_SCHEME_LAUNCH_BROWSER.size(),
                BOX_SCHEME_LAUNCH_BROWSER))
    {
        std::string key("url");
        std::string url = parse(uri, key);
        return handleLaunchBrowser(url);
    }

    if (!uri.compare(
                0,
                BOX_SCHEME_SEND_MESSAGE_TO_BOX.size(),
                BOX_SCHEME_SEND_MESSAGE_TO_BOX))
    {
        std::string key("message");
        std::string message = parse(uri, key);
        return handleSendMessage(MessageManager::TO_BOX, message);
    }

    if (!uri.compare(
                0,
                BOX_SCHEME_SEND_MESSAGE_TO_PD.size(),
                BOX_SCHEME_SEND_MESSAGE_TO_PD))
    {
        std::string key("message");
        std::string message = parse(uri, key);
        return handleSendMessage(MessageManager::TO_PD, message);
    }

    if (!uri.compare(BOX_SCHEME_SCROLL_START)) {
       return handleScroll(true);
    }

    if (!uri.compare(BOX_SCHEME_SCROLL_STOP)) {
       return handleScroll(false);
    }

    LogD("unknown box scheme protocol");
    return false;
}

bool JsInterface::isBoxScheme(std::string& uri)
{
    LogD("enter");

    if(!uri.compare(0, BOX_SCHEME.size(), BOX_SCHEME)) {
        return true;
    }

    return false;
}

bool JsInterface::handleReload()
{
    LogD("enter");

    m_renderView->update();
    return true;
}

bool JsInterface::handleChangePeriod(float requestedPeriod)
{
    LogD("enter");

    if (Service::PeriodChanger::isPopupOpened()) {
        LogD("preiod popup is already opened!");
        return false;
    }

    m_periodChanger =
        Service::PeriodChanger::create(
            m_boxInfo->boxId, m_boxInfo->instanceId,
            m_boxInfo->period, requestedPeriod);

    return m_periodChanger->change();
}

bool JsInterface::handleLaunchBrowser(std::string& url)
{
    LogD("enter");

    if (!url.compare(0, HTTP_SCHEME.size(), HTTP_SCHEME) ||
        !url.compare(0, HTTPS_SCHEME.size(), HTTPS_SCHEME))
    {
        return Service::AppControl::launchBrowser(url);
    }

    return false;
}

bool JsInterface::handleSendMessage(
        MessageManager::ReceiverType receiver,
        std::string& message)
{
    LogD("enter");

    // set webview of receiver
    Evas_Object* webview;
    switch (receiver) {
    case MessageManager::TO_BOX:
        webview = m_renderView->getBoxWebView();
        break;
    case MessageManager::TO_PD:
        webview = m_renderView->getPdWebView();
        break;
    default:
        LogD("not supported receiver");
        return false;
    }

    return m_messageManager->send(webview, receiver, message);
}

bool JsInterface::handleScroll(bool start)
{
    using namespace Service::ScrollHolder;

    LogD("enter");

    holdHorizontalScroll(m_boxInfo->boxId, m_boxInfo->instanceId, start);
    return true;
}

std::string JsInterface::parse(std::string& uri, std::string& key)
{
    LogD("enter");

    // TODO url parameter SHOULD be parsed using std::regex, not manually
    std::string value("");

    unsigned found = uri.find_first_of("?");
    if (found == std::string::npos) {
        LogD("no query");
        return value;
    }

    std::string query = std::string(uri, found + 1);
    found = 0;
    do {
        LogD("enter\n");
        unsigned seperator = query.find_first_of("=", found + 1);
        if (seperator == std::string::npos) {
            LogD("no '=' character\n");
            break;
        }
        std::string cur_key = query.substr(found, seperator - found);
        unsigned next = query.find_first_of("&", seperator + 1);

        if (key == cur_key) {
            LogD("key matched!\n");
            value = std::string(query, seperator + 1, next == std::string::npos ? std::string::npos : next - seperator - 1);
            break;
        }
        if( next == std::string::npos )
            break;
        found = next + 1;
    } while (found && found != std::string::npos);

    LogD("URL query parsing result: key -> %s, value -> %s", key.c_str(), value.c_str());
    return value;
}
