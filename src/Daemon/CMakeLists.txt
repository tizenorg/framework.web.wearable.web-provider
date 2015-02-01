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

SET(TARGET_NAME ${TARGET_DAEMON})
SET(DEPS ${TARGET_NAME}_DEPS)

PKG_CHECK_MODULES(${DEPS}
    aul
    bundle
    appcore-efl
    elementary
    ecore-x
    provider
    dynamicbox_service
    capi-appfw-application
    dlog
    REQUIRED
)
ADD_DEFINITIONS(${${DEPS}_CFLAGS})

SET(SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BoxDaemon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BoxDaemonImpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BoxDaemonUtil.cpp
)

SET(HEADERS
    ${${DEPS}_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
INCLUDE_DIRECTORIES(${HEADERS})

ADD_EXECUTABLE(${TARGET_NAME} ${SRCS})

SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
    COMPILE_FLAGS -fPIC
    LINK_FLAGS "-Wl,--as-needed -Wl,--hash-style=both"
)

TARGET_LINK_LIBRARIES(${TARGET_NAME} 
    ${${DEPS}_LDFLAGS} "-ldl -lcap -lrt"
    ${${DEPS}_LIBRARIES}
    ${TARGET_PLUGIN}
    ${TARGET_CORE}
    ${TARGET_API}
)

INSTALL(TARGETS ${TARGET_NAME}
    DESTINATION /usr/apps/livebox.${PROJECT_NAME}/bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
