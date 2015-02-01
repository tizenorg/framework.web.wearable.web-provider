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
 * @file    AppBoxPdHelper.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */

#ifndef APP_BOX_PD_HELPER_H
#define APP_BOX_PD_HELPER_H

#include <string>
#include <Evas.h>
#include <Core/View/IPdHelper.h>

class AppBoxPdHelper: public IPdHelper {
    public:
        static IPdHelperPtr create(Evas_Object* pdWin)
        {
            return IPdHelperPtr(new AppBoxPdHelper(pdWin));
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
        virtual ~AppBoxPdHelper();

    private:
        AppBoxPdHelper(Evas_Object* pdWin);

        //members
        Evas_Object* m_win;
        Evas_Object* m_boxWebView;
        Evas_Object* m_pdWebView;
        bool m_opened;
};

#endif // APP_BOX_PD_HELPER_H
