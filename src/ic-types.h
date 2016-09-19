/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef __IOTCON_INTERNAL_TYPES_H__
#define __IOTCON_INTERNAL_TYPES_H__

#include <stdint.h>
#include <glib.h>
#include "iotcon-types.h"
#include "ic-ioty.h"

typedef struct {
	icl_cb_s *cb_data;
	GList *resource_list;
} icl_find_cb_s;

typedef struct {
	icl_cb_s *cb_data;
	iotcon_platform_info_h platform_info;
} icl_platform_cb_s;

typedef struct {
	icl_cb_s *cb_data;
	iotcon_device_info_h device_info;
} icl_device_cb_s;

typedef struct {
	iotcon_presence_h presence;
	iotcon_presence_response_h response;
} icl_presence_cb_s;

typedef struct {
	iotcon_remote_resource_observe_cb cb;
	void *user_data;
	iotcon_remote_resource_h resource;
} icl_observe_container_s;

typedef struct {
	iotcon_remote_resource_observe_cb cb;
	void *user_data;
	iotcon_remote_resource_h resource;
	int sequence_number;
	iotcon_response_h response;
} icl_observe_cb_s;

typedef struct {
	iotcon_remote_resource_response_cb cb;
	void *user_data;
	int timeout;
	iotcon_remote_resource_h resource;
	iotcon_request_type_e req_type;
	GMainContext *thread_context;
} icl_response_container_s;

typedef struct {
	iotcon_remote_resource_response_cb cb;
	void *user_data;
	iotcon_request_type_e req_type;
	iotcon_remote_resource_h resource;
	iotcon_response_h response;
} icl_response_cb_s;

typedef struct {
	bool is_destroyed;
	iotcon_remote_resource_state_changed_cb cb;
	void *user_data;
	iotcon_remote_resource_h resource;
	iotcon_presence_h presence;
	iotcon_remote_resource_state_e state;
	int timeout;
} icl_monitoring_container_s;

typedef struct {
	bool is_destroyed;
	iotcon_remote_resource_cached_representation_changed_cb cb;
	void *user_data;
	iotcon_remote_resource_h resource;
	int64_t observe_handle;
	int timeout;
} icl_caching_container_s;

typedef struct {
	iotcon_request_h request;
	iotcon_resource_h resource;
} icl_request_container_s;

void icl_destroy_find_cb_data(icl_find_cb_s *cb_data);
int icl_create_find_cb_data(icl_cb_s *cb_data, GList *resource_list,
		icl_find_cb_s **find_callback_data);

void icl_destroy_device_cb_data(icl_device_cb_s *cb_data);
int icl_create_device_cb_data(icl_cb_s *cb_data,
		iotcon_device_info_h device_info, icl_device_cb_s **dev_cb_data);

void icl_destroy_platform_cb_data(icl_platform_cb_s *cb_data);
int icl_create_platform_cb_data(icl_cb_s *cb_data,
		iotcon_platform_info_h platform_info, icl_platform_cb_s **platform_cb_data);

void icl_destroy_presence_cb_data(icl_presence_cb_s *cb_data);
int icl_create_presence_cb_data(iotcon_presence_h presence,
		iotcon_presence_response_h response, icl_presence_cb_s **presence_cb_data);

void icl_destroy_presence(iotcon_presence_h presence);
void icl_destroy_presence_response(iotcon_presence_response_h presence_response);

void icl_destroy_observe_cb_data(icl_observe_cb_s *cb_data);
int icl_create_observe_cb_data(icl_observe_container_s *cb_container,
		int sequence_number, iotcon_response_h resp, icl_observe_cb_s **observe_cb_data);

void icl_destroy_response_cb_data(icl_response_cb_s *cb_data);
int icl_create_response_cb_data(icl_response_container_s *cb_container,
		iotcon_response_h response, icl_response_cb_s **response_cb_data);

void icl_destroy_monitoring_container(void *data);
void icl_destroy_caching_container(void *data);

void icl_destroy_request_container(icl_request_container_s *cb_container);

#endif /*__IOTCON_INTERNAL_TYPES_H__*/
