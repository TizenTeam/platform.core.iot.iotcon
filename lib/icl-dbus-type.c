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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glib.h>

#include "iotcon.h"
#include "ic-utils.h"
#include "icl.h"
#include "icl-resource-types.h"
#include "icl-ioty.h"
#include "icl-options.h"
#include "icl-query.h"
#include "icl-request.h"
#include "icl-response.h"
#include "icl-client.h"
#include "icl-repr.h"
#include "icl-dbus-type.h"

const char** icl_dbus_resource_types_to_array(iotcon_resource_types_h types)
{
	int i = 0;
	GList *node = types->type_list;
	int len = g_list_length(node);

	const char **array = calloc(len + 1, sizeof(char*));
	if (NULL == array) {
		ERR("calloc() Fail(%d)", errno);
		return NULL;
	}

	for (node = types->type_list; node; node = node->next, i++)
		array[i] = node->data;
	array[i] = NULL;

	return array;
}


GVariant* icl_dbus_notimsg_to_gvariant(struct ic_notify_msg *msg)
{
	char *repr_json = NULL;
	GVariant *value;
	GVariantBuilder *builder;

	builder = g_variant_builder_new(G_VARIANT_TYPE("a(iis)"));

	if (msg) {
		repr_json = icl_repr_generate_json(msg->repr, false);
		if (NULL == repr_json) {
			ERR("icl_repr_generate_json() Fail");
			g_variant_builder_unref(builder);
			return NULL;
		}
		g_variant_builder_add(builder, "(iis)", msg->error_code, msg->iface, repr_json);
	}
	value = g_variant_new("a(iis)", builder);

	free(repr_json);
	g_variant_builder_unref(builder);

	return value;
}


GVariant* icl_dbus_response_to_gvariant(struct ic_resource_response *response)
{
	GHashTableIter iter;
	GVariantBuilder *options;
	gpointer option_id, option_data;

	GVariant *value;

	options = g_variant_builder_new(G_VARIANT_TYPE("a(qs)"));
	if (response->header_options) {
		g_hash_table_iter_init(&iter, response->header_options->hash);
		while (g_hash_table_iter_next(&iter, &option_id, &option_data))
			g_variant_builder_add(options, "(qs)", GPOINTER_TO_INT(option_id), option_data);
	}

	value = g_variant_new("(sia(qs)iisii)",
			ic_utils_dbus_encode_str(response->new_uri),
			response->error_code,
			options,
			response->iface,
			response->result,
			icl_repr_generate_json(response->repr, false),
			GPOINTER_TO_INT(response->request_handle),
			GPOINTER_TO_INT(response->resource_handle));

	g_variant_builder_unref(options);

	return value;
}


GVariant* icl_dbus_client_to_gvariant(struct ic_remote_resource *resource)
{
	FN_CALL;
	GVariant *value;
	GVariantBuilder *options;
	GHashTableIter iter;
	gpointer option_id, option_data;
	GVariantBuilder *types;
	GList *node;

	options = g_variant_builder_new(G_VARIANT_TYPE("a(qs)"));
	if (resource->header_options) {
		g_hash_table_iter_init(&iter, resource->header_options->hash);
		while (g_hash_table_iter_next(&iter, &option_id, &option_data))
			g_variant_builder_add(options, "(qs)", GPOINTER_TO_INT(option_id), option_data);
	}

	types = g_variant_builder_new(G_VARIANT_TYPE("as"));
	for (node = resource->types->type_list; node; node = node->next)
		g_variant_builder_add(types, "s", node->data);

	value = g_variant_new("(ssba(qs)asii)",
			resource->uri,
			resource->host,
			resource->is_observable,
			options,
			types,
			resource->ifaces,
			GPOINTER_TO_INT(resource->observe_handle));

	g_variant_builder_unref(options);
	g_variant_builder_unref(types);

	return value;
}


GVariant* icl_dbus_device_info_to_gvariant(iotcon_device_info_s *device_info)
{
	GVariant *value;

	value = g_variant_new("(ssssssssssss)",
			ic_utils_dbus_encode_str(device_info->name),
			ic_utils_dbus_encode_str(device_info->host_name),
			ic_utils_dbus_encode_str(device_info->uuid),
			ic_utils_dbus_encode_str(device_info->content_type),
			ic_utils_dbus_encode_str(device_info->version),
			ic_utils_dbus_encode_str(device_info->manuf_name),
			ic_utils_dbus_encode_str(device_info->manuf_url),
			ic_utils_dbus_encode_str(device_info->model_number),
			ic_utils_dbus_encode_str(device_info->date_of_manufacture),
			ic_utils_dbus_encode_str(device_info->platform_ver),
			ic_utils_dbus_encode_str(device_info->firmware_ver),
			ic_utils_dbus_encode_str(device_info->support_url));

	return value;
}


GVariant* icl_dbus_query_to_gvariant(iotcon_query_h query)
{
	FN_CALL;
	GVariant *query_value;
	GVariantBuilder *builder;
	GHashTableIter iter;
	gpointer key, value;

	builder = g_variant_builder_new(G_VARIANT_TYPE("a(ss)"));
	if (query) {
		g_hash_table_iter_init(&iter, query->hash);
		while (g_hash_table_iter_next(&iter, &key, &value))
			g_variant_builder_add(builder, "(ss)", key, value);
	}

	query_value = g_variant_new("a(ss)", builder);
	g_variant_builder_unref(builder);

	return query_value;
}


GVariant* icl_dbus_observers_to_gvariant(iotcon_observers_h observers)
{
	GList *node;
	GVariant *obs;
	GVariantBuilder *builder;

	builder = g_variant_builder_new(G_VARIANT_TYPE("ai"));
	for (node = observers; node; node = node->next)
		g_variant_builder_add(builder, "i", GPOINTER_TO_INT(node->data));

	obs = g_variant_new("ai", builder);
	g_variant_builder_unref(builder);

	return obs;
}
