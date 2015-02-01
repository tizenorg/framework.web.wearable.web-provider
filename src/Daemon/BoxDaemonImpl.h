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
 * @file    BoxDaemonImpl.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef BOX_DAEMON_IMPL_H
#define BOX_DAEMON_IMPL_H

#include <string>
#include <memory>
#include <vector>

#include <app.h>
#include <provider.h>
#include <Eina.h>
#include <Ecore.h>
#include <vconf.h>

#include <Plugin/box_plugin_interface.h>
#include <Core/BoxData.h>

// forward declaration
class IBoxManager;
class IBoxPluginConnector;

class BoxDaemonImpl {
    public:
        bool start(std::string& name);
        bool stop();
        bool handleAppControl(app_control_h app_control);

    public:
        explicit BoxDaemonImpl();
        ~BoxDaemonImpl();
    
    private:
        // type definition
        typedef struct event_arg ProviderEventArg;
        typedef ProviderEventArg* ProviderEventArgPtr;
        typedef struct event_handler ProviderCallbacks;
        typedef ProviderCallbacks* ProviderCallbacksPtr;

        // livebox's provider callbacks
        static int connectedCallback(ProviderEventArgPtr arg, void* data);
        static int disconnectedCallback(ProviderEventArgPtr arg, void* data);
        static int boxCreateCallback( 
                        ProviderEventArgPtr arg, 
                        int* width, int* height, 
                        double* priority, void* data);
        static int boxReCreateCallback(ProviderEventArgPtr arg, void* data);
        static int boxDestroyCallback(ProviderEventArgPtr arg, void* data);
        static int pdCreateCallback(ProviderEventArgPtr arg, void* data);
        static int pdDestroyCallback(ProviderEventArgPtr arg, void* data);
        static int clickedCallback(ProviderEventArgPtr arg, void* data);
        static int boxResizeCallback(ProviderEventArgPtr arg, void* data);
        static int boxPauseCallback(ProviderEventArgPtr arg, void* data);
        static int boxResumeCallback(ProviderEventArgPtr arg, void* data);
        static int pauseCallback(ProviderEventArgPtr arg, void* data);
        static int resumeCallback(ProviderEventArgPtr arg, void* data);
        static int boxPeriodChangeCallback(ProviderEventArgPtr arg, void* data);
        static int boxUpdateCallback(ProviderEventArgPtr arg, void* data);
        // callback for app-core event
        static int requestUpdateLangCallback(void* data, void* tmp);
        static int requestLowMemoryCallback(void* data, void* tmp);
        // callback for vconf event
        static void requestUpdateAllCallback(keynode_t* key, void* data);
        static void requestUpdateAppBoxCallback(keynode_t* key, void* data);

        // common private functions
        void setProviderCallbacks(ProviderCallbacks& callbacks);
        std::string getBoxType(const char* boxId);
        BoxInfoPtr initializeBoxInfo(ProviderEventArgPtr arg);
        int requestUpdateAll(void);

        // functions for handling appcontrol per operation
        std::string getBoxIdFromAppControl(app_control_h app_control);
        bool isAppControlCallerBoxOwner(app_control_h app_control);
        BoxInfoPtr handleOperationUpdate(app_control_h app_control);
        std::vector<BoxInfoPtr> handleOperationRemove(app_control_h app_control);

        // callback for ping to master daemon
        static Eina_Bool pingToMasterCallback(void* data);

        // callback for requested jobs of boxes
        static void requestBoxJobCallback(void* data);

        // members
        std::string m_daemonName;
        Ecore_Timer* m_pingTimer;
        std::shared_ptr<IBoxPluginConnector> m_pluginConnector;
};

struct JobInfo {
    request_cmd_type cmdType;
    BoxInfoPtr boxInfo;
    BoxDaemonImpl* daemonImpl;

    JobInfo(request_cmd_type cmdType,
            BoxInfoPtr boxInfo,
            BoxDaemonImpl* daemonImpl) :
        cmdType(cmdType),
        boxInfo(boxInfo),
        daemonImpl(daemonImpl)
    {
    };
};


#endif //BOX_DAEMON_IMPL_H

