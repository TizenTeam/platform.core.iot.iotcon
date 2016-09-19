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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iotcon.h"
#include "ic.h"
#include "ic-utils.h"
#include "ic-representation.h"
#include "ic-device.h"
#include "ic-ioty.h"

API int iotcon_device_info_get_property(iotcon_device_info_h device_info,
		iotcon_device_info_e property, char **value)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == device_info, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == value, IOTCON_ERROR_INVALID_PARAMETER);

	switch (property) {
	case IOTCON_DEVICE_INFO_NAME:
		*value = device_info->device_name;
		break;
	case IOTCON_DEVICE_INFO_SPEC_VER:
		*value = device_info->spec_ver;
		break;
	case IOTCON_DEVICE_INFO_ID:
		*value = device_info->device_id;
		break;
	case IOTCON_DEVICE_INFO_DATA_MODEL_VER:
		*value = device_info->data_model_ver;
		break;
	default:
		ERR("Invalid property(%d)", property);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	return IOTCON_ERROR_NONE;
}

API int iotcon_find_device_info(const char *host_address,
		int connectivity_type,
		iotcon_query_h query,
		iotcon_device_info_cb cb,
		void *user_data)
{
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(false == ic_utils_check_connectivity_type(connectivity_type),
			IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	ret = icl_ioty_find_device_info(host_address, connectivity_type, query, cb,
			user_data);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_find_device_info() Fail(%d)", ret);
		return ret;
	}

	return IOTCON_ERROR_NONE;
}

API int iotcon_set_device_name(const char *device_name)
{
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == device_name || '\0' == *device_name, IOTCON_ERROR_INVALID_PARAMETER);

	ret = icl_ioty_set_device_info(device_name);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_set_device_info() Fail(%d)", ret);
		return ret;
	}

	return IOTCON_ERROR_NONE;
}

API int iotcon_platform_info_get_property(iotcon_platform_info_h platform_info,
		iotcon_platform_info_e property, char **value)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == platform_info, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == value, IOTCON_ERROR_INVALID_PARAMETER);

	switch (property) {
	case IOTCON_PLATFORM_INFO_ID:
		*value = platform_info->platform_id;
		break;
	case IOTCON_PLATFORM_INFO_MANUF_NAME:
		*value = platform_info->manuf_name;
		break;
	case IOTCON_PLATFORM_INFO_MANUF_URL:
		*value = platform_info->manuf_url;
		break;
	case IOTCON_PLATFORM_INFO_MODEL_NUMBER:
		*value = platform_info->model_number;
		break;
	case IOTCON_PLATFORM_INFO_DATE_OF_MANUF:
		*value = platform_info->date_of_manuf;
		break;
	case IOTCON_PLATFORM_INFO_PLATFORM_VER:
		*value = platform_info->platform_ver;
		break;
	case IOTCON_PLATFORM_INFO_OS_VER:
		*value = platform_info->os_ver;
		break;
	case IOTCON_PLATFORM_INFO_HARDWARE_VER:
		*value = platform_info->hardware_ver;
		break;
	case IOTCON_PLATFORM_INFO_FIRMWARE_VER:
		*value = platform_info->firmware_ver;
		break;
	case IOTCON_PLATFORM_INFO_SUPPORT_URL:
		*value = platform_info->support_url;
		break;
	case IOTCON_PLATFORM_INFO_SYSTEM_TIME:
		*value = platform_info->system_time;
		break;
	default:
		ERR("Invalid property(%d)", property);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	return IOTCON_ERROR_NONE;
}

API int iotcon_find_platform_info(const char *host_address,
		int connectivity_type,
		iotcon_query_h query,
		iotcon_platform_info_cb cb,
		void *user_data)
{
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(false == ic_utils_check_connectivity_type(connectivity_type),
			IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	ret = icl_ioty_find_platform_info(host_address, connectivity_type, query, cb,
			user_data);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_find_platform_info() Fail(%d)", ret);
		return ret;
	}

	return IOTCON_ERROR_NONE;
}
