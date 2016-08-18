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

#define ROOM_RESOURCE_URI "/room/1"
#define ROOM_RESOURCE_TYPE "org.tizen.room"
#define LIGHT_RESOURCE_URI "/light/1"
#define LIGHT_RESOURCE_TYPE "org.tizen.light"
#define FAN_RESOURCE_URI "/fan/1"
#define FAN_RESOURCE_TYPE "org.tizen.fan"

/* Light Resource */
typedef struct _light_resource_s {
	int brightness;
	char *uri_path;
	char *type;
	iotcon_resource_interfaces_h ifaces;
	uint8_t policies;
	iotcon_resource_h handle;
} light_resource_s;

/* Fan Resource */
typedef struct _fan_resource_s {
	bool state;
	char *uri_path;
	char *type;
	iotcon_resource_interfaces_h ifaces;
	uint8_t policies;
	iotcon_resource_h handle;
} fan_resource_s;

/* Room Resource */
typedef struct _room_resource_s {
	char *name;
	int today_temp[5];
	char *uri_path;
	char *type;
	iotcon_resource_interfaces_h ifaces;
	uint8_t policies;
	iotcon_resource_h handle;
	light_resource_s *child_light;
	fan_resource_s *child_fan;
} room_resource_s;

static int _set_room_resource(room_resource_s *room)
{
	int ret;

	room->name = strdup("Michael's Room");
	if (NULL == room->name) {
		ERR("strdup() Fail");
		return -1;
	}

	room->today_temp[0] = 13;
	room->today_temp[1] = 19;
	room->today_temp[2] = 24;
	room->today_temp[3] = 21;
	room->today_temp[4] = 14;

	room->uri_path = strdup(ROOM_RESOURCE_URI);
	if (NULL == room->uri_path) {
		ERR("strdup(%s) Fail", ROOM_RESOURCE_URI);
		free(room->name);
		return -1;
	}

	room->type = strdup(ROOM_RESOURCE_TYPE);
	if (NULL == room->type) {
		ERR("strdup(%s) Fail", ROOM_RESOURCE_TYPE);
		free(room->uri_path);
		free(room->name);
		return -1;
	}

	ret = iotcon_resource_interfaces_create(&room->ifaces);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_interfaces_create() Fail(%d)", ret);
		free(room->type);
		free(room->uri_path);
		free(room->name);
		return -1;
	}

	ret = iotcon_resource_interfaces_add(room->ifaces, IOTCON_INTERFACE_DEFAULT);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_interfaces_add() Fail(%d)", ret);
		iotcon_resource_interfaces_destroy(room->ifaces);
		free(room->type);
		free(room->uri_path);
		free(room->name);
		return -1;
	}

	ret = iotcon_resource_interfaces_add(room->ifaces, IOTCON_INTERFACE_BATCH);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_interfaces_add() Fail(%d)", ret);
		iotcon_resource_interfaces_destroy(room->ifaces);
		free(room->type);
		free(room->uri_path);
		free(room->name);
		return -1;
	}

	room->policies = IOTCON_RESOURCE_DISCOVERABLE | IOTCON_RESOURCE_OBSERVABLE
		| IOTCON_RESOURCE_SECURE;

	return 0;
}

static void _free_room_resource(room_resource_s *room)
{
	iotcon_resource_interfaces_destroy(room->ifaces);
	free(room->type);
	free(room->uri_path);
	free(room->name);
}

static int _set_light_resource(light_resource_s *light)
{
	int ret;

	light->brightness = 50;

	light->uri_path = strdup(LIGHT_RESOURCE_URI);
	if (NULL == light->uri_path) {
		ERR("strdup(%s) Fail", LIGHT_RESOURCE_URI);
		return -1;
	}

	light->type = strdup(LIGHT_RESOURCE_TYPE);
	if (NULL == light->type) {
		ERR("strdup(%s) Fail", LIGHT_RESOURCE_TYPE);
		free(light->uri_path);
		return -1;
	}

	ret = iotcon_resource_interfaces_create(&light->ifaces);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_interfaces_create() Fail(%d)", ret);
		free(light->type);
		free(light->uri_path);
		return -1;
	}

	ret = iotcon_resource_interfaces_add(light->ifaces, IOTCON_INTERFACE_DEFAULT);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_interfaces_add() Fail(%d)", ret);
		iotcon_resource_interfaces_destroy(light->ifaces);
		free(light->type);
		free(light->uri_path);
		return -1;
	}

	light->policies = IOTCON_RESOURCE_SECURE;

	return 0;
}

static void _free_light_resource(light_resource_s *light)
{
	iotcon_resource_interfaces_destroy(light->ifaces);
	free(light->type);
	free(light->uri_path);
}

static int _set_fan_resource(fan_resource_s *fan)
{
	int ret;

	fan->state = false;

	fan->uri_path = strdup(FAN_RESOURCE_URI);
	if (NULL == fan->uri_path) {
		ERR("strdup(%s) Fail", FAN_RESOURCE_URI);
		return -1;
	}

	fan->type = strdup(FAN_RESOURCE_TYPE);
	if (NULL == fan->type) {
		ERR("strdup(%s) Fail", FAN_RESOURCE_TYPE);
		free(fan->uri_path);
		return -1;
	}

	ret = iotcon_resource_interfaces_create(&fan->ifaces);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_interfaces_create() Fail(%d)", ret);
		free(fan->type);
		free(fan->uri_path);
		return -1;
	}

	ret = iotcon_resource_interfaces_add(fan->ifaces, IOTCON_INTERFACE_DEFAULT);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_interfaces_add() Fail(%d)", ret);
		iotcon_resource_interfaces_destroy(fan->ifaces);
		free(fan->type);
		free(fan->uri_path);
		return -1;
	}

	fan->policies = IOTCON_RESOURCE_SECURE;

	return 0;
}

static void _free_fan_resource(fan_resource_s *fan)
{
	iotcon_resource_interfaces_destroy(fan->ifaces);
	free(fan->type);
	free(fan->uri_path);
}

static iotcon_resource_h _create_resource(char *uri_path,
		char *type,
		iotcon_resource_interfaces_h ifaces,
		uint8_t policies,
		iotcon_request_handler_cb cb,
		void *user_data)
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

	ret = iotcon_resource_create(uri_path, resource_types, ifaces, policies, cb,
			user_data, &handle);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_create() Fail(%d)", ret);
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

	ret = iotcon_response_set_representation(response, repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_set_representation() Fail(%d)", ret);
		iotcon_response_destroy(response);
		return -1;
	}

	ret = iotcon_response_set_result(response, result);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_response_set_result() Fail(%d)", ret);
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

static iotcon_representation_h _get_light_representation(light_resource_s *light)
{
	int ret;
	iotcon_attributes_h attributes;
	iotcon_representation_h repr;

	/* create a light Representation */
	ret = iotcon_representation_create(&repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_create() Fail(%d)", ret);
		return NULL;
	}

	ret = iotcon_representation_set_uri_path(repr, light->uri_path);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_set_uri_path() Fail(%d)", ret);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	/* create a light attributes */
	ret = iotcon_attributes_create(&attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_create() Fail(%d)", ret);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	ret = iotcon_attributes_add_int(attributes, "brightness", light->brightness);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_add_int() Fail(%d)", ret);
		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	/* Set a light attributes into light Representation */
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

static int _light_request_handler_get(light_resource_s *light,
		iotcon_request_h request)
{
	int ret;
	iotcon_representation_h repr;

	INFO("GET request - Light");

	repr = _get_light_representation(light);
	if (NULL == repr) {
		ERR("_get_light_representation() Fail");
		return -1;
	}

	ret = _send_response(request, repr, IOTCON_RESPONSE_OK);
	if (0 != ret) {
		ERR("_send_response() Fail(%d)", ret);
		iotcon_representation_destroy(repr);
		return -1;
	}

	iotcon_representation_destroy(repr);

	return 0;
}


static iotcon_representation_h _get_fan_representation(fan_resource_s *fan)
{
	int ret;
	iotcon_attributes_h attributes;
	iotcon_representation_h repr;

	/* create a fan Representation */
	ret = iotcon_representation_create(&repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_create() Fail(%d)", ret);
		return NULL;
	}

	ret = iotcon_representation_set_uri_path(repr, fan->uri_path);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_set_uri_path() Fail(%d)", ret);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	/* create a fan attributes */
	ret = iotcon_attributes_create(&attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_create() Fail(%d)", ret);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	ret = iotcon_attributes_add_bool(attributes, "state", fan->state);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_add_bool() Fail(%d)", ret);
		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	/* Set a light attributes into light Representation */
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

static int _fan_request_handler_get(fan_resource_s *fan, iotcon_request_h request)
{
	int ret;
	iotcon_representation_h repr;

	INFO("GET request - Fan");

	repr = _get_fan_representation(fan);
	if (NULL == repr) {
		ERR("_get_fan_representation() Fail");
		return -1;
	}

	ret = _send_response(request, repr, IOTCON_RESPONSE_OK);
	if (0 != ret) {
		ERR("_send_response() Fail(%d)", ret);
		iotcon_representation_destroy(repr);
		return -1;
	}

	iotcon_representation_destroy(repr);

	return 0;
}

static iotcon_representation_h _get_room_representation(room_resource_s *room)
{
	int ret;
	iotcon_attributes_h attributes;
	iotcon_list_h today_temp;
	iotcon_representation_h repr, light_repr, fan_repr;

	/* create a room Representation */
	ret = iotcon_representation_create(&repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_create() Fail(%d)", ret);
		return NULL;
	}

	ret = iotcon_representation_set_uri_path(repr, room->uri_path);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_set_uri_path() Fail(%d)", ret);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	/* create a room attributes */
	ret = iotcon_attributes_create(&attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_create() Fail(%d)", ret);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	ret = iotcon_attributes_add_str(attributes, "name", room->name);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_add_str() Fail(%d)", ret);
		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	/* set null */
	ret = iotcon_attributes_add_null(attributes, "null value");
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_add_null() Fail(%d)", ret);
		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	ret = iotcon_list_create(IOTCON_TYPE_INT, &today_temp);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_list_create() Fail(%d)", ret);
		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	ret = iotcon_list_add_int(today_temp, room->today_temp[0], -1);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_list_add_int() Fail(%d)", ret);
		iotcon_list_destroy(today_temp);
		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	ret = iotcon_list_add_int(today_temp, room->today_temp[1], -1);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_list_add_int() Fail(%d)", ret);
		iotcon_list_destroy(today_temp);
		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	ret = iotcon_list_add_int(today_temp, room->today_temp[2], -1);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_list_add_int() Fail(%d)", ret);
		iotcon_list_destroy(today_temp);
		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	ret = iotcon_list_add_int(today_temp, room->today_temp[3], -1);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_list_add_int() Fail(%d)", ret);
		iotcon_list_destroy(today_temp);
		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	ret = iotcon_list_add_int(today_temp, room->today_temp[4], -1);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_list_add_int() Fail(%d)", ret);
		iotcon_list_destroy(today_temp);
		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	ret = iotcon_attributes_add_list(attributes, "today_temp", today_temp);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_add_list() Fail(%d)", ret);
		iotcon_list_destroy(today_temp);
		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	iotcon_list_destroy(today_temp);

	/* Set a room attributes into room Representation */
	ret = iotcon_representation_set_attributes(repr, attributes);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_set_attributes() Fail(%d)", ret);
		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	iotcon_attributes_destroy(attributes);

	light_repr = _get_light_representation(room->child_light);
	if (NULL == light_repr) {
		ERR("_get_light_representation() fail");
		iotcon_representation_destroy(repr);
		return NULL;
	}

	ret = iotcon_representation_add_child(repr, light_repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_add_child() Fail(%d)", ret);
		iotcon_representation_destroy(light_repr);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	iotcon_representation_destroy(light_repr);

	fan_repr = _get_fan_representation(room->child_fan);
	if (NULL == fan_repr) {
		ERR("_get_fan_representation() fail");
		iotcon_representation_destroy(repr);
		return NULL;
	}

	ret = iotcon_representation_add_child(repr, fan_repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_add_child() Fail(%d)", ret);
		iotcon_representation_destroy(fan_repr);
		iotcon_representation_destroy(repr);
		return NULL;
	}

	iotcon_representation_destroy(fan_repr);

	return repr;
}

static int _room_request_handler_get(room_resource_s *room,
		iotcon_request_h request)
{
	int ret;
	iotcon_query_h query;
	iotcon_representation_h repr;

	INFO("GET request - Room");

	repr = _get_room_representation(room);
	if (NULL == repr) {
		ERR("_get_room_representation() Fail");
		return -1;
	}

	ret = iotcon_request_get_query(request, &query);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_request_get_query() Fail(%d)", ret);
		iotcon_representation_destroy(repr);
		return -1;
	}

	ret = _send_response(request, repr, IOTCON_RESPONSE_OK);
	if (0 != ret) {
		ERR("_send_response() Fail(%d)", ret);
		iotcon_representation_destroy(repr);
		return -1;
	}

	iotcon_representation_destroy(repr);

	return 0;
}

static void _light_request_handler(iotcon_resource_h resource,
		iotcon_request_h request, void *user_data)
{
	int ret;
	iotcon_request_type_e type;
	light_resource_s *light = user_data;

	RET_IF(NULL == request);

	ret = iotcon_request_get_request_type(request, &type);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_request_get_request_type() Fail(%d)", ret);
		_send_response(request, NULL, IOTCON_RESPONSE_ERROR);
		return;
	}

	if (IOTCON_REQUEST_GET == type) {
		ret = _light_request_handler_get(light, request);
		if (0 != ret)
			_send_response(request, NULL, IOTCON_RESPONSE_ERROR);
	} else {
		_send_response(request, NULL, IOTCON_RESPONSE_FORBIDDEN);
	}
}

static void _fan_request_handler(iotcon_resource_h resource,
		iotcon_request_h request, void *user_data)
{
	int ret;
	iotcon_request_type_e type;
	fan_resource_s *fan = user_data;

	RET_IF(NULL == request);

	ret = iotcon_request_get_request_type(request, &type);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_request_get_request_type() Fail(%d)", ret);
		_send_response(request, NULL, IOTCON_RESPONSE_ERROR);
		return;
	}

	if (IOTCON_REQUEST_GET == type) {
		ret = _fan_request_handler_get(fan, request);
		if (0 != ret)
			_send_response(request, NULL, IOTCON_RESPONSE_ERROR);
	} else {
		_send_response(request, NULL, IOTCON_RESPONSE_FORBIDDEN);
	}
}

static void _room_request_handler(iotcon_resource_h resource,
		iotcon_request_h request, void *user_data)
{
	FN_CALL;
	int ret;
	iotcon_request_type_e type;
	char *host_address;
	room_resource_s *room = user_data;

	RET_IF(NULL == request);

	ret = iotcon_request_get_host_address(request, &host_address);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_request_get_host_address() Fail(%d)", ret);
		_send_response(request, NULL, IOTCON_RESPONSE_ERROR);
		return;
	}
	INFO("host address : %s", host_address);

	ret = iotcon_request_get_request_type(request, &type);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_request_get_request_type() Fail(%d)", ret);
		_send_response(request, NULL, IOTCON_RESPONSE_ERROR);
		return;
	}

	if (IOTCON_REQUEST_GET == type) {
		ret = _room_request_handler_get(room, request);
		if (0 != ret)
			_send_response(request, NULL, IOTCON_RESPONSE_ERROR);
	} else {
		_send_response(request, NULL, IOTCON_RESPONSE_FORBIDDEN);
	}
}

int main(int argc, char **argv)
{
	FN_CALL;
	int ret;
	GMainLoop *loop;
	room_resource_s room = {0};
	light_resource_s light = {0};
	fan_resource_s fan = {0};

	loop = g_main_loop_new(NULL, FALSE);

	/* initialize iotcon */
	ret = iotcon_initialize("/usr/bin/iotcon-test-svr-db-server.dat");
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_initialize() Fail(%d)", ret);
		return -1;
	}

	/* set device name */
	ret = iotcon_set_device_name("iotcon-test-iface-server");
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_set_device_name() Fail(%d)", ret);
		iotcon_deinitialize();
		return -1;
	}

	/* set resource */
	ret = _set_room_resource(&room);
	if (0 != ret) {
		ERR("_set_room_resource() Fail(%d)", ret);
		iotcon_deinitialize();
		return -1;
	}

	ret = _set_light_resource(&light);
	if (0 != ret) {
		ERR("_set_room_resource() Fail(%d)", ret);
		_free_room_resource(&room);
		iotcon_deinitialize();
		return -1;
	}

	ret = _set_fan_resource(&fan);
	if (0 != ret) {
		ERR("_set_room_resource() Fail(%d)", ret);
		_free_room_resource(&room);
		_free_light_resource(&light);
		iotcon_deinitialize();
		return -1;
	}

	room.child_light = &light;
	room.child_fan = &fan;

	/* register room resource */
	room.handle = _create_resource(room.uri_path, room.type, room.ifaces, room.policies,
			_room_request_handler, &room);
	if (NULL == room.handle) {
		ERR("_create_resource() Fail");
		_free_fan_resource(&fan);
		_free_light_resource(&light);
		_free_room_resource(&room);
		iotcon_deinitialize();
		return -1;
	}

	/* register light resource */
	light.handle = _create_resource(light.uri_path, light.type, light.ifaces,
			light.policies, _light_request_handler, &light);
	if (NULL == light.handle) {
		ERR("_create_resource() Fail");
		iotcon_resource_destroy(room.handle);
		_free_fan_resource(&fan);
		_free_light_resource(&light);
		_free_room_resource(&room);
		iotcon_deinitialize();
		return -1;
	}

	ret = iotcon_resource_bind_child_resource(room.handle, light.handle);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_bind_child_resource() Fail");
		iotcon_resource_destroy(light.handle);
		iotcon_resource_destroy(room.handle);
		_free_fan_resource(&fan);
		_free_light_resource(&light);
		_free_room_resource(&room);
		iotcon_deinitialize();
		return -1;
	}

	/* register fan resource */
	fan.handle = _create_resource(fan.uri_path, fan.type, fan.ifaces, fan.policies,
			_fan_request_handler, &fan);
	if (NULL == fan.handle) {
		ERR("_create_resource() Fail");
		iotcon_resource_destroy(light.handle);
		iotcon_resource_destroy(room.handle);
		_free_fan_resource(&fan);
		_free_light_resource(&light);
		_free_room_resource(&room);
		iotcon_deinitialize();
		return -1;
	}

	ret = iotcon_resource_bind_child_resource(room.handle, fan.handle);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_resource_bind_child_resource() Fail");
		iotcon_resource_destroy(fan.handle);
		iotcon_resource_destroy(light.handle);
		iotcon_resource_destroy(room.handle);
		_free_fan_resource(&fan);
		_free_light_resource(&light);
		_free_room_resource(&room);
		iotcon_deinitialize();
		return -1;
	}

	g_main_loop_run(loop);
	g_main_loop_unref(loop);

	iotcon_resource_destroy(fan.handle);
	iotcon_resource_destroy(light.handle);
	iotcon_resource_destroy(room.handle);
	_free_fan_resource(&fan);
	_free_light_resource(&light);
	_free_room_resource(&room);

	/* deinitialize iotcon */
	iotcon_deinitialize();

	return 0;
}
