# Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
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

SET(TARGET_API web-provider-api)
SET(TARGET_CORE web-provider-core)
SET(TARGET_DAEMON web-provider)
SET(TARGET_PLUGIN web-provider-plugin)

SET(WEB_PROVIDER_STATIC_REQUIRES
    dpl-efl
)

PKG_CHECK_MODULES(WEB_PROVIDER_STATIC_DEP
    ${WEB_PROVIDER_STATIC_REQUIRES}
    REQUIRED
)

INCLUDE_DIRECTORIES(
    ${WEB_PROVIDER_STATIC_DEP_INCLUDE_DIRS}
)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/API
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Plugin
)

ADD_SUBDIRECTORY(API)
ADD_SUBDIRECTORY(Core)
ADD_SUBDIRECTORY(Daemon)
ADD_SUBDIRECTORY(Plugin)
