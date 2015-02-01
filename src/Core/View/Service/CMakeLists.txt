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

SET(TARGET_NAME ${TARGET_CORE_SERVICE})
SET(DEPS ${TARGET_NAME}_DEPS)

PKG_CHECK_MODULES(${DEPS}
    dlog
    capi-appfw-application
    dynamicbox_service
    evas
    eina
    ecore-x
    elementary
    efl-assist
    provider
    ecore
    REQUIRED
)
ADD_DEFINITIONS(${${DEPS}_CFLAGS})

SET(SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/AppControl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PeriodChanger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ScrollHolder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MessageManager.cpp
)

SET(HEADERS
    ${${DEPS}_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
INCLUDE_DIRECTORIES(${HEADERS})

ADD_LIBRARY(${TARGET_NAME} STATIC ${SRCS})

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
    ${${DEPS}_LIBRARIES}
)

GET_FILENAME_COMPONENT(CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
GET_FILENAME_COMPONENT(PARENT_DIR_ABSOLUTE_PATH ${CMAKE_CURRENT_SOURCE_DIR} PATH)
GET_FILENAME_COMPONENT(PARENT_DIR_NAME ${PARENT_DIR_ABSOLUTE_PATH} NAME)

INSTALL_FILE(PeriodChanger.h include/${PROJECT_NAME}/${PARENT_DIR_NAME}/${CURRENT_DIR_NAME})
INSTALL_FILE(MessageManager.h include/${PROJECT_NAME}/${PARENT_DIR_NAME}/${CURRENT_DIR_NAME})
INSTALL_FILE(ScrollHolder.h include/${PROJECT_NAME}/${PARENT_DIR_NAME}/${CURRENT_DIR_NAME})
INSTALL_FILE(AppControl.h include/${PROJECT_NAME}/${PARENT_DIR_NAME}/${CURRENT_DIR_NAME})
