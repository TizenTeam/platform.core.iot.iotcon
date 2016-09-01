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

#include "iotcon.h"
#include "ic.h"
#include "ic-utils.h"
#include "ic-resource.h"
#include "ic-representation.h"
#include "ic-options.h"
#include "ic-request.h"
#include "ic-response.h"

#include "ic-ioty.h"

/* the last index of iotcon_response_result_e */
#define ICL_RESPONSE_RESULT_MAX (IOTCON_RESPONSE_FORBIDDEN + 1)

API int iotcon_response_create(iotcon_request_h request,
		iotcon_response_h *response)
{
	FN_CALL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == request, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == response, IOTCON_ERROR_INVALID_PARAMETER);

	iotcon_response_h resp = calloc(1, sizeof(struct icl_resource_response));
	if (NULL == resp) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	resp->oic_request_h = request->oic_request_h;
	resp->oic_resource_h = request->oic_resource_h;

	*response = resp;

	return IOTCON_ERROR_NONE;
}


API int iotcon_response_destroy(iotcon_response_h resp)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resp, IOTCON_ERROR_INVALID_PARAMETER);

	if (resp->repr)
		iotcon_representation_destroy(resp->repr);
	if (resp->header_options)
		iotcon_options_destroy(resp->header_options);
	free(resp);

	return IOTCON_ERROR_NONE;
}

API int iotcon_response_get_options(iotcon_response_h resp,
		iotcon_options_h *options)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resp, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == options, IOTCON_ERROR_INVALID_PARAMETER);
	WARN_IF(NULL == resp->header_options, "Not Set header options");

	*options = resp->header_options;

	return IOTCON_ERROR_NONE;
}

API int iotcon_response_get_representation(iotcon_response_h resp,
		iotcon_representation_h *repr)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resp, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == repr, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == resp->repr, IOTCON_ERROR_NO_DATA);

	*repr = resp->repr;

	return IOTCON_ERROR_NONE;

}

API int iotcon_response_get_result(iotcon_response_h resp,
		iotcon_response_result_e *result)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resp, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == result, IOTCON_ERROR_INVALID_PARAMETER);

	*result = resp->result;

	return IOTCON_ERROR_NONE;
}

API int iotcon_response_set_result(iotcon_response_h resp,
		iotcon_response_result_e result)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resp, IOTCON_ERROR_INVALID_PARAMETER);

	if (result < IOTCON_RESPONSE_OK || ICL_RESPONSE_RESULT_MAX <= result) {
		ERR("Invalid result(%d)", result);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}
	resp->result = result;

	return IOTCON_ERROR_NONE;
}


API int iotcon_response_set_representation(iotcon_response_h resp,
		iotcon_representation_h repr)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resp, IOTCON_ERROR_INVALID_PARAMETER);

	if (repr)
		repr = icl_representation_ref(repr);

	if (resp->repr)
		iotcon_representation_destroy(resp->repr);

	resp->repr = repr;

	return IOTCON_ERROR_NONE;
}


API int iotcon_response_set_options(iotcon_response_h resp,
		iotcon_options_h options)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == resp, IOTCON_ERROR_INVALID_PARAMETER);

	if (options)
		options = icl_options_ref(options);

	if (resp->header_options)
		iotcon_options_destroy(resp->header_options);

	resp->header_options = options;

	return IOTCON_ERROR_NONE;
}

API int iotcon_response_send(iotcon_response_h resp)
{
	FN_CALL;
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission(IC_PERMISSION_INTERNET),
			IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(NULL == resp, IOTCON_ERROR_INVALID_PARAMETER);

	ret = icl_ioty_response_send(resp);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_response_send() Fail(%d)", ret);
		return ret;
	}

	return IOTCON_ERROR_NONE;
}
