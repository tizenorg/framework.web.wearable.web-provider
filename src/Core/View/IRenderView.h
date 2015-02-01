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
 * @file    IRenderView.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef I_RENDER_VIEW_H
#define I_RENDER_VIEW_H

#include <string>
#include <memory>
#include <Evas.h>

class IRenderView {
    public:
        enum UrlType {
            URL_TYPE_BOX,
            URL_TYPE_PD
        };
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual void resize() = 0;
        virtual void update() = 0;
        virtual void pause() = 0;
        virtual void resume() = 0;
        virtual void openPd() = 0;
        virtual void closePd() = 0;
        virtual Evas_Object* getBoxWebView() = 0;
        virtual Evas_Object* getPdWebView() = 0;
        virtual ~IRenderView() {};
};

typedef std::shared_ptr<IRenderView> IRenderViewPtr;

#endif // I_RENDER_VIEW_H 
