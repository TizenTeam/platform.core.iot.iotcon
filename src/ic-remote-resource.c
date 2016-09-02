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
#include <errno.h>

#include "iotcon-types.h"
#include "ic.h"
#include "ic-utils.h"
#include "ic-options.h"
#include "ic-representation.h"
#include "ic-remote-resource.h"
#include "ic-resource.h"
#include "ic-resource-types.h"
#include "ic-resource-interfaces.h"
#include "ic-ioty.h"

#define ICL_REMOTE_RESOURCE_MAX_CHECKING_INTERVAL 3600 /* 60 min */
#define ICL_REMOTE_RESOURCE_DEFAULT_CHECKING_INTERVAL 10 /* 10 sec */

/* The length of resource_type should be less than or equal to 61.
 * If resource_type is NULL, then All resources in host are discovered. */
API int iotcon_find_resource(const char *host_address,
		iotcon_connectivity_type_e connectivity_type,
		iotcon_query_h query,
		iotcon_found_resource_cb cb,
		void *user_data)
{
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	switch (connectivity_type) {
	case IOTCON_CONNECTIVITY_IPV4:
	case IOTCON_CONNECTIVITY_IPV6:
	case IOTCON_CONNECTIVITY_ALL:
		ret = icl_ioty_find_resource(host_address, connectivity_type, query, cb,
				user_data);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("icl_ioty_find_resource() Fail(%d)", ret);
			return ret;
		}
		break;
	default:
		ERR("Invalid Connectivity Type(%d)", connectivity_type);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	return IOTCON_ERROR_NONE;
}

/* If you know the information of resource, then you can make a proxy of the resource. */
API int iotcon_remote_resource_create(const char *host_address,
		iotcon_connectivity_type_e connectivity_type,
		const char *uri_path,
		uint8_t policies,
		iotcon_resource_types_h resource_types,
		iotcon_resource_interfaces_h resource_ifaces,
		iotcon_remote_resource_h *resource_handle)
{
	FN_CALL;
	char temp[PATH_MAX] = {0};
	iotcon_remote_resource_h resource = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == host_address, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == uri_path, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(false == icl_resource_check_uri_path(uri_path),
			IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == resource_types, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == resource_ifaces, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == resource_handle, IOTCON_ERROR_INVALID_PARAMETER);
	RETVM_IF(IOTCON_CONNECTIVITY_ALL == connectivity_type, IOTCON_ERROR_INVALID_PARAMETER,
		"Should use specific connectivity type of the remote resource");

	if (IC_EQUAL == strncmp(IC_COAP_PREFIX, host_address, strlen(IC_COAP_PREFIX)))
		snprintf(temp, sizeof(temp), "%s", host_address);
	else if (policies & IOTCON_RESOURCE_SECURE)
		snprintf(temp, sizeof(temp), "%s%s", IC_COAPS, host_address);
	else
		snprintf(temp, sizeof(temp), "%s%s", IC_COAP, host_address);

	resource = calloc(1, sizeof(struct icl_remote_resource));
	if (NULL == resource) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	resource->host_address = ic_utils_strdup(temp);
	resource->connectivity_type = connectivity_type;
	resource->uri_path = ic_utils_strdup(uri_path);
	resource->policies = policies;
	resource->types = icl_resource_types_ref(resource_types);
	resource->ifaces = icl_resource_interfaces_ref(resource_ifaces);
	resource->checking_interval = ICL_REMOTE_RESOURCE_DEFAULT_CHECKING_INTERVAL;
	resource->ref_count = 1;

	*resource_handle = resource;

	return IOTCON_ERROR_NONE;
}

static void _icl_remote_resource_destroy(iotcon_remote_resource_h resource)
{
	RET_IF(NULL == resource);

	if (resource->ref_count < 0) {
		ERR("Invalid ref_count (%d)", resource->ref_count);
		return;
	}

	if (true == resource->is_found) {
		ERR("It can't be destroyed by user.");
		return;
	}

	free(resource->uri_path);
	free(resource->host_address);
	free(resource->device_id);
	free(resource->device_name);
	iotcon_resource_interfaces_destroy(resource->ifaces);
	iotcon_resource_types_destroy(resource->types);

	/* null COULD be allowed */
	if (resource->header_options)
		iotcon_options_destroy(resource->header_options);

	if (resource->monitoring.presence)
		iotcon_remote_resource_stop_monitoring(resource);

	if (resource->caching.observe)
		iotcon_remote_resource_stop_caching(resource);

	free(resource);
}

void icl_remote_resource_ref(iotcon_remote_resource_h resource)
{
	RET_IF(NULL == resource);

	resource->ref_count++;
}

void icl_remote_resource_unref(iotcon_remote_resource_h resource)
{
	RET_IF(NULL == resource);

	resource->ref_count--;
	if (0 == resource->ref_count)
		_icl_remote_resource_destroy(resource);
}


API int iotcon_remote_resource_destroy(iotcon_remote_resource_h resource)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);

	if (resource->observe_handle)
		iotcon_remote_resource_observe_deregister(resource);

	icl_remote_resource_unref(resource);

	return IOTCON_ERROR_NONE;
}

static bool _icl_remote_resource_header_foreach_cb(unsigned short id,
		const char *data, void *user_data)
{
	int ret;
	iotcon_remote_resource_h resource = user_data;

	RETV_IF(NULL == resource, IOTCON_FUNC_STOP);

	if (NULL == resource->header_options) {
		ret = iotcon_options_create(&resource->header_options);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("resource->header_options() Fail(%d)", ret);
			return IOTCON_FUNC_STOP;
		}
	}

	ret = iotcon_options_add(resource->header_options, id, data);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_options_add() Fail(%d)", ret);
		return IOTCON_FUNC_STOP;
	}

	return IOTCON_FUNC_CONTINUE;
}

API int iotcon_remote_resource_clone(iotcon_remote_resource_h src,
		iotcon_remote_resource_h *dest)
{
	int ret;
	iotcon_remote_resource_h resource = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == src, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == dest, IOTCON_ERROR_INVALID_PARAMETER);

	resource = calloc(1, sizeof(struct icl_remote_resource));
	if (NULL == resource) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	resource->uri_path = ic_utils_strdup(src->uri_path);
	resource->host_address = ic_utils_strdup(src->host_address);
	resource->connectivity_type = src->connectivity_type;
	resource->device_id = ic_utils_strdup(src->device_id);
	resource->device_name = ic_utils_strdup(src->device_name);
	resource->policies = src->policies;
	resource->checking_interval = src->checking_interval;
	resource->ref_count = 1;

	if (src->header_options) {
		ret = iotcon_options_foreach(src->header_options,
				_icl_remote_resource_header_foreach_cb, resource);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_options_foreach() Fail(%d)", ret);
			iotcon_remote_resource_destroy(resource);
			return ret;
		}
	}

	ret = iotcon_resource_types_clone(src->types, &resource->types);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_types_clone() Fail(%d)", ret);
		iotcon_remote_resource_destroy(resource);
		return ret;
	}

	ret = iotcon_resource_interfaces_clone(src->ifaces, &resource->ifaces);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_interfaces_clone() Fail(%d)", ret);
		iotcon_remote_resource_destroy(resource);
		return ret;
	}

	resource->connectivity_type = src->connectivity_type;

	*dest = resource;

	return IOTCON_ERROR_NONE;
}


/* The content of the resource should not be freed by user. */
API int iotcon_remote_resource_get_uri_path(iotcon_remote_resource_h resource,
		char **uri_path)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == uri_path, IOTCON_ERROR_INVALID_PARAMETER);

	*uri_path = resource->uri_path;

	return IOTCON_ERROR_NONE;
}


/* The content of the resource should not be freed by user. */
API int iotcon_remote_resource_get_host_address(
		iotcon_remote_resource_h resource, char **host_address)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == host_address, IOTCON_ERROR_INVALID_PARAMETER);

	*host_address = resource->host_address;

	return IOTCON_ERROR_NONE;
}


/* The content of the resource should not be freed by user. */
API int iotcon_remote_resource_get_connectivity_type(
		iotcon_remote_resource_h resource, iotcon_connectivity_type_e *connectivity_type)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == connectivity_type, IOTCON_ERROR_INVALID_PARAMETER);

	*connectivity_type = resource->connectivity_type;

	return IOTCON_ERROR_NONE;
}


/* The content of the resource should not be freed by user. */
API int iotcon_remote_resource_get_device_id(iotcon_remote_resource_h resource,
		char **device_id)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == device_id, IOTCON_ERROR_INVALID_PARAMETER);
	RETVM_IF(NULL == resource->device_id, IOTCON_ERROR_NO_DATA,
			"If you want to get device ID, you should call iotcon_find_resource().");

	*device_id = resource->device_id;

	return IOTCON_ERROR_NONE;
}


/* The content of the resource should not be freed by user. */
API int iotcon_remote_resource_get_device_name(iotcon_remote_resource_h resource,
		char **device_name)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == device_name, IOTCON_ERROR_INVALID_PARAMETER);
	RETVM_IF(NULL == resource->device_name, IOTCON_ERROR_NO_DATA,
			"If you want to get device name, you should call iotcon_find_resource().");

	*device_name = resource->device_name;

	return IOTCON_ERROR_NONE;
}


/* The content of the resource should not be freed by user. */
API int iotcon_remote_resource_get_types(iotcon_remote_resource_h resource,
		iotcon_resource_types_h *types)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == types, IOTCON_ERROR_INVALID_PARAMETER);

	*types = resource->types;

	return IOTCON_ERROR_NONE;
}


/* The content of the resource should not be freed by user. */
API int iotcon_remote_resource_get_interfaces(iotcon_remote_resource_h resource,
		iotcon_resource_interfaces_h *ifaces)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == ifaces, IOTCON_ERROR_INVALID_PARAMETER);

	*ifaces = resource->ifaces;

	return IOTCON_ERROR_NONE;
}


API int iotcon_remote_resource_get_policies(iotcon_remote_resource_h resource,
		uint8_t *policies)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == policies, IOTCON_ERROR_INVALID_PARAMETER);

	*policies = resource->policies;

	return IOTCON_ERROR_NONE;
}

API int iotcon_remote_resource_get_options(iotcon_remote_resource_h resource,
		iotcon_options_h *options)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == options, IOTCON_ERROR_INVALID_PARAMETER);
	WARN_IF(NULL == resource->header_options, "Not Set header options");

	*options = resource->header_options;

	return IOTCON_ERROR_NONE;
}

/* if header_options is NULL, then client's header_options is unset */
API int iotcon_remote_resource_set_options(iotcon_remote_resource_h resource,
		iotcon_options_h options)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);

	if (options)
		options = icl_options_ref(options);

	if (resource->header_options)
		iotcon_options_destroy(resource->header_options);

	resource->header_options = options;

	return IOTCON_ERROR_NONE;
}

API int iotcon_remote_resource_get_checking_interval(iotcon_remote_resource_h resource,
		int *interval)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == interval, IOTCON_ERROR_INVALID_PARAMETER);

	*interval = resource->checking_interval;

	return IOTCON_ERROR_NONE;
}


API int iotcon_remote_resource_set_checking_interval(iotcon_remote_resource_h resource,
		int interval)
{
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(ICL_REMOTE_RESOURCE_MAX_CHECKING_INTERVAL < interval || interval <= 0,
			IOTCON_ERROR_INVALID_PARAMETER);

	if (resource->monitoring.presence || resource->caching.observe) {
		ret = icl_ioty_remote_resource_set_checking_interval(resource, interval);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("icl_ioty_remote_resource_set_checking_interval() Fail(%d)", ret);
			return ret;
		}
	}

	resource->checking_interval = interval;

	return IOTCON_ERROR_NONE;
}

