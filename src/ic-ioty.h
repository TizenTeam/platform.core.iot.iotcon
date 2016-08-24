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
#ifndef __IOTCON_INTERNAL_IOTIVITY_H__
#define __IOTCON_INTERNAL_IOTIVITY_H__

#include <stdint.h>
#include <pthread.h>
#include <octypes.h>

#include "iotcon-client.h"
#include "iotcon-types.h"
#include "ic-utils.h"

typedef enum {
	ICL_FIND_RESOURCE,
	ICL_GET_DEVICE_INFO,
	ICL_GET_PLATFORM_INFO,
	ICL_ADD_PRESENCE,
} icl_operation_e;

typedef struct {
	icl_operation_e op;
	void *cb;
	void *user_data;
	int result;
	int timeout;
	OCDoHandle handle;
} icl_cb_s;

int icl_ioty_mutex_lock();
static inline void icl_ioty_mutex_unlock() { return ic_utils_mutex_unlock(IC_UTILS_MUTEX_IOTY); }
void icl_ioty_deinit(pthread_t thread);
int icl_ioty_init(pthread_t *out_thread);

int icl_ioty_set_persistent_storage(const char *file_path, bool is_pt);

int icl_ioty_set_device_info(const char *device_name);
int icl_ioty_set_platform_info();

/* client APIs */
int icl_ioty_find_resource(const char *host_address,
		iotcon_connectivity_type_e connectivity_type,
		iotcon_query_h query,
		iotcon_found_resource_cb cb,
		void *user_data);
int icl_ioty_find_device_info(const char *host_address,
		iotcon_connectivity_type_e connectivity_type,
		iotcon_query_h query,
		iotcon_device_info_cb cb,
		void *user_data);
int icl_ioty_find_platform_info(const char *host_address,
		iotcon_connectivity_type_e connectivity_type,
		iotcon_query_h query,
		iotcon_platform_info_cb cb,
		void *user_data);

int icl_ioty_add_presence_cb(const char *host_address,
		iotcon_connectivity_type_e connectivity_type,
		const char *resource_type,
		iotcon_presence_cb cb,
		void *user_data,
		iotcon_presence_h *presence_handle);
int icl_ioty_remove_presence_cb(iotcon_presence_h presence);

int icl_ioty_remote_resource_observe_register(
		iotcon_remote_resource_h resource,
		iotcon_observe_policy_e observe_policy,
		iotcon_query_h query,
		iotcon_remote_resource_observe_cb cb,
		void *user_data);
int icl_ioty_remote_resource_observe_deregister(iotcon_remote_resource_h resource);
int icl_ioty_remote_resource_get(iotcon_remote_resource_h resource,
		iotcon_query_h query, iotcon_remote_resource_response_cb cb, void *user_data);
int icl_ioty_remote_resource_put(iotcon_remote_resource_h resource,
		iotcon_representation_h repr,
		iotcon_query_h query,
		iotcon_remote_resource_response_cb cb,
		void *user_data);
int icl_ioty_remote_resource_post(iotcon_remote_resource_h resource,
		iotcon_representation_h repr,
		iotcon_query_h query,
		iotcon_remote_resource_response_cb cb,
		void *user_data);
int icl_ioty_remote_resource_delete(iotcon_remote_resource_h resource,
		iotcon_remote_resource_response_cb cb,
		void *user_data);
int icl_ioty_remote_resource_observe_cancel(iotcon_remote_resource_h resource,
		int64_t handle);
int icl_ioty_remote_resource_start_monitoring(iotcon_remote_resource_h resource,
		iotcon_remote_resource_state_changed_cb cb, void *user_data);
int icl_ioty_remote_resource_stop_monitoring(iotcon_remote_resource_h resource);
int icl_ioty_remote_resource_start_caching(iotcon_remote_resource_h resource,
		iotcon_remote_resource_cached_representation_changed_cb cb, void *user_data);
int icl_ioty_remote_resource_stop_caching(iotcon_remote_resource_h resource);
int icl_ioty_remote_resource_set_time_interval(int time_interval);
int icl_ioty_remote_resource_get_time_interval(int *time_interval);


/* server */
int icl_ioty_start_presence(unsigned int time_to_live);
int icl_ioty_stop_presence();
int icl_ioty_resource_create(const char *uri_path,
		iotcon_resource_types_h res_types,
		iotcon_resource_interfaces_h ifaces,
		uint8_t policies,
		iotcon_request_handler_cb cb,
		void *user_data,
		iotcon_resource_h *resource_handle);
int icl_ioty_resource_bind_type(iotcon_resource_h resource,
		const char *resource_type);
int icl_ioty_resource_bind_interface(iotcon_resource_h resource,
		const char *iface);
int icl_ioty_resource_bind_child_resource(iotcon_resource_h parent,
		iotcon_resource_h child);
int icl_ioty_resource_unbind_child_resource(iotcon_resource_h parent,
		iotcon_resource_h child);
int icl_ioty_resource_notify(iotcon_resource_h resource,
		iotcon_representation_h repr, iotcon_observers_h observers, iotcon_qos_e qos);
int icl_ioty_resource_destroy(iotcon_resource_h resource);

int icl_ioty_lite_resource_create(const char *uri_path,
		iotcon_resource_types_h res_types,
		uint8_t policies,
		iotcon_attributes_h attributes,
		iotcon_lite_resource_post_request_cb cb,
		void *user_data,
		iotcon_lite_resource_h *resource_handle);
int icl_ioty_lite_resource_destroy(iotcon_lite_resource_h resource);
int icl_ioty_lite_resource_update_attributes(iotcon_lite_resource_h resource,
		iotcon_attributes_h attributes);
int icl_ioty_lite_resource_notify(iotcon_lite_resource_h resource);

int icl_ioty_response_send(iotcon_response_h response);

#endif /*__IOTCON_INTERNAL_IOTIVITY_H__*/

