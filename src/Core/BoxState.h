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
 * @file    BoxState.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef BOX_STATE
#define BOX_STATE

#include "IBoxState.h"
#include "IBoxContext.h"

/*
#define CHECK_BOX_STATE(currentState, operation)        \
    IBoxStatePtr state;                                 \
    try {                                               \
        state = currentState->operation();              \
    } catch (...) {                                     \
        return false;                                   \
    }                                                   \

#define SWITCH_BOX_STATE()                              \
    state->switchState()                                \
*/

#define CHECK_BOX_STATE(currentState, operation) 
#define SWITCH_BOX_STATE()

class BoxState: public IBoxState {
    public:
        virtual IBoxStatePtr permitShow() { throw this; };
        virtual IBoxStatePtr permitHide() { throw this; };
        virtual IBoxStatePtr permitResume() { throw this; };
        virtual IBoxStatePtr permitPause() { throw this; }; 
        virtual IBoxStatePtr permitOpenPd() { throw this; };
        virtual IBoxStatePtr permitClosePd() { throw this ; };
        virtual IBoxStatePtr permitShutdown() { throw this; };
        virtual void switchState();
        virtual ~BoxState() {};

    protected:
        explicit BoxState(IBoxContextPtr context) : m_context(context) {};
        IBoxContextPtr getContext();

    private:
        void setContext(IBoxContextPtr context);
        IBoxContextPtr m_context;
};

class BoxInitState: public BoxState {
    public:
        static IBoxStatePtr create(IBoxContextPtr context) 
        { 
            return IBoxStatePtr(new BoxInitState(context)); 
        };
        virtual IBoxStatePtr permitShow();
        virtual ~BoxInitState() {};

    private:
        explicit BoxInitState(IBoxContextPtr context) : BoxState(context) {};
};

class BoxShowState: public BoxState {
    public:
        static IBoxStatePtr create(IBoxContextPtr context) 
        { 
            return IBoxStatePtr(new BoxShowState(context)); 
        };
        virtual IBoxStatePtr permitShow();
        virtual IBoxStatePtr permitHide();
        virtual IBoxStatePtr permitOpenPd();
        virtual IBoxStatePtr permitPause();
        virtual ~BoxShowState() {};

    private:
        explicit BoxShowState(IBoxContextPtr context) : BoxState(context) {};
};

class BoxHideState: public BoxState {
    public:
        static IBoxStatePtr create(IBoxContextPtr context) 
        { 
            return IBoxStatePtr(new BoxHideState(context)); 
        };
        virtual IBoxStatePtr permitShutdown();
        virtual ~BoxHideState() {};

    private:
        explicit BoxHideState(IBoxContextPtr context) : BoxState(context) {};
};

class BoxOpenPdState: public BoxState {
    public:
        static IBoxStatePtr create(IBoxContextPtr context) 
        { 
            return IBoxStatePtr(new BoxOpenPdState(context)); 
        };
        virtual IBoxStatePtr permitClosePd();
        virtual ~BoxOpenPdState() {};

    private:
        explicit BoxOpenPdState(IBoxContextPtr context) : BoxState(context) {};
};

class BoxClosePdState: public BoxState {
    public:
        static IBoxStatePtr create(IBoxContextPtr context) 
        { 
            return IBoxStatePtr(new BoxClosePdState(context)); 
        };
        virtual IBoxStatePtr permitShow();
        virtual ~BoxClosePdState() {};

    private:
        explicit BoxClosePdState(IBoxContextPtr context) : BoxState(context) {};
};

class BoxPauseState: public BoxState {
    public:
        static IBoxStatePtr create(IBoxContextPtr context) 
        { 
            return IBoxStatePtr(new BoxPauseState(context)); 
        };
        virtual IBoxStatePtr permitResume();
        virtual IBoxStatePtr permitHide();
        virtual ~BoxPauseState() {};

    private:
        explicit BoxPauseState(IBoxContextPtr context) : BoxState(context) {};
};

class BoxResumeState: public BoxState {
    public:
        static IBoxStatePtr create(IBoxContextPtr context) 
        { 
            return IBoxStatePtr(new BoxResumeState(context)); 
        };
        virtual IBoxStatePtr permitShow();
        virtual IBoxStatePtr permitHide();
        virtual ~BoxResumeState() {};

    private:
        explicit BoxResumeState(IBoxContextPtr context) : BoxState(context) {};
};
#endif // BOX_STATE
