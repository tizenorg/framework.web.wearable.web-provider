# Copyright (c) 2012-13 Samsung Electronics Co., Ltd All Rights Reserved
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

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(web-provider CXX)

STRING(REGEX MATCH "([^.]*)" CMAKE_PROJECT_API_VERSION "${CMAKE_PROJECT_VERSION}")
INCLUDE(FindPkgConfig)

# Build type
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF(NOT CMAKE_BUILD_TYPE)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
INCLUDE(OptionsTizen)

# Compiler flags
SET(CMAKE_C_FLAGS_PROFILING    "-O0 -g -pg")
SET(CMAKE_CXX_FLAGS_PROFILING  "-O0 -std=c++0x -g -pg")
SET(CMAKE_C_FLAGS_DEBUG        "-O0 -g")
SET(CMAKE_CXX_FLAGS_DEBUG      "-O0 -std=c++0x -g")
SET(CMAKE_C_FLAGS_RELEASE      "-O2 -g")
SET(CMAKE_CXX_FLAGS_RELEASE    "-O2 -std=c++0x -g -fvisibility-inlines-hidden")

SET(GC_SECTIONS_FLAGS "-fdata-sections -ffunction-sections -Wl,--gc-sections")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GC_SECTIONS_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GC_SECTIONS_FLAGS}")

# Set compiler options
ADD_DEFINITIONS("-Wall")
ADD_DEFINITIONS("-Wextra")
ADD_DEFINITIONS("-fPIC")
ADD_DEFINITIONS("-Wno-deprecated")
ADD_DEFINITIONS("-fvisibility=hidden")
ADD_DEFINITIONS("-shared")

# Set Tag Name
ADD_DEFINITIONS("-DLOG_TAG=\"${PROJECT_NAME}\"")

SET(PKGCONFIG_DIR pkgconfig)
SET(DATA_DIR data)

# Macro for easy usage
MACRO(INSTALL_FILE FILE DEST_DIR)
    INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}
        DESTINATION ${DEST_DIR})
ENDMACRO()
MACRO(INSTALL_FILE_RENAMED FILE DEST_DIR NEW_NAME)
    INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}
        DESTINATION ${DEST_DIR} RENAME ${NEW_NAME})
ENDMACRO()
MACRO(CONFIG_FILE SRC_FILE DEST_FILE)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${SRC_FILE}
        ${CMAKE_CURRENT_SOURCE_DIR}/${DEST_FILE} @ONLY)
ENDMACRO()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(Po)

CONFIG_FILE(${PKGCONFIG_DIR}/web-provider-svc.pc.in ${PKGCONFIG_DIR}/web-provider-svc.pc) # DEPRECATED
INSTALL_FILE(${PKGCONFIG_DIR}/web-provider-svc.pc lib/pkgconfig) # DEPRECATED

CONFIG_FILE(${PKGCONFIG_DIR}/web-provider.pc.in ${PKGCONFIG_DIR}/web-provider.pc)
INSTALL_FILE(${PKGCONFIG_DIR}/web-provider.pc lib/pkgconfig)
INSTALL_FILE(livebox.web-provider.xml /usr/share/packages)
INSTALL_FILE(${DATA_DIR}/web_provider_reset_db.sh bin)
INSTALL_FILE(${DATA_DIR}/web_provider_db.sql /usr/share/${PROJECT_NAME})
INSTALL_FILE(${DATA_DIR}/200.livebox.web-provider.patch.sh /etc/opt/upgrade)
INSTALL_FILE(livebox.web-provider.efl /etc/smack/accesses.d)
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "web-provider-svc.pc")
