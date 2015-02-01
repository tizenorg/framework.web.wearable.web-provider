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
 * @file    SqliteDB.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef SQLITE_DB_H
#define SQLITE_DB_H

#include <string>
#include <sqlite3.h>

class SqliteDB {
    public:
        bool openDB();
        void closeDB();
        bool setCommand(const std::string& query, const char* fmt, ...);
        bool executeCommand();
        const char* getText(int col);
        int getInt(int col);

        explicit SqliteDB(const std::string dbPath);
        ~SqliteDB();

    private:
        std::string m_path;
        sqlite3* m_handle;
        sqlite3_stmt* m_stmt;
};

#endif // SQLITE_DB_H
