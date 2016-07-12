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
#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <glib.h>
#include <system_info.h>
#include <system_settings.h>
#include <cynara-client.h>
#include <cynara-error.h>

#include "iotcon-types.h"
#include "ic-common.h"
#include "ic-log.h"
#include "ic-utils.h"

#ifdef TZ_VER_3
static int _ic_ocf_feature = -1;
static const char *IC_FEATURE_OCF = "http://tizen.org/feature/iot.ocf";
#endif

// TODO: Can't access in user side daemon
#if 0
#ifdef TZ_VER_3
static const char *IC_PRIV_FILE_NETWORK_GET = "/usr/share/iotcon/iotcon-network-get";
static const char *IC_PRIV_FILE_INTERNET = "/usr/share/iotcon/iotcon-internet";
#endif
#endif

// TODO: Temporary code (need guide from security team)
#define SMACK_LABEL_LEN 255
static const char *IC_PRIVILEGE_INTERNET = "http://tizen.org/privilege/internet";
static const char *IC_PRIVILEGE_NETWORK_GET = "http://tizen.org/privilege/network.get";

static const char *IC_SYSTEM_INFO_PLATFORM_VERSION = "http://tizen.org/feature/platform.version";
static const char *IC_SYSTEM_INFO_MANUF_NAME = "http://tizen.org/system/manufacturer";
static const char *IC_SYSTEM_INFO_MODEL_NAME = "http://tizen.org/system/model_name";
static const char *IC_SYSTEM_INFO_BUILD_STRING = "http://tizen.org/system/build.string";
static const char *IC_SYSTEM_INFO_TIZEN_ID = "http://tizen.org/system/tizenid";

static pthread_mutex_t ic_utils_mutex_init = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t ic_utils_mutex_ioty = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
static pthread_mutex_t ic_utils_mutex_polling = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t ic_utils_cond_polling;
static __thread int ic_utils_pthread_oldstate;
static __thread int ic_utils_mutex_count;

char* ic_utils_strdup(const char *src)
{
	char *dest = NULL;

	RETV_IF(NULL == src, NULL);

	dest = strdup(src);
	if (NULL == dest) {
		ERR("strdup() Fail(%d)", errno);
		return NULL;
	}

	return dest;
}

bool ic_utils_check_permission(int permssion)
{
	static int has_network_permission = -1;
	static int has_internet_permission = -1;

	if (-1 == has_internet_permission) {
		int ret;
		char smack_label[SMACK_LABEL_LEN + 1] = {0};
		char uid[10];
		FILE *fd;
		cynara *cynara_h;

		ret = cynara_initialize(&cynara_h, NULL);
		if (CYNARA_API_SUCCESS != ret) {
			 ERR("cynara_initialize() Fail(%d)", ret);
			 return false;
		}

		fd = fopen("/proc/self/attr/current", "r");
		if (NULL == fd) {
			 ERR("fopen() Fail(%d)", errno);
			 return false;
		}

		ret = fread(smack_label, sizeof(smack_label), 1, fd);
		fclose(fd);
		if (ret < 0) {
			 ERR("fread() Fail(%d)", ret);
			 return 0;
		}

		snprintf(uid, sizeof(uid), "%d", getuid());

		ret = cynara_check(cynara_h, smack_label, "", uid, IC_PRIVILEGE_INTERNET);
		if (CYNARA_API_ACCESS_ALLOWED == ret)
			has_internet_permission = 1;
		else
			has_internet_permission = 0;

		ret = cynara_check(cynara_h, smack_label, "", uid, IC_PRIVILEGE_NETWORK_GET);
		if (CYNARA_API_ACCESS_ALLOWED == ret)
			has_network_permission = 1;
		else
			has_network_permission = 0;

		cynara_finish(cynara_h);
	}

	if ((IC_PERMISSION_NETWORK_GET & permssion) && (1 != has_network_permission)) {
		ERR("Don't have http://tizen.org/privilege/network.get");
		return false;
	}
	if ((IC_PERMISSION_INTERNET & permssion) && (1 != has_internet_permission)) {
		ERR("Don't have http://tizen.org/privilege/internet");
		return false;
	}

	return true;
}

bool ic_utils_check_ocf_feature()
{
#ifdef TZ_VER_3
	if (_ic_ocf_feature < 0) {
		bool feature_supported = false;
		system_info_get_platform_bool(IC_FEATURE_OCF, &feature_supported);
		_ic_ocf_feature = feature_supported ? 1 : 0;
	}
	return _ic_ocf_feature;
#else
	return true;
#endif
}

void ic_utils_free_platform_info(OCPlatformInfo *platform_info)
{
	RET_IF(NULL == platform_info);

	free(platform_info->manufacturerName);
	free(platform_info->manufacturerUrl);
	free(platform_info->modelNumber);
	free(platform_info->dateOfManufacture);
	free(platform_info->platformVersion);
	free(platform_info->operatingSystemVersion);
	free(platform_info->hardwareVersion);
	free(platform_info->firmwareVersion);
	free(platform_info->supportUrl);
	free(platform_info->systemTime);
}

int ic_utils_get_platform_info(OCPlatformInfo *platform_info)
{
	int ret;
	char *tizen_id = NULL;
	char *device_name = NULL;
	char platform_id[1024];

	RETV_IF(NULL == platform_info, IOTCON_ERROR_INVALID_PARAMETER);

	ret = system_settings_get_value_string(SYSTEM_SETTINGS_KEY_DEVICE_NAME, &device_name);
	WARN_IF(SYSTEM_SETTINGS_ERROR_NONE != ret,
			"system_settings_get_value_string() Fail(%d)", ret);
	ret = system_info_get_platform_string(IC_SYSTEM_INFO_TIZEN_ID, &tizen_id);
	WARN_IF(SYSTEM_INFO_ERROR_NONE != ret, "system_info_get_platform_string() Fail(%d)",
			ret);
	snprintf(platform_id, sizeof(platform_id), "%s(%s)", IC_SAFE_STR(device_name),
			IC_SAFE_STR(tizen_id));
	free(device_name);
	free(tizen_id);
	SECURE_DBG("platform_id: %s", platform_id);

	/* Mandatory (oic.wk.p) */
	platform_info->platformID = strdup(platform_id);

	/* Mandatory (oic.wk.p) */
	ret = system_info_get_platform_string(IC_SYSTEM_INFO_MANUF_NAME,
			&platform_info->manufacturerName);
	if (SYSTEM_INFO_ERROR_NONE != ret) {
		ERR("system_info_get_platform_string(manufacturer) Fail(%d)", ret);
		ic_utils_free_platform_info(platform_info);
		return IOTCON_ERROR_SYSTEM;
	}

	ret = system_info_get_platform_string(IC_SYSTEM_INFO_MODEL_NAME,
			&platform_info->modelNumber);
	if (SYSTEM_INFO_ERROR_NONE != ret)
		WARN("system_info_get_platform_string(model_name) Fail(%d)", ret);

	ret = system_info_get_platform_string(IC_SYSTEM_INFO_PLATFORM_VERSION,
			&platform_info->platformVersion);
	if (SYSTEM_INFO_ERROR_NONE != ret)
		WARN("system_info_get_platform_string(platform_version) Fail(%d)", ret);

	ret = system_info_get_platform_string(IC_SYSTEM_INFO_BUILD_STRING,
			&platform_info->firmwareVersion);
	if (SYSTEM_INFO_ERROR_NONE != ret)
		WARN("system_info_get_platform_string(build_string) Fail(%d)", ret);

	/* platform_info.manufacturerUrl */
	/* platform_info.dateOfManufacture */
	/* platform_info.operatingSystemVersion */
	/* platform_info.hardwareVersion */
	/* platform_info.supportUrl */
	/* platform_info.systemTime */

	return IOTCON_ERROR_NONE;
}


static inline pthread_mutex_t* _utils_mutex_get(int type)
{
	pthread_mutex_t *mutex;

	switch (type) {
	case IC_UTILS_MUTEX_INIT:
		mutex = &ic_utils_mutex_init;
		break;
	case IC_UTILS_MUTEX_IOTY:
		mutex = &ic_utils_mutex_ioty;
		break;
	case IC_UTILS_MUTEX_POLLING:
		mutex = &ic_utils_mutex_polling;
		break;
	default:
		ERR("Invalid type(%d)", type);
		mutex = NULL;
	}

	return mutex;
}

void ic_utils_mutex_lock(int type)
{
	int ret;

	ic_utils_mutex_count++;
	if (1 == ic_utils_mutex_count) {
		ret = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &ic_utils_pthread_oldstate);
		WARN_IF(0 != ret, "pthread_setcancelstate() Fail(%d)", ret);
	}

	ret = pthread_mutex_lock(_utils_mutex_get(type));
	WARN_IF(0 != ret, "pthread_mutex_lock() Fail(%d)", ret);
}

void ic_utils_mutex_unlock(int type)
{
	int ret;

	ic_utils_mutex_count--;
	if (0 == ic_utils_mutex_count) {
		ret = pthread_setcancelstate(ic_utils_pthread_oldstate, NULL);
		WARN_IF(0 != ret, "pthread_setcancelstate() Fail(%d)", ret);
	}

	ret = pthread_mutex_unlock(_utils_mutex_get(type));
	WARN_IF(0 != ret, "pthread_mutex_unlock() Fail(%d)", ret);
}

int ic_utils_cond_polling_init()
{
	FN_CALL;
	int ret;
	pthread_condattr_t attr;

	ret = pthread_condattr_init(&attr);
	if (0 != ret) {
		ERR("pthread_condattr_init() Fail(%d)", ret);
		pthread_condattr_destroy(&attr);
		return IOTCON_ERROR_SYSTEM;
	}

	ret = pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
	if (0 != ret) {
		ERR("pthread_condattr_setclock() Fail(%d)", ret);
		pthread_condattr_destroy(&attr);
		return IOTCON_ERROR_SYSTEM;
	}

	ret = pthread_cond_init(&ic_utils_cond_polling, &attr);
	if (0 != ret) {
		ERR("pthread_cond_init() Fail(%d)", ret);
		pthread_condattr_destroy(&attr);
		return IOTCON_ERROR_SYSTEM;
	}

	pthread_condattr_destroy(&attr);

	return IOTCON_ERROR_NONE;
}

void ic_utils_cond_polling_destroy()
{
	pthread_cond_destroy(&ic_utils_cond_polling);
}

static inline pthread_cond_t* _utils_cond_get(int type)
{
	pthread_cond_t *cond;

	if (IC_UTILS_COND_POLLING == type) {
		cond = &ic_utils_cond_polling;
	} else {
		ERR("Invalid type(%d)", type);
		cond = NULL;
	}

	return cond;
}

void ic_utils_cond_signal(int type)
{
	int ret;

	ret = pthread_mutex_lock(_utils_mutex_get(IC_UTILS_MUTEX_POLLING));
	WARN_IF(0 != ret, "pthread_mutex_lock() Fail(%d)", ret);

	ret = pthread_cond_signal(_utils_cond_get(type));
	WARN_IF(0 != ret, "pthread_cond_signal() Fail(%d)", ret);

	ret = pthread_mutex_unlock(_utils_mutex_get(IC_UTILS_MUTEX_POLLING));
	WARN_IF(0 != ret, "pthread_mutex_unlock() Fail(%d)", ret);
}

void ic_utils_cond_timedwait(int cond_type, int mutex_type, int polling_interval)
{
	int ret, nsec;
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	/* ts.tv_nsec exists between 0 and 1000000000 */
	nsec = (polling_interval % 1000) * 1000000 + ts.tv_nsec;
	ts.tv_sec += (polling_interval / 1000) + (nsec / 1000000000);
	ts.tv_nsec = nsec % 1000000000;

	ret = pthread_cond_timedwait(_utils_cond_get(cond_type), _utils_mutex_get(mutex_type),
			&ts);
	WARN_IF(ETIMEDOUT != ret && 0 != ret, "pthread_cond_timedwait() Fail(%d)", ret);
}
