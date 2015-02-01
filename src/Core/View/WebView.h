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
 * @file    WebView.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef WEB_VIEW_H
#define WEB_VIEW_H

#include <string>
#include <map>
#include <Eina.h>
#include <Evas.h>
#include <EWebKit.h>
#include <EWebKit_internal.h>
#include "IWebView.h"

#define EXPORT_CLASS __attribute__ ((visibility("default")))

class EXPORT_CLASS WebView: public IWebView {
    public:
        static IWebViewPtr create(Evas_Object* win, Ewk_Context* ewkContext)
        {
            if (!win || !ewkContext) {
                return IWebViewPtr();
            }

            return IWebViewPtr(new WebView(win, ewkContext));
        }
        virtual bool show(
            std::string& startUrl, int width, int height,
            WebViewCallback didCreateBaseWebView, void* data);
        virtual bool hide();
        virtual bool suspend();
        virtual bool resume();
        virtual ~WebView();

        // webviw smart callbacks
        static void loadStartedCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void loadFinishedCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void titleChangedCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void uriChangedCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void loadProgressCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void loadProgressFinishedCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void processCrashedCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void createWindowCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void closeWindowCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void policyNavigationDecideCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void policyNewWindowDecideCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void pageResponseDecideCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void contextmenuCustomizeCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void formSubmitCallback(
                void *data, Evas_Object *obj, void *eventInfo);
        static void geolocationPermissionRequestCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void notificationShowCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void notificationCancelCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void notificationPermissionRequestCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void databaseUsagePermissionRequestCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void fileSystemPermissionRequestCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void enterFullscreenCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void exitFullscreenCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void imeChangedCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void imeOpenedCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void imeClosedCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void usermediaPermissionRequestCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void protocolHandlerRegistrationCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void protocolHandlerIsRegisteredCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void protocolHandlerUnregistrationCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void contentHandlerRegistrationCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void contentHandlerIsRegisteredCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void contentHandlerUnregistrationCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void certificateConfirmRequestCallback(
                void* data, Evas_Object* obj, void* eventInfo);
        static void consoleMessageCallback(
                void* data, Evas_Object* obj, void* eventInfo);

    protected:
        bool setBasicSetting(Evas_Object* webview);
        bool unsetBasicSetting(Evas_Object* webview);
        explicit WebView(Evas_Object* win, Ewk_Context* ewkContext);

    private:
        std::string validateUrl(std::string& url);
        void consoleMessage(int level, const char* message);

        // internal implementation for smart callbacks
        virtual void didLoadStarted(Evas_Object* obj);
        virtual void didLoadFinished(Evas_Object* obj);
        virtual void didTitleChanged(Evas_Object* obj, void* eventInfo);
        virtual void didUriChanged(Evas_Object* obj, void* eventInfo);
        virtual void didLoadProgress(Evas_Object* obj, void* eventInfo);
        virtual void didLoadProgressFinished(Evas_Object* obj);
        virtual void didProcessCrashed(Evas_Object* obj);
        virtual void didCreateWindow(Evas_Object* obj, void* eventInfo);
        virtual void didCloseWindow(Evas_Object* obj);
        virtual void didPolicyNavigationDecide(Evas_Object* obj, void* eventInfo);
        virtual void didPolicyNewWindowDecide(Evas_Object* obj, void* eventInfo);
        virtual void didPageResponseDecide(Evas_Object* obj);
        virtual void didContextmenuCustomize(Evas_Object* obj, void* eventInfo);
        virtual void didFormSubmit(Evas_Object* obj);
        virtual void didGeolocationPermissionRequest(Evas_Object* obj, void* eventInfo);
        virtual void didNotificationShow(Evas_Object* obj, void* eventInfo);
        virtual void didNotificationCancel(Evas_Object* obj, void* eventInfo);
        virtual void didNotificationPermissionRequest(Evas_Object* obj, void* eventInfo);
        virtual void didDatabaseUsagePermissionRequest(Evas_Object* obj, void* eventInfo);
        virtual void didFilesystemPermissionRequest(Evas_Object* obj, void* eventInfo);
        virtual void didEnterFullscreen(Evas_Object* obj);
        virtual void didExitFullscreen(Evas_Object* obj);
        virtual void didImeChanged(Evas_Object* obj, void* eventInfo);
        virtual void didImeOpened(Evas_Object* obj);
        virtual void didImeClosed(Evas_Object* obj);
        virtual void didUsermediaPermissionRequest(Evas_Object* obj, void* eventInfo);
        virtual void didProtocolHandlerRegistration(Evas_Object* obj, void* eventInfo);
        virtual void didProtocolHandlerIsRegistered(Evas_Object* obj, void* eventInfo);
        virtual void didProtocolHandlerUnregistration(Evas_Object* obj, void* eventInfo);
        virtual void didContentHandlerRegistration(Evas_Object* obj, void* eventInfo);
        virtual void didContentHandlerIsRegistered(Evas_Object* obj, void* eventInfo);
        virtual void didContentHandlerUnregistration(Evas_Object* obj, void* eventInfo);
        virtual void didCertificateConfirmRequest(Evas_Object* obj, void* eventInfo);

        // orientation lock
        static Eina_Bool orientationLockCallback(
                Evas_Object* obj, Eina_Bool needLock, int orientation, void* data);
        static void executeScriptCallback(Evas_Object* obj, const char* result, void* data);

        // members
        Evas_Object* m_topWebview;
        std::string m_startUrl;
        Evas_Object* m_win;
        Ewk_Context* m_ewkContext;
};

#endif // WEB_VIEW_H
