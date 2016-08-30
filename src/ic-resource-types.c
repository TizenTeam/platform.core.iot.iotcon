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
#include <string.h>
#include <errno.h>
#include <glib.h>

#include "iotcon-types.h"
#include "ic.h"
#include "ic-utils.h"
#include "ic-resource.h"
#include "ic-resource-types.h"

iotcon_resource_types_h icl_resource_types_ref(iotcon_resource_types_h types)
{
	RETV_IF(NULL == types, NULL);
	RETV_IF(types->ref_count <= 0, NULL);

	types->ref_count++;

	return types;
}


API int iotcon_resource_types_create(iotcon_resource_types_h *ret_types)
{
	iotcon_resource_types_h types;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == ret_types, IOTCON_ERROR_INVALID_PARAMETER);

	types = calloc(1, sizeof(struct icl_resource_types));
	if (NULL == types) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	types->ref_count = 1;

	*ret_types = types;

	return IOTCON_ERROR_NONE;
}


API void iotcon_resource_types_destroy(iotcon_resource_types_h types)
{
	RET_IF(NULL == types);

	types->ref_count--;

	if (0 == types->ref_count) {
		g_list_free_full(types->type_list, free);
		free(types);
	}
}


static int _icl_resource_types_strcmp(const void *a, const void *b)
{
	return strcmp(a, b);
}


static bool _icl_resource_types_duplicate_check(iotcon_resource_types_h types,
		const char *type)
{
	GList *found_node = NULL;

	RETV_IF(NULL == types, false);
	RETV_IF(NULL == type, false);

	found_node = g_list_find_custom(types->type_list, type, _icl_resource_types_strcmp);
	if (NULL == found_node)
		return false;

	return true;
}


/* The length of resource type should be less than or equal to 61.
 * Duplicate strings are not allowed. */
API int iotcon_resource_types_add(iotcon_resource_types_h types, const char *type)
{
	char *resource_type;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == types, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == type, IOTCON_ERROR_INVALID_PARAMETER);
	RETVM_IF(1 < types->ref_count, IOTCON_ERROR_INVALID_PARAMETER,
			"Don't modify it. It is already set.");
	RETV_IF(false == icl_resource_check_type(type), IOTCON_ERROR_INVALID_PARAMETER);

	if (true == _icl_resource_types_duplicate_check(types, type)) {
		ERR("%s is already contained.", type);
		return IOTCON_ERROR_ALREADY;
	}

	resource_type = strdup(type);
	if (NULL == resource_type) {
		ERR("strdup() Fail");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	types->type_list = g_list_append(types->type_list, resource_type);

	return IOTCON_ERROR_NONE;
}


API int iotcon_resource_types_remove(iotcon_resource_types_h types,
		const char *type)
{
	GList *found_node;
	char *node_data;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == types, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == type, IOTCON_ERROR_INVALID_PARAMETER);
	RETVM_IF(1 < types->ref_count, IOTCON_ERROR_INVALID_PARAMETER,
			"Don't modify it. It is already set.");

	found_node = g_list_find_custom(types->type_list, type, _icl_resource_types_strcmp);
	if (NULL == found_node) {
		ERR("g_list_find_custom() Fail");
		return IOTCON_ERROR_NO_DATA;
	}

	node_data = found_node->data;
	types->type_list = g_list_delete_link(types->type_list, found_node);
	free(node_data);

	return IOTCON_ERROR_NONE;
}


API int iotcon_resource_types_foreach(iotcon_resource_types_h types,
		iotcon_resource_types_foreach_cb cb, void *user_data)
{
	GList *node;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == types, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	for (node = types->type_list; node; node = node->next) {
		if (IOTCON_FUNC_STOP == cb((const char*)node->data, user_data))
			break;
	}

	return IOTCON_ERROR_NONE;
}


API int iotcon_resource_types_clone(iotcon_resource_types_h src,
		iotcon_resource_types_h *dest)
{
	GList *node;
	char *resource_type;
	iotcon_resource_types_h resource_types;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == src, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == dest, IOTCON_ERROR_INVALID_PARAMETER);

	resource_types = calloc(1, sizeof(struct icl_resource_types));
	if (NULL == resource_types) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	for (node = src->type_list; node; node = node->next) {
		resource_type = ic_utils_strdup(node->data);
		if (NULL == resource_type) {
			iotcon_resource_types_destroy(resource_types);
			ERR("ic_utils_strdup() Fail");
			return IOTCON_ERROR_OUT_OF_MEMORY;
		}
		resource_types->type_list = g_list_append(resource_types->type_list,
				resource_type);
	}

	resource_types->ref_count = 1;

	*dest = resource_types;

	return IOTCON_ERROR_NONE;
}

