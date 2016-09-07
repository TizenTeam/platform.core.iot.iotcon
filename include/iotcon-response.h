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
#ifndef __IOTCON_COMMON_RESPONSE_H__
#define __IOTCON_COMMON_RESPONSE_H__

#include <iotcon-types.h>

/**
 * @file iotcon-response.h
 */

/**
 * @ingroup CAPI_IOT_CONNECTIVITY_COMMON_MODULE
 * @defgroup CAPI_IOT_CONNECTIVITY_COMMON_RESPONSE_MODULE Response
 *
 * @brief IoTCon Response provides API to manage response.
 *
 * @section CAPI_IOT_CONNECTIVITY_COMMON_RESPONSE_MODULE_HEADER Required Header
 *  \#include <iotcon.h>
 *
 * @section CAPI_IOT_CONNECTIVITY_COMMON_RESPONSE_MODULE_OVERVIEW Overview
 * The iotcon response API provides methods for managing handle and get response information.
 *
 * Example (Client side) :
 * @code
#include <iotcon.h>

static void _attributes_foreach(iotcon_attributes_h attributes, const char *key, void *user_data)
{
	// handle attributes
	...
}

static void _on_get(iotcon_remote_resource_h resource, iotcon_error_e err,
		iotcon_request_type_e request_type, iotcon_response_h response, void *user_data)
{
	int ret;
	iotcon_response_result_e response_result;
	iotcon_representation_h repr = NULL;
	iotcon_attributes_h attributes = NULL;

	if (IOTCON_ERROR_NONE != err)
		return;

	ret = iotcon_response_get_result(response, &response_result);
	if (IOTCON_ERROR_NONE != ret)
		return;

	if (IOTCON_RESPONSE_OK != response_result)
		return;

	ret = iotcon_response_get_representation(response, &repr);
	if (IOTCON_ERROR_NONE != ret)
		return;

	ret = iotcon_representation_get_attributes(repr, &attributes);
	if (IOTCON_ERROR_NONE != ret)
		return;

	ret = iotcon_attributes_foreach(attributes, _attributes_foreach, NULL);
	if (IOTCON_ERROR_NONE != ret)
		return;

	...
}

static void _request_get(iotcon_remote_resource_h resource)
{
	int ret;
	ret = iotcon_remote_resource_get(resource, NULL, _on_get, NULL);
	if (IOTCON_ERROR_NONE != ret)
		return;
}
 * @endcode
 *
 *
 * Example (Server side) :
 * @code
#include <iotcon.h>

static iotcon_attributes_h _create_attributes()
{
	int ret;
	iotcon_attributes_h attributes = NULL;

	// create & set attributes
	...

	return attributes;
}

static void _request_handler(iotcon_resource_h resource, iotcon_request_h request, void *user_data)
{
	int ret;
	iotcon_request_type_e type;
	iotcon_query_h query = NULL;

	ret = iotcon_request_get_request_type(request, &type);
	if (IOTCON_ERROR_NONE != ret)
		return;

	ret = iotcon_request_get_query(request, &query);
	if (IOTCON_ERROR_NONE == ret && query) {
		ret = iotcon_query_get_interface(request, &iface);
		if (IOTCON_ERROR_NONE != ret)
			return;
	}

	if (IOTCON_REQUEST_GET == type) {
		iotcon_response_h response = NULL;
		iotcon_representation_h repr = NULL;
		iotcon_attributes_h attributes = NULL;

		ret = iotcon_response_create(request, &response);
		if (IOTCON_ERROR_NONE != ret)
			return;

		ret = iotcon_response_set_result(response, IOTCON_RESPONSE_OK);
		if (IOTCON_ERROR_NONE != ret) {
			iotcon_response_destroy(response);
			return;
		}

		ret = iotcon_representation_create(&repr);
		if (IOTCON_ERROR_NONE != ret) {
			iotcon_response_destroy(response);
			return;
		}

		ret = iotcon_representation_set_uri_path(repr, "/light/1");
		if (IOTCON_ERROR_NONE != ret) {
			iotcon_representation_destroy(repr);
			iotcon_response_destroy(response);
			return;
		}

		ret = iotcon_representation_set_attributes(response, _create_attributes());
		if (IOTCON_ERROR_NONE != ret) {
			iotcon_representation_destroy(repr);
			iotcon_response_destroy(response);
			return;
		}

		ret = iotcon_response_set_representation(response, repr);
		if (IOTCON_ERROR_NONE != ret) {
			iotcon_representation_destroy(repr);
			iotcon_response_destroy(response);
			return;
		}

		ret = iotcon_response_send(response);
		if (IOTCON_ERROR_NONE != ret) {
			iotcon_representation_destroy(repr);
			iotcon_response_destroy(response);
			return;
		}

		iotcon_representation_destroy(repr);
		iotcon_response_destroy(response);
	}
	...
}
 * @endcode
 *
 * @section CAPI_IOT_CONNECTIVITY_COMMON_RESPONSE_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/iot.ocf\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature Element</b>.</a>
 *
 * @{
 */

/**
 * @brief Creates a response handle.
 *
 * @since_tizen 3.0
 *
 * @remarks You must destroy @a response by calling iotcon_response_destroy()
 * if @a response is no longer needed.
 *
 * @param[in] request The handle of received request handle
 * @param[out] response Generated response handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 *
 * @see iotcon_response_destroy()
 */
int iotcon_response_create(iotcon_request_h request, iotcon_response_h *response);

/**
 * @brief Destroys a response handle.
 *
 * @since_tizen 3.0
 *
 * @param[in] resp The handle of the response
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_response_create()
 */
int iotcon_response_destroy(iotcon_response_h resp);


/**
 * @brief Gets header options of the response.
 *
 * @since_tizen 3.0
 *
 * @remarks @a options must not be released using iotcon_options_destroy().
 *
 * @param[in] resp The handle of the response
 * @param[out] options The handle of the header options
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_response_get_representation()
 * @see iotcon_response_get_result()
 */
int iotcon_response_get_options(iotcon_response_h resp, iotcon_options_h *options);

/**
 * @brief Gets representation of the response.
 *
 * @since_tizen 3.0
 *
 * @remarks @a repr must not be released using iotcon_representation_destroy().
 *
 * @param[in] resp The handle of the response
 * @param[out] repr The handle of the representation
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_NO_DATA  No data
 *
 * @see iotcon_response_get_options()
 * @see iotcon_response_get_result()
 */
int iotcon_response_get_representation(iotcon_response_h resp, iotcon_representation_h *repr);

/**
 * @brief Gets result of the response.
 *
 * @since_tizen 3.0
 *
 * @param[in] resp The handle of the response
 * @param[out] result The result of the response
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_response_get_options()
 * @see iotcon_response_get_representation()
 */
int iotcon_response_get_result(iotcon_response_h resp,
		iotcon_response_result_e *result);


/**
 * @brief Sets result into the response.
 * @details The @a result could be one of #iotcon_response_result_e.
 *
 * @since_tizen 3.0
 *
 * @param[in] resp The handle of the response
 * @param[in] result The result to set
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 *
 * @see iotcon_response_create()
 * @see iotcon_response_destroy()
 */
int iotcon_response_set_result(iotcon_response_h resp, iotcon_response_result_e result);

/**
 * @brief Sets representation into the response.
 *
 * @since_tizen 3.0
 *
 * @param[in] resp The handle of the response
 * @param[in] repr The representation of the response
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 *
 * @see iotcon_response_create()
 * @see iotcon_response_destroy()
 */
int iotcon_response_set_representation(iotcon_response_h resp,
		iotcon_representation_h repr);

/**
 * @brief Sets header options into the response.
 *
 * @since_tizen 3.0
 *
 * @param[in] resp The handle of the response
 * @param[in] options The header options of the response
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 *
 * @see iotcon_response_create()
 * @see iotcon_response_destroy()
 */
int iotcon_response_set_options(iotcon_response_h resp, iotcon_options_h options);

/**
 * @brief Send response for incoming request.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resp The handle of the response to send
 *
 * @pre iotcon_initialize() should be called to initialize.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 */
int iotcon_response_send(iotcon_response_h resp);

/**
 * @}
 */

#endif /* __IOTCON_COMMON_RESPONSE_H__ */
