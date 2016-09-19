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
#ifndef __IOTCON_INTERNAL_UTILITY_H__
#define __IOTCON_INTERNAL_UTILITY_H__

#include <octypes.h>
#include "iotcon-types.h"

#define IC_EQUAL 0
#define IC_SAFE_STR(str) (str ? str : "")

enum {
	IC_PERMISSION_NETWORK_GET = (1 << 0),
	IC_PERMISSION_INTERNET = (1 << 1),
};

char* ic_utils_strdup(const char *src);
bool ic_utils_check_permission(int permssion);
bool ic_utils_check_ocf_feature();
int ic_utils_get_platform_info(OCPlatformInfo *platform_info);
void ic_utils_free_platform_info(OCPlatformInfo *platform_info);

void ic_utils_mutex_lock(int type);
void ic_utils_mutex_unlock(int type);

int ic_utils_cond_polling_init();
void ic_utils_cond_polling_destroy();

void ic_utils_cond_signal(int type);
void ic_utils_cond_timedwait(int cond_type, int mutex_type, int polling_interval);

int ic_utils_host_address_get_connectivity(const char *host_address, int conn_type);
bool ic_utils_check_connectivity_type(int conn_type);

enum IC_UTILS_MUTEX {
	IC_UTILS_MUTEX_INIT,
	IC_UTILS_MUTEX_IOTY,
	IC_UTILS_MUTEX_POLLING,
	IC_UTILS_MUTEX_MAX
};

enum IC_UTILS_COND {
	IC_UTILS_COND_POLLING,
	IC_UTILS_COND_MAX
};

enum IC_UTILS_CONNECTIVITY {
	IC_UTILS_CONNECTIVITY_IPV4 = (1 << 0),
	IC_UTILS_CONNECTIVITY_IPV6 = (1 << 1),
	IC_UTILS_CONNECTIVITY_UDP = (1 << 2),
	IC_UTILS_CONNECTIVITY_TCP = (1 << 3)
};

#endif /* __IOTCON_INTERNAL_UTILITY_H__ */
