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
 * @file    Log.h
 * @author  Yunchan Cho (yunchan.cho@samsung.com)
 */
#ifndef LOG_H
#define LOG_H

#include <dlog.h>
#include <string.h>

#define FILE_NAME ((strrchr(__FILE__, '/') ? : __FILE__- 1) + 1) 

//#define LogW(fmt, arg...)     LOGW( "[[32m%s:%d[0m:%s] " fmt "\n", __FILE__, __LINE__, __func__, ##arg)
//#define LogD(fmt, arg...)     LOGD( "[[32m%s:%d[0m:%s] " fmt "\n", __FILE__, __LINE__, __func__, ##arg)
//#define LogE(fmt, arg...)     LOGE( "[[32m%s:%d[0m:%s] " fmt "\n", __FILE__, __LINE__, __func__, ##arg)

#define LogW(fmt, arg...)     LOGW( fmt "\n", ##arg)
#define LogD(fmt, arg...)     LOGD( fmt "\n", ##arg)
#define LogE(fmt, arg...)     LOGE( fmt "\n", ##arg)
#endif // LOG_H
