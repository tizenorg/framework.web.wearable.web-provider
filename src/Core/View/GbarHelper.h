
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
 * @file    GbarHelper.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */

#ifndef PD_HELPER_H
#define PD_HELPER_H

#include <string>
#include <Evas.h>
#include "RenderView.h"
#include "IGbarHelper.h"

#define EXPORT_CLASS    __attribute__ ((visibility("default")))

class EXPORT_CLASS GbarHelper: public IGbarHelper {
    public:
        static IGbarHelperPtr create(
                RenderInfoPtr gbarRenderInfo,
                std::string gbarStartUrl)
        {
            return IGbarHelperPtr(new GbarHelper(gbarRenderInfo, gbarStartUrl));
        }
        virtual void startOpen();
        virtual void finishOpen(Evas_Object* child);
        virtual void close();
        virtual void setBoxWebView(Evas_Object* webview);
        virtual void setGbarWebView(Evas_Object* webview);
        virtual Evas_Object* getBoxWebView() const;
        virtual Evas_Object* getGbarWebView() const;
        virtual Evas* getGbarCanvas() const;
        virtual bool isGbarOpened() const;
        virtual ~GbarHelper();

    private:
        virtual void didExecuteScript(Evas_Object* webview, std::string& result);

        std::string validateUrl(std::string& url);
        static void executeScriptCallback(
            Evas_Object* webview, const char* result, void* data);

        GbarHelper(RenderInfoPtr gbarRenderInfo, std::string gbarStartUrl);

        //members
        Evas_Object* m_boxWebView;
        Evas_Object* m_gbarWebView;
        RenderInfoPtr m_gbarRenderInfo;
        std::string m_startUrl;
        bool m_opened;
};

#endif // PD_HELPER_H