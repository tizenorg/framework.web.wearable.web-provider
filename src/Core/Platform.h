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
 * @file    Platform.h
 * @author  Minhyung Ko (minhyung.ko@samsung.com)
 */

#ifndef WEB_PROVIDER_PLATFORM_H
#define WEB_PROVIDER_PLATFORM_H

// Use Features definition
//   Use a particular optional platform service or third-party library
//
// Description : <text>
//               <text>
// Author : <text>(<email>) - <date>
// #define USE_<DEPENDENT_MODULE_NAME>_<FEATURE_NAME> <value>(0 or 1)
#define USE(FEATURE) (defined WEB_PROVIDER_USE_##FEATURE && WEB_PROVIDER_USE_##FEATURE)


// Enable Features definition
//   Turn on a specific feature of WRT
//
// Description : <text>
//               <text>
// Author : <text>(<email>) - <date>
// #define ENABLE_<FEATURE_NAME> <value>(0 or 1)
#define ENABLE(FEATURE) (defined WEB_PROVIDER_ENABLE_##FEATURE && WEB_PROVIDER_ENABLE_##FEATURE)

#define WEB_PROVIDER_ENABLE_BOX_LOAD_BALANCER 0
#define WEB_PROVIDER_ENABLE_GL_RENDERING 1
#define WEB_PROVIDER_ENABLE_SHOW_PRE_ICON 0
#define WEB_PROVIDER_ENABLE_WEBKIT_UPVERSION 1

#endif // WEB_PROVIDER_PLATFORM_H
