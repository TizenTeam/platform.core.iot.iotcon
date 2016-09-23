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
#ifndef __IOTCON_CLIENT_REMOTE_RESOURCE_H__
#define __IOTCON_CLIENT_REMOTE_RESOURCE_H__

#include <stdint.h>
#include <iotcon-types.h>

/**
 * @file iotcon-remote-resource.h
 */

/**
 * @ingroup CAPI_IOT_CONNECTIVITY_CLIENT_MODULE
 * @defgroup CAPI_IOT_CONNECTIVITY_CLIENT_REMOTE_RESOURCE_MODULE Remote Resource
 *
 * @brief IoTCon Remote Resource provides API to manage remote resource.
 *
 * @section CAPI_IOT_CONNECTIVITY_CLIENT_REMOTE_RESOURCE_MODULE_HEADER Required Header
 *  \#include <iotcon.h>
 *
 * @section CAPI_IOT_CONNECTIVITY_CLIENT_REMOTE_RESOURCE_MODULE_OVERVIEW Overview
 * The iotcon remote resource API provides methods for managing resource handle and send request.
 *
 * Example :
 * @code
#include <iotcon.h>
...
static void _on_get(iotcon_remote_resource_h resource, iotcon_error_e err,
		iotcon_request_type_e request_type, iotcon_response_h response, void *user_data)
{
	if (IOTCON_ERROR_NONE != err)
		return;

	// handle get result
	...
}

static bool _on_find(iotcon_remote_resource_h resource, iotcon_error_e result,
		void *user_data)
{
	int ret;
	iotcon_remote_resource_h resource_clone = NULL;

	if (IOTCON_ERROR_NONE != result)
		return IOTCON_FUNC_STOP;

	if (NULL == resource)
		return IOTCON_FUNC_CONTINUE;

	// clone handle
	ret = iotcon_remote_resource_clone(resource, &resource_clone);
	if (IOTCON_ERROR_NONE != ret)
		return IOTCON_FUNC_CONTINUE;

	// request get
	ret = iotcon_remote_resource_get(resource_clone, NULL, _on_get, NULL);
	if (IOTCON_ERROR_NONE != ret) {
		iotcon_remote_resource_destroy(resource_clone);
		return IOTCON_FUNC_CONTINUE;
	}

	...

	return IOTCON_FUNC_CONTINUE;
}

static void _find_light_resource()
{
	int ret;

	ret = iotcon_find_resource(IOTCON_MULTICAST_ADDRESS, IOTCON_CONNECTIVITY_IPV4,
			"org.tizen.light", false, _on_find, NULL);
	if (IOTCON_ERROR_NONE != ret)
		return;
}

 * @endcode
 *
 * @section CAPI_IOT_CONNECTIVITY_CLIENT_REMOTE_RESOURCE_MODULE_FEATURE Related Features
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
 * @brief Creates a new resource handle.
 * @details Creates a resource proxy object so that iotcon_remote_resource_get(),
 * iotcon_remote_resource_put(), iotcon_remote_resource_post(),
 * iotcon_remote_resource_delete(), iotcon_remote_resource_observe_register(),
 * iotcon_remote_resource_start_caching() and iotcon_remote_resource_start_monitoring()
 * API can be used without discovering the object in advance.\n
 * To use this API, you should provide all of the details required to correctly contact and
 * observe the object.\n
 * If not, you should discover the resource object manually.\n
 * The @a policies can contain multiple policies like
 * IOTCON_RESOURCE_DISCOVERABLE | IOTCON_RESOURCE_OBSERVABLE.\n
 *
 * @since_tizen 3.0
 *
 * @remarks You must destroy @a remote_resource by calling iotcon_remote_resource_destroy()
 * if @a remote_resource is no longer needed.
 *
 * @param[in] host_address The host address of the resource
 * @param[in] connectivity_type The connectivity type. Can not use @a IOTCON_CONNECTIVITY_ALL. \n
 * Connectivity type of specific remote resource(It has specific host address, not @a IOTCON_MULTICAST_ADDRESS) is already decided. \n
 * Use specific connectivity type of the remote resource. \n
 * You can refer to the @a iotcon_remote_resource_get_connectivity_type().
 * @param[in] uri_path The URI path of the resource
 * @param[in] policies The policies of the resource\n Set of #iotcon_resource_policy_e
 * @param[in] resource_types The resource types of the resource. For example, "core.light"
 * @param[in] resource_ifaces The resource interfaces of the resource.
 * @param[out] remote_resource Generated resource handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_IOTIVITY  IoTivity errors
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 *
 * @see iotcon_remote_resource_destroy()
 * @see iotcon_remote_resource_clone()
 */
int iotcon_remote_resource_create(const char *host_address,
		iotcon_connectivity_type_e connectivity_type,
		const char *uri_path,
		uint8_t policies,
		iotcon_resource_types_h resource_types,
		iotcon_resource_interfaces_h resource_ifaces,
		iotcon_remote_resource_h *remote_resource);

/**
 * @brief Destroys a resource handle.
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_remote_resource_create()
 * @see iotcon_remote_resource_clone()
 */
int iotcon_remote_resource_destroy(iotcon_remote_resource_h resource);

/**
 * @brief Clones a clone of a remote resource.
 *
 * @since_tizen 3.0
 *
 * @remarks You must destroy @a dest by calling iotcon_remote_resource_destroy()
 * if @a dest is no longer needed.
 *
 * @param[in] src The Source of resource
 * @param[out] dest The cloned resource handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_remote_resource_create()
 * @see iotcon_remote_resource_destroy()
 */
int iotcon_remote_resource_clone(iotcon_remote_resource_h src, iotcon_remote_resource_h *dest);

/**
 * @brief Specifies the type of observe callback passed to
 * iotcon_remote_resource_observe_register().
 * The @a err could be one of #iotcon_error_e.\n
 * The @a response is created by a server. Therefore, you can't get any values that the server didn't set.
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the remote resource
 * @param[in] err The error code
 * @param[in] sequence_number The sequence number of observe
 * @param[in] response The handle of the response
 * @param[in] user_data The user data passed from the callback registration function
 *
 * @pre The callback must be registered using iotcon_remote_resource_observe_register()
 *
 * @see iotcon_remote_resource_observe_register()
 * @see iotcon_remote_resource_observe_deregister()
 * @see iotcon_resource_notify()
 */
typedef void (*iotcon_remote_resource_observe_cb)(iotcon_remote_resource_h resource,
		iotcon_error_e err, int sequence_number, iotcon_response_h response, void *user_data);

/**
 * @brief Registers observe callback on the resource.
 * @details When server sends notification message, iotcon_remote_resource_observe_cb() will be called.
 * The @a observe_policy could be one of #iotcon_observe_policy_e.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the resource
 * @param[in] observe_policy The type to specify how client wants to observe.
 * @param[in] query The query to send to server
 * @param[in] cb The callback function to get notifications from server
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_IOTIVITY  IoTivity errors
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @pre iotcon_initialize() should be called to initialize.
 * @post When the @a resource receive notification message, iotcon_remote_resource_observe_cb() will be called.
 *
 * @see iotcon_remote_resource_observe_cb()
 * @see iotcon_remote_resource_observe_deregister()
 * @see iotcon_resource_notify()
 */
int iotcon_remote_resource_observe_register(iotcon_remote_resource_h resource,
		iotcon_observe_policy_e observe_policy,
		iotcon_query_h query,
		iotcon_remote_resource_observe_cb cb,
		void *user_data);

/**
 * @brief Deregisters observe callback on the resource.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @pre iotcon_initialize() should be called to initialize.
 *
 * @see iotcon_remote_resource_observe_cb()
 * @see iotcon_remote_resource_observe_register()
 * @see iotcon_resource_notify()
 */
int iotcon_remote_resource_observe_deregister(iotcon_remote_resource_h resource);

/**
 * @brief Specifies the type of response function.
 * @details The function passed to iotcon_remote_resource_get(), iotcon_remote_resource_put(),
 * iotcon_remote_resource_post(), iotcon_remote_resource_delete().\n
 * The @a err could be one of #iotcon_error_e.\n
 * The @a request_type could be one of #iotcon_request_type_e.\n
 * The @a response is created by a server. Therefore, you can't get any values that the server didn't set.
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the resource
 * @param[in] err The error code
 * @param[in] request_type The request type
 * @param[in] response The handle of the response
 * @param[in] user_data The user data passed from the callback registration function
 *
 * @pre The callback must be registered using iotcon_remote_resource_get(),
 * iotcon_remote_resource_put(), iotcon_remote_resource_post(), iotcon_remote_resource_delete()
 *
 * @see iotcon_remote_resource_get()
 * @see iotcon_remote_resource_put()
 * @see iotcon_remote_resource_post()
 * @see iotcon_remote_resource_delete()
 */
typedef void (*iotcon_remote_resource_response_cb)(iotcon_remote_resource_h resource,
		iotcon_error_e err,
		iotcon_request_type_e request_type,
		iotcon_response_h response,
		void *user_data);

/**
 * @brief Gets the attributes of a resource, asynchronously.
 * @details When server sends response on get request, iotcon_remote_resource_response_cb() will be called.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the resource
 * @param[in] query The query to send to server
 * @param[in] cb The callback function
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @pre iotcon_initialize() should be called to initialize.
 * @post When the client receive get response, iotcon_remote_resource_response_cb() will be called.
 *
 * @see iotcon_remote_resource_response_cb()
 * @see iotcon_set_timeout()
 */
int iotcon_remote_resource_get(iotcon_remote_resource_h resource, iotcon_query_h query,
		iotcon_remote_resource_response_cb cb, void *user_data);

/**
 * @brief Puts the representation of a resource, asynchronously.
 * @details When server sends response on put request, iotcon_remote_resource_response_cb() will be called.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the resource
 * @param[in] repr The handle of the representation
 * @param[in] query The query to send to server
 * @param[in] cb The callback function
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @pre iotcon_initialize() should be called to initialize.
 * @post When the client receive put response, iotcon_remote_resource_response_cb() will be called.
 *
 * @see iotcon_remote_resource_response_cb()
 * @see iotcon_set_timeout()
 */
int iotcon_remote_resource_put(iotcon_remote_resource_h resource,
		iotcon_representation_h repr,
		iotcon_query_h query,
		iotcon_remote_resource_response_cb cb,
		void *user_data);

/**
 * @brief Posts on a resource, asynchronously.
 * @details When server sends response on post request, iotcon_remote_resource_response_cb() will be called.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the resource
 * @param[in] repr The handle of the representation
 * @param[in] query The query to send to server
 * @param[in] cb The callback function
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @pre iotcon_initialize() should be called to initialize.
 * @post When the client receive post response, iotcon_remote_resource_response_cb() will be called.
 *
 * @see iotcon_remote_resource_response_cb()
 * @see iotcon_set_timeout()
 */
int iotcon_remote_resource_post(iotcon_remote_resource_h resource,
		iotcon_representation_h repr,
		iotcon_query_h query,
		iotcon_remote_resource_response_cb cb,
		void *user_data);

/**
 * @brief Deletes a resource, asynchronously.
 * @details When server sends response on delete request, iotcon_remote_resource_response_cb() will be called.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the resource
 * @param[in] cb The callback function
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @pre iotcon_initialize() should be called to initialize.
 * @post When the client receive delete response, iotcon_remote_resource_response_cb() will be called.
 *
 * @see iotcon_remote_resource_response_cb()
 * @see iotcon_set_timeout()
 */
int iotcon_remote_resource_delete(iotcon_remote_resource_h resource,
		iotcon_remote_resource_response_cb cb, void *user_data);

/**
 * @brief Specifies the type of function passed to iotcon_remote_resource_start_caching().
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the remote resource
 * @param[in] representation The handle of the representation
 * @param[in] user_data The user data to pass to the function
 *
 * @pre The callback must be registered using iotcon_remote_resource_start_caching()
 *
 * @see iotcon_remote_resource_start_caching()
 * @see iotcon_remote_resource_stop_caching()
 */
typedef void (*iotcon_remote_resource_cached_representation_changed_cb)(
		iotcon_remote_resource_h resource,
		iotcon_representation_h representation,
		void *user_data);

/**
 * @brief Starts caching of a remote resource.
 * @details Use this function to start caching the resource's attribute.\n
 * Although, remote resource is not observable, it keeps the representation up-to-date.
 * Because It checks whether representation is changed, periodically.\n
 * Therefore, you can get the cached representation even when the remote resource is off-line.
 * The default checking interval is 10 seconds.
 * It can be changed by iotcon_remote_resource_set_checking_interval().
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the remote resource to be cached
 * @param[in] cb The callback function to add into callback list
 * @param[in] user_data The user data to pass to the callback function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_ALREADY Already done
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @pre iotcon_initialize() should be called to initialize.
 *
 * @see iotcon_remote_resource_stop_caching()
 * @see iotcon_remote_resource_cached_representation_changed_cb()
 */
int iotcon_remote_resource_start_caching(iotcon_remote_resource_h resource,
		iotcon_remote_resource_cached_representation_changed_cb cb, void *user_data);

/**
 * @brief Stops caching of a remote resource.
 * @details Use this function to stop caching the resource's attribute.\n
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the remote resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @pre iotcon_initialize() should be called to initialize.
 *
 * @see iotcon_remote_resource_start_caching()
 * @see iotcon_remote_resource_cached_representation_changed_cb()
 */
int iotcon_remote_resource_stop_caching(iotcon_remote_resource_h resource);

/**
 * @brief Specifies the type of function passed to iotcon_remote_resource_start_monitoring().
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the remote resource
 * @param[in] state The state of the remote resource
 * @param[in] user_data The user data to pass to the function
 *
 * @pre The callback must be registered using iotcon_remote_resource_start_monitoring()
 *
 * @see iotcon_remote_resource_start_monitoring()
 * @see iotcon_remote_resource_stop_monitoring()
 */
typedef void (*iotcon_remote_resource_state_changed_cb)(iotcon_remote_resource_h resource,
		iotcon_remote_resource_state_e state, void *user_data);

/**
 * @brief Starts monitoring of a remote resource.
 * @details When remote resource's state are changed, registered callbacks will be called
 * in turn. Although, remote resource does not call iotcon_start_presence(), it knows
 * the state of resource. Because it checks the state of resource, periodically.\n
 * The default checking interval is 10 seconds.
 * It can be changed by iotcon_remote_resource_set_checking_interval().
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the remote resource
 * @param[in] cb The callback function to add into callback list
 * @param[in] user_data The user data to pass to the callback function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_ALREADY  Already done
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @pre iotcon_initialize() should be called to initialize.
 *
 * @see iotcon_remote_resource_stop_monitoring()
 * @see iotcon_remote_resource_state_changed_cb()
 */
int iotcon_remote_resource_start_monitoring(iotcon_remote_resource_h resource,
		iotcon_remote_resource_state_changed_cb cb, void *user_data);

/**
 * @brief Stops monitoring of a remote resource.
 * @details Use this function to stop monitoring the remote resource.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the remote resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @pre iotcon_initialize() should be called to initialize.
 *
 * @see iotcon_remote_resource_start_monitoring()
 * @see iotcon_remote_resource_state_changed_cb()
 */
int iotcon_remote_resource_stop_monitoring(iotcon_remote_resource_h resource);

/**
 * @brief Gets an URI path of the remote resource.
 *
 * @since_tizen 3.0
 *
 * @remarks @a uri_path must not be released using free().
 *
 * @param[in] resource The handle of the remote resource
 * @param[out] uri_path The URI path of the remote resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_remote_resource_get_host_address()
 * @see iotcon_remote_resource_get_connectivity_type()
 * @see iotcon_remote_resource_get_device_id()
 * @see iotcon_remote_resource_get_device_name()
 * @see iotcon_remote_resource_get_types()
 * @see iotcon_remote_resource_get_interfaces()
 * @see iotcon_remote_resource_get_policies()
 * @see iotcon_remote_resource_set_options()
 */
int iotcon_remote_resource_get_uri_path(iotcon_remote_resource_h resource,
		char **uri_path);

/**
 * @brief Gets a connectivity type of the remote resource.
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the remote resource
 * @param[out] connectivity_type The connectivity type of the remote resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter

 * @see iotcon_remote_resource_get_uri_path()
 * @see iotcon_remote_resource_get_host_address()
 * @see iotcon_remote_resource_get_connectivity_type()
 * @see iotcon_remote_resource_get_device_id()
 * @see iotcon_remote_resource_get_device_name()
 * @see iotcon_remote_resource_get_types()
 * @see iotcon_remote_resource_get_interfaces()
 * @see iotcon_remote_resource_get_policies()
 * @see iotcon_remote_resource_set_options()
 */
int iotcon_remote_resource_get_connectivity_type(iotcon_remote_resource_h resource,
		iotcon_connectivity_type_e *connectivity_type);

/**
 * @brief Gets a host address of the remote resource.
 *
 * @since_tizen 3.0
 *
 * @remarks @a host_address must not be released using free().
 *
 * @param[in] resource The handle of the remote resource
 * @param[out] host_address The host address of the remote resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_remote_resource_get_connectivity_type()
 * @see iotcon_remote_resource_get_uri_path()
 * @see iotcon_remote_resource_get_device_id()
 * @see iotcon_remote_resource_get_device_name()
 * @see iotcon_remote_resource_get_types()
 * @see iotcon_remote_resource_get_interfaces()
 * @see iotcon_remote_resource_get_policies()
 * @see iotcon_remote_resource_set_options()
 */
int iotcon_remote_resource_get_host_address(iotcon_remote_resource_h resource,
		char **host_address);

/**
 * @brief Gets a device id of the remote resource.
 *
 * @since_tizen 3.0
 *
 * @remarks @a device_id must not be released using free().\n
 * If @a resource is created by calling iotcon_remote_resource_create(), you cannot get
 * @a device_id. In this case, the return value of this function is #IOTCON_ERROR_NO_DATA.
 *
 * @param[in] resource The handle of the remote resource
 * @param[out] device_id The device id of the remote resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_NO_DATA No data available
 *
 * @see iotcon_remote_resource_get_uri_path()
 * @see iotcon_remote_resource_get_device_name()
 * @see iotcon_remote_resource_get_host_address()
 * @see iotcon_remote_resource_get_connectivity_type()
 * @see iotcon_remote_resource_get_types()
 * @see iotcon_remote_resource_get_interfaces()
 * @see iotcon_remote_resource_get_policies()
 * @see iotcon_remote_resource_set_options()
 */
int iotcon_remote_resource_get_device_id(iotcon_remote_resource_h resource,
		char **device_id);

/**
 * @brief Gets the device name of the remote resource.
 *
 * @since_tizen 3.0
 *
 * @remarks @a device_name must not be released using free().\n
 * If @a resource is created by calling iotcon_remote_resource_create(), you cannot get
 * @a device_name. In this case, the return value of this function is #IOTCON_ERROR_NO_DATA.
 *
 * @param[in] resource The handle of the remote resource
 * @param[out] device_name The device name of the remote resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_NO_DATA No data available
 *
 * @see iotcon_set_device_name()
 * @see iotcon_remote_resource_get_uri_path()
 * @see iotcon_remote_resource_get_device_id()
 * @see iotcon_remote_resource_get_device_name()
 * @see iotcon_remote_resource_get_host_address()
 * @see iotcon_remote_resource_get_connectivity_type()
 * @see iotcon_remote_resource_get_types()
 * @see iotcon_remote_resource_get_interfaces()
 * @see iotcon_remote_resource_get_properties()
 * @see iotcon_remote_resource_set_options()
 */
int iotcon_remote_resource_get_device_name(iotcon_remote_resource_h resource,
		char **device_name);

/**
 * @brief Gets resource types of the remote resource.
 *
 * @since_tizen 3.0
 *
 * @remarks @a types must not be released using iotcon_resource_types_destroy().
 *
 * @param[in] resource The handle of the remote resource
 * @param[out] types The resource types of the remote resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_remote_resource_get_uri_path()
 * @see iotcon_remote_resource_get_host_address()
 * @see iotcon_remote_resource_get_connectivity_type()
 * @see iotcon_remote_resource_get_device_id()
 * @see iotcon_remote_resource_get_device_name()
 * @see iotcon_remote_resource_get_interfaces()
 * @see iotcon_remote_resource_get_policies()
 * @see iotcon_remote_resource_set_options()
 */
int iotcon_remote_resource_get_types(iotcon_remote_resource_h resource,
		iotcon_resource_types_h *types);

/**
 * @brief Gets resource interfaces of the remote resource.
 *
 * @since_tizen 3.0
 *
 * @remarks @a ifaces must not be released using iotcon_resource_interfaces_destroy().
 *
 * @param[in] resource The handle of the remote resource
 * @param[out] ifaces The resource interfaces of the remote resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_remote_resource_get_uri_path()
 * @see iotcon_remote_resource_get_host_address()
 * @see iotcon_remote_resource_get_connectivity_type()
 * @see iotcon_remote_resource_get_device_id()
 * @see iotcon_remote_resource_get_device_name()
 * @see iotcon_remote_resource_get_types()
 * @see iotcon_remote_resource_get_policies()
 * @see iotcon_remote_resource_set_options()
 */
int iotcon_remote_resource_get_interfaces(iotcon_remote_resource_h resource,
		iotcon_resource_interfaces_h *ifaces);

/**
 * @brief Checks whether the remote resource is observable or not.
 *
 * @details The @a policies can contain multiple policies like
 * IOTCON_RESOURCE_DISCOVERABLE | IOTCON_RESOURCE_OBSERVABLE.
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the resource
 * @param[out] policies The policies of the resource\n Set of #iotcon_resource_policy_e
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_remote_resource_get_uri_path()
 * @see iotcon_remote_resource_get_host_address()
 * @see iotcon_remote_resource_get_connectivity_type()
 * @see iotcon_remote_resource_get_device_id()
 * @see iotcon_remote_resource_get_device_name()
 * @see iotcon_remote_resource_get_types()
 * @see iotcon_remote_resource_get_interfaces()
 * @see iotcon_remote_resource_set_options()
 */
int iotcon_remote_resource_get_policies(iotcon_remote_resource_h resource,
		uint8_t *policies);


/**
 * @brief Gets options of the remote resource.
 *
 * @since_tizen 3.0
 *
 * @remarks @a options must not be released using iotcon_options_destroy().
 *
 * @param[in] resource The handle of the remote resource
 * @param[out] options The handle of the header options
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_remote_resource_get_uri_path()
 * @see iotcon_remote_resource_get_host_address()
 * @see iotcon_remote_resource_get_connectivity_type()
 * @see iotcon_remote_resource_get_device_id()
 * @see iotcon_remote_resource_get_device_name()
 * @see iotcon_remote_resource_get_types()
 * @see iotcon_remote_resource_get_interfaces()
 * @see iotcon_remote_resource_set_options()
 * @see iotcon_remote_resource_get_policies()
 */
int iotcon_remote_resource_get_options(iotcon_remote_resource_h resource,
		iotcon_options_h *options);

/**
 * @brief Sets options into the remote resource.
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the remote resource
 * @param[in] options The handle of the header options
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_remote_resource_get_uri_path()
 * @see iotcon_remote_resource_get_host_address()
 * @see iotcon_remote_resource_get_connectivity_type()
 * @see iotcon_remote_resource_get_device_id()
 * @see iotcon_remote_resource_get_device_name()
 * @see iotcon_remote_resource_get_types()
 * @see iotcon_remote_resource_get_interfaces()
 * @see iotcon_remote_resource_get_options()
 * @see iotcon_remote_resource_get_policies()
 */
int iotcon_remote_resource_set_options(iotcon_remote_resource_h resource,
		iotcon_options_h options);

/**
 * @brief Gets cached representation from the remote resource.
 *
 * @since_tizen 3.0
 *
 * @remarks @a representation must not be released using iotcon_representation_destroy().
 *
 * @param[in] resource The handle of the remote resource
 * @param[out] representation The handle of the representation
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 */
int iotcon_remote_resource_get_cached_representation(
		iotcon_remote_resource_h resource,
		iotcon_representation_h *representation);

/**
 * @brief Gets the checking interval which is using on the monitoring & caching of remote resource.
 * @details This API get the checking interval which is using in the process of monitoring
 * and caching() of remote resource.\n
 * The monitoring and caching will operate "GET method" with the checking interval.
 * Default checking interval is 10 seconds.
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the remote resource
 * @param[out] interval Seconds for time interval
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see iotcon_remote_resource_set_checking_interval()
 * @see iotcon_remote_resource_start_monitoring()
 * @see iotcon_remote_resource_start_caching()
 */
int iotcon_remote_resource_get_checking_interval(iotcon_remote_resource_h resource,
		int *interval);

/**
 * @brief Sets the checking interval which is using on the monitoring & caching of remote resource.
 * @details This API set the checking interval which is using in the process of monitoring
 * and caching() of remote resource.\n
 * The monitoring and caching will operate "GET method" with the checking interval changed by this function.
 * Default checking interval is 10 seconds.
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the remote resource
 * @param[in] interval Seconds for time interval (must be in range from 1 to 3600)
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see iotcon_remote_resource_get_checking_interval()
 * @see iotcon_remote_resource_start_monitoring()
 * @see iotcon_remote_resource_start_caching()
 */
int iotcon_remote_resource_set_checking_interval(iotcon_remote_resource_h resource,
		int interval);

/**
 * @}
 */

#endif /* __IOTCON_CLIENT_REMOTE_RESOURCE_H__ */
