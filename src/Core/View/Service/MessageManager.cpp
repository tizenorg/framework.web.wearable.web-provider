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
 * @file    MessageMenager.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */

#include <string>
#include <Evas.h>
#include <Eina.h>
#include <EWebKit.h>
#include <EWebKit_internal.h>
#include <Core/Util/Log.h>
#include <Core/Util/Util.h>
#include "MessageManager.h"

namespace Service {

static const std::string jsFireWindowEventFunction("webprovider.fireAppWidgetEvent");
static const std::string jsGbarMessageEvent("gbarmessage");
static const std::string jsBoxMessageEvent("boxmessage");

MessageManager::MessageManager()
{
    LogD("enter");
}

MessageManager::~MessageManager()
{
    LogD("enter");
}

bool MessageManager::send(Evas_Object* webview, ReceiverType receiver, std::string& message)
{
    LogD("enter");

    std::string eventName;
    if(!webview) {
        return false;
    }
    // set message event name triggered by receiver
    switch (receiver) {
    case TO_BOX:
        eventName = jsGbarMessageEvent;
        break;
    case TO_GBAR:
        eventName = jsBoxMessageEvent;
        break;
    default:
        return false;
    }

    std::string script = jsFireWindowEventFunction;
    script += "(\"";
    script += eventName;
    script += "\", \"";
    script += message;
    script +="\");";
    LogD("calling javascript: %s", script.c_str());

    // execute js code for sending message
    if (EINA_FALSE == ewk_view_script_execute(
                webview, script.c_str(), executeScriptCallback, this)) {
        LogD("ewk_view_script_execute fail.");
    }

    return true;
}

void MessageManager::executeScriptCallback(
        Evas_Object* webview, const char* result, void* data)
{
    LogD("enter");
    UNUSED_PARAM(webview);
    UNUSED_PARAM(data);

    std::string resultStr(result ? result : "null");
    LogD("result: %s", resultStr.c_str());
}
} // Service
