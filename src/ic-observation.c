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
#include "ic-observation.h"

API int iotcon_observers_create(iotcon_observers_h *ret_observers)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == ret_observers, IOTCON_ERROR_INVALID_PARAMETER);

	iotcon_observers_h observers = calloc(1, sizeof(struct icl_observers));
	if (NULL == observers) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	*ret_observers = observers;

	return IOTCON_ERROR_NONE;
}


API int iotcon_observers_destroy(iotcon_observers_h observers)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == observers, IOTCON_ERROR_INVALID_PARAMETER);

	g_list_free(observers->observers_list);
	free(observers);

	return IOTCON_ERROR_NONE;
}


API int iotcon_observers_add(iotcon_observers_h observers, int obs_id)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == observers, IOTCON_ERROR_INVALID_PARAMETER);

	observers->observers_list = g_list_append(observers->observers_list,
			GUINT_TO_POINTER(obs_id));

	return IOTCON_ERROR_NONE;
}


API int iotcon_observers_remove(iotcon_observers_h observers, int obs_id)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == observers, IOTCON_ERROR_INVALID_PARAMETER);

	observers->observers_list = g_list_remove(observers->observers_list,
			GUINT_TO_POINTER(obs_id));

	return IOTCON_ERROR_NONE;
}

