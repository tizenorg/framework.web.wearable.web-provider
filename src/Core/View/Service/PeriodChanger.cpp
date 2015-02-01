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
 * @file    PeriodChanger.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */

#include <string>
#include <Evas.h>
#include <Ecore_X.h>
#include <Ecore.h>
#include <Elementary.h>
#include <efl_assist.h>
#include <dynamicbox_service.h>
#include <Core/Util/Log.h>
#include <Core/Util/Util.h>
#include "PeriodChanger.h"

#define UPDATE_PERIOD_MIN   1800.0f
#define UPDATE_PERIOD_HOUR  60.0 * 60.0
#define U_(str) dgettext("web-provider", str)

namespace Service {

Evas_Object* PeriodChanger::s_window = NULL;
bool PeriodChanger::s_isPopupOpened = false;

static const char * const TEXT_POPUP_TITLE = "IDS_BR_HEADER_AUTO_REFRESH";
static const char * const TEXT_POPUP_CANCEL_BUTTON = "IDS_ST_BUTTON_CANCEL";
static const char * const TEXT_POPUP_UPDATE_PERIOD_1 ="IDS_ST_BODY_1_HOUR";
static const char * const TEXT_POPUP_UPDATE_PERIOD_3 ="IDS_ST_BODY_3HOURS";
static const char * const TEXT_POPUP_UPDATE_PERIOD_6 ="IDS_ST_BODY_6_HOURS_TMO";
static const char * const TEXT_POPUP_UPDATE_PERIOD_12 ="IDS_ST_BODY_12_HOURS";
static const char * const TEXT_POPUP_UPDATE_PERIOD_NEVER ="IDS_BR_OPT_NEVER";
static const char * const MO_INSTALL_DIR = "/usr/share/locale";
static const char * const MO_PROJECT_NAME = "web-provider";

PeriodChanger::PeriodChanger(
        std::string& boxId, std::string& instanceId,
        double currentPeriod, double requestedPeriod)
    : m_boxId(boxId)
    , m_instanceId(instanceId)
    , m_currentPeriod(currentPeriod)
    , m_requestedPeriod(requestedPeriod)
    , m_hour()
{
    LogD("enter");
}

PeriodChanger::~PeriodChanger()
{
    LogD("enter");
}

bool PeriodChanger::change()
{
    LogD("enter");

    if (m_requestedPeriod < 0) {
        showPeriodPopup();
        return true;
    }

    double newPeriod;
    if (m_requestedPeriod == 0) {
        newPeriod = 0.0;
    } else if (m_requestedPeriod > 0) {
        if (m_requestedPeriod >= UPDATE_PERIOD_MIN) {
            newPeriod = m_requestedPeriod;
        } else {
            newPeriod = UPDATE_PERIOD_MIN;
        }
    } else {
        LogD("negative value can't be handled here");
        newPeriod = 0.0;
    }

    // after selecting one among period list, the following should be executed
    return requestToPlatform(newPeriod);
}

void PeriodChanger::showPeriodPopup()
{
    LogD("enter");

    Evas_Object* window = createWindow();
    Evas_Object* periodList = elm_list_add(window);

    if (!periodList) {
        LogD("failed to add elm_list_add");
    }
    elm_list_mode_set(periodList, ELM_LIST_EXPAND);
    bindtextdomain(MO_PROJECT_NAME, MO_INSTALL_DIR);
    setPopupListData();
    // TODO Language ID should be used, not static string
    for(unsigned int i = 0 ; i < sizeof(m_hour) / sizeof(PopupListData); i++) {
        m_hour[i].radio = elm_radio_add(periodList);
        elm_radio_state_value_set(m_hour[i].radio,
            m_currentPeriod == m_hour[i].newPeriod ? EINA_FALSE : EINA_TRUE);
        elm_list_item_append(periodList,
            m_hour[i].period,
            m_hour[i].radio,
            NULL,
            selectPeriodCallback, &m_hour[i]);
    }

    // create popup
    Evas_Object *popup = elm_popup_add(window);
    if (!popup) {
        LogD("failed to add elm_popup_add");
        return;
    }
    elm_object_style_set(popup, "popup-item list");
    evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_part_text_set(popup, "title,text", U_(TEXT_POPUP_TITLE));
    elm_object_content_set(popup, periodList);
    evas_object_show(popup);

    // register back key callback
    ea_object_event_callback_add(
            popup, EA_CALLBACK_BACK,
            pressHardwareBackKeyCallback, this);
}

void PeriodChanger::destroyPeriodPopup(Evas_Object *obj)
{
    LogD("enter");
    Evas_Object* parent = elm_object_parent_widget_get(obj);
    Evas_Object* popup = elm_popup_add(parent);
    while (parent) {
        const char* type = elm_object_widget_type_get(parent);
        if (type && !strcmp(type, elm_object_widget_type_get(popup))) {
            evas_object_del(parent);
            break;
        }
        parent = elm_object_parent_widget_get(parent);
    }

    // register back key callback
    ea_object_event_callback_del(popup, EA_CALLBACK_BACK, pressHardwareBackKeyCallback);
    evas_object_del(popup);
    destroyWindow();
}

void PeriodChanger::setPopupListData()
{
    LogD("enter");

    m_hour[0].periodChanger = this;
    m_hour[0].newPeriod = 1.0 * UPDATE_PERIOD_HOUR;
    m_hour[0].period = U_(TEXT_POPUP_UPDATE_PERIOD_1);

    m_hour[1].periodChanger = this;
    m_hour[1].newPeriod = 3.0 * UPDATE_PERIOD_HOUR;
    m_hour[1].period = U_(TEXT_POPUP_UPDATE_PERIOD_3);

    m_hour[2].periodChanger = this;
    m_hour[2].newPeriod = 6.0 * UPDATE_PERIOD_HOUR;
    m_hour[2].period = U_(TEXT_POPUP_UPDATE_PERIOD_6);

    m_hour[3].periodChanger = this;
    m_hour[3].newPeriod = 12.0 * UPDATE_PERIOD_HOUR;
    m_hour[3].period = U_(TEXT_POPUP_UPDATE_PERIOD_12);

    m_hour[4].periodChanger = this;
    m_hour[4].newPeriod = 0.0;
    m_hour[4].period = U_(TEXT_POPUP_UPDATE_PERIOD_NEVER);

}

bool PeriodChanger::requestToPlatform(double newPeriod)
{
    int ret = dynamicbox_service_change_period(
                m_boxId.c_str(), m_instanceId.c_str(), newPeriod);

    if (ret < 0) {
        LogD("during update period, error occurs");
        return false;
    }

    LogD("Instance's period is set to %f", newPeriod);
    return true;
}


Evas_Object* PeriodChanger::createWindow()
{
    LogD("enter");

    if (s_window) {
        evas_object_show(s_window);
        elm_win_raise(s_window);
        return s_window;
    }

    s_window = elm_win_add(NULL, "web-provider-popup", ELM_WIN_BASIC);
    elm_win_alpha_set(s_window, EINA_TRUE);
    elm_win_title_set(s_window, "change update period");
    elm_win_borderless_set(s_window, EINA_TRUE);

    int width = 0;
    int height = 0;
    ecore_x_window_size_get(ecore_x_window_root_first_get(), &width, &height);
    evas_object_resize(s_window, width, height);
    elm_win_indicator_mode_set(s_window, ELM_WIN_INDICATOR_SHOW);

    evas_object_color_set(s_window, 255, 255, 255, 0);
    evas_object_show(s_window);
    s_isPopupOpened = true;
    return s_window;
}

void PeriodChanger::destroyWindow()
{
    LogD("enter");

    if (!s_window) {
        return;
    }
    evas_object_del(s_window);
    s_window = NULL;
    s_isPopupOpened = false;
}

void PeriodChanger::selectPeriodCallback(void *data, Evas_Object *obj, void *event_info)
{
    LogD("enter");
    UNUSED_PARAM(obj);
    UNUSED_PARAM(event_info);

    PopupListData* popupData = static_cast<PopupListData*>(data);

    LogD("Update period is set to %f", popupData->newPeriod);
    popupData->periodChanger->requestToPlatform(popupData->newPeriod);
    ecore_idler_add(popupDestroyIdlerCallback, popupData);
}

void PeriodChanger::cancelButtonCallback(void *data, Evas_Object *obj, void *event_info)
{
    LogD("enter");
    UNUSED_PARAM(event_info);

    PeriodChanger* This = static_cast<PeriodChanger*>(data);
    This->destroyPeriodPopup(obj);
}

void PeriodChanger::pressHardwareBackKeyCallback(void *data, Evas_Object *obj, void *event_info)
{
    LogD("enter");
    UNUSED_PARAM(event_info);

    PeriodChanger* This = static_cast<PeriodChanger*>(data);
    This->destroyPeriodPopup(obj);
}

Eina_Bool PeriodChanger::popupDestroyIdlerCallback(void *data)
{
    LogD("enter");
    PopupListData* popupData = static_cast<PopupListData*>(data);
    popupData->periodChanger->destroyPeriodPopup(popupData->radio);
    return ECORE_CALLBACK_CANCEL;
}
} // Service
