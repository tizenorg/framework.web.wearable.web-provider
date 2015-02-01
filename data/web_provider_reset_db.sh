#!/bin/sh
# Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
#
#    Licensed under the Flora License, Version 1.1 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://floralicense.org/license/
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#

web_provider_db="/opt/usr/dbspace/.web_provider.db"
web_provider_sql="/usr/share/web-provider/web_provider_db.sql"

if [ -f $web_provider_db ]; then
    /bin/echo "DB already exists."
else
    /bin/echo "create web livebox DB..."
    /bin/rm -f $web_provider_db
    /bin/rm -f $web_provider_db-journal
    sql="PRAGMA journal_mode = PERSIST;"
    /usr/bin/sqlite3 $web_provider_db "$sql"
    sql=".read "$web_provider_sql
    /usr/bin/sqlite3 $web_provider_db "$sql"
    /bin/touch $web_provider_db-journal
    /bin/chown 0:6026 $web_provider_db
    /bin/chown 0:6026 $web_provider_db-journal
    /bin/chmod 660 $web_provider_db
    /bin/chmod 660 $web_provider_db-journal
    /bin/echo "finish creation of db."
fi
