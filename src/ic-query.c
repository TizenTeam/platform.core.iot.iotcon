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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glib.h>

#include "iotcon-types.h"
#include "ic.h"
#include "ic-utils.h"
#include "ic-resource.h"
#include "ic-resource-types.h"
#include "ic-query.h"

API int iotcon_query_create(iotcon_query_h *ret_query)
{
	iotcon_query_h query;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == ret_query, IOTCON_ERROR_INVALID_PARAMETER);

	query = calloc(1, sizeof(struct icl_query));
	if (NULL == query) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	query->hash = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);

	*ret_query = query;

	return IOTCON_ERROR_NONE;
}


API int iotcon_query_destroy(iotcon_query_h query)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == query, IOTCON_ERROR_INVALID_PARAMETER);

	g_hash_table_unref(query->hash);
	free(query);

	return IOTCON_ERROR_NONE;
}


API int iotcon_query_get_resource_type(iotcon_query_h query,
		char **resource_type)
{
	char *type;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == query, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == resource_type, IOTCON_ERROR_INVALID_PARAMETER);

	iotcon_query_lookup(query, ICL_QUERY_KEY_RESOURCE_TYPE, &type);
	if (NULL == type) {
		ERR("resource_type is NULL");
		return IOTCON_ERROR_NO_DATA;
	}

	*resource_type = type;

	return IOTCON_ERROR_NONE;
}

API int iotcon_query_get_interface(iotcon_query_h query, char **resource_iface)
{
	char *iface = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == query, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == resource_iface, IOTCON_ERROR_INVALID_PARAMETER);

	iotcon_query_lookup(query, ICL_QUERY_KEY_INTERFACE, &iface);
	if (NULL == iface) {
		ERR("iface is NULL");
		return IOTCON_ERROR_NO_DATA;
	}

	*resource_iface = iface;

	return IOTCON_ERROR_NONE;
}

API int iotcon_query_set_resource_type(iotcon_query_h query,
		const char *resource_type)
{
	int length_old = 0;
	int length_new = 0;
	char *value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == query, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(resource_type && (false == icl_resource_check_type(resource_type)),
			IOTCON_ERROR_INVALID_PARAMETER);

	value = g_hash_table_lookup(query->hash, ICL_QUERY_KEY_RESOURCE_TYPE);
	if (value)
		length_old = (sizeof(ICL_QUERY_KEY_RESOURCE_TYPE) - 1) + strlen(value) + 2;

	if (resource_type)
		length_new = (sizeof(ICL_QUERY_KEY_RESOURCE_TYPE) - 1) + strlen(resource_type) + 2;

	if (ICL_QUERY_LENGTH_MAX < query->len - length_old + length_new) {
		ERR("Length of query is invalid.");
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	if (value)
		iotcon_query_remove(query, ICL_QUERY_KEY_RESOURCE_TYPE);

	if (resource_type)
		iotcon_query_add(query, ICL_QUERY_KEY_RESOURCE_TYPE, resource_type);

	return IOTCON_ERROR_NONE;
}

API int iotcon_query_set_interface(iotcon_query_h query, const char *resource_iface)
{
	int length_old = 0;
	int length_new = 0;
	char *value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == query, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(resource_iface && (false == icl_resource_check_interface(resource_iface)),
			IOTCON_ERROR_INVALID_PARAMETER);

	value = g_hash_table_lookup(query->hash, ICL_QUERY_KEY_INTERFACE);
	if (value)
		length_old = (sizeof(ICL_QUERY_KEY_INTERFACE) - 1) + strlen(value) + 2;

	if (resource_iface)
		length_new = (sizeof(ICL_QUERY_KEY_INTERFACE) - 1) + strlen(resource_iface) + 2;

	if (ICL_QUERY_LENGTH_MAX < query->len - length_old + length_new) {
		ERR("Length of query is invalid.");
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	if (value)
		iotcon_query_remove(query, ICL_QUERY_KEY_INTERFACE);

	iotcon_query_add(query, ICL_QUERY_KEY_INTERFACE, resource_iface);

	return IOTCON_ERROR_NONE;
}

/* The full length of query should be less than or equal to 64. */
API int iotcon_query_add(iotcon_query_h query, const char *key, const char *value)
{
	int query_len;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == query, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == value, IOTCON_ERROR_INVALID_PARAMETER);

	/* first query : ?key=value
	 * Rest of query : ;key=value */
	query_len = strlen(key) + strlen(value) + 2;
	if (ICL_QUERY_LENGTH_MAX < (query->len + query_len)) {
		ERR("Length of query is invalid.");
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	g_hash_table_insert(query->hash, ic_utils_strdup(key), ic_utils_strdup(value));
	query->len += query_len;

	return IOTCON_ERROR_NONE;
}


API int iotcon_query_remove(iotcon_query_h query, const char *key)
{
	gboolean is_removed;
	int query_len;
	char *value;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == query, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);

	value = g_hash_table_lookup(query->hash, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	query_len = strlen(key) + strlen(value) + 2;

	is_removed = g_hash_table_remove(query->hash, key);
	if (FALSE == is_removed) {
		ERR("g_hash_table_remove() Fail");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}
	query->len -= query_len;

	return IOTCON_ERROR_NONE;
}


API int iotcon_query_lookup(iotcon_query_h query, const char *key, char **data)
{
	char *value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == query, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == data, IOTCON_ERROR_INVALID_PARAMETER);

	value = g_hash_table_lookup(query->hash, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	*data = value;

	return IOTCON_ERROR_NONE;
}

API int iotcon_query_foreach(iotcon_query_h query, iotcon_query_foreach_cb cb,
		void *user_data)
{
	GHashTableIter iter;
	gpointer key, value;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == query, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	g_hash_table_iter_init(&iter, query->hash);
	while (g_hash_table_iter_next(&iter, &key, &value)) {
		if (IOTCON_FUNC_STOP == cb(key, value, user_data))
			break;
	}

	return IOTCON_ERROR_NONE;
}

