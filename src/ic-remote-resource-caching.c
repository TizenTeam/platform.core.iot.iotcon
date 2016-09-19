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
#include "ic-utils.h"
#include "ic-types.h"
#include "ic-representation.h"
#include "ic-list.h"
#include "ic-value.h"
#include "ic-remote-resource.h"
#include "ic-ioty.h"

API int iotcon_remote_resource_start_caching(iotcon_remote_resource_h resource,
		iotcon_remote_resource_cached_representation_changed_cb cb, void *user_data)
{
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);

	if (true == resource->is_found) {
		ERR("The resource should be cloned.");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}
	if (resource->caching.observe) {
		ERR("Already Start Caching");
		return IOTCON_ERROR_ALREADY;
	}

	INFO("Start Caching");

	ret = icl_ioty_remote_resource_start_caching(resource, cb, user_data);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_remote_resource_start_caching() Fail(%d)", ret);
		icl_remote_resource_unref(resource);
		return ret;
	}

	return IOTCON_ERROR_NONE;
}


API int iotcon_remote_resource_stop_caching(iotcon_remote_resource_h resource)
{
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);

	if (0 == resource->caching.observe) {
		ERR("Not Cached");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	INFO("Stop Caching");

	ret = icl_ioty_remote_resource_stop_caching(resource);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_remote_resource_stop_caching() Fail(%d)", ret);
		return ret;
	}
	icl_remote_resource_unref(resource);

	return IOTCON_ERROR_NONE;
}


API int iotcon_remote_resource_get_cached_representation(
		iotcon_remote_resource_h resource,
		iotcon_representation_h *representation)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == representation, IOTCON_ERROR_INVALID_PARAMETER);
	WARN_IF(NULL == resource->caching.repr, "No Cached Representation");

	*representation = resource->caching.repr;

	return IOTCON_ERROR_NONE;
}

