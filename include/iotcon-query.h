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
#ifndef __IOTCON_STRUCT_QUERY_H__
#define __IOTCON_STRUCT_QUERY_H__

#include <iotcon-types.h>

/**
 * @file iotcon-query.h
 */

/**
 * @ingroup CAPI_IOT_CONNECTIVITY_COMMON_MODULE
 * @defgroup CAPI_IOT_CONNECTIVITY_COMMON_QUERY_MODULE Query
 *
 * @brief IoTCon Query provides API to manage query.
 *
 * @section CAPI_IOT_CONNECTIVITY_COMMON_QUERY_MODULE_HEADER Required Header
 *  \#include <iotcon.h>
 *
 * @section CAPI_IOT_CONNECTIVITY_COMMON_QUERY_MODULE_OVERVIEW Overview
 * The iotcon query API provides methods for managing query of request.
 *
 * Example (Client side) :
 * @code
#include <iotcon.h>
...
static void _request_get(iotcon_remote_resource_h resource)
{
	int ret;
	iotcon_query_h query = NULL;

	ret = iotcon_query_create(&query);
	if (IOTCON_ERROR_NONE != ret)
		return;

	ret = iotcon_query_add(query, "key", "value");
	if (IOTCON_ERROR_NONE != ret) {
		iotcon_query_destroy(query);
		return;
	}

	ret = iotcon_remote_resource_get(resource, query, _on_get, NULL);
	if (IOTCON_ERROR_NONE != ret) {
		iotcon_query_destroy(query);
		return;
	}

	iotcon_query_destroy(query);
}
 * @endcode
 *
 * Example (Server side) :
 * @code
#include <iotcon.h>
...
static bool _query_foreach(const char *key, const char *value, void *user_data)
{
	// handle query
	return IOTCON_FUNC_CONTINUE;
}

static void _request_handler(iotcon_resource_h resource, iotcon_request_h request,
		void *user_data)
{
	int ret;
	iotcon_query_h query = NULL;

	ret = iotcon_request_get_query(request, &query);
	if (IOTCON_ERROR_NONE != ret)
		return;

	if (IOTCON_ERROR_NONE == ret && query) {
		ret = iotcon_query_foreach(query, _query_foreach, NULL);
		if (IOTCON_ERROR_NONE != ret)
			return;
	}
	...
}
 * @endcode
 *
 * @section CAPI_IOT_CONNECTIVITY_COMMON_QUERY_MODULE_FEATURE Related Features
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
 * @brief Creates a new query handle.
 *
 * @since_tizen 3.0
 *
 * @remarks You must destroy @a query by calling iotcon_query_destroy()
 * if @a query is no longer needed.
 *
 * @param[out] query A newly allocated query handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_query_destroy()
 * @see iotcon_query_add()
 * @see iotcon_query_remove()
 * @see iotcon_query_lookup()
 */
int iotcon_query_create(iotcon_query_h *query);

/**
 * @brief Destroys a query handle.
 *
 * @since_tizen 3.0
 *
 * @param[in] query The handle of the query
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_query_create()
 * @see iotcon_query_add()
 * @see iotcon_query_remove()
 * @see iotcon_query_lookup()
 */
int iotcon_query_destroy(iotcon_query_h query);

/**
 * @brief Gets resource type from the query.
 *
 * @since_tizen 3.0
 *
 * @remarks @a resource_type must not be released using free().
 * @a resource_type must start with a lowercase alphabetic character, followed by a sequence
 * of lowercase alphabetic, numeric, ".", or "-" characters, and contains no white space.
 *
 * @param[in] query The handle of the query
 * @param[out] resource_type Found resource type from query
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_NO_DATA  No data available
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 *
 * @see iotcon_query_create()
 * @see iotcon_query_destroy()
 * @see iotcon_query_add()
 * @see iotcon_query_remove()
 * @see iotcon_query_set_resource_type()
 */
int iotcon_query_get_resource_type(iotcon_query_h query, char **resource_type);

/**
 * @brief Gets resource interface from the query.
 *
 * @since_tizen 3.0
 *
 * @remarks @a resource_iface could be a value such as #IOTCON_INTERFACE_DEFAULT.
 * @remarks @a resource_iface must not be released using free().
 *
 * @param[in] query The handle of the query
 * @param[out] resource_iface Found resource interface from query
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_NO_DATA  No data available
 *
 * @see iotcon_query_create()
 * @see iotcon_query_destroy()
 * @see iotcon_query_add()
 * @see iotcon_query_remove()
 * @see iotcon_query_set_interface()
 */
int iotcon_query_get_interface(iotcon_query_h query, char **resource_iface);

/**
 * @brief Sets the resource type into the query.
 *
 * @since_tizen 3.0
 *
 * @param[in] query The handle of the query
 * @param[in] resource_type The resource type to set into the query
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_query_create()
 * @see iotcon_query_destroy()
 * @see iotcon_query_add()
 * @see iotcon_query_remove()
 * @see iotcon_query_lookup()
 * @see iotcon_query_get_resource_type()
 */
int iotcon_query_set_resource_type(iotcon_query_h query, const char *resource_type);

/**
 * @brief Sets the resource interface into the query.
 *
 * @since_tizen 3.0
 *
 * @remarks @a resource_iface could be a value such as #IOTCON_INTERFACE_DEFAULT.
 *
 * @param[in] query The handle of the query
 * @param[in] resource_iface The resource interface to add into the query
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_query_create()
 * @see iotcon_query_destroy()
 * @see iotcon_query_add()
 * @see iotcon_query_remove()
 * @see iotcon_query_lookup()
 * @see iotcon_query_get_interface()
 */
int iotcon_query_set_interface(iotcon_query_h query, const char *resource_iface);

/**
 * @brief Adds a new key and corresponding value into the query.
 *
 * @since_tizen 3.0
 * @remarks The full length of query should be less than or equal to 64.
 *
 * @param[in] query The handle of the query
 * @param[in] key The key of the query to insert
 * @param[in] value The string data to insert into the query
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_query_create()
 * @see iotcon_query_destroy()
 * @see iotcon_query_remove()
 * @see iotcon_query_lookup()
 */
int iotcon_query_add(iotcon_query_h query, const char *key, const char *value);

/**
 * @brief Removes the key and its associated value from the query.
 *
 * @since_tizen 3.0
 *
 * @param[in] query The handle of the query
 * @param[in] key The key of the option to delete
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_query_create()
 * @see iotcon_query_destroy()
 * @see iotcon_query_add()
 * @see iotcon_query_lookup()
 */
int iotcon_query_remove(iotcon_query_h query, const char *key);

/**
 * @brief Looks up data at the given key from the query.
 *
 * @since_tizen 3.0
 *
 * @remarks @a data must not be released using free().
 *
 * @param[in] query The handle of the query
 * @param[in] key The key of the query to lookup
 * @param[out] data Found data from query
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_query_create()
 * @see iotcon_query_destroy()
 * @see iotcon_query_add()
 * @see iotcon_query_remove()
 */
int iotcon_query_lookup(iotcon_query_h query, const char *key, char **data);

/**
 * @brief Specifies the type of function passed to iotcon_query_foreach().
 *
 * @since_tizen 3.0
 *
 * @param[in] key The key of the query
 * @param[in] value The value of the query
 * @param[in] user_data The user data to pass to the function
 *
 * @return true to continue with the next iteration of the loop,
 * otherwise false to break out of the loop. #IOTCON_FUNC_CONTINUE and #IOTCON_FUNC_STOP
 * are more friendly values for the return.
 *
 * @pre iotcon_query_foreach() will invoke this callback function.
 *
 * @see iotcon_query_foreach()
 */
typedef bool (*iotcon_query_foreach_cb)(const char *key, const char *value,
		void *user_data);

/**
 * @brief Gets all data of the query by invoking the callback function.
 * @details iotcon_query_foreach_cb() will be called for each query.\n
 * If iotcon_query_foreach_cb() returns false, iteration will be stop.
 *
 * @since_tizen 3.0
 *
 * @param[in] query The handle of the query
 * @param[in] cb The callback function to get data
 * @param[in] user_data The user data to be passed to the callback function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @post iotcon_query_foreach_cb() will be called for each query.
 *
 * @see iotcon_query_foreach_cb()
 */
int iotcon_query_foreach(iotcon_query_h query, iotcon_query_foreach_cb cb,
		void *user_data);

/**
 * @}
 */

#endif /* __IOTCON_STRUCT_QUERY_H__ */
