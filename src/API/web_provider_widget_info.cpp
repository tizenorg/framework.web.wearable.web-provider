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
 * @file    web_provider_widget_info.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include <cstring>
#include <memory>
#include <vector>
#include "WebProviderDB.h"
#include "web_provider_widget_info.h"

namespace {
static const std::string kInfoTable("LiveboxInfo");
}

enum InfoTableField {
    BOX_ID = 0,
    APP_ID,
    BOX_TYPE,
    AUTO_LAUNCH,
    MOUSE_EVENT,
    GBAR_FAST_OPEN,
};

// TODO this default type should be retrieved more automatically
namespace{
static const std::string kDefaultBoxType("app");
}

const char* web_provider_widget_get_default_type()
{
    return kDefaultBoxType.c_str();
}

char* web_provider_widget_get_box_type(const char* box_id)
{
    if (!box_id) {
        return NULL;
    }

    std::shared_ptr<WebProviderDB> handle(new WebProviderDB());
    if (!handle->openDB()) {
        return NULL;
    }

    std::string query = "select * from " + kInfoTable + " where box_id = ?";
    if (!handle->setCommand(query, "s", box_id)) {
        handle->closeDB();
        return NULL;
    }

    if (!handle->executeCommand()) {
        handle->closeDB();
        return NULL;
    }

    const char* box_type = handle->getText(InfoTableField::BOX_TYPE);
    char* box_type_dup = NULL;

    if (box_type) {
        box_type_dup = strdup(box_type);
    }

    handle->closeDB();

    return box_type_dup;
}

char* web_provider_widget_get_app_id(const char* box_id)
{
    if (!box_id) {
        return NULL;
    }

    std::shared_ptr<WebProviderDB> handle(new WebProviderDB());
    if (!handle->openDB()) {
        return NULL;
    }

    std::string query = "select * from " + kInfoTable + " where box_id = ?";
    if (!handle->setCommand(query, "s", box_id)) {
        handle->closeDB();
        return NULL;
    }

    if (!handle->executeCommand()) {
        handle->closeDB();
        return NULL;
    }

    const char* app_id = handle->getText(InfoTableField::APP_ID);
    char* app_id_dup = NULL;

    if (app_id) {
        app_id_dup = strdup(app_id);
    }

    handle->closeDB();

    return app_id_dup;
}

int web_provider_widget_get_auto_launch(const char* box_id)
{
    if (!box_id) {
        return 0;
    }

    std::shared_ptr<WebProviderDB> handle(new WebProviderDB());
    if (!handle->openDB()) {
        return 0;
    }

    std::string query = "select * from " + kInfoTable + " where box_id = ?";
    if (!handle->setCommand(query, "s", box_id)) {
        handle->closeDB();
        return 0;
    }

    if (!handle->executeCommand()) {
        handle->closeDB();
        return 0;
    }

    int autoLaunch  = handle->getInt(InfoTableField::AUTO_LAUNCH);
    handle->closeDB();

    return autoLaunch;
}

int web_provider_widget_get_mouse_event(const char* box_id)
{
    if (!box_id) {
        return 0;
    }

    std::shared_ptr<WebProviderDB> handle(new WebProviderDB());
    if (!handle->openDB()) {
        return 0;
    }

    std::string query = "select * from " + kInfoTable + " where box_id = ?";
    if (!handle->setCommand(query, "s", box_id)) {
        handle->closeDB();
        return 0;
    }

    if (!handle->executeCommand()) {
        handle->closeDB();
        return 0;
    }

    int mouseEvent = handle->getInt(InfoTableField::MOUSE_EVENT);
    handle->closeDB();

    return mouseEvent;
}

bool web_provider_widget_get_gbar_fast_open(const char* box_id)
{
    if (!box_id) {
        return 0;
    }

    std::shared_ptr<WebProviderDB> handle(new WebProviderDB());
    if (!handle->openDB()) {
        return 0;
    }

    std::string query = "select * from " + kInfoTable + " where box_id = ?";
    if (!handle->setCommand(query, "s", box_id)) {
        handle->closeDB();
        return 0;
    }

    if (!handle->executeCommand()) {
        handle->closeDB();
        return 0;
    }

    int gbarFastOpen = handle->getInt(InfoTableField::GBAR_FAST_OPEN);
    handle->closeDB();

    return gbarFastOpen;
}

int web_provider_widget_insert_box_info(
        const char* box_id,
        const char* app_id,
        const char* box_type,
        int auto_launch,
        int mouse_event,
        int pd_fast_open)
{
    if (!box_id || !app_id || !box_type) {
        return -1;
    }

    std::shared_ptr<WebProviderDB> handle(new WebProviderDB());
    if (!handle->openDB()) {
        return -1;
    }

    std::string query =
        "insert into " + kInfoTable +
        " (box_id, app_id, box_type, auto_launch, mouse_event, pd_fast_open) \
         values (?,?,?,?,?,?)";

    if (!handle->setCommand(
                query, "sssiii",
                box_id, app_id, box_type, auto_launch, mouse_event, pd_fast_open)) {
        handle->closeDB();
        return -1;
    }

    if (!handle->executeCommand()) {
        handle->closeDB();
        return -1;
    }

    handle->closeDB();
    return 0;
}

int web_provider_widget_delete_by_box_id(const char* box_id)
{
    if (!box_id) {
        return -1;
    }

    std::shared_ptr<WebProviderDB> handle(new WebProviderDB());
    if (!handle->openDB()) {
        return -1;
    }

    std::string query =
        "delete from " + kInfoTable + " where box_id=?";

    if (!handle->setCommand(query, "s", box_id)) {
        handle->closeDB();
        return -1;
    }

    if (!handle->executeCommand()) {
        handle->closeDB();
        return -1;
    }

    handle->closeDB();
    return 0;
}

int web_provider_widget_delete_by_app_id(const char* app_id)
{
    if (!app_id) {
        return -1;
    }

    std::shared_ptr<WebProviderDB> handle(new WebProviderDB());
    if (!handle->openDB()) {
        return -1;
    }

    std::string query =
        "delete from " + kInfoTable + " where app_id=?";

    if (!handle->setCommand(query, "s", app_id)) {
        handle->closeDB();
        return -1;
    }

    if (!handle->executeCommand()) {
        handle->closeDB();
        return -1;
    }

    handle->closeDB();
    return 0;
}

int web_provider_widget_delete_by_type(const char* type)
{
    if (!type) {
        return -1;
    }

    std::shared_ptr<WebProviderDB> handle(new WebProviderDB());
    if (!handle->openDB()) {
        return -1;
    }

    std::string query =
        "delete from " + kInfoTable + " where type=?";

    if (!handle->setCommand(query, "s", type)) {
        handle->closeDB();
        return -1;
    }

    if (!handle->executeCommand()) {
        handle->closeDB();
        return -1;
    }

    handle->closeDB();
    return 0;
}

char** web_provider_widget_get_box_id_list(const char* app_id, int* count)
{
    if (!app_id) {
        return NULL;
    }

    std::shared_ptr<WebProviderDB> handle(new WebProviderDB());
    if (!handle->openDB()) {
        return NULL;
    }

    std::string query = "select * from " + kInfoTable + " where app_id = ?";
    if (!handle->setCommand(query, "s", app_id)) {
        handle->closeDB();
        return NULL;
    }

    std::vector<char*> boxIdList;
    while (handle->executeCommand()) {
        const char* boxId = handle->getText(InfoTableField::BOX_ID);
        if (boxId != NULL) {
            boxIdList.push_back(strdup(boxId));
        }
    }

    *count = boxIdList.size();
    if (*count == 0) {
        handle->closeDB();
        return NULL;
    }

    char** boxIds = static_cast<char**>(malloc((*count) * sizeof(char*)));
    if (boxIds != NULL) {
        for (int i = 0; i < *count; i++) {
            boxIds[i] = boxIdList[i];
        }
    }

    handle->closeDB();
    return boxIds;
}

void web_provider_widget_release_box_id_list(char** box_id_list, int count)
{
    if (!box_id_list) {
        return;
    }

    for (int i = 0; i < count; i++) {
        if (!box_id_list[i]) {
            continue;
        }
        free(box_id_list[i]);
    }

    free(box_id_list);
}

bool web_provider_widget_check_box_installed(const char* box_id)
{
    if (!box_id) {
        return 0;
    }

    std::shared_ptr<WebProviderDB> handle(new WebProviderDB());
    if (!handle->openDB()) {
        return 0;
    }

    std::string query = "select count(box_id) from " + kInfoTable + " where box_id = ?";
    if (!handle->setCommand(query, "s", box_id)) {
        handle->closeDB();
        return 0;
    }

    if (!handle->executeCommand()) {
        handle->closeDB();
        return 0;
    }

    int count = handle->getInt(0);
    return count;
}
