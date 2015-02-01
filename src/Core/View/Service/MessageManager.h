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
 * @file    MessageManager.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */

#ifndef MESSAGE_MANAGER_H
#define MESSAGE_MANAGER_H

#include <string>
#include <memory>
#include <Evas.h>

namespace Service {

class MessageManager;
typedef std::shared_ptr<MessageManager> MessageManagerPtr;

class MessageManager {
    public:
        enum ReceiverType {
            TO_BOX,
            TO_PD
        };

        static MessageManagerPtr create() {
            return MessageManagerPtr(new MessageManager());
        }
        bool send(Evas_Object* webview, ReceiverType receiver, std::string& message);
        ~MessageManager();

    private:
        static void executeScriptCallback(
                Evas_Object* webview, const char* result, void* data);
        MessageManager();
};
} // Service

#endif // MESSAGE_MANAGER_H
