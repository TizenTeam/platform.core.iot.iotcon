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
#include "ic-resource-interfaces.h"

iotcon_resource_interfaces_h icl_resource_interfaces_ref(
		iotcon_resource_interfaces_h ifaces)
{
	RETV_IF(NULL == ifaces, NULL);
	RETV_IF(ifaces->ref_count <= 0, NULL);

	ifaces->ref_count++;

	return ifaces;
}

API int iotcon_resource_interfaces_create(iotcon_resource_interfaces_h *ret_ifaces)
{
	iotcon_resource_interfaces_h ifaces;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == ret_ifaces, IOTCON_ERROR_INVALID_PARAMETER);

	ifaces = calloc(1, sizeof(struct icl_resource_ifaces));
	if (NULL == ifaces) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	ifaces->ref_count = 1;

	*ret_ifaces = ifaces;

	return IOTCON_ERROR_NONE;
}


API void iotcon_resource_interfaces_destroy(iotcon_resource_interfaces_h ifaces)
{
	RET_IF(NULL == ifaces);

	ifaces->ref_count--;

	if (0 == ifaces->ref_count) {
		g_list_free_full(ifaces->iface_list, free);
		free(ifaces);
	}
}


static int _icl_resource_interfaces_strcmp(const void *a, const void *b)
{
	return strcmp(a, b);
}


static bool _icl_resource_interfaces_duplicate_check(
		iotcon_resource_interfaces_h ifaces, const char *iface)
{
	GList *node = NULL;

	RETV_IF(NULL == ifaces, false);
	RETV_IF(NULL == iface, false);

	node = g_list_find_custom(ifaces->iface_list, iface, _icl_resource_interfaces_strcmp);
	if (NULL == node)
		return false;

	return true;
}


/* Duplicate strings are not allowed. */
API int iotcon_resource_interfaces_add(iotcon_resource_interfaces_h ifaces,
		const char *iface)
{
	char *resource_iface;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == ifaces, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == iface, IOTCON_ERROR_INVALID_PARAMETER);
	RETVM_IF(1 < ifaces->ref_count, IOTCON_ERROR_INVALID_PARAMETER,
			"Don't modify it. It is already set.");
	RETV_IF(false == icl_resource_check_interface(iface), IOTCON_ERROR_INVALID_PARAMETER);

	if (true == _icl_resource_interfaces_duplicate_check(ifaces, iface)) {
		ERR("%s is already contained.", iface);
		return IOTCON_ERROR_ALREADY;
	}

	resource_iface = strdup(iface);
	if (NULL == resource_iface) {
		ERR("strdup() Fail");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	ifaces->iface_list = g_list_append(ifaces->iface_list, resource_iface);

	return IOTCON_ERROR_NONE;
}


API int iotcon_resource_interfaces_remove(iotcon_resource_interfaces_h ifaces,
		const char *iface)
{
	GList *node;
	char *node_data;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == ifaces, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == iface, IOTCON_ERROR_INVALID_PARAMETER);
	RETVM_IF(1 < ifaces->ref_count, IOTCON_ERROR_INVALID_PARAMETER,
			"Don't modify it. It is already set.");

	node = g_list_find_custom(ifaces->iface_list, iface, _icl_resource_interfaces_strcmp);
	if (NULL == node) {
		ERR("g_list_find_custom() Fail");
		return IOTCON_ERROR_NO_DATA;
	}

	node_data = node->data;
	ifaces->iface_list = g_list_delete_link(ifaces->iface_list, node);
	free(node_data);

	return IOTCON_ERROR_NONE;
}


API int iotcon_resource_interfaces_foreach(iotcon_resource_interfaces_h ifaces,
		iotcon_resource_interfaces_foreach_cb cb, void *user_data)
{
	GList *node;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == ifaces, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	for (node = ifaces->iface_list; node; node = node->next) {
		if (IOTCON_FUNC_STOP == cb((const char*)node->data, user_data))
			break;
	}

	return IOTCON_ERROR_NONE;
}


API int iotcon_resource_interfaces_clone(iotcon_resource_interfaces_h src,
		iotcon_resource_interfaces_h *dest)
{
	GList *node;
	char *resource_iface;
	iotcon_resource_interfaces_h resource_ifaces;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == src, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == dest, IOTCON_ERROR_INVALID_PARAMETER);

	resource_ifaces = calloc(1, sizeof(struct icl_resource_ifaces));
	if (NULL == resource_ifaces) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	for (node = src->iface_list; node; node = node->next) {
		resource_iface = ic_utils_strdup(node->data);
		if (NULL == resource_iface) {
			iotcon_resource_interfaces_destroy(resource_ifaces);
			ERR("ic_utils_strdup() Fail");
			return IOTCON_ERROR_OUT_OF_MEMORY;
		}
		resource_ifaces->iface_list = g_list_append(resource_ifaces->iface_list,
				resource_iface);
	}

	resource_ifaces->ref_count = 1;

	*dest = resource_ifaces;

	return IOTCON_ERROR_NONE;
}

