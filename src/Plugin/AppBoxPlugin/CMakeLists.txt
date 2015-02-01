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

SET(TARGET_NAME web-provider-plugin-app)
SET(DEPS ${TARGET_NAME}_DEPS)

PKG_CHECK_MODULES(${DEPS}
    ail
    ewebkit2
    wrt-core
    dpl-efl
    evas
    ecore
    eina
    dynamicbox_service
    dlog
    provider # this should be removed
    REQUIRED
)
ADD_DEFINITIONS(${${DEPS}_CFLAGS})

SET(SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/box_plugin_interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AppBoxManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AppBoxPluginFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AppBoxRenderView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AppBoxPdHelper.cpp
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
    ${TARGET_CORE}
)

INSTALL(TARGETS ${TARGET_NAME}
    DESTINATION lib/${PROJECT_NAME}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL_FILE(app.json lib/${PROJECT_NAME}) 
