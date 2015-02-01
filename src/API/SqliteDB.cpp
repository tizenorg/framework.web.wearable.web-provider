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
 * @file    SqliteDB.cpp
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#include <string>
#include <cstring>
#include <cstdarg>
#include <sqlite3.h>
#include <db-util.h>

#include "SqliteDB.h"

SqliteDB::SqliteDB(std::string dbPath)
    : m_path(dbPath)
    , m_handle(NULL)
    , m_stmt(NULL)
{
}

SqliteDB::~SqliteDB()
{
}

bool SqliteDB::openDB()
{
    closeDB();
    int ret;
    ret = db_util_open(m_path.c_str(), &m_handle, DB_UTIL_REGISTER_HOOK_METHOD);
    if (ret != SQLITE_OK) {
        return false;
    }

    return true;
}

void SqliteDB::closeDB()
{
    if (!m_handle) {
        return;
    }

    if (!m_stmt) {
        db_util_close(m_handle);
        m_handle = NULL;
        return;
    }

    sqlite3_finalize(m_stmt);
    db_util_close(m_handle);
    m_stmt = NULL;
    m_handle = NULL;
}

bool SqliteDB::setCommand(const std::string& query, const char* fmt, ...)
{
    if (!m_handle || !fmt) {
        return false;
    }

    int ret =
        sqlite3_prepare_v2(m_handle, query.c_str(), -1, &m_stmt, NULL);

    if (ret != SQLITE_OK) {
        return false;
    }

    va_list ap;
    va_start(ap, fmt);
    for (unsigned int i = 0; i < strlen(fmt); i++) {
        switch (fmt[i]) {
        case 'i':
        {
            int intValue = va_arg(ap, int);
            ret = sqlite3_bind_int(m_stmt, i + 1, intValue);
            if (ret != SQLITE_OK) {
                va_end(ap);
                return false;
            }
            break;
        }
        case 's':
        {
            char* stringValue = va_arg(ap, char*);
            ret = sqlite3_bind_text(m_stmt, i + 1, stringValue, -1, NULL);
            if (ret != SQLITE_OK) {
                va_end(ap);
                return false;
            }
            break;
        }
        default:
            break;
        }
    }
    va_end(ap);

    return true;
}

bool SqliteDB::executeCommand()
{
    int ret = sqlite3_step(m_stmt);

    if (ret != SQLITE_ROW) {
        return false;
    }

    return true;
}

const char* SqliteDB::getText(int col)
{
    const char* ret =
        reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, col));

    return ret;
}

int SqliteDB::getInt(int col)
{
    return sqlite3_column_int(m_stmt, col);
}
