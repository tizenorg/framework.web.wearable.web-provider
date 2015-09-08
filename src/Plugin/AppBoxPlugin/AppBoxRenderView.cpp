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
 * @file    AppBoxRenderView.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */

#include "config.h"
#include "AppBoxRenderView.h"

#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <functional>
#include <streambuf>
#include <pkgmgr-info.h>
#include <aul.h>
#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Elementary.h>
#include <EWebKit.h>
#include <EWebKit_internal.h>
#include <widget_service.h>
#include <widget_service_internal.h>
#include <i_runnable_widget_object.h>
#include <core_module.h>
#include <widget_provider.h>
#include <API/web_provider_widget_info.h>
#include <Core/BoxData.h>
#include <Core/View/IGbarHelper.h>
#include <Core/View/GbarHelper.h>
#include <Core/Util/Log.h>
#include <Core/Util/Util.h>
#include "AppBoxGbarHelper.h"

#define RENDER_MAX_TIME 30.0
#define RENDER_MAX_TIME_FOR_INSPECTOR 1200.0
#define SNAPSHOT_REMOVE_TIME 0.5
#define TOUCH_MAX_WAIT_TIME 30.0
#define WEB_WIDGET_OBJ_MOVE_TO_OUTSIDE_POINT_VALUE -10000

namespace{
// injection javascript file regarding creating js object used by box and gbar
static const std::string kInjectionFile("/usr/share/web-provider/injection.js");
static const std::string kConsoleMessageLogTag("ConsoleMessage");
static const std::string kJsRegisterBoxInfoFunction(
        "webprovider.registerAppWidgetContextInfo");
}

AppBoxRenderView::AppBoxRenderView(
        BoxInfoPtr boxInfo,
        EwkContextPtr ewkContext)
    : RenderView(boxInfo, false)
    , m_appId()
    , m_boxId()
    , m_instanceId()
    , m_contentInfo()
    , m_blockedURI()
    , m_ewkContext(ewkContext)
    , m_boxRenderInfo()
    , m_boxWrt()
    , m_gbarWrt()
    , m_boxRenderBuffer()
    , m_gbarRenderBuffer()
    , m_boxSnapshot()
    , m_boxRenderTimer()
    , m_boxTouchTimer()
    , m_boxRemoveSnapShotTimer()
    , m_gbarHelper()
    , m_boxIcon()
    , m_gbarFastOpen(false)
    , m_boxFinishLoad(false)
    , m_boxFrameRendered(false)
    , m_boxWaitFrameRender(false)
    , m_isTouchableBox(false)
    , m_boxWrt_isSuspended(false)
    , m_showed(false)
    , m_showIcon(false)
{
    LogD("enter");

    if( boxInfo == NULL ){
        LogE("boxInfo is NULL!");
        throw;
    }

    // member initialization
    m_boxId = boxInfo->boxId;
    m_instanceId = boxInfo->instanceId;
    m_contentInfo = boxInfo->contentInfo;
    m_appId = getAppId(m_boxId);
    if (m_appId.empty()) {
        throw; //exception throw!
    }

    // get box render buffer
    m_boxRenderBuffer = getBoxBuffer();

#if ENABLE(SHOW_PRE_ICON)
    if (!m_showIcon) {
        pkgmgrinfo_appinfo_h handle;
        int ret;
        char *icon;

        ret = pkgmgrinfo_appinfo_get_appinfo(m_appId.c_str(), &handle);
        if (ret != PMINFO_R_OK) {
            LogE("[%s] DB_FAILED(0x) : failed to get the app-info", __FUNCTION__);
        } else {
            ret = pkgmgrinfo_appinfo_get_icon(handle, &icon);
            if (ret != PMINFO_R_OK) {
                LogE("pkgmgrinfo_appinfo_get_icon failed");
            } else {
                showIcon(icon, boxInfo->boxWidth, boxInfo->boxHeight);
            }
            ret = pkgmgrinfo_appinfo_destroy_appinfo(handle);
            if (ret != PMINFO_R_OK) {
                LogE("pkgmgrinfo_appinfo_get_icon failed");
            }
        }
    }
#endif

    // use fastopen to default
    // m_gbarFastOpen = web_provider_widget_get_gbar_fast_open(m_boxId.c_str()) ? true : false;
    m_gbarFastOpen = true;
    m_isTouchableBox =
        web_provider_widget_get_mouse_event(m_boxId.c_str()) ? true : false;
}

AppBoxRenderView::~AppBoxRenderView()
{
    LogD("enter");

    destroyWrtCore(m_boxWrt);
    destroyWrtCore(m_gbarWrt);
}

void AppBoxRenderView::showBox(RenderInfoPtr boxRenderInfo)
{
    LogD("enter");

    // stop updating render buffer
    m_boxRenderBuffer->stopCanvasUpdate();

    if (m_boxRenderTimer) {
        // delete already running timer
        stopRenderBox();
    }

    // delete touch timer
    if (m_isTouchableBox) {
        deleteTimer(&m_boxTouchTimer);
    }

    // set boxFinishLoad and m_boxFrameRendered to false
    m_boxFinishLoad = false;
    m_boxFrameRendered = false;
    m_boxWaitFrameRender = false;

    // copy to url
    std::string boxStartUrl = getStartUrl(URL_TYPE_BOX, boxRenderInfo->defaultUrlParams);
    if (m_boxWrt) {
        LogD("existing wrt core is removed");
        destroyBoxWrtCore();
    }

    m_boxWrt = createWrtCore(
                URL_TYPE_BOX, boxStartUrl,
                boxRenderInfo->window, m_ewkContext);
    m_boxWrt_isSuspended = false;

    // in case of showing box by request of gbar open
    if (m_gbarHelper) {
        m_gbarHelper->setBoxWebView(m_boxWrt->GetCurrentWebview());
    }

    // resize webview fitted to width, height of Box
    evas_object_resize(
            m_boxWrt->GetCurrentWebview(),
            boxRenderInfo->width,
            boxRenderInfo->height);

    showSnapShot();
    evas_object_show(m_boxWrt->GetCurrentWebview());
    // webview window move to outside of viewport because of overlap issue with snapshot image
    evas_object_move(m_boxWrt->GetCurrentWebview(), WEB_WIDGET_OBJ_MOVE_TO_OUTSIDE_POINT_VALUE, WEB_WIDGET_OBJ_MOVE_TO_OUTSIDE_POINT_VALUE);

    m_boxWrt->Show();
    m_boxRenderInfo = boxRenderInfo;

    // start timer for clearing existing snapshot in case of only gabr open
    addTimer(&m_boxRemoveSnapShotTimer,
            SNAPSHOT_REMOVE_TIME, removeBoxSnapShotTimerCallback);
    m_showed = true;
}

AppBoxRenderView::WrtCorePtr AppBoxRenderView::createWrtCore(
        UrlType type, std::string& startUrl,
        Evas_Object* win, EwkContextPtr ewkContext)
{
    LogD("enter");

    WrtCorePtr wrt;
    wrt = WRT::CoreModuleSingleton::
                Instance().getRunnableWidgetObject(m_appId);

    // prepare webview
    if (startUrl.empty()) {
        LogD("no start url");
        return WrtCorePtr();
    }
    wrt->PrepareView(startUrl, win, ewkContext.get());
    wrt->CheckBeforeLaunch();

    // set callback functions of RunnableWidgetObject
    WRT::UserDelegatesPtr cbs(new WRT::UserDelegates);
    using namespace std::placeholders;
    cbs->loadStartedCallback = std::bind(&AppBoxRenderView::startLoadCallback, this, _1, _2);
    if (type == URL_TYPE_BOX) {
        cbs->loadFinishedCallback = std::bind(&AppBoxRenderView::finishBoxLoadCallback, this, _1, _2);
    } else {
        cbs->loadFinishedCallback = std::bind(&AppBoxRenderView::finishGbarLoadCallback, this, _1, _2);
    }

    cbs->setWebviewCallback = std::bind(&AppBoxRenderView::setBufferCallback, this, _1);
    cbs->unsetWebviewCallback = std::bind(&AppBoxRenderView::unsetBufferCallback, this, _1);

    cbs->policyNavigationDecideCallback = std::bind(&AppBoxRenderView::decideNavigationCallback, this, _1, _2);
    cbs->processCrashedCallback = std::bind(&AppBoxRenderView::crashWebProcessCallback, this, _1, _2);
    cbs->consoleMessageCallback = std::bind(&AppBoxRenderView::consoleMessageCallback, this, _1, _2);
    cbs->blockedUrlPolicyCallback = std::bind(&AppBoxRenderView::blockedUrlPolicy, this, _1);
    wrt->SetUserDelegates(cbs);

    // set basic webview setting
    setWebViewBasicSetting(wrt->GetCurrentWebview());

    // only box type needed
    if (type == URL_TYPE_BOX) {
        Ewk_Settings* setting = ewk_view_settings_get(wrt->GetCurrentWebview());
        ewk_settings_link_effect_enabled_set(setting, EINA_FALSE);
    }

    return wrt;
}

void AppBoxRenderView::destroyBoxWrtCore()
{
    LogD("enter");

    // stop updating render buffer
    m_boxRenderBuffer->stopCanvasUpdate();
    deleteTimer(&m_boxRenderTimer);
    deleteTimer(&m_boxRemoveSnapShotTimer);
    destroyWrtCore(m_boxWrt);
    m_boxWrt.reset();

    // temp
    m_boxWrt_isSuspended = false;
}

void AppBoxRenderView::destroyGbarWrtCore()
{
    LogD("enter");

    destroyWrtCore(m_gbarWrt);
    m_gbarWrt.reset();
}

void AppBoxRenderView::destroyWrtCore(WrtCorePtr wrt)
{
    LogD("enter");

    if (wrt) {
        wrt->Hide();
    }
}

void AppBoxRenderView::hideBox()
{
    LogD("enter");
    if (m_isTouchableBox) {
        deleteTimer(&m_boxTouchTimer);
    }

    if (m_boxRenderInfo) {
        destroyBoxWrtCore();
        if (m_boxRenderInfo->window) {
            evas_object_hide(m_boxRenderInfo->window);
        }
    }
}

void AppBoxRenderView::pauseBox()
{
    LogD("enter");
}

void AppBoxRenderView::resumeBox()
{
    LogD("enter");
    if (!m_showed) {
        RenderView::show();
    }
}

void AppBoxRenderView::showGbar(RenderInfoPtr gbarRenderInfo, RenderInfoPtr boxRenderInfo)
{
    LogD("enter");

    std::string gbarStartUrl = getStartUrl(URL_TYPE_GBAR, gbarRenderInfo->defaultUrlParams);
    if (m_gbarFastOpen) {
        destroyGbarWrtCore();
        m_gbarWrt = createWrtCore(URL_TYPE_GBAR, gbarStartUrl, gbarRenderInfo->window, m_ewkContext);
        if (!m_gbarWrt) {
            LogD("no wrt core instance");
            return;
        }
        m_gbarHelper = AppBoxGbarHelper::create(gbarRenderInfo->window);

        // resize webview fitted to width, height of gbar
        evas_object_resize(
                m_gbarWrt->GetCurrentWebview(),
                gbarRenderInfo->width,
                gbarRenderInfo->height);
        // show gbar
        m_gbarWrt->Show();
        m_gbarHelper->finishOpen(m_gbarWrt->GetCurrentWebview());
    } else {
        m_gbarHelper = GbarHelper::create(gbarRenderInfo, gbarStartUrl);
    }

    // show gbar window
    evas_object_show(gbarRenderInfo->window);

    // need to  create new snapshot when m_napshot is empty
    if (!m_boxSnapshot) {
        evas_object_show(getCurrentSnapShot());
    }
    getGbarBuffer()->setWebView(m_gbarWrt->GetCurrentWebview());
    // show box
    showBox(boxRenderInfo);

    // start timer for clearing existing snapshot in case of only gbar open
    addTimer(&m_boxRemoveSnapShotTimer, SNAPSHOT_REMOVE_TIME, removeBoxSnapShotTimerCallback);
}

void AppBoxRenderView::hideGbar()
{
    LogD("enter");

    if (m_gbarFastOpen) {
        destroyGbarWrtCore();
    }
    m_gbarHelper->close();
    m_gbarHelper.reset();

    // start timer for clearing existing snapshot in case of only gbar open
    struct stat tmp;
    if (stat(WEB_PROVIDER_INSPECTOR_FILE_PATH, &tmp) == 0) {
        addTimer(&m_boxRenderTimer, RENDER_MAX_TIME_FOR_INSPECTOR, fireBoxRenderTimerCallback);
    } else {
        addTimer(&m_boxRenderTimer, RENDER_MAX_TIME, fireBoxRenderTimerCallback);
    }

}

void AppBoxRenderView::didBoxTouched(int x, int y)
{
    LogD("x : %d, y: %d", x, y);
    if (!m_isTouchableBox) {
        return;
    }

    if (!m_boxWrt) {
        LogD("no webview");
        return;
    }

    // if needed, resume render view
    if (m_boxRenderTimer) {
        deleteTimer(&m_boxRenderTimer);
    } else {
        if (!m_boxTouchTimer) {
            m_boxRenderBuffer->startCanvasUpdate();

            // temp condition
            if (m_boxWrt_isSuspended == true)
            {
                m_boxWrt_isSuspended = false;
                m_boxWrt->Resume();
            }
        } else {
            deleteTimer(&m_boxTouchTimer);
        }
    }
    addTimer(&m_boxTouchTimer, TOUCH_MAX_WAIT_TIME, fireBoxTouchTimerCallback);
}

void AppBoxRenderView::didGbarTouched(int x, int y)
{
    LogD("x : %d, y: %d", x, y);
}

Evas_Object* AppBoxRenderView::getBoxWebView()
{
    if (!m_gbarHelper) {
        return m_boxWrt->GetCurrentWebview();
    } else {
        // Here, we can't use GetCurrentWebView() of wrt-core to get Box' webview,
        // because in the non fast-open, GetCurrentWebview() returns GBAR's webview.
        return m_gbarHelper->getBoxWebView();
    }
}

Evas_Object* AppBoxRenderView::getGbarWebView()
{
    if (!m_gbarHelper) {
        return NULL;
    }

    return m_gbarHelper->getGbarWebView();
}

std::string AppBoxRenderView::getAppId(std::string& boxId)
{
    LogD("enter");

    char* appId = web_provider_widget_get_app_id(boxId.c_str());
    if (!appId) {
        LogD("no appid of %s", boxId.c_str());
        return std::string();
    }

	std::string ret = std::string(appId);
	free(appId);
    return ret;
}

std::string AppBoxRenderView::getStartUrl(UrlType type, std::string& defaultParams)
{
    char* path = NULL;
    switch (type) {
    case URL_TYPE_BOX:
        path = widget_service_get_widget_script_path(m_boxId.c_str());
        break;
    case URL_TYPE_GBAR:
        path = widget_service_get_gbar_script_path(m_boxId.c_str());
        break;
    default:
        LogD("no available type");
    }

    std::string startUrl;
    if (path) {
        LogD("path : %s", path);
        startUrl = path;
        free(path);
        path = NULL;
    } else {
        // TODO In this case, fallback page will be loaded.
        LogE("Fail to get service lib script path");
    }

    // add default parameters to start url
    startUrl += defaultParams;

    return startUrl;
}

Evas_Object* AppBoxRenderView::getCurrentSnapShot()
{
    LogD("enter");

    clearSnapShot();
    m_boxSnapshot = m_boxRenderBuffer->getSnapshot();

    return m_boxSnapshot;
}

void AppBoxRenderView::clearSnapShot()
{
    LogD("enter");
    if (m_boxSnapshot) {
        evas_object_del(m_boxSnapshot);
        m_boxSnapshot = NULL;
    }
}

void AppBoxRenderView::showSnapShot()
{
    LogD("enter");
    if (m_boxSnapshot) {
        evas_object_raise(m_boxSnapshot);
        evas_object_show(m_boxSnapshot);
    }
}

void AppBoxRenderView::hideSnapShot()
{
    LogD("enter");
    if (m_boxSnapshot) {
        evas_object_hide(m_boxSnapshot);
        evas_object_lower(m_boxSnapshot);
    }
}

void AppBoxRenderView::addTimer(Ecore_Timer** timer, double interval, Ecore_Task_Cb callback)
{
    LogD("enter");
    if( timer == NULL )
        return;
    if (*timer) {
        deleteTimer(timer);
    }

    *timer = ecore_timer_add(interval, callback, this);
}

void AppBoxRenderView::deleteTimer(Ecore_Timer** timer)
{
    LogD("enter");
    if( timer == NULL )
        return;
    if (*timer) {
        ecore_timer_del(*timer);
        *timer = NULL;
    }
}

void AppBoxRenderView::stopRenderBox()
{
    deleteTimer(&m_boxRenderTimer);
    m_boxRenderBuffer->stopCanvasUpdate();
    if (m_isTouchableBox) {
        // stop touch timer
        deleteTimer(&m_boxTouchTimer);

        // temp condition
        if (m_boxWrt_isSuspended == false)
        {
            m_boxWrt_isSuspended = true;
            m_boxWrt->Suspend();
        }
    } else {
        // Before webview should be removed,
        // new evas object with last render data should be created
        // otherwise, after webview is removed, box is white screen.
        evas_object_show(getCurrentSnapShot());
        destroyBoxWrtCore();
    }
}

void AppBoxRenderView::setWebViewBasicSetting(Evas_Object* webview)
{
    LogD("enter");

    if (!webview) {
        return;
    }
#if !ENABLE(WEBKIT_UPVERSION)
    Ewk_Settings* setting = ewk_view_settings_get(webview);
    // disable shadow effect on scrolling
    ewk_settings_edge_effect_enabled_set(setting, EINA_FALSE);
    // Disable ime features
    ewk_settings_default_keypad_enabled_set(setting, EINA_FALSE);
#endif
    // To support transparent background
    ewk_view_bg_color_set(webview, 0, 0, 0, 0);

    // To know starting point for updating buffer
    evas_object_smart_callback_add(
            webview,
            "load,nonemptylayout,finished",
            loadNonEmptyLayoutFinishedCallback,
            this);
    evas_object_smart_callback_add(
            webview,
            "frame,rendered",
            frameRenderedCallback,
            this);
    // To set font type whenever font changed
    ewk_view_use_settings_font(webview);
}

void AppBoxRenderView::consoleMessage(int level, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    switch (level) {
    case DLOG_DEBUG:
        ALOG_VA(LOG_DEBUG, kConsoleMessageLogTag.c_str(), format, args);
        break;
    case DLOG_WARN:
        ALOG_VA(LOG_WARN, kConsoleMessageLogTag.c_str(), format, args);
        break;
    case DLOG_ERROR:
        ALOG_VA(LOG_ERROR, kConsoleMessageLogTag.c_str(), format, args);
        break;
    default:
        ALOG_VA(LOG_DEBUG, kConsoleMessageLogTag.c_str(), format, args);
        break;
    }
    va_end(args);
}

Eina_Bool AppBoxRenderView::fireBoxRenderTimerCallback(void* data)
{
    LogD("enter");

    AppBoxRenderView* This = static_cast<AppBoxRenderView*>(data);
    if( This == NULL)
        return ECORE_CALLBACK_CANCEL;

    This->m_boxRenderTimer = NULL;
    This->stopRenderBox();

    return ECORE_CALLBACK_CANCEL;
}

Eina_Bool AppBoxRenderView::fireBoxTouchTimerCallback(void* data)
{

    LogD("enter");
    AppBoxRenderView* This = static_cast<AppBoxRenderView*>(data);
    if( This == NULL)
        return ECORE_CALLBACK_CANCEL;

    This->m_boxRenderBuffer->stopCanvasUpdate();

    // temp condition
    if (This->m_boxWrt_isSuspended == false)
    {
        This->m_boxWrt_isSuspended = true;
        This->m_boxWrt->Suspend();
    }

    This->m_boxTouchTimer = NULL;

    return ECORE_CALLBACK_CANCEL;
}


Eina_Bool AppBoxRenderView::removeBoxSnapShotTimerCallback(void* data)
{
    LogD("enter");

    AppBoxRenderView* This = static_cast<AppBoxRenderView*>(data);
    if( This == NULL)
        return ECORE_CALLBACK_CANCEL;

    if (!(This->m_boxFinishLoad && This->m_boxFrameRendered)) {
        return ECORE_CALLBACK_RENEW;
    }

    if (!This->m_boxWaitFrameRender) {
        This->m_boxWaitFrameRender = true;
        return ECORE_CALLBACK_RENEW;
    }
    // hide snapshot because valid frame has been prepared generally.
    This->clearSnapShot();
    This->m_boxRenderBuffer->startCanvasUpdate();

    if (This->m_showIcon) {
        evas_object_del(This->m_boxIcon);
        This->m_showIcon = false;
    }

    // move to inside of viewport to prevent overlap with snapshot image
    evas_object_move(This->m_boxWrt->GetCurrentWebview(), 0, 0);
    evas_object_show(This->m_boxWrt->GetCurrentWebview());

    This->m_boxRemoveSnapShotTimer = NULL;
    return ECORE_CALLBACK_CANCEL;
}

Eina_Bool AppBoxRenderView::openGbarIdlerCallback(void* data)
{
    LogD("enter");
    AppBoxRenderView* This = static_cast<AppBoxRenderView*>(data);
    if (This && This->m_gbarHelper) {
        This->m_gbarHelper->startOpen();
    }
    return ECORE_CALLBACK_CANCEL;
}

void AppBoxRenderView::executeScriptCallback(
        Evas_Object* webview, const char* result, void* data)
{
    LogD("enter");
    UNUSED_PARAM(webview);
    UNUSED_PARAM(data);

    std::string resultStr(result ? result : "null");
    LogD("result: %s", resultStr.c_str());
}

void AppBoxRenderView::startLoadCallback(Evas_Object* webview, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);
    if(!webview) {
        return;
    }
    // execute injection for creating js objects
    std::ifstream jsFile(kInjectionFile);
    std::string jsString((std::istreambuf_iterator<char>(jsFile)),
                        std::istreambuf_iterator<char>());

    std::ostringstream script;
    script << jsString;

    // add javascripts for operation of synchronous call
    script << kJsRegisterBoxInfoFunction << "('box-id', '" << m_boxId << "');";
    script << kJsRegisterBoxInfoFunction << "('instance-id', '" << m_contentInfo << "');";

    //LogD("injected js code: %s", script.str().c_str());
    ewk_view_script_execute(webview, script.str().c_str(), executeScriptCallback, this);
}

void AppBoxRenderView::finishBoxLoadCallback(Evas_Object* webview, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);
    if (!webview) {
        return;
    }

    if (!m_gbarHelper) {
        // start render timer
        struct stat tmp;
        if (stat(WEB_PROVIDER_INSPECTOR_FILE_PATH, &tmp) == 0) {
            addTimer(&m_boxRenderTimer, RENDER_MAX_TIME_FOR_INSPECTOR, fireBoxRenderTimerCallback);
        } else {
            addTimer(&m_boxRenderTimer, RENDER_MAX_TIME, fireBoxRenderTimerCallback);
        }
    } else {
        if (!m_gbarFastOpen) {
            if (!(m_gbarHelper->isGbarOpened()) &&
                    webview == m_gbarHelper->getBoxWebView())
            {
                // open gbar
                ecore_idler_add(openGbarIdlerCallback, this);
            }
        }
    }

    // set flag
    m_boxFinishLoad = true;
}

void AppBoxRenderView::finishGbarLoadCallback(Evas_Object* webview, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(eventInfo);
    if (!webview) {
        return;
    }
}

void AppBoxRenderView::createWindowBeforeCallback(Evas** canvas, Evas_Object* parent)
{
    LogD("enter");

    if (m_gbarHelper) {
        if (!(m_gbarHelper->isGbarOpened()) &&
                parent == m_gbarHelper->getBoxWebView())
        {
            LogD("gbar canvas is used");
            *canvas = m_gbarHelper->getGbarCanvas();
            return;
        }
    }

    LogD("canvas of this webview is used");
    *canvas = evas_object_evas_get(parent);
}

void AppBoxRenderView::createWindowAfterCallback(Evas_Object* parent, Evas_Object* child)
{
    LogD("enter");
    if (!parent) {
        return;
    }

    if (m_gbarHelper) {
        Evas* parentCanvas = evas_object_evas_get(parent);
        Evas* childCanvas = evas_object_evas_get(child);

        if (parentCanvas != childCanvas) {
           // wrt-core change visibility value to false internally
           // So plugin should reset this value to true for painting parent webview
           evas_object_show(parent);
           m_gbarHelper->finishOpen(child);
        }
    }

    setWebViewBasicSetting(child);
    evas_object_show(child);
}

void AppBoxRenderView::setBufferCallback(Evas_Object* webview)
{
    LogD("enter");
    evas_object_show(webview);
    evas_object_focus_set(webview, EINA_TRUE);
}

void AppBoxRenderView::unsetBufferCallback(Evas_Object* webview)
{
    LogD("enter");
    evas_object_hide(webview);
}

bool AppBoxRenderView::decideNavigationCallback(Evas_Object* webview, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(webview);

    Ewk_Policy_Decision* policyDecision = static_cast<Ewk_Policy_Decision*>(eventInfo);
    const char* url = ewk_policy_decision_url_get(policyDecision);
    if (!url || !*url) {
        LogE("url is empty");
        return false;
    }
    std::string uri(url);
    if(m_blockedURI == uri) {
        LogE("url is blocked by policy: %s ", url);
        return false;
    }

    // navigation of box scheme should be ignored
    processBoxScheme(uri);
    return true;
}

void AppBoxRenderView::crashWebProcessCallback(Evas_Object* webview, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(webview);
    UNUSED_PARAM(eventInfo);
    LogD("instanceId: %s", m_instanceId.c_str());
    elm_exit();
}

void AppBoxRenderView::loadNonEmptyLayoutFinishedCallback(
        void* data, Evas_Object* webview, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(data);
    UNUSED_PARAM(webview);
    UNUSED_PARAM(eventInfo);
}

void AppBoxRenderView::frameRenderedCallback(
        void* data, Evas_Object* webview, void* eventInfo)
{
    LogD("enter");
    UNUSED_PARAM(webview);
    UNUSED_PARAM(eventInfo);

    // start to update render buffer!
    AppBoxRenderView* This = static_cast<AppBoxRenderView*>(data);

    if( This == NULL )
        return;

    // set flag
    This->m_boxFrameRendered = true;

    // move to inside of viewport to prevent overlap with snapshot image
    if (!This->m_boxRemoveSnapShotTimer) {
        evas_object_move(This->m_boxWrt->GetCurrentWebview(), 0, 0);
    }
}

void AppBoxRenderView::consoleMessageCallback(Evas_Object* webview, void* eventInfo)
{
    UNUSED_PARAM(webview);

    Ewk_Console_Message* consoleMessage = static_cast<Ewk_Console_Message*>(eventInfo);

    std::stringstream buf;
    unsigned int lineNumber = ewk_console_message_line_get(consoleMessage);
    const char* text = ewk_console_message_text_get(consoleMessage);
    const char* source = ewk_console_message_source_get(consoleMessage);
    if (lineNumber) {
        buf << source << ":";
        buf << lineNumber << ":";
    }
    buf << text;

    int level;
    switch (ewk_console_message_level_get(consoleMessage)) {
    case EWK_CONSOLE_MESSAGE_LEVEL_TIP:
    case EWK_CONSOLE_MESSAGE_LEVEL_LOG:
    case EWK_CONSOLE_MESSAGE_LEVEL_DEBUG:
        level = DLOG_DEBUG;
        break;
    case EWK_CONSOLE_MESSAGE_LEVEL_WARNING:
        level = DLOG_WARN;
        break;
    case EWK_CONSOLE_MESSAGE_LEVEL_ERROR:
        level = DLOG_ERROR;
        break;
    default:
        level = DLOG_DEBUG;
        break;
    }
    AppBoxRenderView::consoleMessage(level, "%s", buf.str().c_str());
}

bool AppBoxRenderView::blockedUrlPolicy (const std::string& blockedUrl)
{
    LogD("Request was blocked : %s \n", blockedUrl.c_str() );
    m_blockedURI = blockedUrl;
    return true;
}

void AppBoxRenderView::showIcon(char *iconString, int width, int height)
{
    LogD("enter");

    Evas_Load_Error err;
    Evas_Object *snapshot;
    int w,h;
    int x = 0;
    int y = 0;

    if (!iconString) {
        LogE("iconString is NULL");
        return;
    }

    snapshot = evas_object_image_add(m_boxRenderBuffer->getCanvas());
    if (!snapshot) {
        LogE("evas_object_image_add FAILED");
        return;
    }

    const char *string = const_cast<const char *>(iconString);
    evas_object_image_colorspace_set(snapshot, EVAS_COLORSPACE_ARGB8888);
    evas_object_image_alpha_set(snapshot, EINA_TRUE);
    evas_object_image_file_set(snapshot, string, NULL);
    err = evas_object_image_load_error_get(snapshot);
    if (err != EVAS_LOAD_ERROR_NONE) {
        LogE("Load error: %s\n", evas_load_error_str(err));
        evas_object_del(snapshot);
        return;
    }
    evas_object_image_size_get(snapshot, &w, &h);
    evas_object_image_filled_set(snapshot, EINA_TRUE);


    if (width > w) {
        x = (width - w) >> 1;
    }

    if (height > h) {
        y = (height - h) >> 1;
    }

    evas_object_image_fill_set(snapshot, 0, 0, w, h);
    evas_object_move(snapshot, x, y);
    evas_object_show(snapshot);
    evas_object_resize(snapshot, w, h);

    widget_damage_region_s region;
    region.x = 0;
    region.y = 0;
    region.w = width;
    region.h = height;

    widget_provider_send_updated(
        m_boxId.c_str(),
        m_instanceId.c_str(),
        WIDGET_PRIMARY_BUFFER,
        &region,
        0,
        NULL);

    m_boxIcon = snapshot;
    m_showIcon = true;
}
