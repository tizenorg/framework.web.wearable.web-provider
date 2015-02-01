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
 * @file    RenderView.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef RENDER_VIEW_H
#define RENDER_VIEW_H

#include <string>
#include <memory>
#include <functional>
#include <Evas.h>
#include <Core/BoxData.h>
#include <Core/Buffer/IRenderBuffer.h>
#include "IJsInterface.h"
#include "IRenderView.h"

#define EXPORT_CLASS    __attribute__ ((visibility("default")))

typedef std::function<void(int, int)> TouchViewCallback;

struct RenderInfo {
    int width;
    int height;
    Evas_Object* window;
    std::string defaultUrlParams;
};

typedef std::shared_ptr<RenderInfo> RenderInfoPtr;

class EXPORT_CLASS RenderView: public IRenderView {
    public:
        static IRenderViewPtr create(BoxInfoPtr boxInfo)
        {
            return IRenderViewPtr(new RenderView(boxInfo));
        };
        void show();
        void hide();
        void resize();
        void update();
        void resume();
        void pause();
        void openPd();
        void closePd();
        virtual Evas_Object* getBoxWebView();
        virtual Evas_Object* getPdWebView();
        virtual ~RenderView();

    protected:
        virtual void showBox(RenderInfoPtr boxRenderInfo);
        virtual void hideBox();
        virtual void pauseBox();
        virtual void resumeBox();
        virtual void showPd(RenderInfoPtr pdRenderInfo, RenderInfoPtr boxRenderInfo);
        virtual void hidePd();
        virtual void didBoxTouched(int x, int y);
        virtual void didPdTouched(int x, int y);

        // To be used by derived class
        bool processBoxScheme(std::string& uri);
        IRenderBufferPtr getBoxBuffer() const;
        IRenderBufferPtr getPdBuffer() const;

        RenderView(BoxInfoPtr boxInfo, bool hwEnable = false);
        RenderInfoPtr makeRenderInfo(const std::string& renderType, UrlType urlType) const;

    private:
        void showInternal(const std::string& renderType);
        static void touchBoxCallback(int x, int y, RenderView* This);
        static void touchPdCallback(int x, int y, RenderView* This);
        static Eina_Bool startUpdateRenderBufferIdlerCallback(void* data);

        // members
        BoxInfoPtr m_boxInfo;
        IRenderBufferPtr m_boxBuffer;
        IRenderBufferPtr m_pdBuffer;
        IJsInterfacePtr m_jsInterface;
        // timestamp for last time of showing box.
        time_t m_lastShowBoxTime;

        friend class BoxRenderBuffer;
        friend class PdRenderBuffer;
};

#endif // RENDER_VIEW_H
