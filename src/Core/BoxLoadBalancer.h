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
 * @file    BoxLoadBalancer.h
 * @author  Minhyung Ko (minhyung.ko@samsung.com)
 */
#ifndef BOX_LOAD_BALANCER_H
#define BOX_LOAD_BALANCER_H

#include <Ecore.h>
#include <memory>
#include <string>
#include <list>
#include "BoxData.h"
#include "IBox.h"
#include "IBoxManager.h"

typedef enum {
    BOX_AYNC_REQUEST_CMD_SHOW,
    BOX_AYNC_REQUEST_CMD_RESUME,
    BOX_AYNC_REQUEST_CMD_UPDATE
} request_box_cmd_type;


class BoxAsyncCommandInfo
{
private:
    BoxAsyncCommandInfo() {};
public:
    BoxAsyncCommandInfo(request_box_cmd_type, IBoxPtr, BoxInfoPtr);
    BoxAsyncCommandInfo(request_box_cmd_type, IBoxPtr, std::string&);
    ~BoxAsyncCommandInfo() {};

    request_box_cmd_type getType() { return m_type; }
    IBoxPtr getBoxPtr() { return m_box; }
    BoxInfoPtr getBoxInfoPtr() {return m_boxInfo; }
    std::string getAppContentInfo() {return m_appContentInfo; }

private:
    request_box_cmd_type m_type;
    IBoxPtr m_box;
    BoxInfoPtr m_boxInfo;
    std::string m_appContentInfo;
};

typedef std::shared_ptr<BoxAsyncCommandInfo> BoxAsyncCommandInfoPtr;

class BoxLoadBalancer
{
public:
    BoxLoadBalancer();
    ~BoxLoadBalancer();

    static Eina_Bool requestBoxLoadBalanceCallback(void* data);

    void eraseBoxAsyncCommandFromQueue(IBoxPtr boxPtr);
    void pushBoxAsyncCommandIntoQueue(request_box_cmd_type type, IBoxPtr boxPtr, BoxInfoPtr boxInfoPtr);
    void pushBoxAsyncCommandIntoQueue(request_box_cmd_type, IBoxPtr, std::string&);
    void pushBoxAsyncCommandIntoTop(request_box_cmd_type cmdType, IBoxPtr boxPtr, BoxInfoPtr boxInfoPtr);
    void stopLoadBalancer();

private:
    // timer for load balance of boxes
    Ecore_Timer* m_boxLoadBalanceTimer;
    std::list<BoxAsyncCommandInfoPtr> m_boxAsyncCommandList;
    float m_updateTimeCount;
};
#endif // BOX_LOAD_BALANCER_H
