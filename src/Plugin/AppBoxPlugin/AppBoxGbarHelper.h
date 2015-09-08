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
 * @file    AppBoxGbarHelper.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */

#ifndef APP_BOX_GBAR_HELPER_H
#define APP_BOX_GBAR_HELPER_H

#include <string>
#include <Evas.h>
#include <Core/View/IGbarHelper.h>

class AppBoxGbarHelper: public IGbarHelper {
    public:
        static IGbarHelperPtr create(Evas_Object* gbarWin)
        {
            return IGbarHelperPtr(new AppBoxGbarHelper(gbarWin));
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
        virtual ~AppBoxGbarHelper();

    private:
        AppBoxGbarHelper(Evas_Object* gbarWin);

        //members
        Evas_Object* m_win;
        Evas_Object* m_boxWebView;
        Evas_Object* m_GbarWebView;
        bool m_opened;
};

#endif // APP_BOX_GBAR_HELPER_H