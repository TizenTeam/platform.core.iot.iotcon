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

#define DOOR_RESOURCE_TYPE "org.tizen.door"

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

static void _state_changed_cb(iotcon_remote_resource_h resource,
		iotcon_remote_resource_state_e state, void *user_data)
{
	INFO("Resource State is Changed");

	switch (state) {
	case IOTCON_REMOTE_RESOURCE_ALIVE:
		INFO(" --- ALIVE");
		break;
	case IOTCON_REMOTE_RESOURCE_LOST_SIGNAL:
		INFO(" --- LOST_SIGNAL");
		break;
	default:
		break;
	}
}

static void _representation_changed_cb(iotcon_remote_resource_h resource,
		iotcon_representation_h representation, void *user_data)
{
	int ret;
	bool opened;
	iotcon_attributes_h attributes;

	INFO("Resource is cached");

	ret = iotcon_representation_get_attributes(representation, &attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_get_attributes() Fail(%d)", ret);
		return;
	}

	ret = iotcon_attributes_get_bool(attributes, "opened", &opened);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_get_bool() Fail(%d)", ret);
		return;
	}

	switch (opened) {
	case true:
		INFO("door is opened");
		break;
	case false:
		INFO("door is closed");
		break;
	default:
		break;
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

	/* get the device unique id.
	 * this is unique per-server independent on how it was discovered. */
	ret = iotcon_remote_resource_get_device_id(resource, &resource_device_id);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get_device_id() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}
	DBG("[%s] resource device id : %s", resource_uri_path, resource_device_id);

	node = g_list_find_custom(device_id_list, resource_device_id, _device_id_compare);

	door_resource_device_id = strdup(resource_device_id);
	if (NULL == door_resource_device_id) {
		ERR("strdup(door_resource_device_id) Fail");
		return IOTCON_FUNC_CONTINUE;
	}

	device_id_list = g_list_append(device_id_list, door_resource_device_id);

	if (node) {
		DBG("This device(%s) is already found. skip !", door_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
	}

	/* get the resource host address */
	ret = iotcon_remote_resource_get_host_address(resource, &resource_host);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_get_host_address() Fail(%d)", ret);
		device_id_list = g_list_remove(device_id_list, door_resource_device_id);
		free(door_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
	}
	DBG("[%s] resource host : %s", resource_uri_path, resource_host);

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

	ret = iotcon_remote_resource_clone(resource, &cloned_resource);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_clone() Fail(%d)", ret);
		device_id_list = g_list_remove(device_id_list, door_resource_device_id);
		free(door_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
	}

	/* Start Monitoring */
	ret = iotcon_remote_resource_start_monitoring(cloned_resource, _state_changed_cb,
			NULL);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_start_monitoring() Fail(%d)", ret);
		device_id_list = g_list_remove(device_id_list, door_resource_device_id);
		free(door_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
	}

	/* Start Caching */
	ret = iotcon_remote_resource_start_caching(cloned_resource,
			_representation_changed_cb, NULL);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_start_caching() Fail(%d)", ret);
		iotcon_remote_resource_stop_monitoring(resource);
		device_id_list = g_list_remove(device_id_list, door_resource_device_id);
		free(door_resource_device_id);
		return IOTCON_FUNC_CONTINUE;
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
	iotcon_query_h query;
	iotcon_remote_resource_h resource;

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

	ret = iotcon_query_set_resource_type(query, DOOR_RESOURCE_TYPE);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_query_set_resource_type() Fail(%d)", ret);
		iotcon_query_destroy(query);
		return -1;
	}

	/* find door typed resources */
	ret = iotcon_find_resource(IOTCON_MULTICAST_ADDRESS, IOTCON_CONNECTIVITY_ALL,
			query, _found_resource, &resource);
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
