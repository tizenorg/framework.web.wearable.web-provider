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
# @author   Yunchan Cho (yunchan.cho@samsung.com)

SET(TARGET_NAME ${TARGET_API})
SET(DEPS ${TARGET_NAME}_DEPS)

PKG_CHECK_MODULES(${DEPS}
    db-util
    glib-2.0
    json-glib-1.0
    dlog
    capi-appfw-application
    libsmack
    REQUIRED
)
ADD_DEFINITIONS(${${DEPS}_CFLAGS})

SET(SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/SqliteDB.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/web_provider_livebox_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/web_provider_plugin_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/web_provider_service.cpp
)

SET(HEADERS
    ${${DEPS}_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
INCLUDE_DIRECTORIES(${HEADERS})

ADD_LIBRARY(${TARGET_NAME} SHARED ${SRCS})

SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
    COMPILE_FLAGS -fPIC
    LINK_FLAGS "-Wl,--as-needed -Wl,--hash-style=both"
)

SET_TARGET_PROPERTIES(${TARGET_NAME} 
    PROPERTIES
    SOVERSION ${CMAKE_PROJECT_API_VERSION}
    VERSION ${CMAKE_PROJECT_VERSION}
)

TARGET_LINK_LIBRARIES(${TARGET_NAME} 
    ${${DEPS}_LDFLAGS} "-ldl"
    ${${DEPS}_LIBRARIES}
)

INSTALL(TARGETS ${TARGET_NAME}
    DESTINATION lib
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

GET_FILENAME_COMPONENT(CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
INSTALL_FILE(web-provider-info.h include/${PROJECT_NAME}/${CURRENT_DIR_NAME}) # Deprecated
INSTALL_FILE(web_provider_livebox_info.h include/${PROJECT_NAME}/${CURRENT_DIR_NAME}) 
INSTALL_FILE(web_provider_plugin_info.h include/${PROJECT_NAME}/${CURRENT_DIR_NAME}) 
INSTALL_FILE(web_provider_service.h include/${PROJECT_NAME}/${CURRENT_DIR_NAME})
