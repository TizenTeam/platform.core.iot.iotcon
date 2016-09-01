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
#ifndef __IOTCON_STRUCT_RESOURCE_TYPES_H__
#define __IOTCON_STRUCT_RESOURCE_TYPES_H__

#include <iotcon-types.h>

/**
 * @file iotcon-resource-types.h
 */

/**
 * @ingroup CAPI_IOT_CONNECTIVITY_COMMON_MODULE
 * @defgroup CAPI_IOT_CONNECTIVITY_COMMON_RESOURCE_TYPES_MODULE Resource Types
 *
 * @brief IoTCon Resource Types provides API to manage resource types.
 *
 * @section CAPI_IOT_CONNECTIVITY_COMMON_RESOURCE_TYPES_MODULE_HEADER Required Header
 *  \#include <iotcon.h>
 *
 * @section CAPI_IOT_CONNECTIVITY_COMMON_RESOURCE_TYPES_MODULE_OVERVIEW Overview
 * The iotcon resource types API provides methods for managing handle and add, remove resource types.
 * A resource type indicates a class or category of resources.
 *
 * Example :
 * @code
#include <iotcon.h>
static void _request_handler(iotcon_resource_h resource, iotcon_request_h request,
		void *user_data)
{
	// handle request
	...
}

static void _create_light_resource()
{
	int ret;
	iotcon_resource_h resource = NULL;
	iotcon_resource_interfaces_h resource_ifaces = NULL;
	iotcon_resource_types_h resource_types = NULL;

	ret = iotcon_resource_types_create(&resource_types);
	if (IOTCON_ERROR_NONE != ret)
		return;

	ret = iotcon_resource_types_add(resource_types, "org.tizen.light");
	if (IOTCON_ERROR_NONE != ret) {
		iotcon_resource_types_destroy(resource_types);
		return;
	}

	ret = iotcon_resource_interfaces_create(&resource_ifaces);
	if (IOTCON_ERROR_NONE != ret)
		iotcon_resource_types_destroy(resource_types);
		return;

	ret = iotcon_resource_interfaces_add(resource_ifaces, IOTCON_INTERFACE_DEFAULT);
	if (IOTCON_ERROR_NONE != ret) {
		iotcon_resource_interfaces_destroy(resource_ifaces);
		iotcon_resource_types_destroy(resource_types);
		return;
	}

	ret = iotcon_resource_create("/light/1", resource_types, resource_ifaces,
			IOTCON_RESOURCE_DISCOVERABLE | IOTCON_RESOURCE_OBSERVABLE, _request_handler, NULL, &resource);
	if (IOTCON_ERROR_NONE != ret) {
		iotcon_resource_interfaces_destroy(resource_ifaces);
		iotcon_resource_types_destroy(resource_types);
		return;
	}

	iotcon_resource_interfaces_destroy(resource_ifaces);
	iotcon_resource_types_destroy(resource_types);
}
 * @endcode
 *
 * @section CAPI_IOT_CONNECTIVITY_COMMON_RESOURCE_TYPES_MODULE_FEATURE Related Features
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
 * @brief Creates a new resource types handle.
 *
 * @since_tizen 3.0
 *
 * @remarks You must destroy @a types by calling iotcon_resource_types_destroy()
 * if @a types is no longer needed.
 *
 * @param[out] types A newly allocated list of resource types handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_resource_types_destroy()
 * @see iotcon_resource_types_add()
 * @see iotcon_resource_types_remove()
 * @see iotcon_resource_types_clone()
 */
int iotcon_resource_types_create(iotcon_resource_types_h *types);

/**
 * @brief Destroys a resource types handle.
 *
 * @since_tizen 3.0
 *
 * @param[in] types The handle of the resource types
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_resource_types_create()
 * @see iotcon_resource_types_add()
 * @see iotcon_resource_types_remove()
 * @see iotcon_resource_types_clone()
 */
int iotcon_resource_types_destroy(iotcon_resource_types_h types);

/**
 * @brief Inserts a resource type into the list.
 *
 * @since_tizen 3.0
 * @remarks The length of @a type should be less than or equal to 61.\n
 * The @a type must start with a lowercase alphabetic character, followed by a sequence
 * of lowercase alphabetic, numeric, ".", or "-" characters, and contains no white space.\n
 * Duplicate strings are not allowed.
 *
 * @param[in] types The handle of the resource types
 * @param[in] type The string data to insert into the resource types (e.g. "org.tizen.light")
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_ALREADY  Already done
 *
 * @see iotcon_resource_types_create()
 * @see iotcon_resource_types_destroy()
 * @see iotcon_resource_types_remove()
 * @see iotcon_resource_types_clone()
 */
int iotcon_resource_types_add(iotcon_resource_types_h types, const char *type);

/**
 * @brief Delete a resource type form the list.
 *
 * @since_tizen 3.0
 *
 * @param[in] types The handle of the resource types
 * @param[in] type The string data to delete from the resource types
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_NO_DATA  No data available
 *
 * @see iotcon_resource_types_create()
 * @see iotcon_resource_types_destroy()
 * @see iotcon_resource_types_add()
 * @see iotcon_resource_types_clone()
 */
int iotcon_resource_types_remove(iotcon_resource_types_h types, const char *type);

/**
 * @brief Specifies the type of function passed to iotcon_resource_types_foreach().
 *
 * @since_tizen 3.0
 *
 * @param[in] type The value of the resource types
 * @param[in] user_data The user data to pass to the function
 *
 * @return true to continue with the next iteration of the loop,
 * otherwise false to break out of the loop. #IOTCON_FUNC_CONTINUE and #IOTCON_FUNC_STOP
 * are more friendly values for the return.
 *
 * @pre iotcon_resource_types_foreach() will invoke this callback function.
 *
 * @see iotcon_resource_types_foreach()
 */
typedef bool (*iotcon_resource_types_foreach_cb)(const char *type, void *user_data);

/**
 * @brief Gets all of the resource types of the list by invoking the callback function.
 * @details iotcon_resource_types_foreach_cb() will be called for each type.\n
 * If iotcon_resource_types_foreach_cb() returns false, iteration will be stop.
 *
 * @since_tizen 3.0
 *
 * @param[in] types The handle of resource types
 * @param[in] cb The callback function to get data
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @post iotcon_resource_types_foreach() will be called for each type.
 *
 * @see iotcon_resource_types_foreach_cb()
 */
int iotcon_resource_types_foreach(iotcon_resource_types_h types,
		iotcon_resource_types_foreach_cb cb, void *user_data);

/**
 * @brief Clones the resource types handle.
 * @details Makes a deep copy of a source list of resource types.
 *
 * @since_tizen 3.0
 *
 * @remarks You must @a destroy dest by calling iotcon_resource_types_destroy()
 * if @a dest is no longer needed.
 *
 * @param[in] src The origin handle of the resource types
 * @param[out] dest Clone of a source list of resource types
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_resource_types_create()
 * @see iotcon_resource_types_destroy()
 * @see iotcon_resource_types_add()
 * @see iotcon_resource_types_remove()
 */
int iotcon_resource_types_clone(iotcon_resource_types_h src,
		iotcon_resource_types_h *dest);

/**
 * @}
 */

#endif /* __IOTCON_STRUCT_RESOURCE_TYPES_H__ */
