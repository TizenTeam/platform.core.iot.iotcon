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
#ifndef __IOTCON_STRUCT_ATTRIBUTES_H__
#define __IOTCON_STRUCT_ATTRIBUTES_H__

#include <iotcon-types.h>

/**
 * @file iotcon-attributes.h
 */

/**
 * @ingroup CAPI_IOT_CONNECTIVITY_COMMON_REPRESENTATION_MODULE
 * @defgroup CAPI_IOT_CONNECTIVITY_COMMON_REPRESENTATION_ATTRIBUTES_MODULE State
 *
 * @brief IoTCon State provides API to manage attributes.
 *
 * @section CAPI_IOT_CONNECTIVITY_COMMON_REPRESENTATION_ATTRIBUTES_MODULE_HEADER Required Header
 *  \#include <iotcon.h>
 *
 * @section CAPI_IOT_CONNECTIVITY_COMMON_REPRESENTATION_ATTRIBUTES_MODULE_OVERVIEW Overview
 * The iotcon attributes API provides string key based hash table.
 *
 * Example :
 * @code
#include <iotcon.h>
...
static void _request_handler(iotcon_resource_h resource, iotcon_request_h request,
		void *user_data)
{
	int ret;
	iotcon_request_type_e type;

	ret = iotcon_request_get_request_type(request, &type);
	if (IOTCON_ERROR_NONE != ret)
		return;

	if (IOTCON_REQUEST_GET == type) {
		iotcon_response_h response = NULL;
		iotcon_representation_h representation = NULL;
		iotcon_attributes_h attributes = NULL;

		ret = iotcon_response_create(request, &response);
		if (IOTCON_ERROR_NONE != ret)
			return;

		ret = iotcon_representation_create(&representation);
		if (IOTCON_ERROR_NONE != ret) {
			iotcon_response_destroy(response);
			return;
		}

		...

		ret = iotcon_attributes_create(&attributes);
		if (IOTCON_ERROR_NONE != ret) {
			iotcon_representation_destroy(representation);
			iotcon_response_destroy(response);
			return;
		}

		ret = iotcon_attributes_add_bool(attributes, "power", true);
		if (IOTCON_ERROR_NONE != ret) {
			iotcon_attributes_destroy(attributes);
			iotcon_representation_destroy(representation);
			iotcon_response_destroy(response);
			return;
		}

		ret = itocon_attributes_add_int(attributes, "brightness", 75);
		if (IOTCON_ERROR_NONE != ret) {
			iotcon_attributes_destroy(attributes);
			iotcon_representation_destroy(representation);
			iotcon_response_destroy(response);
			return;
		}

		ret = iotcon_representation_set_attributes(representation, attributes);
		if (IOTCON_ERROR_NONE != ret) {
			iotcon_attributes_destroy(attributes);
			iotcon_representation_destroy(representation);
			iotcon_response_destroy(response);
			return;
		}

		...

		ret = iotcon_response_set_representation(response, representation);
		if (IOTCON_ERROR_NONE != ret) {
			iotcon_attributes_destroy(attributes);
			iotcon_representation_destroy(representation);
			iotcon_response_destroy(response);
			return;
		}

		ret = iotcon_response_send(response);
		if (IOTCON_ERROR_NONE != ret) {
			iotcon_attributes_destroy(attributes);
			iotcon_representation_destroy(representation);
			iotcon_response_destroy(response);
			return;
		}

		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(representation);
		iotcon_response_destroy(response);
	}
	...
}
 * @endcode
 *
 * @section CAPI_IOT_CONNECTIVITY_COMMON_REPRESENTATION_ATTRIBUTES_MODULE_FEATURE Related Features
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
 * @brief Creates a new attributes handle.
 *
 * @since_tizen 3.0
 *
 * @remarks You must destroy @a attributes by calling iotcon_attributes_destroy()
 * if @a attributes is no longer needed.
 *
 * @param[out] attributes A newly allocated attributes handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_attributes_destroy()
 */
int iotcon_attributes_create(iotcon_attributes_h *attributes);

/**
 * @brief Destroys a attributes.
 * @details Releases a @a attributes and its internal data.
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle to free
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_attributes_create()
 */
int iotcon_attributes_destroy(iotcon_attributes_h attributes);

/**
 * @brief Clones a attributes handle.
 *
 * @since_tizen 3.0
 *
 * @remarks You must destroy @a attributes_clone by calling iotcon_attributes_destroy()
 * if @a attributes_clone is no longer needed.
 *
 * @param[in] attributes The attributes handle
 * @param[out] attributes_clone The cloned attributes handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_attributes_create()
 * @see iotcon_attributes_destroy()
 */
int iotcon_attributes_clone(iotcon_attributes_h attributes, iotcon_attributes_h *attributes_clone);

/**
 * @brief Adds a new key and integer value into the attributes.
 * @details If @a key is already exists, current value will be replaced with new @a val.
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key
 * @param[in] val The value
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 */
int iotcon_attributes_add_int(iotcon_attributes_h attributes, const char *key, int val);

/**
 * @brief Adds a new key and boolean value into the attributes.
 * @details If @a key is already exists, current value will be replaced with new @a val.
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key
 * @param[in] val The value
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 */
int iotcon_attributes_add_bool(iotcon_attributes_h attributes, const char *key, bool val);

/**
 * @brief Adds a new key and double value into the attributes.
 * @details If @a key is already exists, current value will be replaced with new @a val.
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key
 * @param[in] val The value
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 */
int iotcon_attributes_add_double(iotcon_attributes_h attributes, const char *key, double val);

/**
 * @brief Adds a new key and string value into the attributes.
 * @details If @a key is already exists, current value will be replaced with new @a val.
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key
 * @param[in] val The value
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 */
int iotcon_attributes_add_str(iotcon_attributes_h attributes, const char *key, char *val);

/**
 * @brief Adds a new key and byte string value into the attributes.
 * @details If @a key is already exists, current value will be replaced with new @a val.
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key
 * @param[in] val The value
 * @param[in] len The length of @a val
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 */
int iotcon_attributes_add_byte_str(iotcon_attributes_h attributes, const char *key, unsigned char *val,
		int len);

/**
 * @brief Adds a new key and list value into the attributes.
 * @details If @a key is already exists, current list will be replaced with new @a list.
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key
 * @param[in] list The value
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 */
int iotcon_attributes_add_list(iotcon_attributes_h attributes, const char *key, iotcon_list_h list);

/**
 * @brief Adds a new key and attributes value into the attributes.
 * @details If @a key is already exists, current attributes will be replaced with new @a src.
 *
 * @since_tizen 3.0
 *
 * @param[in] dest The attributes handle
 * @param[in] key The key
 * @param[in] src The attributes handle to set newly
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 */
int iotcon_attributes_add_attributes(iotcon_attributes_h dest, const char *key, iotcon_attributes_h src);

/**
 * @brief Adds a new key with NULL value into the attributes.
 * @details If @a key is already exists, current value will be replaced with NULL.
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key to be set NULL
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 */
int iotcon_attributes_add_null(iotcon_attributes_h attributes, const char *key);

/**
 * @brief Gets the integer value from the given key.
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key
 * @param[out] val The integer value
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_NO_DATA  No data available
 * @retval #IOTCON_ERROR_INVALID_TYPE  Invalid type
 */
int iotcon_attributes_get_int(iotcon_attributes_h attributes, const char *key, int *val);

/**
 * @brief Gets the boolean value from the given key.
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key
 * @param[out] val The boolean value
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_NO_DATA  No data available
 * @retval #IOTCON_ERROR_INVALID_TYPE  Invalid type
 */
int iotcon_attributes_get_bool(iotcon_attributes_h attributes, const char *key, bool *val);

/**
 * @brief Gets the double value from the given key.
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key
 * @param[out] val The double value
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_NO_DATA  No data available
 * @retval #IOTCON_ERROR_INVALID_TYPE  Invalid type
 */
int iotcon_attributes_get_double(iotcon_attributes_h attributes, const char *key, double *val);

/**
 * @brief Gets the string value from the given key.
 *
 * @since_tizen 3.0
 *
 * @remarks @a val must not be released using free().
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key
 * @param[out] val The string value
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_NO_DATA  No data available
 * @retval #IOTCON_ERROR_INVALID_TYPE  Invalid type
 */
int iotcon_attributes_get_str(iotcon_attributes_h attributes, const char *key, char **val);

/**
 * @brief Gets the byte string value from the given key.
 *
 * @since_tizen 3.0
 *
 * @remarks @a val must not be released using free().
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key
 * @param[out] val The byte string value
 * @param[out] len The length of @a val
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_NO_DATA  No data available
 * @retval #IOTCON_ERROR_INVALID_TYPE  Invalid type
 */
int iotcon_attributes_get_byte_str(iotcon_attributes_h attributes, const char *key, unsigned char **val,
		int *len);

/**
 * @brief Gets the list value from the given key.
 *
 * @since_tizen 3.0
 *
 * @remarks @a list must not be released using iotcon_list_destroy().
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key
 * @param[out] list The list value
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_NO_DATA  No data available
 * @retval #IOTCON_ERROR_INVALID_TYPE  Invalid type
 */
int iotcon_attributes_get_list(iotcon_attributes_h attributes, const char *key, iotcon_list_h *list);

/**
 * @brief Gets the attributes value from the given key.
 *
 * @since_tizen 3.0
 *
 * @remarks @a attributes must not be released using iotcon_attributes_destroy().
 *
 * @param[in] src The attributes handle
 * @param[in] key The key
 * @param[out] dest The attributes value at the key
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_NO_DATA  No data available
 * @retval #IOTCON_ERROR_INVALID_TYPE  Invalid type
 */
int iotcon_attributes_get_attributes(iotcon_attributes_h src, const char *key, iotcon_attributes_h *dest);

/**
 * @brief Checks whether the value of given key is NULL or not.
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key
 * @param[out] is_null true if the type of the given key is null, otherwise false
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 */
int iotcon_attributes_is_null(iotcon_attributes_h attributes, const char *key, bool *is_null);

/**
 * @brief Removes the key and its associated value from the attributes.
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_NO_DATA  No data available
 */
int iotcon_attributes_remove(iotcon_attributes_h attributes, const char *key);

/**
 * @brief Gets the type of a value at the given key.
 * @details It gets the data type of value related the @a key in @a attributes.
 * The data type could be one of #iotcon_type_e.
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key
 * @param[out] type The data type of value related the key in attributes handle.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_NO_DATA  No data available
 */
int iotcon_attributes_get_type(iotcon_attributes_h attributes, const char *key,
		iotcon_type_e *type);

/**
 * @brief Specifies the type of function passed to iotcon_attributes_foreach().
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle
 * @param[in] key The key
 * @param[in] user_data The user data to pass to the function
 *
 * @return true to continue with the next iteration of the loop,
 * otherwise false to break out of the loop. #IOTCON_FUNC_CONTINUE and #IOTCON_FUNC_STOP
 * are more friendly values for the return.
 *
 * @pre iotcon_attributes_foreach() will invoke this callback function.
 *
 * @see iotcon_attributes_foreach()
 */
typedef bool (*iotcon_attributes_cb)(iotcon_attributes_h attributes, const char *key, void *user_data);

/**
 * @brief Calls a function for each element of attributes.
 * @details iotcon_attributes_cb() will be called for each child.
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle
 * @param[in] cb The callback function to invoke
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @post iotcon_attributes_cb() will be called for each child.
 *
 * @see iotcon_attributes_cb()
 */
int iotcon_attributes_foreach(iotcon_attributes_h attributes, iotcon_attributes_cb cb, void *user_data);

/**
 * @brief  Gets the number of keys in the attributes.
 *
 * @since_tizen 3.0
 *
 * @param[in] attributes The attributes handle
 * @param[out] count The number of keys
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 */
int iotcon_attributes_get_keys_count(iotcon_attributes_h attributes, unsigned int *count);

/**
 * @}
 */

#endif /* __IOTCON_STRUCT_ATTRIBUTES_H__ */
