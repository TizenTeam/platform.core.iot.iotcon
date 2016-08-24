/*
 * Copyright (c) 2015 - 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __IOTCON_INTERNAL_COMMON_H__
#define __IOTCON_INTERNAL_COMMON_H__

#include <stdint.h>
#include "iotcon-errors.h"

#define IC_INT64_TO_POINTER(i) ((void*)(intptr_t)(i))
#define IC_POINTER_TO_INT64(p) ((int64_t)(intptr_t)(p))

#define IC_REMOTE_RESOURCE_DEFAULT_TIME_INTERVAL 10 /* 10 sec */
#define IC_PRESENCE_TTL_SECONDS_DEFAULT 60 /* 60 sec */
#define IC_PRESENCE_TTL_SECONDS_MAX (60 * 60 * 24) /* 60 sec/min * 60 min/hr * 24 hr/day */

#define IC_OBSERVE_ID_MAX_LEN 256

#define IC_COAP "coap://"
#define IC_COAPS "coaps://"
#define IC_COAP_PREFIX "coap"
#define IC_MULTICAST_ADDRESS "224.0.1.187:5683"

#endif /* __IOTCON_INTERNAL_COMMON_H__ */
