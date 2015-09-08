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
 * @file    AppBoxRenderView.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef APP_BOX_RENDER_VIEW_H
#define APP_BOX_RENDER_VIEW_H

#include <string>
#include <memory>
#include <Eina.h>
#include <Ecore.h>
#include <Evas.h>
#include <EWebKit.h>
#include <EWebKit_internal.h>
#include <i_runnable_widget_object.h>
#include <Core/BoxData.h>
#include <Core/View/IGbarHelper.h>
#include <Core/View/RenderView.h>

class AppBoxRenderBuffer;

class AppBoxRenderView: public RenderView {
    public:
        typedef std::shared_ptr<Ewk_Context> EwkContextPtr;

        static IRenderViewPtr create(BoxInfoPtr boxInfo, EwkContextPtr ewkContext)
        {
            return IRenderViewPtr(new AppBoxRenderView(boxInfo, ewkContext));
        };
        virtual void showBox(RenderInfoPtr boxRenderInfo);
        virtual void hideBox();
        virtual void pauseBox();
        virtual void resumeBox();
        virtual void showGbar(RenderInfoPtr gbarRenderInfo, RenderInfoPtr boxRenderInfo);
        virtual void hideGbar();
        virtual void didBoxTouched(int x, int y);
        virtual void didGbarTouched(int x, int y);
        virtual ~AppBoxRenderView();

    private:
        // type definition
        typedef std::shared_ptr<WRT::IRunnableWidgetObject> WrtCorePtr;
        enum UrlType {
            URL_TYPE_BOX,
            URL_TYPE_GBAR
        };

        WrtCorePtr createWrtCore(
                UrlType type, std::string& startUrl,
                Evas_Object* win, EwkContextPtr ewkContext);
        void setWebViewBasicSetting(Evas_Object* webview);
        void destroyWrtCore(WrtCorePtr wrt);
        void destroyBoxWrtCore();
        void destroyGbarWrtCore();
        Evas_Object* getBoxWebView();
        Evas_Object* getGbarWebView();
        std::string getAppId(std::string& boxId);
        std::string getStartUrl(UrlType type, std::string& defaultParams);
        Evas_Object* getCurrentSnapShot();
        void clearSnapShot();
        void showSnapShot();
        void hideSnapShot();
        void addTimer(Ecore_Timer** timer, double interval, Ecore_Task_Cb callback);
        void deleteTimer(Ecore_Timer** timer);
        void stopRenderBox();
        void consoleMessage(int level, const char* format, ...);
        void showIcon(char *iconString, int width, int height);

        // timer and idler callback
        static Eina_Bool fireBoxRenderTimerCallback(void* data);
        static Eina_Bool fireBoxTouchTimerCallback(void* data);
        static Eina_Bool removeBoxSnapShotTimerCallback(void* data);
        static Eina_Bool openGbarIdlerCallback(void* data);

        // ewk view callback
        static void executeScriptCallback(
                Evas_Object* webview, const char* result, void* data);
        static void loadNonEmptyLayoutFinishedCallback(
                void* data, Evas_Object* webview, void* eventInfo);
        static void frameRenderedCallback(
                void* data, Evas_Object* webview, void* eventInfo);

        // user Callbacks of RunnableWidgetObject
        void startLoadCallback(Evas_Object* webview, void* eventInfo);
        void finishBoxLoadCallback(Evas_Object* webview, void* eventInfo);
        void finishGbarLoadCallback(Evas_Object* webview, void* eventInfo);
        void createWindowBeforeCallback(Evas** canvas, Evas_Object* parent);
        void createWindowAfterCallback(Evas_Object* parent, Evas_Object* child);
        void setBufferCallback(Evas_Object* webview);
        void unsetBufferCallback(Evas_Object* webview);
        bool decideNavigationCallback(Evas_Object* webview, void* eventInfo);
        void crashWebProcessCallback(Evas_Object* webview, void* eventInfo);
        void consoleMessageCallback(Evas_Object* webview, void* eventInfo);
        bool blockedUrlPolicy (const std::string& blockedUrl);

        // constructor
        explicit AppBoxRenderView(BoxInfoPtr boxInfo, EwkContextPtr ewkContext);

        // members
        std::string m_appId;
        std::string m_boxId;
        std::string m_instanceId;
        std::string m_contentInfo;
        std::string m_blockedURI;
        EwkContextPtr m_ewkContext;
        RenderInfoPtr m_boxRenderInfo;
        WrtCorePtr m_boxWrt;
        WrtCorePtr m_gbarWrt;
        IRenderBufferPtr m_boxRenderBuffer;
        IRenderBufferPtr m_gbarRenderBuffer;
        Evas_Object* m_boxSnapshot;
        Ecore_Timer* m_boxRenderTimer;
        Ecore_Timer* m_boxTouchTimer;
        Ecore_Timer* m_boxRemoveSnapShotTimer;
        IGbarHelperPtr m_gbarHelper;
        Evas_Object* m_boxIcon;

        // for check status of webview
        bool m_gbarFastOpen;
        bool m_boxFinishLoad;
        bool m_boxFrameRendered;
        bool m_boxWaitFrameRender;
        bool m_isTouchableBox;

        // TODO this temporary flag should removed!
        bool m_boxWrt_isSuspended;
        bool m_showed;
        bool m_showIcon;
};

#endif // APP_BOX_RENDER_VIEW_H
