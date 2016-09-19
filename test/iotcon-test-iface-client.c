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
#include <glib.h>
#include <iotcon.h>

#include "test.h"

static char *room_resource_device_id;
static GList *device_id_list;

#define ROOM_RESOURCE_TYPE "org.tizen.room"
#define ROOM_RESOURCE_URI_PREFIX "/room"
#define LIGHT_RESOURCE_URI_PREFIX "/light"
#define FAN_RESOURCE_URI_PREFIX "/fan"

static bool _get_int_list_cb(int pos, const int value, void *user_data)
{
	DBG("%d", value);

	return IOTCON_FUNC_CONTINUE;
}

static void _print_repr(iotcon_representation_h recv_repr)
{
	int i, ret, int_val;
	bool is_null, bool_val;
	char *uri_path, *str_val;
	iotcon_list_h list_val;
	iotcon_representation_h child_repr;
	iotcon_attributes_h recv_attributes, child_attributes;
	unsigned int key_count, children_count;

	INFO("GET request was successful");

	DBG("[ parent representation ]");
	ret = iotcon_representation_get_uri_path(recv_repr, &uri_path);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_get_uri_path() Fail(%d)", ret);
		return;
	}
	DBG("uri_path : %s", uri_path);

	ret = iotcon_representation_get_attributes(recv_repr, &recv_attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_get_attributes() Fail(%d)", ret);
		return;
	}

	ret = iotcon_attributes_get_keys_count(recv_attributes, &key_count);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_get_keys_count() Fail(%d)", ret);
		return;
	}

	if (key_count) {
		ret = iotcon_attributes_get_str(recv_attributes, "name", &str_val);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_attributes_get_str() Fail(%d)", ret);
			return;
		}
		DBG("name : %s", str_val);

		ret = iotcon_attributes_get_list(recv_attributes, "today_temp", &list_val);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_attributes_get_list() Fail(%d)", ret);
			return;
		}

		DBG("today's temperature :");
		ret = iotcon_list_foreach_int(list_val, _get_int_list_cb, NULL);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_list_foreach_int() Fail(%d)", ret);
			return;
		}

		ret = iotcon_attributes_is_null(recv_attributes, "null value", &is_null);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_attributes_is_null() Fail(%d)", ret);
			return;
		}

		if (is_null)
			DBG("null value is null");
	}

	ret = iotcon_representation_get_child_count(recv_repr, &children_count);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_get_child_count() Fail(%d)", ret);
		return;
	}

	for (i = 0; i < children_count; i++) {
		DBG("[ child representation ]");

		ret = iotcon_representation_get_nth_child(recv_repr, i, &child_repr);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_representation_get_nth_child(%d) Fail(%d)", i, ret);
			continue;
		}

		ret = iotcon_representation_get_uri_path(child_repr, &uri_path);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_representation_get_uri_path() Fail(%d)", ret);
			continue;
		}
		DBG("uri_path : %s", uri_path);

		ret = iotcon_representation_get_attributes(child_repr, &child_attributes);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_representation_get_attributes() Fail(%d)", ret);
			continue;
		}

		if (TEST_STR_EQUAL == strncmp(LIGHT_RESOURCE_URI_PREFIX, uri_path,
					strlen(LIGHT_RESOURCE_URI_PREFIX))) {
			ret = iotcon_attributes_get_keys_count(child_attributes, &key_count);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_attributes_get_keys_count() Fail(%d)", ret);
				continue;
			}

			if (key_count) {
				ret = iotcon_attributes_get_int(child_attributes, "brightness", &int_val);
				if (IOTCON_ERROR_NONE != ret) {
					ERR("iotcon_attributes_get_int() Fail(%d)", ret);
					continue;
				}
				DBG("brightness : %d", int_val);
			}
		} else if (TEST_STR_EQUAL == strncmp(FAN_RESOURCE_URI_PREFIX, uri_path,
					strlen(FAN_RESOURCE_URI_PREFIX))) {
			ret = iotcon_attributes_get_keys_count(child_attributes, &key_count);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_attributes_get_keys_count() Fail(%d)", ret);
				continue;
			}
			if (key_count) {
				ret = iotcon_attributes_get_bool(child_attributes, "state", &bool_val);
				if (IOTCON_ERROR_NONE != ret) {
					ERR("iotcon_attributes_get_bool() Fail(%d)", ret);
					continue;
				}
				DBG("state : %d", bool_val);
			}
		}
	}
}

static void _on_get_2nd(iotcon_remote_resource_h resource,
		iotcon_error_e err,
		iotcon_request_type_e request_type,
		iotcon_response_h response,
		void *user_data)
{
	int ret;
	iotcon_response_result_e response_result;
	iotcon_representation_h recv_repr = NULL;

	RETM_IF(IOTCON_ERROR_NONE != err, "Invalid err(%d)", err);

	ret = iotcon_response_get_result(response, &response_result);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_get_result() Fail(%d)", ret);
		return;
	}

	ret = iotcon_response_get_representation(response, &recv_repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_get_representation() Fail(%d)", ret);
		return;
	}

	if (IOTCON_RESPONSE_OK == response_result)
		_print_repr(recv_repr);
	else
		ERR("Invalid result(%d)", response_result);

	iotcon_remote_resource_destroy(resource);
}

static void _on_response_1st(iotcon_remote_resource_h resource,
		iotcon_error_e err,
		iotcon_request_type_e request_type,
		iotcon_response_h response,
		void *user_data)
{
	int ret;
	iotcon_response_result_e response_result;
	iotcon_query_h query_params;
	iotcon_representation_h recv_repr = NULL;

	RETM_IF(IOTCON_ERROR_NONE != err, "Invalid err(%d)", err);

	ret = iotcon_response_get_result(response, &response_result);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_get_result() Fail(%d)", ret);
		return;
	}

	ret = iotcon_response_get_representation(response, &recv_repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_get_representation() Fail(%d)", ret);
		return;
	}

	if (IOTCON_RESPONSE_OK == response_result)
		_print_repr(recv_repr);
	else
		ERR("Invalid result(%d)", response_result);

	ret = iotcon_query_create(&query_params);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_query_create() Fail(%d)", ret);
		return;
	}

	ret = iotcon_query_set_interface(query_params, IOTCON_INTERFACE_BATCH);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_query_set_interface() Fail(%d)", ret);
		iotcon_query_destroy(query_params);
		return;
	}

	/* send GET request again with BATCH interface */
	ret = iotcon_remote_resource_get(resource, query_params, _on_get_2nd, NULL);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get() Fail(%d)", ret);
		iotcon_query_destroy(query_params);
		return;
	}

	iotcon_query_destroy(query_params);
}

static bool _get_res_iface_cb(const char *string, void *user_data)
{
	char *resource_uri_path = user_data;

	DBG("[%s] resource interface : %s", resource_uri_path, string);

	return IOTCON_FUNC_CONTINUE;
}

static bool _get_res_type_cb(const char *string, void *user_data)
{
	char *resource_uri_path = user_data;

	DBG("[%s] resource type : %s", resource_uri_path, string);

	return IOTCON_FUNC_CONTINUE;
}

static int _device_id_compare(const void *a, const void *b)
{
	return strcmp(a, b);
}

static bool _found_resource(iotcon_remote_resource_h resource, iotcon_error_e result,
		void *user_data)
{
	int ret;
	GList *node;
	char *resource_host;
	char *resource_uri_path;
	char *resource_device_id;
	char *resource_device_name;
	iotcon_resource_interfaces_h resource_interfaces;
	iotcon_resource_types_h resource_types;
	iotcon_remote_resource_h cloned_resource;

	RETVM_IF(IOTCON_ERROR_NONE != result, IOTCON_FUNC_STOP, "Invalid result(%d)", result);

	if (NULL == resource)
		return IOTCON_FUNC_CONTINUE;

	INFO("===== resource found =====");

	/* get the resource URI */
	ret = iotcon_remote_resource_get_uri_path(resource, &resource_uri_path);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get_uri_path() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}

	/* get the resource device id */
	ret = iotcon_remote_resource_get_device_id(resource, &resource_device_id);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get_device_id() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}
	DBG("[%s] resource device id : %s", resource_uri_path, resource_device_id);

	ret = iotcon_remote_resource_get_device_name(resource, &resource_device_name);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get_device_name() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}
	DBG("[%s] resource device name : %s", resource_uri_path, resource_device_name);

	node = g_list_find_custom(device_id_list, resource_device_id, _device_id_compare);

	if (node && TEST_STR_EQUAL == strncmp(ROOM_RESOURCE_URI_PREFIX, resource_uri_path,
				strlen(ROOM_RESOURCE_URI_PREFIX))) {
		DBG("uri_path \"%s\" already found. skip !", resource_uri_path);
		return IOTCON_FUNC_CONTINUE;
	}

	room_resource_device_id = strdup(resource_device_id);
	if (NULL == room_resource_device_id) {
		ERR("strdup(room_resource_device_id) Fail");
		return IOTCON_FUNC_CONTINUE;
	}

	device_id_list = g_list_append(device_id_list, room_resource_device_id);

	/* get the resource host address */
	ret = iotcon_remote_resource_get_host_address(resource, &resource_host);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get_host_address() Fail(%d)", ret);
		device_id_list = g_list_remove(device_id_list, room_resource_device_id);
		free(room_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
	}
	DBG("[%s] resource host : %s", resource_uri_path, resource_host);

	/* get the resource interfaces */
	ret = iotcon_remote_resource_get_interfaces(resource, &resource_interfaces);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get_interfaces() Fail(%d)", ret);
		device_id_list = g_list_remove(device_id_list, room_resource_device_id);
		free(room_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_resource_interfaces_foreach(resource_interfaces, _get_res_iface_cb,
			resource_uri_path);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_interfaces_foreach() Fail(%d)", ret);
		device_id_list = g_list_remove(device_id_list, room_resource_device_id);
		free(room_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
	}

	/* get the resource types */
	ret = iotcon_remote_resource_get_types(resource, &resource_types);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get_types() Fail(%d)", ret);
		device_id_list = g_list_remove(device_id_list, room_resource_device_id);
		free(room_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
	}
	ret = iotcon_resource_types_foreach(resource_types, _get_res_type_cb,
			resource_uri_path);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_types_foreach() Fail(%d)", ret);
		device_id_list = g_list_remove(device_id_list, room_resource_device_id);
		free(room_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
	}

	if (TEST_STR_EQUAL == strncmp(ROOM_RESOURCE_URI_PREFIX, resource_uri_path,
				strlen(ROOM_RESOURCE_URI_PREFIX))) {
		ret = iotcon_remote_resource_clone(resource, &cloned_resource);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_remote_resource_clone() Fail(%d)", ret);
			device_id_list = g_list_remove(device_id_list, room_resource_device_id);
			free(room_resource_device_id);
			return IOTCON_FUNC_CONTINUE;
		}

		/* send GET request */
		ret = iotcon_remote_resource_get(cloned_resource, NULL, _on_response_1st, NULL);
		if (IOTCON_ERROR_NONE != ret)
			ERR("iotcon_remote_resource_get() Fail(%d)", ret);
	}

	device_id_list = g_list_remove(device_id_list, room_resource_device_id);
	free(room_resource_device_id);

	return IOTCON_FUNC_CONTINUE;
}

int main(int argc, char **argv)
{
	FN_CALL;
	int ret;
	GMainLoop *loop;
	iotcon_query_h query;

	loop = g_main_loop_new(NULL, FALSE);

	/* initialize iotcon */
	ret = iotcon_initialize("/usr/bin/iotcon-test-svr-db-client.dat");
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_initialize() Fail(%d)", ret);
		return -1;
	}

	ret = iotcon_query_create(&query);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_query_create() Fail(%d)", ret);
		return -1;
	}

	ret = iotcon_query_set_resource_type(query, ROOM_RESOURCE_TYPE);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_query_set_resource_type() Fail(%d)", ret);
		iotcon_query_destroy(query);
		return -1;
	}

	/* find room typed resources */
	ret = iotcon_find_resource(IOTCON_MULTICAST_ADDRESS, IOTCON_CONNECTIVITY_ALL
			| IOTCON_CONNECTIVITY_PREFER_UDP, query, _found_resource, NULL);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_find_resource() Fail(%d)", ret);
		iotcon_query_destroy(query);
		iotcon_deinitialize();
		return -1;
	}

	iotcon_query_destroy(query);

	g_main_loop_run(loop);
	g_main_loop_unref(loop);

	g_list_free_full(device_id_list, free);

	/* deinitialize iotcon */
	iotcon_deinitialize();

	return 0;
}
