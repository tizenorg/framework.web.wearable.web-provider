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
 * @file    IRenderBuffer.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef I_RENDER_BUFFER_H
#define I_RENDER_BUFFER_H

#include <memory>
#include <Evas.h>
#include <Util/Noncopyable.h>

class IRenderBuffer: Noncopyable {
    public:
        virtual bool allocate(bool hwEnable = false) = 0;
        virtual bool reallocate(int width, int height) = 0;
        virtual bool free() = 0;
        virtual void startCanvasUpdate() = 0;
        virtual void stopCanvasUpdate() = 0;
        virtual Evas_Object* getWindow() = 0;
        virtual Evas_Object* getSnapshot() = 0;
        virtual void* getBufferAddr() = 0;
        virtual ~IRenderBuffer() {};
        virtual void setWebView(Evas_Object* webview) = 0;
        virtual Evas_Object* getWebView() = 0;
        virtual Evas* getCanvas() = 0;
};

typedef std::shared_ptr<IRenderBuffer> IRenderBufferPtr;

#endif
