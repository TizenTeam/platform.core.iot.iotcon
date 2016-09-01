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
#include <errno.h>
#include <glib.h>

#include "iotcon-types.h"
#include "ic.h"
#include "ic-utils.h"
#include "ic-options.h"

/**
 * @brief HeaderOption range from 2048 to 3000.
 * NOTE: HeaderOptionID  is an unsigned integer value which MUST be within
 * range of 2048 to 3000 inclusive of lower and upper bound.
 * HeaderOptions instance creation fails if above condition is not satisfied.
 *
 * @since_tizen 3.0
 */
#define ICL_OPTIONID_MIN 2048

/**
 * @brief The maximum value of option id which can be held in a resource.
 *
 * @since_tizen 3.0
 */
#define ICL_OPTIONID_MAX 3000

/**
 * @brief The maximum number of option which can be held in a resource.
 *
 * @since_tizen 3.0
 */
#define ICL_OPTIONS_MAX 2

/**
 * @brief The maximum length of option data which can be held in a resource.
 *
 * @since_tizen 3.0
 */
#define ICL_OPTION_DATA_LENGTH_MAX 16


iotcon_options_h icl_options_ref(iotcon_options_h options)
{
	RETV_IF(NULL == options, NULL);
	RETV_IF(options->ref_count <= 0, NULL);

	options->ref_count++;

	return options;
}


API int iotcon_options_create(iotcon_options_h *ret_options)
{
	iotcon_options_h options;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == ret_options, IOTCON_ERROR_INVALID_PARAMETER);

	options = calloc(1, sizeof(struct icl_options));
	if (NULL == options) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	options->hash = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, free);
	options->ref_count = 1;

	*ret_options = options;

	return IOTCON_ERROR_NONE;
}


API int iotcon_options_destroy(iotcon_options_h options)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == options, IOTCON_ERROR_INVALID_PARAMETER);

	options->ref_count--;

	if (0 == options->ref_count) {
		g_hash_table_unref(options->hash);
		free(options);
	}

	return IOTCON_ERROR_NONE;
}


/* iotcon_options_h can have up to 2 options.
 * option id is always situated between 2048 and 3000.
 * Length of option data is less than or equal to 15. */
API int iotcon_options_add(iotcon_options_h options, unsigned short id,
		const char *data)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == options, IOTCON_ERROR_INVALID_PARAMETER);
	RETVM_IF(1 < options->ref_count, IOTCON_ERROR_INVALID_PARAMETER,
			"Don't modify it. It is already set.");
	RETVM_IF(ICL_OPTIONS_MAX <= g_hash_table_size(options->hash),
			IOTCON_ERROR_OUT_OF_MEMORY, "Options already have maximum elements.");

	RETVM_IF(((id < ICL_OPTIONID_MIN) || (ICL_OPTIONID_MAX < id)),
			IOTCON_ERROR_INVALID_PARAMETER, "Invalid id(%d)", id);

	RETV_IF(NULL == data, IOTCON_ERROR_INVALID_PARAMETER);
	RETVM_IF(ICL_OPTION_DATA_LENGTH_MAX < strlen(data), IOTCON_ERROR_INVALID_PARAMETER,
			"The length of option data(%s) is invalid.", data);

	g_hash_table_insert(options->hash, GUINT_TO_POINTER(id), ic_utils_strdup(data));

	return IOTCON_ERROR_NONE;
}


API int iotcon_options_remove(iotcon_options_h options, unsigned short id)
{
	gboolean is_removed;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == options, IOTCON_ERROR_INVALID_PARAMETER);
	RETVM_IF(1 < options->ref_count, IOTCON_ERROR_INVALID_PARAMETER,
			"Don't modify it. It is already set.");

	is_removed = g_hash_table_remove(options->hash, GUINT_TO_POINTER(id));
	if (FALSE == is_removed) {
		ERR("g_hash_table_remove() Fail");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}
	return IOTCON_ERROR_NONE;
}


API int iotcon_options_lookup(iotcon_options_h options, unsigned short id,
		char **data)
{
	char *value;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == options, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == data, IOTCON_ERROR_INVALID_PARAMETER);

	value = g_hash_table_lookup(options->hash, GUINT_TO_POINTER(id));
	if (NULL == value) {
		ERR("g_hash_table_lookup() Fail");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	*data = value;

	return IOTCON_ERROR_NONE;
}


API int iotcon_options_foreach(iotcon_options_h options,
		iotcon_options_foreach_cb cb, void *user_data)
{
	GHashTableIter iter;
	gpointer key, value;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == options, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	g_hash_table_iter_init(&iter, options->hash);
	while (g_hash_table_iter_next(&iter, &key, &value)) {
		if (IOTCON_FUNC_STOP == cb(GPOINTER_TO_UINT(key), value, user_data))
			break;
	}

	return IOTCON_ERROR_NONE;
}
