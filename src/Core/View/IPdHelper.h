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
 * @file    IPdHelper.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef I_PD_HELPER_H
#define I_PD_HELPER_H

#include <Evas.h>
#include <memory>

class IPdHelper {
    public:
        virtual void startOpen() = 0;
        virtual void finishOpen(Evas_Object* child) = 0;
        virtual void close() = 0;
        virtual void setBoxWebView(Evas_Object* webview) = 0;
        virtual void setPdWebView(Evas_Object* webview) = 0;
        virtual Evas_Object* getBoxWebView() const = 0;
        virtual Evas_Object* getPdWebView() const = 0;
        virtual Evas* getPdCanvas() const = 0;
        virtual bool isPdOpened() const = 0;
};

typedef std::shared_ptr<IPdHelper> IPdHelperPtr;

#endif // I_PD_HELPER_H
