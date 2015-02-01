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
 * @file    WebView.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include "WebView.h"

#include "config.h"

#include <sstream>
#include <string>
#include <fstream>
#include <streambuf>
#include <Evas.h>
#include <Eina.h>
#include <EWebKit.h>
#include <EWebKit_internal.h>
#include <Core/Util/Log.h>
#include <Core/Util/Util.h>
#include "Service/AppControl.h"

// injection javascript file regarding creating js object used by box and pd
static const std::string injectionFile("/usr/share/web-provider/injection.js");
static const char* consoleMessageDlogTag = "ConsoleMessage";

namespace {
static std::map<const std::string, const Evas_Smart_Cb> smartCallbacksMap =
{
    {"load,started", WebView::loadStartedCallback},
    {"load,finished", WebView::loadFinishedCallback},
    {"title,changed", WebView::titleChangedCallback},
    {"uri,changed", WebView::uriChangedCallback},
    {"load,progress", WebView::loadProgressCallback},
    {"load,progress,finished", WebView::loadProgressFinishedCallback},
    {"process,crashed", WebView::processCrashedCallback},
    {"create,window", WebView::createWindowCallback},
    {"close,window", WebView::closeWindowCallback},
    {"policy,navigation,decide", WebView::policyNavigationDecideCallback},
    {"policy,newwindow,decide", WebView::policyNewWindowDecideCallback},
    {"policy,response,decide", WebView::pageResponseDecideCallback},
    {"contextmenu,customize", WebView::contextmenuCustomizeCallback},
    {"form,submit", WebView::formSubmitCallback},
    {"request,geolocation,permission", WebView::geolocationPermissionRequestCallback},
    {"notification,show", WebView::notificationShowCallback},
    {"notification,cancel", WebView::notificationCancelCallback},
    {"notification,permission,request", WebView::notificationPermissionRequestCallback},
    {"database,quota,exceeded", WebView::databaseUsagePermissionRequestCallback},
    {"filesystem,permission,request", WebView::fileSystemPermissionRequestCallback},
    {"fullscreen,enterfullscreen", WebView::enterFullscreenCallback},
    {"fullscreen,exitfullscreen", WebView::exitFullscreenCallback},
    {"inputmethod,changed", WebView::imeChangedCallback},
    {"editorclient,ime,opened", WebView::imeOpenedCallback},
    {"editorclient,ime,closed", WebView::imeClosedCallback},
    {"usermedia,permission,request", WebView::usermediaPermissionRequestCallback},
    {"protocolhandler,registration,requested", WebView::protocolHandlerRegistrationCallback},
    {"protocolhandler,isregistered", WebView::protocolHandlerIsRegisteredCallback},
    {"protocolhandler,unregistration,requested", WebView::protocolHandlerUnregistrationCallback},
    {"contenthandler,registration,requested", WebView::contentHandlerRegistrationCallback},
    {"contenthandler,isregistered", WebView::contentHandlerIsRegisteredCallback},
    {"contenthandler,unregistration,requested", WebView::contentHandlerUnregistrationCallback},
    {"request,certificate,confirm", WebView::certificateConfirmRequestCallback},
    {"console,message", WebView::consoleMessageCallback}
};
} //namespace anonymous

WebView::WebView(Evas_Object* win, Ewk_Context* ewkContext)
    : m_topWebview(NULL)
    , m_win(win)
    , m_ewkContext(ewkContext)
{
}

WebView::~WebView()
{
}

bool WebView::show(
        std::string& startUrl, int width, int height,
        WebViewCallback didCreateBaseWebView, void* data)
{
    LogD("enter");
    if (!m_topWebview) {
        m_topWebview = ewk_view_add_with_context(
                        evas_object_evas_get(m_win), m_ewkContext);
        if (!setBasicSetting(m_topWebview)) {
            return false;
        }
    }

    // inform base webview to caller
    if (didCreateBaseWebView) {
        didCreateBaseWebView(data, static_cast<void*>(m_topWebview));
    }

    // check scheme of url
    std::string url = validateUrl(startUrl);
    LogD("load: %s", url.c_str());
    ewk_view_url_set(m_topWebview, url.c_str());

    evas_object_resize(m_topWebview, width, height);
    evas_object_show(m_topWebview);

    return true;
}

bool WebView::hide()
{
    LogD("enter");

    // TODO If created webviews are managed by WebView Class,
    //  add code regarding created all webviews
    if (!unsetBasicSetting(m_topWebview)) {
        return false;
    }
    if(!m_topWebview) {
        return false;
    }
    for (auto it = smartCallbacksMap.begin();
            it != smartCallbacksMap.end(); it++) {
        evas_object_smart_callback_del(
            m_topWebview, it->first.c_str(), it->second);
    }

    evas_object_del(m_topWebview);
    m_topWebview = NULL;

    return true;
}

bool WebView::suspend()
{
    LogD("enter");
    ewk_view_suspend(m_topWebview);
    return true;
}

bool WebView::resume()
{
    LogD("enter");
    ewk_view_resume(m_topWebview);
    return true;
}

bool WebView::setBasicSetting(Evas_Object* webview)
{
    LogD("enter");

    if (!webview) {
        return false;
    }

    for (auto it = smartCallbacksMap.begin();
            it != smartCallbacksMap.end(); it++) {
        evas_object_smart_callback_add(
            webview, it->first.c_str(), it->second, this);
    }

    // set specific features
    Ewk_Settings* setting = ewk_view_settings_get(webview);
    // set user agent like WRT, or Browser?
    // ewk_view_user_agent_set(webview, "some_ua_string");
    ewk_settings_plugins_enabled_set(setting, EINA_TRUE);
    ewk_settings_javascript_enabled_set(setting, EINA_TRUE);
    ewk_settings_loads_images_automatically_set(setting, EINA_TRUE);
    ewk_settings_auto_fitting_set(setting, EINA_FALSE);
#if !ENABLE(WEBKIT_UPVERSION)
    ewk_settings_default_keypad_enabled_set(setting, EINA_FALSE);
#endif
    ewk_settings_text_selection_enabled_set(setting, EINA_FALSE);

    evas_object_color_set(webview, 0, 0, 0, 1);
    ewk_view_use_settings_font(webview);

    return true;
}

bool WebView::unsetBasicSetting(Evas_Object* webview)
{
    LogD("enter");

    if (!webview) {
        return false;
    }

    for (auto it = smartCallbacksMap.begin();
            it != smartCallbacksMap.end(); it++) {
        evas_object_smart_callback_del(
            m_topWebview, it->first.c_str(), it->second);
    }

    return true;
}

std::string WebView::validateUrl(std::string& url)
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

void WebView::loadStartedCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);

    WebView* This = static_cast<WebView*>(data);

    std::ifstream jsFile(injectionFile);
    std::string script((std::istreambuf_iterator<char>(jsFile)),
                        std::istreambuf_iterator<char>());
    LogD("injected js code: %s", script.c_str());

    ewk_view_script_execute(obj, script.c_str(), executeScriptCallback, This);
    This->didLoadStarted(obj);
}

void WebView::loadFinishedCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);

    WebView* This = static_cast<WebView*>(data);
    This->didLoadFinished(obj);
}

void WebView::titleChangedCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);

    WebView* This = static_cast<WebView*>(data);
    This->didTitleChanged(obj, eventInfo);
}

void WebView::uriChangedCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);

    WebView* This = static_cast<WebView*>(data);
    This->didUriChanged(obj, eventInfo);
}

void WebView::loadProgressCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);

    WebView* This = static_cast<WebView*>(data);
    This->didLoadProgress(obj, eventInfo);
}

void WebView::loadProgressFinishedCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);

    WebView* This = static_cast<WebView*>(data);
    This->didLoadProgressFinished(obj);
}

void WebView::processCrashedCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);

    WebView* This = static_cast<WebView*>(data);
    This->didProcessCrashed(obj);
}

void WebView::createWindowCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);

    WebView* This = static_cast<WebView*>(data);
    This->didCreateWindow(obj, eventInfo);
}

void WebView::closeWindowCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);

    WebView* This = static_cast<WebView*>(data);
    This->didCloseWindow(obj);
}

void WebView::policyNavigationDecideCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didPolicyNavigationDecide(obj, eventInfo);
}

void WebView::policyNewWindowDecideCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didPolicyNewWindowDecide(obj, eventInfo);
}

void WebView::pageResponseDecideCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    Ewk_Policy_Decision *policyDecision = static_cast<Ewk_Policy_Decision *>(eventInfo);
    Ewk_Policy_Decision_Type policyType = ewk_policy_decision_type_get(policyDecision);
    std::string url(ewk_policy_decision_url_get(policyDecision));
    std::string cookie(ewk_policy_decision_cookie_get(policyDecision));
    const char* contentType = ewk_policy_decision_response_mime_get(policyDecision);

    switch (policyType) {
    case EWK_POLICY_DECISION_USE:
        LogD("policy use");
        ewk_policy_decision_use(policyDecision);
        break;

    case EWK_POLICY_DECISION_DOWNLOAD:
        LogD("policy download: %s, %s, %s", url.c_str(), cookie.c_str(), contentType);
        ewk_policy_decision_suspend(policyDecision);
        Service::AppControl::launchDownloader(url, cookie);
        ewk_policy_decision_ignore(policyDecision);
        break;

    case EWK_POLICY_DECISION_IGNORE:
    default:
        LogD("policy ignore");
        ewk_policy_decision_ignore(policyDecision);
        break;
    }

    if (policyType == EWK_POLICY_DECISION_DOWNLOAD) {
        if (ewk_view_back_possible(obj)) {
            ewk_view_back(obj);
        } else {
            // TODO Add handling code in case that new window is opened
            //ecore_idler_add(windowCloseIdlerCallback, data);
        }
    }

    WebView* This = static_cast<WebView*>(data);
    This->didPageResponseDecide(obj);
}

void WebView::contextmenuCustomizeCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didContextmenuCustomize(obj, eventInfo);
}

void WebView::formSubmitCallback(
        void* data, Evas_Object *obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);

    WebView* This = static_cast<WebView*>(data);
    This->didFormSubmit(obj);
}

void WebView::geolocationPermissionRequestCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didGeolocationPermissionRequest(obj, eventInfo);
}

void WebView::notificationShowCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didNotificationShow(obj, eventInfo);
}

void WebView::notificationCancelCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didNotificationCancel(obj, eventInfo);
}

void WebView::notificationPermissionRequestCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didNotificationPermissionRequest(obj, eventInfo);
}

void WebView::databaseUsagePermissionRequestCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didDatabaseUsagePermissionRequest(obj, eventInfo);
}

void WebView::fileSystemPermissionRequestCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didFilesystemPermissionRequest(obj, eventInfo);
}

void WebView::enterFullscreenCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);

    WebView* This = static_cast<WebView*>(data);
    This->didEnterFullscreen(obj);
}

void WebView::exitFullscreenCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);

    WebView* This = static_cast<WebView*>(data);
    This->didExitFullscreen(obj);
}

void WebView::imeChangedCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didImeChanged(obj, eventInfo);
}

void WebView::imeOpenedCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);

    WebView* This = static_cast<WebView*>(data);
    This->didImeOpened(obj);
}

void WebView::imeClosedCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);

    WebView* This = static_cast<WebView*>(data);
    This->didImeClosed(obj);
}

void WebView::usermediaPermissionRequestCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didUsermediaPermissionRequest(obj, eventInfo);
}

void WebView::protocolHandlerRegistrationCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didProtocolHandlerRegistration(obj, eventInfo);
}

void WebView::protocolHandlerIsRegisteredCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didProtocolHandlerIsRegistered(obj, eventInfo);
}

void WebView::protocolHandlerUnregistrationCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didProtocolHandlerUnregistration(obj, eventInfo);
}

void WebView::contentHandlerRegistrationCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didContentHandlerRegistration(obj, eventInfo);
}

void WebView::contentHandlerIsRegisteredCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didContentHandlerIsRegistered(obj, eventInfo);
}

void WebView::contentHandlerUnregistrationCallback(
        void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didContentHandlerUnregistration(obj, eventInfo);
}

void WebView::certificateConfirmRequestCallback(
    void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");

    WebView* This = static_cast<WebView*>(data);
    This->didCertificateConfirmRequest(obj, eventInfo);
}

void WebView::didLoadStarted(Evas_Object* obj)
{
    LogD("enter");
    UNUSED_PARAM(obj);

    // This Will be implemented by derived class
}

void WebView::didLoadFinished(Evas_Object* obj)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    // This Will be implemented by derived class
}

void WebView::didTitleChanged(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);

    // This Will be implemented by derived class
}

void WebView::didUriChanged(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didLoadProgress(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didLoadProgressFinished(Evas_Object* obj)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    // This Will be implemented by derived class
}

void WebView::didProcessCrashed(Evas_Object* obj)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    // This Will be implemented by derived class
}

void WebView::didCreateWindow(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didCloseWindow(Evas_Object* obj)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    // This Will be implemented by derived class
}

void WebView::didPolicyNavigationDecide(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didPolicyNewWindowDecide(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didPageResponseDecide(Evas_Object* obj)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    // This Will be implemented by derived class
}

void WebView::didContextmenuCustomize(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didFormSubmit(Evas_Object* obj)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    // This Will be implemented by derived class
}

void WebView::didGeolocationPermissionRequest(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didNotificationShow(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didNotificationCancel(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didNotificationPermissionRequest(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didDatabaseUsagePermissionRequest(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didFilesystemPermissionRequest(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didEnterFullscreen(Evas_Object* obj)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    // This Will be implemented by derived class
}

void WebView::didExitFullscreen(Evas_Object* obj)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    // This Will be implemented by derived class
}

void WebView::didImeChanged(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didImeOpened(Evas_Object* obj)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    // This Will be implemented by derived class
}

void WebView::didImeClosed(Evas_Object* obj)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    // This Will be implemented by derived class
}

void WebView::didUsermediaPermissionRequest(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didProtocolHandlerRegistration(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didProtocolHandlerIsRegistered(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didProtocolHandlerUnregistration(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didContentHandlerRegistration(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didContentHandlerIsRegistered(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didContentHandlerUnregistration(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

void WebView::didCertificateConfirmRequest(Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(eventInfo);
    // This Will be implemented by derived class
}

Eina_Bool WebView::orientationLockCallback(
        Evas_Object* obj, Eina_Bool needLock, int orientation, void* data)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(needLock);
    UNUSED_PARAM(orientation);
    UNUSED_PARAM(data);
    return EINA_TRUE;
}

void WebView::executeScriptCallback(
                Evas_Object* obj, const char* result, void* data)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(result);
    UNUSED_PARAM(data);
}

void WebView::consoleMessageCallback(void* data, Evas_Object* obj, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(obj);

    WebView* This = static_cast<WebView*>(data);

    Ewk_Console_Message* consoleMessage =
        static_cast<Ewk_Console_Message*>(eventInfo);

    std::stringstream buf;
    unsigned int lineNumber = ewk_console_message_line_get(consoleMessage);
    const char* source = ewk_console_message_source_get(consoleMessage);
    const char* text = ewk_console_message_text_get(consoleMessage);
    if (lineNumber) {
        buf << source << ":" << lineNumber << ":";
    }
    buf << text;
    This->consoleMessage(ewk_console_message_level_get(consoleMessage), buf.str().c_str());
}

void WebView::consoleMessage(int level, const char* message)
{
    LogD("enter");

    // this console message should be secure mode
    switch (level) {
    case EWK_CONSOLE_MESSAGE_LEVEL_DEBUG:
        SECURE_LOG(LOG_DEBUG, consoleMessageDlogTag, "%s", message);
        break;
    case EWK_CONSOLE_MESSAGE_LEVEL_WARNING:
        SECURE_LOG(LOG_WARN, consoleMessageDlogTag, "%s", message);
        break;
    case EWK_CONSOLE_MESSAGE_LEVEL_ERROR:
        SECURE_LOG(LOG_ERROR, consoleMessageDlogTag, "%s", message);
        break;
    default:
        SECURE_LOG(LOG_DEBUG, consoleMessageDlogTag, "%s", message);
        break;
    }
}
