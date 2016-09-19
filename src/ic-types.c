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
#include <stdlib.h>
#include <errno.h>

#include "iotcon.h"
#include "ic.h"
#include "ic-device.h"
#include "ic-remote-resource.h"
#include "ic-presence.h"
#include "ic-request.h"
#include "ic-ioty-parse.h"
#include "ic-types.h"

void icl_destroy_find_cb_data(icl_find_cb_s *cb_data)
{
	RET_IF(NULL == cb_data);

	g_list_free_full(cb_data->resource_list, ic_ioty_free_resource_list);

	free(cb_data);
}

int icl_create_find_cb_data(icl_cb_s *cb_data, GList *resource_list,
		icl_find_cb_s **find_cb_data)
{
	icl_find_cb_s *cd;

	RETV_IF(NULL == cb_data, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == resource_list, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == find_cb_data, IOTCON_ERROR_INVALID_PARAMETER);

	cd = calloc(1, sizeof(icl_find_cb_s));
	if (NULL == cd) {
		ERR("calloc(icl_find_cb_s) Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	cd->cb_data = cb_data;
	cd->resource_list = resource_list;

	*find_cb_data = cd;
	return IOTCON_ERROR_NONE;
}


void icl_destroy_device_cb_data(icl_device_cb_s *cb_data)
{
	RET_IF(NULL == cb_data);

	if (cb_data->device_info) {
		free(cb_data->device_info->device_name);
		free(cb_data->device_info->spec_ver);
		free(cb_data->device_info->device_id);
		free(cb_data->device_info->data_model_ver);
		free(cb_data->device_info);
	}

	free(cb_data);
}

int icl_create_device_cb_data(icl_cb_s *cb_data,
		iotcon_device_info_h device_info, icl_device_cb_s **dev_cb_data)
{
	icl_device_cb_s *cd = NULL;

	RETV_IF(NULL == cb_data, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == device_info, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == dev_cb_data, IOTCON_ERROR_INVALID_PARAMETER);

	cd = calloc(1, sizeof(icl_device_cb_s));
	if (NULL == cd) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}
	cd->cb_data = cb_data;
	cd->device_info = device_info;

	*dev_cb_data = cd;

	return IOTCON_ERROR_NONE;
}

void icl_destroy_platform_cb_data(icl_platform_cb_s *cb_data)
{
	RET_IF(NULL == cb_data);

	if (cb_data->platform_info) {
		free(cb_data->platform_info->platform_id);
		free(cb_data->platform_info->manuf_name);
		free(cb_data->platform_info->manuf_url);
		free(cb_data->platform_info->model_number);
		free(cb_data->platform_info->date_of_manuf);
		free(cb_data->platform_info->platform_ver);
		free(cb_data->platform_info->os_ver);
		free(cb_data->platform_info->hardware_ver);
		free(cb_data->platform_info->firmware_ver);
		free(cb_data->platform_info->support_url);
		free(cb_data->platform_info->system_time);
		free(cb_data->platform_info);
	}

	free(cb_data);
}

int icl_create_platform_cb_data(icl_cb_s *cb_data,
		iotcon_platform_info_h platform_info, icl_platform_cb_s **platform_cb_data)
{
	icl_platform_cb_s *cd = NULL;

	RETV_IF(NULL == cb_data, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == platform_info, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == platform_cb_data, IOTCON_ERROR_INVALID_PARAMETER);

	cd = calloc(1, sizeof(icl_platform_cb_s));
	if (NULL == cd) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}
	cd->cb_data = cb_data;
	cd->platform_info = platform_info;

	*platform_cb_data = cd;

	return IOTCON_ERROR_NONE;
}

void icl_destroy_presence_cb_data(icl_presence_cb_s *cb_data)
{
	RET_IF(NULL == cb_data);

	icl_destroy_presence_response(cb_data->response);
	free(cb_data);
}

int icl_create_presence_cb_data(iotcon_presence_h presence,
		iotcon_presence_response_h response, icl_presence_cb_s **presence_cb_data)
{
	icl_presence_cb_s *cd = NULL;

	RETV_IF(NULL == presence, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == response, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == presence_cb_data, IOTCON_ERROR_INVALID_PARAMETER);

	cd = calloc(1, sizeof(icl_presence_cb_s));
	if (NULL == presence_cb_data) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	cd->presence = presence;
	cd->response = response;

	*presence_cb_data = cd;

	return IOTCON_ERROR_NONE;
}

void icl_destroy_presence_response(iotcon_presence_response_h presence_response)
{
	RET_IF(NULL == presence_response);

	free(presence_response->host_address);
	free(presence_response->resource_type);
	free(presence_response);
}

void icl_destroy_presence(iotcon_presence_h presence)
{
	RET_IF(NULL == presence);

	free(presence->host_address);
	free(presence->resource_type);
	free(presence);
}

void icl_destroy_observe_cb_data(icl_observe_cb_s *cb_data)
{
	RET_IF(NULL == cb_data);

	iotcon_response_destroy(cb_data->response);
	icl_remote_resource_unref(cb_data->resource);

	free(cb_data);
}

int icl_create_observe_cb_data(icl_observe_container_s *cb_container,
		int sequence_number, iotcon_response_h response, icl_observe_cb_s **observe_cb_data)
{
	icl_observe_cb_s *cb_data;

	RETV_IF(NULL == cb_container, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == observe_cb_data, IOTCON_ERROR_INVALID_PARAMETER);

	cb_data = calloc(1, sizeof(icl_observe_cb_s));
	if (NULL == cb_data) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}
	cb_data->cb = cb_container->cb;
	cb_data->user_data = cb_container->user_data;
	cb_data->resource = cb_container->resource;
	icl_remote_resource_ref(cb_data->resource);
	cb_data->response = response;
	cb_data->sequence_number = sequence_number;

	*observe_cb_data = cb_data;

	return IOTCON_ERROR_NONE;
}

void icl_destroy_response_cb_data(icl_response_cb_s *cb_data)
{
	RET_IF(NULL == cb_data);

	iotcon_response_destroy(cb_data->response);
	icl_remote_resource_unref(cb_data->resource);
	free(cb_data);
}

int icl_create_response_cb_data(icl_response_container_s *cb_container,
		iotcon_response_h response, icl_response_cb_s **response_cb_data)
{
	icl_response_cb_s *cb_data;

	RETV_IF(NULL == cb_container, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == response_cb_data, IOTCON_ERROR_INVALID_PARAMETER);

	cb_data = calloc(1, sizeof(icl_response_cb_s));
	if (NULL == cb_data) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}
	cb_data->cb = cb_container->cb;
	cb_data->user_data = cb_container->user_data;
	cb_data->resource = cb_container->resource;
	cb_data->req_type = cb_container->req_type;
	icl_remote_resource_ref(cb_data->resource);
	cb_data->response = response;

	*response_cb_data = cb_data;

	return IOTCON_ERROR_NONE;
}

void icl_destroy_monitoring_container(void *data)
{
	icl_monitoring_container_s *cb_container = data;

	RET_IF(NULL == cb_container);

	if (false == cb_container->is_destroyed) {
		/* cb_container will destroy in _icl_ioty_monitoring_get_cb */
		cb_container->is_destroyed = true;
		return;
	}

	if (cb_container->presence) {
		icl_ioty_remove_presence_cb(cb_container->presence);
		cb_container->presence = NULL;
	}

	if (cb_container->timeout) {
		g_source_remove(cb_container->timeout);
		cb_container->timeout = 0;
	}

	if (cb_container->resource) {
		icl_remote_resource_unref(cb_container->resource);
		cb_container->resource = NULL;
	}
}

void icl_destroy_caching_container(void *data)
{
	icl_caching_container_s *cb_container = data;

	RET_IF(NULL == cb_container);

	if (false == cb_container->is_destroyed) {
		/* cb_container will destroy in _icl_ioty_caching_get_cb */
		cb_container->is_destroyed = true;
		return;
	}

	if (cb_container->observe_handle) {
		icl_ioty_remote_resource_observe_cancel(cb_container->resource,
				cb_container->observe_handle);
		cb_container->observe_handle = 0;
	}

	if (cb_container->timeout) {
		g_source_remove(cb_container->timeout);
		cb_container->timeout = 0;
	}

	if (cb_container->resource) {
		icl_remote_resource_unref(cb_container->resource);
		cb_container->resource = NULL;
	}
}

static void _icl_destroy_request(iotcon_request_h request)
{
	RET_IF(NULL == request);

	free(request->host_address);
	if (request->header_options)
		iotcon_options_destroy(request->header_options);
	if (request->query)
		iotcon_query_destroy(request->query);
	if (request->repr)
		iotcon_representation_destroy(request->repr);
	free(request);
}

void icl_destroy_request_container(icl_request_container_s *cb_container)
{
	RET_IF(NULL == cb_container);

	_icl_destroy_request(cb_container->request);
	free(cb_container);
}

