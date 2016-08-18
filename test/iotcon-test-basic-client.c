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
#include <tizen_type.h>

#include <iotcon.h>
#include "test.h"

static char *door_resource_device_id;
static GList *device_id_list;

#define DOOR_RESOURCE_URI_PREFIX "/door"
#define DOOR_RESOURCE_TYPE "org.tizen.door"

static void _on_response(iotcon_remote_resource_h resource, iotcon_error_e err,
		iotcon_request_type_e request_type, iotcon_response_h response, void *user_data);

static void _on_observe(iotcon_remote_resource_h resource, iotcon_error_e err,
		int sequence_number, iotcon_response_h response, void *user_data)
{
	int ret;
	bool opened;
	static int i = 0;
	iotcon_attributes_h attributes;
	iotcon_representation_h repr;
	iotcon_response_result_e response_result;

	RETM_IF(IOTCON_ERROR_NONE != err, "_on_observe error(%d)", err);

	ret = iotcon_response_get_result(response, &response_result);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_get_result() Fail(%d)", ret);
		return;
	}

	if (IOTCON_RESPONSE_OK != response_result) {
		ERR("_on_response_observe Response error(%d)", response_result);
		return;
	}

	ret = iotcon_response_get_representation(response, &repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_get_representation() Fail(%d)", ret);
		return;
	}

	ret = iotcon_representation_get_attributes(repr, &attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_get_attributes() Fail(%d)", ret);
		return;
	}

	ret = iotcon_attributes_get_bool(attributes, "opened", &opened);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_get_bool() Fail(%d)", ret);
		return;
	}

	INFO("notify_cb information");
	switch (opened) {
	case true:
		INFO("[Door] opened.");
		break;
	case false:
		INFO("[Door] closed.");
		break;
	default:
		break;
	}

	if (5 == i++) {
		iotcon_remote_resource_observe_deregister(resource);
		iotcon_remote_resource_destroy(resource);
	}
}

static void _on_response_delete(iotcon_remote_resource_h resource,
		iotcon_response_h response, void *user_data)
{
	int ret;
	iotcon_response_result_e response_result;

	ret = iotcon_response_get_result(response, &response_result);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_get_result() Fail(%d)", ret);
		return;
	}

	if (IOTCON_RESPONSE_OK != response_result
			&& IOTCON_RESPONSE_RESOURCE_DELETED != response_result) {
		ERR("_on_response_delete Response error(%d)", response_result);
		return;
	}
	INFO("DELETE request was successful");

	/* delete callback operations */

	iotcon_remote_resource_destroy(resource);
}

static void _on_response_post(iotcon_remote_resource_h resource,
		iotcon_response_h response, void *user_data)
{
	int ret;
	iotcon_attributes_h recv_attributes;
	char *host, *created_uri_path;
	iotcon_connectivity_type_e connectivity_type;
	iotcon_response_result_e response_result;
	iotcon_resource_types_h types = NULL;
	iotcon_resource_interfaces_h ifaces = NULL;
	iotcon_remote_resource_h new_door_resource;
	iotcon_representation_h recv_repr = NULL;

	ret = iotcon_response_get_result(response, &response_result);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_get_result() Fail(%d)", ret);
		return;
	}

	if (IOTCON_RESPONSE_OK != response_result
			&& IOTCON_RESPONSE_RESOURCE_CREATED != response_result) {
		ERR("_on_response_post Response error(%d)", response_result);
		return;
	}
	INFO("POST request was successful");

	ret = iotcon_response_get_representation(response, &recv_repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_get_representation() Fail(%d)", ret);
		return;
	}

	ret = iotcon_representation_get_attributes(recv_repr, &recv_attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_get_attributes() Fail(%d)", ret);
		return;
	}

	ret = iotcon_attributes_get_str(recv_attributes, "createduripath", &created_uri_path);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_get_str() Fail(%d)", ret);
		return;
	}
	DBG("New resource created : %s", created_uri_path);

	ret = iotcon_remote_resource_get_host_address(resource, &host);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get_host_address() Fail(%d)", ret);
		return;
	}

	ret = iotcon_remote_resource_get_connectivity_type(resource, &connectivity_type);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get_connectivity_type() Fail(%d)", ret);
		return;
	}

	ret = iotcon_remote_resource_get_types(resource, &types);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get_types() Fail(%d)", ret);
		return;
	}

	ret = iotcon_remote_resource_get_interfaces(resource, &ifaces);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get_interfaces() Fail(%d)", ret);
		return;
	}

	ret = iotcon_remote_resource_create(host, connectivity_type, created_uri_path,
			IOTCON_RESOURCE_NO_POLICY, types, ifaces, &new_door_resource);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_create() Fail(%d)", ret);
		return;
	}

	ret = iotcon_remote_resource_delete(new_door_resource, _on_response, NULL);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_delete() Fail(%d)", ret);
		iotcon_remote_resource_destroy(new_door_resource);
		return;
	}
}

static void _on_response_put(iotcon_remote_resource_h resource,
		iotcon_response_h response, void *user_data)
{
	int ret;
	iotcon_response_result_e response_result;
	iotcon_representation_h send_repr;

	ret = iotcon_response_get_result(response, &response_result);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_get_result() Fail(%d)", ret);
		return;
	}

	if (IOTCON_RESPONSE_OK != response_result) {
		ERR("_on_response_put Response error(%d)", response_result);
		return;
	}
	INFO("PUT request was successful");

	ret = iotcon_representation_create(&send_repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_create() Fail(%d)", ret);
		return;
	}

	/* send POST request */
	ret = iotcon_remote_resource_post(resource, send_repr, NULL, _on_response, NULL);
	if (IOTCON_ERROR_NONE != ret)
		ERR("iotcon_remote_resource_post() Fail(%d)", ret);

	iotcon_representation_destroy(send_repr);
}

static void _on_response_get(iotcon_remote_resource_h resource,
		iotcon_response_h response, void *user_data)
{
	int ret;
	bool opened = true;
	iotcon_response_result_e response_result;
	iotcon_representation_h send_repr;
	iotcon_representation_h recv_repr;
	iotcon_attributes_h send_attributes;
	iotcon_attributes_h recv_attributes = NULL;

	ret = iotcon_response_get_result(response, &response_result);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_get_result() Fail(%d)", ret);
		return;
	}

	if (IOTCON_RESPONSE_OK != response_result) {
		ERR("_on_response_get Response error(%d)", response_result);
		return;
	}

	/* get the resource host address */
	char *resource_host = NULL;
	iotcon_remote_resource_get_host_address(resource, &resource_host);
	INFO("resource host : %s", resource_host);

	ret = iotcon_response_get_representation(response, &recv_repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_get_representation() Fail(%d)", ret);
		return;
	}

	ret = iotcon_representation_get_attributes(recv_repr, &recv_attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_get_attributes() Fail(%d)", ret);
		return;
	}

	ret = iotcon_attributes_get_bool(recv_attributes, "opened", &opened);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_get_bool() Fail(%d)", ret);
		return;
	}

	ret = iotcon_representation_create(&send_repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_create() Fail(%d)", ret);
		return;
	}

	ret = iotcon_attributes_create(&send_attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_create() Fail(%d)", ret);
		iotcon_representation_destroy(send_repr);
		return;
	}

	ret = iotcon_attributes_add_bool(send_attributes, "opened", !opened);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_add_bool() Fail(%d)", ret);
		iotcon_attributes_destroy(send_attributes);
		iotcon_representation_destroy(send_repr);
		return;
	}

	ret = iotcon_representation_set_attributes(send_repr, send_attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_set_attributes() Fail(%d)", ret);
		iotcon_attributes_destroy(send_attributes);
		iotcon_representation_destroy(send_repr);
		return;
	}

	iotcon_attributes_destroy(send_attributes);

	/* send PUT request */
	ret = iotcon_remote_resource_put(resource, send_repr, NULL, _on_response, NULL);
	if (IOTCON_ERROR_NONE != ret)
		ERR("iotcon_remote_resource_put() Fail(%d)", ret);

	iotcon_representation_destroy(send_repr);
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

static void _presence_handler(iotcon_presence_h presence, iotcon_error_e err,
		iotcon_presence_response_h response, void *user_data)
{
	char *host_address;
	char *resource_type;
	int ret;
	iotcon_presence_result_e result;
	iotcon_presence_trigger_e trigger;
	iotcon_connectivity_type_e connectivity_type;

	RETM_IF(IOTCON_ERROR_NONE != err, "_presence_handler error(%d)", err);

	ret = iotcon_presence_response_get_result(response, &result);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_presence_response_get_result() Fail(%d)", ret);
		return;
	}

	if (IOTCON_PRESENCE_OK == result) {
		ret = iotcon_presence_response_get_trigger(response, &trigger);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_presence_response_get_trigger() Fail(%d)", ret);
			return;
		}
	}

	ret = iotcon_presence_response_get_host_address(response, &host_address);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_presence_response_get_host_address() Fail(%d)", ret);
		return;
	}

	ret = iotcon_presence_response_get_connectivity_type(response, &connectivity_type);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_presence_response_get_connectivity_type() Fail(%d)", ret);
		return;
	}

	ret = iotcon_presence_response_get_resource_type(response, &resource_type);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_presence_response_get_resource_type() Fail(%d)", ret);
		return;
	}

	INFO("_presence_handler");
	INFO("result : %d", result);
	if (IOTCON_PRESENCE_OK == result)
		INFO("trigger : %d", trigger);
	INFO("host_address : %s", host_address);
	INFO("resource_type : %s", resource_type);
}

static int _device_id_compare(const void *a, const void *b)
{
	return strcmp(a, b);
}

static void _on_response(iotcon_remote_resource_h resource, iotcon_error_e err,
		iotcon_request_type_e request_type, iotcon_response_h response, void *user_data)
{
	RETM_IF(IOTCON_ERROR_NONE != err, "_on_response error(%d)", err);
	INFO("request(%d) was successful", request_type);

	switch (request_type) {
	case IOTCON_REQUEST_GET:
		_on_response_get(resource, response, user_data);
		break;
	case IOTCON_REQUEST_PUT:
		_on_response_put(resource, response, user_data);
		break;
	case IOTCON_REQUEST_POST:
		_on_response_post(resource, response, user_data);
		break;
	case IOTCON_REQUEST_DELETE:
		_on_response_delete(resource, response, user_data);
		break;
	default:
		ERR("Invalid request type (%d)", request_type);
		return;
	}
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
	iotcon_presence_h presence_handle;
	iotcon_resource_types_h resource_types;
	iotcon_resource_interfaces_h resource_interfaces;
	iotcon_connectivity_type_e connectivity_type;
	iotcon_remote_resource_h resource_clone = NULL;

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

	/* get the device unique id.
	 * this is unique per-server independent on how it was discovered. */
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

	if (node && TEST_STR_EQUAL == strncmp(DOOR_RESOURCE_URI_PREFIX, resource_uri_path,
				strlen(DOOR_RESOURCE_URI_PREFIX))) {
		DBG("uri_path \"%s\" already found. skip !", resource_uri_path);
		return IOTCON_FUNC_CONTINUE;
	}

	door_resource_device_id = strdup(resource_device_id);
	if (NULL == door_resource_device_id) {
		ERR("strdup(door_resource_device_id) Fail");
		return IOTCON_FUNC_CONTINUE;
	}

	device_id_list = g_list_append(device_id_list, door_resource_device_id);

	/* get the resource host address */
	ret = iotcon_remote_resource_get_host_address(resource, &resource_host);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get_host_address() Fail(%d)", ret);
		device_id_list = g_list_remove(device_id_list, door_resource_device_id);
		free(door_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
	}
	DBG("[%s] resource host : %s", resource_uri_path, resource_host);

	ret = iotcon_remote_resource_get_connectivity_type(resource, &connectivity_type);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get_connectivity_type() Fail(%d)", ret);
		device_id_list = g_list_remove(device_id_list, door_resource_device_id);
		free(door_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
	}

	/* get the resource interfaces */
	ret = iotcon_remote_resource_get_interfaces(resource, &resource_interfaces);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get_interfaces() Fail(%d)", ret);
		device_id_list = g_list_remove(device_id_list, door_resource_device_id);
		free(door_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_resource_interfaces_foreach(resource_interfaces, _get_res_iface_cb,
			resource_uri_path);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_interfaces_foreach() Fail(%d)", ret);
		device_id_list = g_list_remove(device_id_list, door_resource_device_id);
		free(door_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
	}

	/* get the resource types */
	ret = iotcon_remote_resource_get_types(resource, &resource_types);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get_types() Fail(%d)", ret);
		device_id_list = g_list_remove(device_id_list, door_resource_device_id);
		free(door_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_resource_types_foreach(resource_types, _get_res_type_cb,
			resource_uri_path);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_types_foreach() Fail(%d)", ret);
		device_id_list = g_list_remove(device_id_list, door_resource_device_id);
		free(door_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_add_presence_cb(resource_host, connectivity_type, DOOR_RESOURCE_TYPE,
			_presence_handler, NULL, &presence_handle);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_add_presence_cb() Fail(%d)", ret);
		device_id_list = g_list_remove(device_id_list, door_resource_device_id);
		free(door_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
	}

	if (TEST_STR_EQUAL == strncmp(DOOR_RESOURCE_URI_PREFIX, resource_uri_path,
				strlen(DOOR_RESOURCE_URI_PREFIX))) {
		iotcon_query_h query;

		ret = iotcon_remote_resource_clone(resource, &resource_clone);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_remote_resource_clone() Fail(%d)", ret);
			device_id_list = g_list_remove(device_id_list, door_resource_device_id);
			free(door_resource_device_id);
			return IOTCON_FUNC_CONTINUE;
		}

		ret = iotcon_remote_resource_observe_register(resource_clone,
				IOTCON_OBSERVE_IGNORE_OUT_OF_ORDER, NULL, _on_observe, NULL);

		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_remote_resource_observe_register() Fail(%d)", ret);
			device_id_list = g_list_remove(device_id_list, door_resource_device_id);
			free(door_resource_device_id);
			return IOTCON_FUNC_CONTINUE;
		}

		ret = iotcon_query_create(&query);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_query_create() Fail(%d)", ret);
			device_id_list = g_list_remove(device_id_list, door_resource_device_id);
			free(door_resource_device_id);
			return IOTCON_FUNC_CONTINUE;
		}

		ret = iotcon_query_add(query, "query_key", "query_value");
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_query_add() Fail(%d)", ret);
			iotcon_query_destroy(query);
			device_id_list = g_list_remove(device_id_list, door_resource_device_id);
			free(door_resource_device_id);
			return IOTCON_FUNC_CONTINUE;
		}

		/* send GET Request */
		ret = iotcon_remote_resource_get(resource_clone, query, _on_response, NULL);
		if (IOTCON_ERROR_NONE != ret)
			ERR("iotcon_remote_resource_get() Fail(%d)", ret);

		iotcon_query_destroy(query);
	}

	device_id_list = g_list_remove(device_id_list, door_resource_device_id);
	free(door_resource_device_id);

	return IOTCON_FUNC_CONTINUE;
}

int main(int argc, char **argv)
{
	FN_CALL;
	int ret;
	GMainLoop *loop;

	loop = g_main_loop_new(NULL, FALSE);

	/* initialize iotcon */
	ret = iotcon_initialize("/usr/bin/iotcon-test-svr-db-client.dat");
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_initialize() Fail(%d)", ret);
		return -1;
	}

	/* find door typed resources */
	ret = iotcon_find_resource(IOTCON_MULTICAST_ADDRESS, IOTCON_CONNECTIVITY_ALL,
			DOOR_RESOURCE_TYPE, true, _found_resource, NULL);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_find_resource() Fail(%d)", ret);
		iotcon_deinitialize();
		return -1;
	}

	g_main_loop_run(loop);
	g_main_loop_unref(loop);

	g_list_free_full(device_id_list, free);

	/* deinitialize iotcon */
	iotcon_deinitialize();

	return 0;
}
