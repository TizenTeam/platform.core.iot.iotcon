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

#define DOOR_RESOURCE_URI "/door/1"
#define DOOR_RESOURCE_URI2 "/door/2"
#define DOOR_RESOURCE_TYPE "org.tizen.door"

/* Door Resource */
typedef struct _door_resource_s {
	bool attributes;
	char *uri_path;
	char *type;
	iotcon_resource_interfaces_h ifaces;
	uint8_t policies;
	iotcon_resource_h handle;
	iotcon_observers_h observers;
	iotcon_representation_h repr;
} door_resource_s;

static bool _resource_created;

static void _request_handler(iotcon_resource_h resource, iotcon_request_h request,
		void *user_data);

static int _set_door_resource(door_resource_s *door)
{
	int ret;

	door->attributes = false;

	door->uri_path = strdup(DOOR_RESOURCE_URI);
	if (NULL == door->uri_path) {
		ERR("strdup(%s) Fail", DOOR_RESOURCE_URI);
		return -1;
	}

	door->type = strdup(DOOR_RESOURCE_TYPE);
	if (NULL == door->type) {
		ERR("strdup(%s) Fail", DOOR_RESOURCE_TYPE);
		free(door->uri_path);
		return -1;
	}

	ret = iotcon_resource_interfaces_create(&door->ifaces);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_interfaces_create() Fail(%d)", ret);
		free(door->type);
		free(door->uri_path);
		return -1;
	}

	ret = iotcon_resource_interfaces_add(door->ifaces, IOTCON_INTERFACE_DEFAULT);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_interfaces_add() Fail(%d)", ret);
		iotcon_resource_interfaces_destroy(door->ifaces);
		free(door->type);
		free(door->uri_path);
		return -1;
	}

	door->policies = IOTCON_RESOURCE_DISCOVERABLE | IOTCON_RESOURCE_OBSERVABLE
		| IOTCON_RESOURCE_SECURE;

	ret = iotcon_observers_create(&door->observers);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_observers_create() Fail");
		iotcon_resource_interfaces_destroy(door->ifaces);
		free(door->type);
		free(door->uri_path);
		return -1;
	}

	return 0;
}

static void _free_door_resource(door_resource_s *door)
{
	iotcon_observers_destroy(door->observers);
	iotcon_resource_interfaces_destroy(door->ifaces);
	free(door->type);
	free(door->uri_path);
}

static void _check_door_attributes(door_resource_s door)
{
	if (false == door.attributes)
		INFO("[Door] closed.");
	else
		INFO("[Door] opened.");
}

static iotcon_resource_h _create_door_resource(char *uri_path, char *type,
		iotcon_resource_interfaces_h ifaces, uint8_t policies, void *user_data)
{
	int ret;
	iotcon_resource_h handle;
	iotcon_resource_types_h resource_types;

	ret = iotcon_resource_types_create(&resource_types);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_types_create() Fail(%d)", ret);
		return NULL;
	}

	ret = iotcon_resource_types_add(resource_types, type);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_types_add() Fail(%d)", ret);
		iotcon_resource_types_destroy(resource_types);
		return NULL;
	}

	/* register door resource */
	ret = iotcon_resource_create(uri_path, resource_types, ifaces, policies,
			_request_handler, user_data, &handle);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_create() Fail");
		iotcon_resource_types_destroy(resource_types);
		return NULL;
	}

	iotcon_resource_types_destroy(resource_types);

	return handle;
}

static int _send_response(iotcon_request_h request, iotcon_representation_h repr,
		iotcon_response_result_e result)
{
	int ret;
	iotcon_response_h response;

	ret = iotcon_response_create(request, &response);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_create() Fail(%d)", ret);
		return -1;
	}

	ret = iotcon_response_set_result(response, result);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_set_result() Fail(%d)", ret);
		iotcon_response_destroy(response);
		return -1;
	}

	ret = iotcon_response_set_representation(response, repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_set_representation() Fail(%d)", ret);
		iotcon_response_destroy(response);
		return -1;
	}

	/* send Representation to the client */
	ret = iotcon_response_send(response);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_send() Fail(%d)", ret);
		iotcon_response_destroy(response);
		return -1;
	}

	iotcon_response_destroy(response);

	return 0;
}

static iotcon_representation_h _get_door_representation(door_resource_s *door)
{
	int ret;
	iotcon_attributes_h attributes;
	iotcon_representation_h repr;

	/* create a door Representation */
	ret = iotcon_representation_create(&repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_create() Fail(%d)", ret);
		return NULL;
	}

	/* create a door attributes */
	ret = iotcon_attributes_create(&attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_create() Fail(%d)", ret);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	ret = iotcon_representation_set_uri_path(repr, door->uri_path);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_set_uri_path() Fail(%d)", ret);
		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	ret = iotcon_attributes_add_bool(attributes, "opened", door->attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_add_bool() Fail(%d)", ret);
		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	ret = iotcon_representation_set_attributes(repr, attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_set_attributes() Fail(%d)", ret);
		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	iotcon_attributes_destroy(attributes);

	return repr;
}

static int _request_handler_get(door_resource_s *door, iotcon_request_h request)
{
	int ret;
	iotcon_representation_h resp_repr;
	INFO("GET request");

	resp_repr = _get_door_representation(door);
	if (NULL == resp_repr) {
		ERR("_get_door_representation() Fail");
		return -1;
	}

	ret = _send_response(request, resp_repr, IOTCON_RESPONSE_OK);
	if (0 != ret) {
		ERR("_send_response() Fail(%d)", ret);
		iotcon_representation_destroy(resp_repr);
		return -1;
	}

	iotcon_representation_destroy(resp_repr);

	return 0;
}

static int _set_door_representation(door_resource_s *door,
		iotcon_representation_h repr)
{
	int ret;
	bool bval;
	iotcon_attributes_h attributes;

	ret = iotcon_representation_get_attributes(repr, &attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_get_attributes() Fail(%d)", ret);
		return -1;
	}

	ret = iotcon_attributes_get_bool(attributes, "opened", &bval);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_get_bool() Fail(%d)", ret);
		return -1;
	}

	door->attributes = bval;

	return 0;
}

static int _request_handler_put(door_resource_s *door, iotcon_request_h request)
{
	int ret;
	iotcon_representation_h req_repr, resp_repr;
	INFO("PUT request");

	ret = iotcon_request_get_representation(request, &req_repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_request_get_representation() Fail(%d)", ret);
		return -1;
	}

	ret = _set_door_representation(door, req_repr);
	if (0 != ret) {
		ERR("_set_door_representation() Fail(%d)", ret);
		return -1;
	}

	_check_door_attributes(*door);

	resp_repr = _get_door_representation(door);
	if (NULL == resp_repr) {
		ERR("_get_door_representation() Fail");
		return -1;
	}

	ret = _send_response(request, resp_repr, IOTCON_RESPONSE_OK);
	if (0 != ret) {
		ERR("_send_response() Fail(%d)", ret);
		iotcon_representation_destroy(resp_repr);
		return -1;
	}

	/* notify */
	ret = iotcon_resource_notify(door->handle, resp_repr, door->observers, IOTCON_QOS_HIGH);
	if (IOTCON_ERROR_NONE != ret)
		ERR("iotcon_resource_notify() Fail(%d)", ret);

	iotcon_representation_destroy(resp_repr);

	return 0;
}

static gboolean _door_attributes_changer(gpointer user_data)
{
	int ret;
	static int i = 0;
	iotcon_representation_h repr;
	door_resource_s *door = user_data;

	if ((5 == i++) || NULL == door->observers)
		return G_SOURCE_REMOVE;

	if (false == door->attributes) {
		door->attributes = true;
		INFO("[Door] closed -> opened");
	} else {
		door->attributes = false;
		INFO("[Door] opened -> closed");
	}

	INFO("NOTIFY!");

	repr = _get_door_representation(door);
	if (NULL == repr) {
		ERR("_get_door_representation() Fail");
		return G_SOURCE_REMOVE;
	}

	ret = iotcon_resource_notify(door->handle, repr, door->observers, IOTCON_QOS_HIGH);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_notify() Fail(%d)", ret);
		iotcon_representation_destroy(repr);
		return G_SOURCE_REMOVE;
	}

	iotcon_representation_destroy(repr);
	return G_SOURCE_CONTINUE;
}

static int _request_handler_post(door_resource_s *door, iotcon_request_h request)
{
	int ret;
	iotcon_attributes_h resp_attributes;
	iotcon_representation_h resp_repr = NULL;
	iotcon_resource_h new_door_handle;
	INFO("POST request");

	if (_resource_created) {
		ERR("Resource(%s) is already created", DOOR_RESOURCE_URI2);
		return -1;
	}

	new_door_handle = _create_door_resource(DOOR_RESOURCE_URI2, door->type,
			door->ifaces, IOTCON_RESOURCE_SECURE, door);
	if (NULL == new_door_handle) {
		ERR("_create_door_resource() Fail");
		return -1;
	}
	_resource_created = true;

	/* send information that new resource was created */
	ret = iotcon_representation_create(&resp_repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_create() Fail(%d)", ret);
		return -1;
	}

	ret = iotcon_attributes_create(&resp_attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_create() Fail(%d)", ret);
		iotcon_representation_destroy(resp_repr);
		return -1;
	}

	ret = iotcon_attributes_add_str(resp_attributes, "createduripath", DOOR_RESOURCE_URI2);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_add_str() Fail(%d)", ret);
		iotcon_attributes_destroy(resp_attributes);
		iotcon_representation_destroy(resp_repr);
		return -1;
	}

	ret = iotcon_representation_set_attributes(resp_repr, resp_attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_set_attributes() Fail(%d)", ret);
		iotcon_attributes_destroy(resp_attributes);
		iotcon_representation_destroy(resp_repr);
		return -1;
	}

	iotcon_attributes_destroy(resp_attributes);

	ret = _send_response(request, resp_repr, IOTCON_RESPONSE_RESOURCE_CREATED);
	if (0 != ret) {
		ERR("_send_response() Fail(%d)", ret);
		iotcon_representation_destroy(resp_repr);
		return -1;
	}

	iotcon_representation_destroy(resp_repr);

	return 0;
}

static int _request_handler_delete(iotcon_resource_h resource,
		iotcon_request_h request)
{
	int ret;
	INFO("DELETE request");

	ret = iotcon_resource_destroy(resource);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_destroy() Fail(%d)", ret);
		return -1;
	}

	ret = _send_response(request, NULL, IOTCON_RESPONSE_RESOURCE_DELETED);
	if (0 != ret) {
		ERR("_send_response() Fail(%d)", ret);
		return -1;
	}

	return 0;
}

static bool _query_cb(const char *key, const char *value, void *user_data)
{
	INFO("key : %s, value : %s", key, value);

	return IOTCON_FUNC_CONTINUE;
}

static void _request_handler(iotcon_resource_h resource, iotcon_request_h request,
		void *user_data)
{
	door_resource_s *door;
	iotcon_query_h query;
	int ret, observe_id;
	iotcon_request_type_e type;
	iotcon_observe_type_e observe_type;
	char *host_address;

	RET_IF(NULL == request);

	ret = iotcon_request_get_host_address(request, &host_address);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_request_get_host_address() Fail(%d)", ret);
		_send_response(request, NULL, IOTCON_RESPONSE_ERROR);
		return;
	}
	INFO("host_address : %s", host_address);

	ret = iotcon_request_get_query(request, &query);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_request_get_query() Fail(%d)", ret);
		_send_response(request, NULL, IOTCON_RESPONSE_ERROR);
		return;
	}
	if (query)
		iotcon_query_foreach(query, _query_cb, NULL);

	ret = iotcon_request_get_request_type(request, &type);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_request_get_request_type() Fail(%d)", ret);
		_send_response(request, NULL, IOTCON_RESPONSE_ERROR);
		return;
	}

	door = user_data;

	if (IOTCON_REQUEST_GET == type)
		ret = _request_handler_get(door, request);

	else if (IOTCON_REQUEST_PUT == type)
		ret = _request_handler_put(door, request);

	else if (IOTCON_REQUEST_POST == type)
		ret = _request_handler_post(door, request);

	else if (IOTCON_REQUEST_DELETE == type)
		ret = _request_handler_delete(resource, request);

	if (0 != ret) {
		_send_response(request, NULL, IOTCON_RESPONSE_ERROR);
		return;
	}

	ret = iotcon_request_get_observe_type(request, &observe_type);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_request_get_observe_type() Fail(%d)", ret);
		return;
	}

	if (IOTCON_OBSERVE_REGISTER == observe_type) {
		ret = iotcon_request_get_observe_id(request, &observe_id);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_request_get_observe_id() Fail(%d)", ret);
			return;
		}

		ret = iotcon_observers_add(door->observers, observe_id);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_observers_add() Fail(%d)", ret);
			return;
		}
	} else if (IOTCON_OBSERVE_DEREGISTER == observe_type) {
		ret = iotcon_request_get_observe_id(request, &observe_id);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_request_get_observe_id() Fail(%d)", ret);
			return;
		}
		ret = iotcon_observers_remove(door->observers, observe_id);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_observers_remove() Fail(%d)", ret);
			return;
		}
	}
}

static gboolean _presence_timer(gpointer user_data)
{
	static int i = 0;
	i++;
	if (i % 2)
		iotcon_stop_presence();
	else
		iotcon_start_presence(10);

	if (3 == i)
		return G_SOURCE_REMOVE;

	return G_SOURCE_CONTINUE;
}

int main(int argc, char **argv)
{
	FN_CALL;
	int ret;
	GMainLoop *loop;
	door_resource_s my_door = {0};

	loop = g_main_loop_new(NULL, FALSE);

	/* initialize iotcon */
	ret = iotcon_initialize("/usr/bin/iotcon-test-svr-db-server.dat");
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_initialize() Fail(%d)", ret);
		return -1;
	}

	/* set device name */
	ret = iotcon_set_device_name("iotcon-test-basic-server");
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_set_device_name() Fail(%d)", ret);
		iotcon_deinitialize();
		return -1;
	}

	/* set local door resource */
	ret = _set_door_resource(&my_door);
	if (0 != ret) {
		ERR("_set_door_resource() Fail");
		iotcon_deinitialize();
		return -1;
	}

	/* add resource options */
	ret = iotcon_resource_interfaces_add(my_door.ifaces, IOTCON_INTERFACE_BATCH);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_interfaces_add() Fail(%d)", ret);
		_free_door_resource(&my_door);
		iotcon_deinitialize();
		return -1;
	}

	/* add presence */
	g_timeout_add_seconds(10, _presence_timer, NULL);
	iotcon_start_presence(10);

	/* create new door resource */
	my_door.handle = _create_door_resource(my_door.uri_path, my_door.type, my_door.ifaces,
			my_door.policies, &my_door);
	if (NULL == my_door.handle) {
		ERR("_create_door_resource() Fail");
		_free_door_resource(&my_door);
		iotcon_deinitialize();
		return -1;
	}

	_check_door_attributes(my_door);

	/* add observe */
	g_timeout_add_seconds(5, _door_attributes_changer, &my_door);

	g_main_loop_run(loop);
	g_main_loop_unref(loop);

	iotcon_resource_destroy(my_door.handle);

	_free_door_resource(&my_door);

	/* deinitialize iotcon */
	iotcon_deinitialize();

	return 0;
}
