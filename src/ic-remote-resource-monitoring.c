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

#include "iotcon.h"
#include "ic.h"
#include "ic-types.h"
#include "ic-utils.h"
#include "ic-remote-resource.h"
#include "ic-ioty.h"

API int iotcon_remote_resource_start_monitoring(
		iotcon_remote_resource_h resource,
		iotcon_remote_resource_state_changed_cb cb,
		void *user_data)
{
	int ret, connectivity_type;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	if (true == resource->is_found) {
		ERR("The resource should be cloned.");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}
	if (resource->monitoring.presence) {
		ERR("Already Start Monitoring");
		return IOTCON_ERROR_ALREADY;
	}

	INFO("Start Monitoring");

	resource->monitoring.state = IOTCON_REMOTE_RESOURCE_ALIVE;

	connectivity_type = resource->connectivity_type;

	switch (connectivity_type) {
	case IOTCON_CONNECTIVITY_IPV4:
	case IOTCON_CONNECTIVITY_IPV6:
	case IOTCON_CONNECTIVITY_ALL:
		icl_remote_resource_ref(resource);
		ret = icl_ioty_remote_resource_start_monitoring(resource, cb, user_data);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("icl_ioty_remote_resource_start_monitoring() Fail(%d)", ret);
			icl_remote_resource_unref(resource);
			return ret;
		}
		break;
	default:
		ERR("Invalid Connectivity Type(%d)", connectivity_type);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	return IOTCON_ERROR_NONE;
}


API int iotcon_remote_resource_stop_monitoring(iotcon_remote_resource_h resource)
{
	int ret, connectivity_type;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);

	if (NULL == resource->monitoring.presence) {
		ERR("Not Monitoring");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	INFO("Stop Monitoring");

	connectivity_type = resource->connectivity_type;

	switch (connectivity_type) {
	case IOTCON_CONNECTIVITY_IPV4:
	case IOTCON_CONNECTIVITY_IPV6:
	case IOTCON_CONNECTIVITY_ALL:
		ret = icl_ioty_remote_resource_stop_monitoring(resource);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("icl_ioty_remote_resource_stop_monitoring() Fail(%d)", ret);
			return ret;
		}
		icl_remote_resource_unref(resource);
		break;
	default:
		ERR("Invalid Connectivity Type(%d)", connectivity_type);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	return IOTCON_ERROR_NONE;
}

