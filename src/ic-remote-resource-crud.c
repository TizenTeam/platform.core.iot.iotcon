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
#include "iotcon.h"
#include "ic.h"
#include "ic-utils.h"
#include "ic-ioty.h"
#include "ic-options.h"
#include "ic-response.h"
#include "ic-representation.h"
#include "ic-remote-resource.h"

API int iotcon_remote_resource_get(iotcon_remote_resource_h resource,
		iotcon_query_h query, iotcon_remote_resource_response_cb cb, void *user_data)
{
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	if (true == resource->is_found) {
		ERR("The resource should be cloned.");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	ret = icl_ioty_remote_resource_get(resource, query, cb, user_data);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_remote_resource_get() Fail(%d)", ret);
		return ret;
	}

	return IOTCON_ERROR_NONE;
}


API int iotcon_remote_resource_put(iotcon_remote_resource_h resource,
		iotcon_representation_h repr,
		iotcon_query_h query,
		iotcon_remote_resource_response_cb cb,
		void *user_data)
{
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == repr, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	if (true == resource->is_found) {
		ERR("The resource should be cloned.");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	ret = icl_ioty_remote_resource_put(resource, repr, query, cb, user_data);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_remote_resource_put() Fail(%d)", ret);
		return ret;
	}

	return IOTCON_ERROR_NONE;
}


API int iotcon_remote_resource_post(iotcon_remote_resource_h resource,
		iotcon_representation_h repr,
		iotcon_query_h query,
		iotcon_remote_resource_response_cb cb,
		void *user_data)
{

	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == repr, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	if (true == resource->is_found) {
		ERR("The resource should be cloned.");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	ret = icl_ioty_remote_resource_post(resource, repr, query, cb, user_data);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_remote_resource_post() Fail(%d)", ret);
		return ret;
	}

	return IOTCON_ERROR_NONE;
}

API int iotcon_remote_resource_delete(iotcon_remote_resource_h resource,
		iotcon_remote_resource_response_cb cb, void *user_data)
{
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	if (true == resource->is_found) {
		ERR("The resource should be cloned.");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	ret = icl_ioty_remote_resource_delete(resource, cb, user_data);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_remote_resource_delete() Fail(%d)", ret);
		return ret;
	}

	return IOTCON_ERROR_NONE;
}

API int iotcon_remote_resource_observe_register(
		iotcon_remote_resource_h resource,
		iotcon_observe_policy_e observe_policy,
		iotcon_query_h query,
		iotcon_remote_resource_observe_cb cb,
		void *user_data)
{
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(resource->observe_handle, IOTCON_ERROR_ALREADY);

	if (true == resource->is_found) {
		ERR("The resource should be cloned.");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	ret = icl_ioty_remote_resource_observe_register(resource, observe_policy, query, cb,
			user_data);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_remote_resource_observe_register() Fail(%d)", ret);
		return ret;
	}

	return IOTCON_ERROR_NONE;
}


API int iotcon_remote_resource_observe_deregister(
		iotcon_remote_resource_h resource)
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

	if (0 == resource->observe_handle) {
		ERR("It doesn't have a observe_handle");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	ret = icl_ioty_remote_resource_observe_deregister(resource);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_remote_resource_observe_deregister() Fail(%d)", ret);
		return ret;
	}

	return IOTCON_ERROR_NONE;
}

