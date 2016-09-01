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
#include <glib.h>
#include <glib-object.h>

#include "ic.h"
#include "ic-utils.h"
#include "ic-ioty.h"

#define ICL_TIMEOUT_DEFAULT 30 /* 30 sec */
#define ICL_TIMEOUT_MAX 60*60 /* 60 min */

#define ICL_DEFAULT_DEVICE_NAME "UNKNOWN"

static pthread_t icl_thread;
static int icl_timeout_seconds = ICL_TIMEOUT_DEFAULT;
static int icl_init_count;

int icl_initialize(const char *file_path, bool is_pt)
{
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission((IC_PERMISSION_INTERNET|IC_PERMISSION_NETWORK_GET)),
			IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(NULL == file_path, IOTCON_ERROR_INVALID_PARAMETER);

#if !GLIB_CHECK_VERSION(2, 35, 0)
	g_type_init();
#endif

	ic_utils_mutex_lock(IC_UTILS_MUTEX_INIT);
	if (0 == icl_init_count) {
		ret = icl_ioty_set_persistent_storage(file_path, is_pt);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("icl_set_persistent_storage() Fail(%d)", ret);
			return ret;
		}

		ret = icl_ioty_init(&icl_thread);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("icl_ioty_init() Fail(%d)", ret);
			ic_utils_mutex_unlock(IC_UTILS_MUTEX_INIT);
			return ret;
		}

		ret = icl_ioty_set_platform_info();
		if (IOTCON_ERROR_NONE != ret) {
			ERR("icl_ioty_set_platform_info() Fail(%d)", ret);
			icl_ioty_deinit(icl_thread);
			ic_utils_mutex_unlock(IC_UTILS_MUTEX_INIT);
			return ret;
		}

		ret = icl_ioty_set_device_info(ICL_DEFAULT_DEVICE_NAME);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("icl_ioty_set_device_info() Fail(%d)", ret);
			icl_ioty_deinit(icl_thread);
			ic_utils_mutex_unlock(IC_UTILS_MUTEX_INIT);
			return ret;
		}
	}
	icl_init_count++;
	ic_utils_mutex_unlock(IC_UTILS_MUTEX_INIT);

	return IOTCON_ERROR_NONE;
}

API int iotcon_initialize(const char *file_path)
{
	return icl_initialize(file_path, false);
}

API int iotcon_deinitialize(void)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);

	ic_utils_mutex_lock(IC_UTILS_MUTEX_INIT);

	icl_init_count--;
	if (0 == icl_init_count) {
		icl_ioty_deinit(icl_thread);
		icl_thread = 0;
	}

	ic_utils_mutex_unlock(IC_UTILS_MUTEX_INIT);

	return IOTCON_ERROR_NONE;
}

API int iotcon_get_timeout(int *timeout_seconds)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == timeout_seconds, IOTCON_ERROR_INVALID_PARAMETER);

	*timeout_seconds = icl_timeout_seconds;

	return IOTCON_ERROR_NONE;
}


API int iotcon_set_timeout(int timeout_seconds)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	if (ICL_TIMEOUT_MAX < timeout_seconds || timeout_seconds <= 0) {
		ERR("timeout_seconds(%d) must be in range from 1 to 3600", timeout_seconds);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	icl_timeout_seconds = timeout_seconds;

	return IOTCON_ERROR_NONE;
}


