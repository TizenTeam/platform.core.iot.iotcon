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

#include "iotcon-types.h"
#include "ic.h"
#include "ic-utils.h"
#include "ic-resource.h"
#include "ic-presence.h"
#include "ic-resource-types.h"
#include "ic-ioty.h"

API int iotcon_start_presence(unsigned int time_to_live)
{
	FN_CALL;
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);

	if (0 == time_to_live)
		time_to_live = IC_PRESENCE_TTL_SECONDS_DEFAULT;
	else if (IC_PRESENCE_TTL_SECONDS_MAX < time_to_live)
		time_to_live = IC_PRESENCE_TTL_SECONDS_MAX;

	ret = icl_ioty_start_presence(time_to_live);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_start_presence() Fail(%d)", ret);
		return ret;
	}

	return IOTCON_ERROR_NONE;
}


API int iotcon_stop_presence(void)
{
	FN_CALL;
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);

	ret = icl_ioty_stop_presence();
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_stop_presence() Fail(%d)", ret);
		return ret;
	}

	return IOTCON_ERROR_NONE;
}

/* The length of resource_type should be less than or equal to 61. */
API int iotcon_add_presence_cb(const char *host_address,
		iotcon_connectivity_type_e connectivity_type,
		const char *resource_type,
		iotcon_presence_cb cb,
		void *user_data,
		iotcon_presence_h *presence_handle)
{
	FN_CALL;
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == presence_handle, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(resource_type && (false == icl_resource_check_type(resource_type)),
			IOTCON_ERROR_INVALID_PARAMETER);

	ret = icl_ioty_add_presence_cb(host_address, connectivity_type, resource_type,
			cb, user_data, presence_handle);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_add_presence_cb() Fail(%d)", ret);
		return ret;
	}

	return IOTCON_ERROR_NONE;
}


API int iotcon_remove_presence_cb(iotcon_presence_h presence)
{
	FN_CALL;
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(NULL == presence, IOTCON_ERROR_INVALID_PARAMETER);

	ret = icl_ioty_remove_presence_cb(presence);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_remove_presence_cb() Fail(%d)", ret);
		return ret;
	}

	return IOTCON_ERROR_NONE;
}


API int iotcon_presence_get_host_address(iotcon_presence_h presence,
		char **host_address)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == presence, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == host_address, IOTCON_ERROR_INVALID_PARAMETER);

	*host_address = presence->host_address;

	return IOTCON_ERROR_NONE;
}


API int iotcon_presence_get_connectivity_type(iotcon_presence_h presence,
		iotcon_connectivity_type_e *connectivity_type)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == presence, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == connectivity_type, IOTCON_ERROR_INVALID_PARAMETER);

	*connectivity_type = presence->connectivity_type;

	return IOTCON_ERROR_NONE;
}


API int iotcon_presence_get_resource_type(iotcon_presence_h presence,
		char **resource_type)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == presence, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == resource_type, IOTCON_ERROR_INVALID_PARAMETER);

	*resource_type = presence->resource_type;

	return IOTCON_ERROR_NONE;
}


API int iotcon_presence_response_get_host_address(
		iotcon_presence_response_h response, char **host_address)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == response, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == host_address, IOTCON_ERROR_INVALID_PARAMETER);

	*host_address = response->host_address;

	return IOTCON_ERROR_NONE;
}


API int iotcon_presence_response_get_connectivity_type(
		iotcon_presence_response_h response, iotcon_connectivity_type_e *connectivity_type)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == response, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == connectivity_type, IOTCON_ERROR_INVALID_PARAMETER);

	*connectivity_type = response->connectivity_type;

	return IOTCON_ERROR_NONE;
}


API int iotcon_presence_response_get_resource_type(
		iotcon_presence_response_h response, char **resource_type)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == response, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == resource_type, IOTCON_ERROR_INVALID_PARAMETER);

	*resource_type = response->resource_type;

	return IOTCON_ERROR_NONE;
}


API int iotcon_presence_response_get_result(iotcon_presence_response_h response,
		iotcon_presence_result_e *result)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == response, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == result, IOTCON_ERROR_INVALID_PARAMETER);

	*result = response->result;

	return IOTCON_ERROR_NONE;
}


API int iotcon_presence_response_get_trigger(iotcon_presence_response_h response,
		iotcon_presence_trigger_e *trigger)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == response, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == trigger, IOTCON_ERROR_INVALID_PARAMETER);

	if (IOTCON_PRESENCE_OK != response->result) {
		ERR("trigger is valid if IOTCON_PRESENCE_OK");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	*trigger = response->trigger;

	return IOTCON_ERROR_NONE;
}

