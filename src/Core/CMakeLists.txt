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

SET(TARGET_NAME ${TARGET_CORE})
SET(TARGET_CORE_BUFFER web-provider-core-buffer)
SET(TARGET_CORE_VIEW web-provider-core-view)
SET(TARGET_CORE_UTIL web-provider-core-util)
SET(DEPS ${TARGET_NAME}_DEPS)

PKG_CHECK_MODULES(${DEPS}
    ecore
    ecore-evas
    ewebkit2
    dlog
    vconf
    REQUIRED
)
ADD_DEFINITIONS(${${DEPS}_CFLAGS})

SET(SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/Box.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BoxLoadBalancer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BoxManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BoxState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BoxUpdateTimer.cpp
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
    ${${DEPS}_LIBRARIES}
    "-Wl,--whole-archive"
    ${TARGET_CORE_VIEW}
    ${TARGET_CORE_BUFFER}
    ${TARGET_CORE_UTIL}
    "-Wl,--no-whole-archive"
)

ADD_SUBDIRECTORY(Buffer)
ADD_SUBDIRECTORY(View)
ADD_SUBDIRECTORY(Util)

INSTALL(TARGETS ${TARGET_NAME}
    DESTINATION lib
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

GET_FILENAME_COMPONENT(CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)


INSTALL_FILE(IBox.h include/${PROJECT_NAME}/${CURRENT_DIR_NAME})
INSTALL_FILE(BoxData.h include/${PROJECT_NAME}/${CURRENT_DIR_NAME})
INSTALL_FILE(IBoxManager.h include/${PROJECT_NAME}/${CURRENT_DIR_NAME})
INSTALL_FILE(BoxManager.h include/${PROJECT_NAME}/${CURRENT_DIR_NAME})
INSTALL_FILE(Platform.h include/${PROJECT_NAME}/${CURRENT_DIR_NAME})
INSTALL_FILE(config.h include/${PROJECT_NAME}/${CURRENT_DIR_NAME})
INSTALL_FILE(BoxLoadBalancer.h include/${PROJECT_NAME}/${CURRENT_DIR_NAME})

