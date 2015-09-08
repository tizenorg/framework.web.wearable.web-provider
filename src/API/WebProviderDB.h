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
 * @file    WebProviderDB.h 
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef WIDGET_DB_H
#define WIDGET_DB_H

#include <string>
#include "SqliteDB.h"

const std::string dbPath("/opt/usr/dbspace/.web_provider.db");

class WebProviderDB : public SqliteDB {
    public:
        WebProviderDB() : SqliteDB (dbPath) {};
        ~WebProviderDB() {};
};

#endif //WIDGET_DB_H
