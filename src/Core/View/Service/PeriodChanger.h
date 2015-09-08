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
 * @file    PeriodChanger.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef PERIOD_CHNAGER_H
#define PERIOD_CHNAGER_H

#include <string>
#include <memory>
#include <Evas.h>

namespace Service {
class PeriodChanger;
typedef std::shared_ptr<PeriodChanger> PeriodChangerPtr;

class PeriodChanger {
    public:
        static PeriodChangerPtr create(
                std::string& boxId, std::string& instanceId,
                double currentPeriod, double requestedPeriod)
        {
            return PeriodChangerPtr(
                    new PeriodChanger(boxId, instanceId, currentPeriod, requestedPeriod));
        }
        bool change();
        static bool isPopupOpened() { return s_isPopupOpened; };
        ~PeriodChanger();

    private:
        void showPeriodPopup();
        void destroyPeriodPopup(Evas_Object *obj);
        void setPopupListData();
        bool requestToPlatform(double newPeriod);
        static Evas_Object* createWindow();
        static void destroyWindow();

        static void selectPeriodCallback(void *data, Evas_Object *obj, void *event_info);
        static void cancelButtonCallback(void *data, Evas_Object *obj, void *event_info);
        static void pressHardwareBackKeyCallback(void *data, Evas_Object *obj, void *event_info);
        static Eina_Bool popupDestroyIdlerCallback(void *data);

        PeriodChanger(
                std::string& boxId, std::string& instanceId,
                double currentPeriod, double requestedPeriod);

        static Evas_Object* s_window;
        std::string m_boxId;
        std::string m_instanceId;
        float m_currentPeriod;
        float m_requestedPeriod;
        static bool s_isPopupOpened;

        struct PopupListData {
            PeriodChanger* periodChanger;
            double newPeriod;
            const char* period;
            Evas_Object* radio;
        };

        PopupListData m_hour[5];
};
} // Service

#endif // PERIOD_CHNAGER_H
