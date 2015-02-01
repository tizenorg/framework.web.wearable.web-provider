
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
 * @file    PdHelper.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */

#ifndef PD_HELPER_H
#define PD_HELPER_H

#include <string>
#include <Evas.h>
#include "RenderView.h"
#include "IPdHelper.h"

#define EXPORT_CLASS    __attribute__ ((visibility("default")))

class EXPORT_CLASS PdHelper: public IPdHelper {
    public:
        static IPdHelperPtr create(
                RenderInfoPtr pdRenderInfo,
                std::string pdStartUrl)
        {
            return IPdHelperPtr(new PdHelper(pdRenderInfo, pdStartUrl));
        }
        virtual void startOpen();
        virtual void finishOpen(Evas_Object* child);
        virtual void close();
        virtual void setBoxWebView(Evas_Object* webview);
        virtual void setPdWebView(Evas_Object* webview);
        virtual Evas_Object* getBoxWebView() const;
        virtual Evas_Object* getPdWebView() const;
        virtual Evas* getPdCanvas() const;
        virtual bool isPdOpened() const;
        virtual ~PdHelper();

    private:
        virtual void didExecuteScript(Evas_Object* webview, std::string& result);

        std::string validateUrl(std::string& url);
        static void executeScriptCallback(
            Evas_Object* webview, const char* result, void* data);

        PdHelper(RenderInfoPtr pdRenderInfo, std::string pdStartUrl);

        //members
        Evas_Object* m_boxWebView;
        Evas_Object* m_pdWebView;
        RenderInfoPtr m_pdRenderInfo;
        std::string m_startUrl;
        bool m_opened;
};

#endif // PD_HELPER_H

