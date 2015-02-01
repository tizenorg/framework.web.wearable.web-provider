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
 * @file    IWebView.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef I_WEB_VIEW_H
#define I_WEB_VIEW_H

#include <string>
#include <memory>

class IWebView {
    public:
        typedef void (*WebViewCallback)(void* data, void* eventInfo);
        virtual bool show(
            std::string& startUrl, int width, int height,
            WebViewCallback didCreateBaseWebView, void* data) = 0;
        virtual bool hide() = 0;
        virtual bool suspend() = 0;
        virtual bool resume() = 0;
        virtual bool setBasicSetting(Evas_Object* webview) = 0;
        virtual bool unsetBasicSetting(Evas_Object* webview) = 0;
    
    public:
        virtual ~IWebView() {};
};

typedef std::shared_ptr<IWebView> IWebViewPtr;

#endif // I_WEB_VIEW_H
