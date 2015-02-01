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
 * @file    PdRenderBuffer.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef PD_RENDER_BUFFER_H
#define PD_RENDER_BUFFER_H

#include <string>
#include <provider_buffer.h>
#include <Core/BoxData.h>
#include <Core/View/RenderView.h>
#include "IRenderBuffer.h"
#include "RenderBuffer.h"

#define EXPORT_CLASS    __attribute__ ((visibility("default")))

class EXPORT_CLASS PdRenderBuffer: public RenderBuffer {
    public:
        enum TouchType {
            TOUCH_EVENT_UNRECOGNIZED = -1,
            TOUCH_EVENT_MOVE = 0,
            TOUCH_EVENT_DOWN,
            TOUCH_EVENT_UP,
            TOUCH_EVENT_ON_HOLD
        };

        static IRenderBufferPtr create(BoxInfoPtr boxInfo, TouchViewCallback touchCallback)
        {
            return IRenderBufferPtr(new PdRenderBuffer(boxInfo, touchCallback));
        };
        ~PdRenderBuffer();

    protected:
        // this function may be overriden by derived class
        virtual void didHandleTouchEvent(
                TouchType type, double timestamp, int x, int y);
        explicit PdRenderBuffer(BoxInfoPtr boxInfo, TouchViewCallback touchCallback);

    private:
        // RenderBuffer Implementation
        int getWidth() { return m_width; };
        int getHeight() { return m_height; };
        void setWidth(int width) { m_width = width; };
        void setHeight(int height) { m_height = height; };
        BufferInfoPtr acquireBuffer();
        void updateBuffer();

        // touch callback
        static int handleTouchEventCallback(
                BufferInfoPtr bufferInfo,
                struct buffer_event_data *evt,
                void* data);

        // members
        std::string m_boxId;
        std::string m_instanceId;
        int m_width;
        int m_height;
        TouchViewCallback m_touchCallback;
        bool m_accessibiilty;
};

#endif // PD_RENDER_BUFFER_H
