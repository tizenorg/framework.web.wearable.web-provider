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

SET(TARGET_NAME ${TARGET_PLUGIN})
SET(DEPS ${TARGET_NAME}_DEPS)

PKG_CHECK_MODULES(${DEPS}
    evas
    ewebkit2
    dlog
    REQUIRED
)
ADD_DEFINITIONS(${${DEPS}_CFLAGS})

SET(SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/BoxPluginConnector.cpp
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
    ${${DEPS}_LDFLAGS} "-ldl"
    ${${DEPS}_LIBRARIES}
)

INSTALL(TARGETS ${TARGET_NAME}
    DESTINATION lib
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

GET_FILENAME_COMPONENT(CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
INSTALL_FILE(IBoxPluginFactory.h include/${PROJECT_NAME}/${CURRENT_DIR_NAME}) 
INSTALL_FILE(box_plugin_interface.h include/${PROJECT_NAME}/${CURRENT_DIR_NAME}) 

# openable plugins of web livebox
ADD_SUBDIRECTORY(AppBoxPlugin)
