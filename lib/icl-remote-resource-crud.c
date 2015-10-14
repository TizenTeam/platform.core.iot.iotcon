/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <glib.h>

#include "iotcon.h"
#include "ic-utils.h"
#include "ic-dbus.h"
#include "icl.h"
#include "icl-options.h"
#include "icl-dbus.h"
#include "icl-dbus-type.h"
#include "icl-repr.h"
#include "icl-remote-resource.h"
#include "icl-payload.h"

typedef struct {
	iotcon_remote_resource_cru_cb cb;
	void *user_data;
	iotcon_remote_resource_h resource;
} icl_on_cru_s;

typedef struct {
	iotcon_remote_resource_delete_cb cb;
	void *user_data;
	iotcon_remote_resource_h resource;
} icl_on_delete_s;

typedef struct {
	iotcon_remote_resource_observe_cb cb;
	void *user_data;
	iotcon_remote_resource_h resource;
} icl_on_observe_s;


static void _icl_on_cru_cb(GVariant *result, icl_on_cru_s *cb_container)
{
	int res, ret;
	iotcon_representation_h repr;
	GVariantIter *options;
	unsigned short option_id;
	char *option_data;
	GVariant *repr_gvar;
	iotcon_options_h header_options = NULL;
	iotcon_remote_resource_cru_cb cb = cb_container->cb;

	g_variant_get(result, "(a(qs)vi)", &options, &repr_gvar, &res);

	if (IOTCON_ERROR_NONE == res && g_variant_iter_n_children(options)) {
		ret = iotcon_options_create(&header_options);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_options_create() Fail(%d)", ret);
			return;
		}

		while (g_variant_iter_loop(options, "(q&s)", &option_id, &option_data))
			iotcon_options_insert(header_options, option_id, option_data);
	}
	g_variant_iter_free(options);

	repr = icl_representation_from_gvariant(repr_gvar);
	if (NULL == repr) {
		ERR("icl_representation_from_gvariant() Fail");
		if (header_options)
			iotcon_options_destroy(header_options);

		iotcon_remote_resource_destroy(cb_container->resource);
		free(cb_container);
		return;
	}

	res = icl_dbus_convert_daemon_error(res);

	if (cb)
		cb(cb_container->resource, repr, header_options, res,
				cb_container->user_data);

	if (repr)
		iotcon_representation_destroy(repr);
	if (header_options)
		iotcon_options_destroy(header_options);

	iotcon_remote_resource_destroy(cb_container->resource);
	free(cb_container);
}


static void _icl_on_get_cb(GObject *object, GAsyncResult *g_async_res,
		gpointer user_data)
{
	GVariant *result;
	GError *error = NULL;
	icl_on_cru_s *cb_container = user_data;

	ic_dbus_call_get_finish(IC_DBUS(object), &result, g_async_res, &error);
	if (error) {
		ERR("ic_dbus_call_get_finish() Fail(%s)", error->message);
		g_error_free(error);
		iotcon_remote_resource_destroy(cb_container->resource);
		free(cb_container);
		return;
	}

	_icl_on_cru_cb(result, cb_container);
}


static void _icl_on_put_cb(GObject *object, GAsyncResult *g_async_res,
		gpointer user_data)
{
	GVariant *result;
	GError *error = NULL;
	icl_on_cru_s *cb_container = user_data;

	ic_dbus_call_put_finish(IC_DBUS(object), &result, g_async_res, &error);
	if (error) {
		ERR("ic_dbus_call_put_finish() Fail(%s)", error->message);
		g_error_free(error);
		iotcon_remote_resource_destroy(cb_container->resource);
		free(cb_container);
		return;
	}

	_icl_on_cru_cb(result, cb_container);
}


static void _icl_on_post_cb(GObject *object, GAsyncResult *g_async_res,
		gpointer user_data)
{
	GVariant *result;
	GError *error = NULL;
	icl_on_cru_s *cb_container = user_data;

	ic_dbus_call_post_finish(IC_DBUS(object), &result, g_async_res, &error);
	if (error) {
		ERR("ic_dbus_call_post_finish() Fail(%s)", error->message);
		g_error_free(error);
		iotcon_remote_resource_destroy(cb_container->resource);
		free(cb_container);
		return;
	}

	_icl_on_cru_cb(result, cb_container);
}


API int iotcon_remote_resource_get(iotcon_remote_resource_h resource,
		iotcon_query_h query, iotcon_remote_resource_cru_cb cb, void *user_data)
{
	int ret;
	GVariant *arg_query;
	GVariant *arg_remote_resource;
	icl_on_cru_s *cb_container;

	RETV_IF(NULL == icl_dbus_get_object(), IOTCON_ERROR_DBUS);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	cb_container = calloc(1, sizeof(icl_on_cru_s));
	if (NULL == cb_container) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	ret = iotcon_remote_resource_ref(resource, &cb_container->resource);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_ref() Fail");
		free(cb_container);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	cb_container->cb = cb;
	cb_container->user_data = user_data;

	arg_remote_resource = icl_dbus_remote_resource_to_gvariant(resource);
	arg_query = icl_dbus_query_to_gvariant(query);

	ic_dbus_call_get(icl_dbus_get_object(), arg_remote_resource, arg_query, NULL,
			_icl_on_get_cb, cb_container);

	return IOTCON_ERROR_NONE;
}


API int iotcon_remote_resource_put(iotcon_remote_resource_h resource,
		iotcon_representation_h repr,
		iotcon_query_h query,
		iotcon_remote_resource_cru_cb cb,
		void *user_data)
{
	int ret;
	GVariant *arg_repr;
	GVariant *arg_remote_resource;
	GVariant *arg_query;
	icl_on_cru_s *cb_container;

	RETV_IF(NULL == icl_dbus_get_object(), IOTCON_ERROR_DBUS);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == repr, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	cb_container = calloc(1, sizeof(icl_on_cru_s));
	if (NULL == cb_container) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	ret = iotcon_remote_resource_ref(resource, &cb_container->resource);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_ref() Fail");
		free(cb_container);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	cb_container->cb = cb;
	cb_container->user_data = user_data;

	arg_repr = icl_representation_to_gvariant(repr);
	if (NULL == arg_repr) {
		ERR("icl_representation_to_gvariant() Fail");
		iotcon_remote_resource_destroy(cb_container->resource);
		free(cb_container);
		return IOTCON_ERROR_REPRESENTATION;
	}

	arg_remote_resource = icl_dbus_remote_resource_to_gvariant(resource);
	arg_query = icl_dbus_query_to_gvariant(query);

	ic_dbus_call_put(icl_dbus_get_object(), arg_remote_resource, arg_repr, arg_query,
			NULL, _icl_on_put_cb, cb_container);

	return IOTCON_ERROR_NONE;
}


API int iotcon_remote_resource_post(iotcon_remote_resource_h resource,
		iotcon_representation_h repr,
		iotcon_query_h query,
		iotcon_remote_resource_cru_cb cb,
		void *user_data)
{
	int ret;
	GVariant *arg_repr;
	GVariant *arg_query;
	GVariant *arg_remote_resource;
	icl_on_cru_s *cb_container;

	RETV_IF(NULL == icl_dbus_get_object(), IOTCON_ERROR_DBUS);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == repr, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	cb_container = calloc(1, sizeof(icl_on_cru_s));
	if (NULL == cb_container) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	ret = iotcon_remote_resource_ref(resource, &cb_container->resource);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_ref() Fail");
		free(cb_container);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	cb_container->cb = cb;
	cb_container->user_data = user_data;

	arg_repr = icl_representation_to_gvariant(repr);
	if (NULL == arg_repr) {
		ERR("icl_representation_to_gvariant() Fail");
		iotcon_remote_resource_destroy(cb_container->resource);
		free(cb_container);
		return IOTCON_ERROR_REPRESENTATION;
	}

	arg_remote_resource = icl_dbus_remote_resource_to_gvariant(resource);
	arg_query = icl_dbus_query_to_gvariant(query);

	ic_dbus_call_post(icl_dbus_get_object(), arg_remote_resource, arg_repr, arg_query,
			NULL, _icl_on_post_cb, cb_container);

	return IOTCON_ERROR_NONE;
}


static void _icl_on_delete_cb(GObject *object, GAsyncResult *g_async_res,
		gpointer user_data)
{
	int res, ret;
	GVariant *result;
	char *option_data;
	GError *error = NULL;
	GVariantIter *options;
	unsigned short option_id;
	iotcon_options_h header_options = NULL;

	icl_on_delete_s *cb_container = user_data;
	iotcon_remote_resource_delete_cb cb = cb_container->cb;

	ic_dbus_call_delete_finish(IC_DBUS(object), &result, g_async_res, &error);
	if (error) {
		ERR("ic_dbus_call_delete_finish() Fail(%s)", error->message);
		g_error_free(error);
		iotcon_remote_resource_destroy(cb_container->resource);
		free(cb_container);
		return;
	}
	g_variant_get(result, "(a(qs)i)", &options, &res);

	if (IOTCON_ERROR_NONE == res && g_variant_iter_n_children(options)) {
		ret = iotcon_options_create(&header_options);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_options_create() Fail(%d)", ret);
			g_variant_iter_free(options);
			iotcon_remote_resource_destroy(cb_container->resource);
			free(cb_container);
			return;
		}

		while (g_variant_iter_loop(options, "(q&s)", &option_id, &option_data))
			iotcon_options_insert(header_options, option_id, option_data);
	}
	g_variant_iter_free(options);

	res = icl_dbus_convert_daemon_error(res);

	if (cb)
		cb(cb_container->resource, header_options, res, cb_container->user_data);

	if (header_options)
		iotcon_options_destroy(header_options);

	iotcon_remote_resource_destroy(cb_container->resource);
	free(cb_container);
}


API int iotcon_remote_resource_delete(iotcon_remote_resource_h resource,
		iotcon_remote_resource_delete_cb cb, void *user_data)
{
	int ret;
	GVariant *arg_remote_resource;
	icl_on_delete_s *cb_container;

	RETV_IF(NULL == icl_dbus_get_object(), IOTCON_ERROR_DBUS);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	cb_container = calloc(1, sizeof(icl_on_delete_s));
	if (NULL == cb_container) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	ret = iotcon_remote_resource_ref(resource, &cb_container->resource);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_ref() Fail");
		free(cb_container);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	cb_container->cb = cb;
	cb_container->user_data = user_data;

	arg_remote_resource = icl_dbus_remote_resource_to_gvariant(resource);

	ic_dbus_call_delete(icl_dbus_get_object(), arg_remote_resource, NULL,
			_icl_on_delete_cb, cb_container);

	return IOTCON_ERROR_NONE;
}


static void _icl_on_observe_cb(GDBusConnection *connection,
		const gchar *sender_name,
		const gchar *object_path,
		const gchar *interface_name,
		const gchar *signal_name,
		GVariant *parameters,
		gpointer user_data)
{
	int res, ret;
	int seq_num;
	iotcon_representation_h repr;
	GVariantIter *options;
	unsigned short option_id;
	char *option_data;
	GVariant *repr_gvar;
	iotcon_options_h header_options = NULL;

	icl_on_observe_s *cb_container = user_data;
	iotcon_remote_resource_observe_cb cb = cb_container->cb;

	g_variant_get(parameters, "(a(qs)vii)", &options, &repr_gvar, &res,
			&seq_num);

	if (IOTCON_ERROR_NONE == res && g_variant_iter_n_children(options)) {
		ret = iotcon_options_create(&header_options);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_options_create() Fail(%d)", ret);
			g_variant_iter_free(options);
			iotcon_remote_resource_destroy(cb_container->resource);
			free(cb_container);
			return;
		}

		while (g_variant_iter_loop(options, "(q&s)", &option_id, &option_data))
			iotcon_options_insert(header_options, option_id, option_data);
	}
	g_variant_iter_free(options);

	repr = icl_representation_from_gvariant(repr_gvar);
	if (NULL == repr) {
		ERR("icl_representation_from_gvariant() Fail");
		if (header_options)
			iotcon_options_destroy(header_options);

		iotcon_remote_resource_destroy(cb_container->resource);
		free(cb_container);
		return;
	}

	res = icl_dbus_convert_daemon_error(res);

	if (cb)
		cb(cb_container->resource, repr, header_options, res, seq_num,
				cb_container->user_data);

	if (repr)
		iotcon_representation_destroy(repr);
	if (header_options)
		iotcon_options_destroy(header_options);
}


static void _icl_observe_conn_cleanup(icl_on_observe_s *cb_container)
{
	cb_container->resource->observe_handle = 0;
	cb_container->resource->observe_sub_id = 0;
	iotcon_remote_resource_destroy(cb_container->resource);
	free(cb_container);
}


API int iotcon_remote_resource_observer_start(iotcon_remote_resource_h resource,
		int observe_type,
		iotcon_query_h query,
		iotcon_remote_resource_observe_cb cb,
		void *user_data)
{
	GVariant *arg_query;
	unsigned int sub_id;
	GVariant *arg_remote_resource;
	GError *error = NULL;
	int64_t observe_handle;
	icl_on_observe_s *cb_container;
	int ret, signal_number;
	char signal_name[IC_DBUS_SIGNAL_LENGTH] = {0};

	RETV_IF(NULL == icl_dbus_get_object(), IOTCON_ERROR_DBUS);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	signal_number = icl_dbus_generate_signal_number();

	arg_remote_resource = icl_dbus_remote_resource_to_gvariant(resource);
	arg_query = icl_dbus_query_to_gvariant(query);

	ic_dbus_call_observer_start_sync(icl_dbus_get_object(), arg_remote_resource,
			observe_type, arg_query, signal_number, &observe_handle, NULL, &error);
	if (error) {
		ERR("ic_dbus_call_observer_start_sync() Fail(%s)", error->message);
		ret = icl_dbus_convert_dbus_error(error->code);
		g_error_free(error);
		g_variant_unref(arg_query);
		g_variant_unref(arg_remote_resource);
		return ret;
	}

	if (0 == observe_handle) {
		ERR("iotcon-daemon Fail");
		return IOTCON_ERROR_IOTIVITY;
	}

	snprintf(signal_name, sizeof(signal_name), "%s_%u", IC_DBUS_SIGNAL_OBSERVE,
			signal_number);

	cb_container = calloc(1, sizeof(icl_on_observe_s));
	if (NULL == cb_container) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	ret = iotcon_remote_resource_ref(resource, &cb_container->resource);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_ref() Fail");
		free(cb_container);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	cb_container->cb = cb;
	cb_container->user_data = user_data;

	sub_id = icl_dbus_subscribe_signal(signal_name, cb_container,
			_icl_observe_conn_cleanup, _icl_on_observe_cb);
	if (0 == sub_id) {
		ERR("icl_dbus_subscribe_signal() Fail");
		iotcon_remote_resource_destroy(cb_container->resource);
		free(cb_container);
		return IOTCON_ERROR_DBUS;
	}
	resource->observe_sub_id = sub_id;
	resource->observe_handle = observe_handle;

	return IOTCON_ERROR_NONE;
}


API int iotcon_remote_resource_observer_stop(iotcon_remote_resource_h resource)
{
	int ret;
	GError *error = NULL;
	GVariant *arg_options;

	RETV_IF(NULL == icl_dbus_get_object(), IOTCON_ERROR_DBUS);
	RETV_IF(NULL == resource, IOTCON_ERROR_INVALID_PARAMETER);
	if (0 == resource->observe_handle) {
		ERR("It doesn't have a observe_handle");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	arg_options = icl_dbus_options_to_gvariant(resource->header_options);

	ic_dbus_call_observer_stop_sync(icl_dbus_get_object(), resource->observe_handle,
			arg_options, &ret, NULL, &error);
	if (error) {
		ERR("ic_dbus_call_observer_stop_sync() Fail(%s)", error->message);
		ret = icl_dbus_convert_dbus_error(error->code);
		g_error_free(error);
		return ret;
	}
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon-daemon Fail(%d)", ret);
		return icl_dbus_convert_daemon_error(ret);
	}

	icl_dbus_unsubscribe_signal(resource->observe_sub_id);

	return ret;
}
