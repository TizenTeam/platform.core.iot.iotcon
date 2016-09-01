/* Copyright (c) 2015 - 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <errno.h>
#include <glib.h>

#include "iotcon-types.h"
#include "ic.h"
#include "ic-list.h"
#include "ic-value.h"
#include "ic-utils.h"
#include "ic-representation.h"
#include "ic-attributes.h"

iotcon_attributes_h icl_attributes_ref(iotcon_attributes_h attributes)
{
	RETV_IF(NULL == attributes, NULL);
	RETV_IF(attributes->ref_count <= 0, NULL);

	attributes->ref_count++;

	return attributes;
}


API int iotcon_attributes_create(iotcon_attributes_h *ret_attributes)
{
	iotcon_attributes_h attributes;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == ret_attributes, IOTCON_ERROR_INVALID_PARAMETER);

	attributes = calloc(1, sizeof(struct icl_attributes_s));
	if (NULL == attributes) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	attributes->hash_table = g_hash_table_new_full(g_str_hash, g_str_equal, free,
			icl_value_destroy);
	attributes->ref_count = 1;

	*ret_attributes = attributes;

	return IOTCON_ERROR_NONE;
}


API int iotcon_attributes_destroy(iotcon_attributes_h attributes)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);

	attributes->ref_count--;

	if (0 != attributes->ref_count)
		return IOTCON_ERROR_NONE;

	g_hash_table_destroy(attributes->hash_table);
	free(attributes);

	return IOTCON_ERROR_NONE;
}


API int iotcon_attributes_remove(iotcon_attributes_h attributes, const char *key)
{
	gboolean ret = FALSE;
	iotcon_value_h value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);

	value = g_hash_table_lookup(attributes->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup(%s) Fail", key);
		return IOTCON_ERROR_NO_DATA;
	}

	ret = g_hash_table_remove(attributes->hash_table, key);
	if (FALSE == ret) {
		ERR("g_hash_table_remove(%s) Fail", key);
		return IOTCON_ERROR_NO_DATA;
	}

	return IOTCON_ERROR_NONE;
}


API int iotcon_attributes_get_int(iotcon_attributes_h attributes, const char *key,
		int *val)
{
	iotcon_value_h value;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == val, IOTCON_ERROR_INVALID_PARAMETER);

	value = g_hash_table_lookup(attributes->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return IOTCON_ERROR_NO_DATA;
	}

	icl_basic_s *real = (icl_basic_s*)value;
	if (IOTCON_TYPE_INT != real->type) {
		ERR("Invalid Type(%d)", real->type);
		return IOTCON_ERROR_INVALID_TYPE;
	}

	*val = real->val.i;

	return IOTCON_ERROR_NONE;
}

API int iotcon_attributes_add_int(iotcon_attributes_h attributes, const char *key,
		int val)
{
	iotcon_value_h value;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);

	value = icl_value_create_int(val);
	if (NULL == value) {
		ERR("icl_value_create_int(%d) Fail", val);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	g_hash_table_replace(attributes->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}

API int iotcon_attributes_get_bool(iotcon_attributes_h attributes, const char *key,
		bool *val)
{
	icl_basic_s *real = NULL;
	iotcon_value_h value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == val, IOTCON_ERROR_INVALID_PARAMETER);

	value = g_hash_table_lookup(attributes->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return IOTCON_ERROR_NO_DATA;
	}

	real = (icl_basic_s*)value;
	if (IOTCON_TYPE_BOOL != real->type) {
		ERR("Invalid Type(%d)", real->type);
		return IOTCON_ERROR_INVALID_TYPE;
	}

	*val = real->val.b;

	return IOTCON_ERROR_NONE;
}

API int iotcon_attributes_add_bool(iotcon_attributes_h attributes, const char *key,
		bool val)
{
	iotcon_value_h value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);

	value = icl_value_create_bool(val);
	if (NULL == value) {
		ERR("icl_value_create_bool(%d) Fail", val);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	g_hash_table_replace(attributes->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}

API int iotcon_attributes_get_double(iotcon_attributes_h attributes,
		const char *key, double *val)
{
	icl_basic_s *real = NULL;
	iotcon_value_h value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == val, IOTCON_ERROR_INVALID_PARAMETER);

	value = g_hash_table_lookup(attributes->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return IOTCON_ERROR_NO_DATA;
	}

	real = (icl_basic_s*)value;
	if (IOTCON_TYPE_DOUBLE != real->type) {
		ERR("Invalid Type(%d)", real->type);
		return IOTCON_ERROR_INVALID_TYPE;
	}

	*val = real->val.d;

	return IOTCON_ERROR_NONE;
}

API int iotcon_attributes_add_double(iotcon_attributes_h attributes,
		const char *key, double val)
{
	iotcon_value_h value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);

	value = icl_value_create_double(val);
	if (NULL == value) {
		ERR("icl_value_create_double(%f) Fail", val);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	g_hash_table_replace(attributes->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}

API int iotcon_attributes_get_str(iotcon_attributes_h attributes, const char *key,
		char **val)
{
	icl_basic_s *real = NULL;
	iotcon_value_h value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == val, IOTCON_ERROR_INVALID_PARAMETER);

	value = g_hash_table_lookup(attributes->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return IOTCON_ERROR_NO_DATA;
	}

	real = (icl_basic_s*)value;
	if (IOTCON_TYPE_STR != real->type) {
		ERR("Invalid Type(%d)", real->type);
		return IOTCON_ERROR_INVALID_TYPE;
	}

	*val = real->val.s;

	return IOTCON_ERROR_NONE;
}

API int iotcon_attributes_add_str(iotcon_attributes_h attributes, const char *key,
		char *val)
{
	iotcon_value_h value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == val, IOTCON_ERROR_INVALID_PARAMETER);

	value = icl_value_create_str(val);
	if (NULL == value) {
		ERR("icl_value_create_str(%s) Fail", val);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	g_hash_table_replace(attributes->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}

API int iotcon_attributes_get_byte_str(iotcon_attributes_h attributes, const char *key,
		unsigned char **val, int *len)
{
	iotcon_value_h value = NULL;
	icl_val_byte_str_s *real = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == val, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == len, IOTCON_ERROR_INVALID_PARAMETER);

	value = g_hash_table_lookup(attributes->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return IOTCON_ERROR_NO_DATA;
	}

	real = (icl_val_byte_str_s*)value;
	if (IOTCON_TYPE_BYTE_STR != real->type) {
		ERR("Invalid Type(%d)", real->type);
		return IOTCON_ERROR_INVALID_TYPE;
	}

	*val = real->s;
	*len = real->len;

	return IOTCON_ERROR_NONE;
}

API int iotcon_attributes_add_byte_str(iotcon_attributes_h attributes,
		const char *key, unsigned char *val, int len)
{
	iotcon_value_h value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == val, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(len <= 0, IOTCON_ERROR_INVALID_PARAMETER);

	value = icl_value_create_byte_str(val, len);
	if (NULL == value) {
		ERR("icl_value_create_byte_str() Fail");
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	g_hash_table_replace(attributes->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}

API int iotcon_attributes_is_null(iotcon_attributes_h attributes, const char *key,
		bool *is_null)
{
	icl_basic_s *real = NULL;
	iotcon_value_h value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == is_null, IOTCON_ERROR_INVALID_PARAMETER);

	value = (iotcon_value_h) g_hash_table_lookup(attributes->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return IOTCON_ERROR_NO_DATA;
	}

	real = (icl_basic_s*)value;
	*is_null = (IOTCON_TYPE_NULL == real->type) ? true : false;

	return IOTCON_ERROR_NONE;
}

API int iotcon_attributes_add_null(iotcon_attributes_h attributes, const char *key)
{
	iotcon_value_h value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);

	value = icl_value_create_null();
	if (NULL == value) {
		ERR("icl_value_create_null() Fail");
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	g_hash_table_replace(attributes->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}

API int iotcon_attributes_get_list(iotcon_attributes_h attributes, const char *key,
		iotcon_list_h *list)
{
	iotcon_value_h value = NULL;
	icl_val_list_s *real = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == list, IOTCON_ERROR_INVALID_PARAMETER);

	value = g_hash_table_lookup(attributes->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return IOTCON_ERROR_NO_DATA;
	}

	real = (icl_val_list_s*)value;
	if (IOTCON_TYPE_LIST != real->type) {
		ERR("Invalid Type(%d)", real->type);
		return IOTCON_ERROR_INVALID_TYPE;
	}

	*list = real->list;

	return IOTCON_ERROR_NONE;
}

API int iotcon_attributes_add_list(iotcon_attributes_h attributes, const char *key,
		iotcon_list_h list)
{
	iotcon_value_h value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == list, IOTCON_ERROR_INVALID_PARAMETER);


	value = icl_value_create_list(list);
	if (NULL == value) {
		ERR("icl_value_create_list() Fail");
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	g_hash_table_replace(attributes->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}

API int iotcon_attributes_get_attributes(iotcon_attributes_h src, const char *key,
		iotcon_attributes_h *dest)
{
	icl_val_attributes_s *real = NULL;
	iotcon_value_h value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == src, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == dest, IOTCON_ERROR_INVALID_PARAMETER);

	value = g_hash_table_lookup(src->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return IOTCON_ERROR_NO_DATA;
	}

	real = (icl_val_attributes_s*)value;
	if (IOTCON_TYPE_ATTRIBUTES != real->type) {
		ERR("Invalid Type(%d)", real->type);
		return IOTCON_ERROR_INVALID_TYPE;
	}

	*dest = real->attributes;

	return IOTCON_ERROR_NONE;
}

API int iotcon_attributes_add_attributes(iotcon_attributes_h attributes,
		const char *key, iotcon_attributes_h val)
{
	iotcon_value_h value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == val, IOTCON_ERROR_INVALID_PARAMETER);

	value = icl_value_create_attributes(val);
	if (NULL == value) {
		ERR("icl_value_create_attributes(%p) Fail", val);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	g_hash_table_replace(attributes->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}

API int iotcon_attributes_get_type(iotcon_attributes_h attributes, const char *key,
		iotcon_type_e *type)
{
	iotcon_value_h value = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == type, IOTCON_ERROR_INVALID_PARAMETER);

	value = g_hash_table_lookup(attributes->hash_table, key);
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return IOTCON_ERROR_NO_DATA;
	}
	*type = value->type;

	return IOTCON_ERROR_NONE;
}

int icl_attributes_set_value(iotcon_attributes_h attributes, const char *key,
		iotcon_value_h value)
{
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == value, IOTCON_ERROR_INVALID_PARAMETER);

	g_hash_table_replace(attributes->hash_table, ic_utils_strdup(key), value);

	return IOTCON_ERROR_NONE;
}


API int iotcon_attributes_get_keys_count(iotcon_attributes_h attributes,
		unsigned int *count)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == count, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == attributes->hash_table, IOTCON_ERROR_INVALID_PARAMETER);

	*count = g_hash_table_size(attributes->hash_table);

	return IOTCON_ERROR_NONE;
}


API int iotcon_attributes_clone(iotcon_attributes_h attributes,
		iotcon_attributes_h *attributes_clone)
{
	int ret;

	iotcon_attributes_h temp = NULL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == attributes_clone, IOTCON_ERROR_INVALID_PARAMETER);

	if (attributes->hash_table) {
		ret = iotcon_attributes_create(&temp);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_attributes_create() Fail(%d)", ret);
			return ret;
		}

		g_hash_table_foreach(attributes->hash_table, (GHFunc)icl_attributes_clone_foreach,
				temp);
	}

	*attributes_clone = temp;

	return IOTCON_ERROR_NONE;
}


void icl_attributes_clone_foreach(char *key, iotcon_value_h src_val,
		iotcon_attributes_h dest_attributes)
{
	FN_CALL;
	iotcon_value_h copied_val;

	copied_val = icl_value_clone(src_val);
	if (NULL == copied_val) {
		ERR("icl_value_clone() Fail");
		return;
	}

	icl_attributes_set_value(dest_attributes, key, copied_val);
}


API int iotcon_attributes_foreach(iotcon_attributes_h attributes,
		iotcon_attributes_cb cb, void *user_data)
{
	GHashTableIter iter;
	gpointer key;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	g_hash_table_iter_init(&iter, attributes->hash_table);
	while (g_hash_table_iter_next(&iter, &key, NULL)) {
		if (IOTCON_FUNC_STOP == cb(attributes, key, user_data))
			break;
	}

	return IOTCON_ERROR_NONE;
}

