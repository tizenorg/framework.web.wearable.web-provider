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
 * @file    RenderBuffer.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef RENDER_BUFFER_H
#define RENDER_BUFFER_H

#include <memory>
#include <Ecore_Evas.h>
#include <Evas.h>
#include "IRenderBuffer.h"

// forward declaration
struct widget_buffer;

// type definition
typedef struct widget_buffer* BufferInfoPtr;
typedef void* BufferAddrPtr;

#define EXPORT_CLASS    __attribute__ ((visibility("default")))

class EXPORT_CLASS RenderBuffer: public IRenderBuffer {
    public:
        // IRenderBuffer Implementation
        bool allocate(bool hwEnable = false);
        bool reallocate(int width, int height);
        bool free();
        void startCanvasUpdate();
        void stopCanvasUpdate();
        Evas_Object* getWindow();
        Evas_Object* getSnapshot();
        void setWebView(Evas_Object* webview){ m_webview = webview; };
        Evas_Object* getWebView(){ return m_webview; };
        Evas* getCanvas();

        static void preRenderCallback(void* data, Evas* canvas, void* eventInfo);
        static void postRenderCallback(void* data, Evas* canvas, void* eventInfo);

        virtual ~RenderBuffer();

    protected:
        void paintColor(unsigned int color);


        // provided by derived class
        virtual int getWidth() = 0;
        virtual int getHeight() = 0;
        virtual void setWidth(int width) = 0;
        virtual void setHeight(int height) = 0;
        virtual BufferInfoPtr acquireBuffer() = 0;
        virtual void updateBuffer() = 0;
        void* getBufferAddr() { return m_hwEnable ? m_bufferAddr : 0; }
        RenderBuffer();
        Evas_Object* m_win;
        Evas_Object* m_layout;

    private:
        // callbacks
        static void* allocateCallback(void* data, int size);
        static void freeCallback(void* data, void *pix);
        bool bufferAllocationForGLRendering();
        bool bufferAllocationForSWRendering();
        void bufferReAllocation(int width, int height);

        // members
        Evas* m_canvas;

        BufferAddrPtr m_bufferAddr;
        BufferInfoPtr m_bufferInfo;
        bool m_hwEnable;
        Evas_Object* m_webview;
};

#endif
