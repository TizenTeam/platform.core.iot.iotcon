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
#ifndef __IOTCON_CLIENT_H__
#define __IOTCON_CLIENT_H__

#include <iotcon-types.h>
#include <iotcon-remote-resource.h>

/**
 * @file iotcon-client.h
 */

/**
 * @ingroup CAPI_IOT_CONNECTIVITY_MODULE
 * @defgroup CAPI_IOT_CONNECTIVITY_CLIENT_MODULE Client
 *
 * @brief IoTCon Client provides API for client side.
 *
 * @section CAPI_IOT_CONNECTIVITY_CLIENT_MODULE_HEADER Required Header
 *  \#include <iotcon.h>
 *
 * @section CAPI_IOT_CONNECTIVITY_CLIENT_MODULE_OVERVIEW Overview
 * This API set consists of client side API for @ref CAPI_IOT_CONNECTIVITY_CLIENT_REMOTE_RESOURCE_MODULE.
 *
 * @section CAPI_IOT_CONNECTIVITY_CLIENT_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 * - http://tizen.org/feature/iot.ocf\n
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
 * @brief Specifies the type of function passed to iotcon_add_presence_cb().
 * @details Called when client receive presence events from the server.
 * The @a response handle contains about presence information.
 *
 * @since_tizen 3.0
 *
 * @param[in] presence The presence handle
 * @param[in] err The error code(0 on success, otherwise a negative error value)
 * @param[in] response The presence response handle
 * @param[in] user_data The user data to pass to the function
 *
 * @pre The callback must be registered using iotcon_add_presence_cb()
 *
 * @see iotcon_add_presence_cb()
 * @see iotcon_remove_presence_cb()
 * @see iotcon_presence_response_get_connectivity_type()
 * @see iotcon_presence_response_get_host_address()
 * @see iotcon_presence_response_get_resource_type()
 * @see iotcon_presence_response_get_result()
 * @see iotcon_presence_response_get_trigger()
 */
typedef void (*iotcon_presence_cb)(iotcon_presence_h presence, iotcon_error_e err,
		iotcon_presence_response_h response, void *user_data);

/**
 * @brief Adds callback to a server to receive presence events.
 * @details Request to receive presence to an interested server's resource with @a resource_type.\n
 * If succeed to subscribe, iotcon_presence_cb() will be invoked when the server sends presence.\n
 * A server sends presence events when adds/removes/alters a resource or start/stop presence.\n
 * @a host_address could be #IOTCON_MULTICAST_ADDRESS for IPv4 multicast.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @remarks The length of @a resource_type should be less than or equal to 61.\n
 * The @a resource_type must start with a lowercase alphabetic character, followed by a sequence
 * of lowercase alphabetic, numeric, ".", or "-" characters, and contains no white space.\n
 * You must destroy @a presence by calling iotcon_unsubscribe_presence()
 * if @a presence is no longer needed.
 *
 * @param[in] host_address The address or addressable name of the server
 * @param[in] connectivity_type The connectivity type
 * @param[in] resource_type A resource type that a client has interested in
 * @param[in] cb The callback function to invoke
 * @param[in] user_data The user data to pass to the function
 * @param[out] presence_handle The generated presence handle
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
 * @post When the resource receive presence, iotcon_presence_cb() will be called.
 *
 * @see iotcon_start_presence()
 * @see iotcon_stop_presence()
 * @see iotcon_remove_presence_cb()
 * @see iotcon_presence_cb()
 */
int iotcon_add_presence_cb(const char *host_address,
		iotcon_connectivity_type_e connectivity_type,
		const char *resource_type,
		iotcon_presence_cb cb,
		void *user_data,
		iotcon_presence_h *presence_handle);

/**
 * @brief Removes callback to a server's presence events.
 * @details Request not to receive server's presence any more.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] presence_handle The presence handle to be unsubscribed
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
 * @see iotcon_start_presence()
 * @see iotcon_stop_presence()
 * @see iotcon_add_presence_cb()
 * @see iotcon_presence_cb()
 */
int iotcon_remove_presence_cb(iotcon_presence_h presence_handle);

/**
 * @brief Gets host address from the presence handle.
 *
 * @since_tizen 3.0
 *
 * @remarks @a host_address must not be released using free().
 *
 * @param[in] presence The handle of the presence
 * @param[out] host_address The host address of the presence
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_presence_get_connectivity_type()
 * @see iotcon_presence_get_resource_type()
 */
int iotcon_presence_get_host_address(iotcon_presence_h presence, char **host_address);

/**
 * @brief Gets connectivity type from the presence handle.
 * @details The @a connectivity_type could be one of #iotcon_connectivity_type_e.
 *
 * @since_tizen 3.0
 *
 * @param[in] presence The handle of the presence
 * @param[out] connectivity_type The connectivity type of the presence
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_presence_get_host_address()
 * @see iotcon_presence_get_resource_type()
 */
int iotcon_presence_get_connectivity_type(iotcon_presence_h presence,
		iotcon_connectivity_type_e *connectivity_type);

/**
 * @brief Gets resource type from the presence handle.
 *
 * @since_tizen 3.0
 *
 * @remarks @a resource_type must not be released using free().
 *
 * @param[in] presence The handle of the presence
 * @param[out] resource_type The resource type of the presence
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_presence_get_host_address()
 * @see iotcon_presence_get_connectivity_type()
 */
int iotcon_presence_get_resource_type(iotcon_presence_h presence,
		char **resource_type);

/**
 * @brief Gets result from the presence response handle.
 *
 * @details The @a result could be one of #iotcon_presence_result_e.
 * @since_tizen 3.0
 *
 * @param[in] response The handle of the presence response
 * @param[out] result The result code
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_presence_response_get_trigger()
 * @see iotcon_presence_response_get_host_address()
 * @see iotcon_presence_response_get_connectivity_type()
 * @see iotcon_presence_response_get_resource_type()
 */
int iotcon_presence_response_get_result(iotcon_presence_response_h response,
		iotcon_presence_result_e *result);

/**
 * @brief Gets trigger from the presence response handle.
 *
 * @details The @a trigger could be one of #iotcon_presence_trigger_e.
 * It is set only if a response result is IOTCON_PRESENCE_OK.
 * @since_tizen 3.0
 *
 * @param[in] response The handle of the presence response
 * @param[out] trigger The presence trigger value.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_presence_response_get_result()
 * @see iotcon_presence_response_get_host_address()
 * @see iotcon_presence_response_get_connectivity_type()
 * @see iotcon_presence_response_get_resource_type()
 */
int iotcon_presence_response_get_trigger(iotcon_presence_response_h response,
		iotcon_presence_trigger_e *trigger);

/**
 * @brief Gets host address from the presence response handle.
 *
 * @since_tizen 3.0
 *
 * @remarks @a host_address must not be released using free().
 *
 * @param[in] response The handle of the presence response
 * @param[out] host_address The host address of the presence response
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_presence_response_get_result()
 * @see iotcon_presence_response_get_trigger()
 * @see iotcon_presence_response_get_connectivity_type()
 * @see iotcon_presence_response_get_resource_type()
 */
int iotcon_presence_response_get_host_address(iotcon_presence_response_h response,
		char **host_address);

/**
 * @brief Gets connectivity type from the presence response handle.
 *
 * @details The @a connectivity_type could be one of #iotcon_connectivity_type_e.
 * @since_tizen 3.0
 *
 * @param[in] response The handle of the presence response
 * @param[out] connectivity_type The connectivity type of the presence response
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_presence_response_get_result()
 * @see iotcon_presence_response_get_trigger()
 * @see iotcon_presence_response_get_host_address()
 * @see iotcon_presence_response_get_resource_type()
 */
int iotcon_presence_response_get_connectivity_type(iotcon_presence_response_h response,
		iotcon_connectivity_type_e *connectivity_type);

/**
 * @brief Gets resource type from the presence response handle.
 *
 * @since_tizen 3.0
 *
 * @remarks @a resource_type must not be released using free().
 *
 * @param[in] response The handle of the presence response
 * @param[out] resource_type The resource type of the presence response
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_presence_response_get_result()
 * @see iotcon_presence_response_get_trigger()
 * @see iotcon_presence_response_get_host_address()
 * @see iotcon_presence_response_get_connectivity_type()
 */
int iotcon_presence_response_get_resource_type(iotcon_presence_response_h response,
		char **resource_type);

/**
 * @brief Specifies the type of function passed to iotcon_find_resource().
 * @details Called when a resource is found from the remote server. \n
 * The @a resource completes its life cycle in the function.
 * If you want to send requests(GET, PUT, POST, DELETE, and OBSERVE) on the @a resource,
 * you must clone the @a resource using iotcon_remote_resource_clone(). \n
 * The @a result could be one of #iotcon_error_e.
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of resource which is found
 * @param[in] result The result code (Lesser than 0 on fail, otherwise a response result value)
 * @param[in] user_data The user data to pass to the function
 *
 * @return true to continue to find resource, otherwise false to stop. \n
 * #IOTCON_FUNC_CONTINUE and #IOTCON_FUNC_STOP are more friendly values for the return.
 *
 * @pre The callback must be registered using iotcon_find_resource()
 *
 * @see iotcon_find_resource()
 */
typedef bool (*iotcon_found_resource_cb)(iotcon_remote_resource_h resource,
		iotcon_error_e result, void *user_data);

/**
 * @brief Finds resources, asynchronously.
 * @details Request to find a resource of @a host_address server with @a query.\n
 * @a host_address could be #IOTCON_MULTICAST_ADDRESS for multicast.\n
 * If succeed to find the resource, iotcon_found_resource_cb() will be invoked with
 * information of the resource.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @remarks @a host_address should be in the format coap://address:port.\n
 * (ex. coaps://[fe80::ae5a:14ff:fe24:b8fe]:12345, or coaps://192.168.1.10:12345)\n
 * @a connectivity_type can be set with extra options.
 * If @a connectivity_type is set without IOTCON_CONNECTIVITY_IPV4_ONLY
 * and IOTCON_CONNECTIVITY_IPV6_ONLY, the resources are discovered by default setting
 * of IoTivity(IPv6 is preferred over IPv4).\n
 * If there are no preference options(PREFER_XXX), resources will reply using each of
 * opened protocol(TCP/UDP).
 * Therefore, you can get remote resource handles which include different host address,
 * but indicates same resource.\n (coaps://192.168.0.2:1234/res/light,
 * coaps+tcp://192.168.0.2/1235/res/light)
 * @a query can be set using specific resource types, or resource interfaces.
 * If @a query is NULL, every resource discovered is listed.
 *
 * @param[in] host_address The address or addressable name of server
 * @param[in] connectivity_type The connectivity type
 * @param[in] query The query specified as a filter for the resource
 * @param[in] cb The callback function to invoke
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @pre iotcon_initialize() should be called to initialize.
 * @post iotcon_found_resource_cb() will be invoked.
 *
 * @see iotcon_found_resource_cb()
 * @see iotcon_set_timeout()
 */
int iotcon_find_resource(const char *host_address,
		int connectivity_type,
		iotcon_query_h query,
		iotcon_found_resource_cb cb,
		void *user_data);

/**
 * @brief Specifies the type of function passed to iotcon_find_device_info().
 * @details The @a result could be one of #iotcon_error_e.
 *
 * @since_tizen 3.0
 *
 * @param[in] device_info the device information from remote server.
 * @param[in] result The result code (0 on success, other wise a negative error value)
 * @param[in] user_data The user data to pass to the function
 *
 * @return true to continue to find device information, otherwise false to stop. \n
 * #IOTCON_FUNC_CONTINUE and #IOTCON_FUNC_STOP are more friendly values for the return.
 *
 * @pre iotcon_find_device_info() will invoke this callback function.
 *
 * @see iotcon_find_device_info()
 * @see iotcon_device_info_get_property()
 */
typedef bool (*iotcon_device_info_cb)(iotcon_device_info_h device_info,
		iotcon_error_e result, void *user_data);

/**
 * @brief Finds the device information of remote server, asynchronously.
 * @details Request device information to server and pass the information by calling
 * iotcon_device_info_cb().\n
 * @a host_address could be #IOTCON_MULTICAST_ADDRESS for multicast.\n
 * If succeed to getting device information, iotcon_device_info_cb() will be invoked with
 * information.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @remarks @a host_address should be in the format coap://address:port.\n
 * (ex. coap://[fe80::ae5a:14ff:fe24:b8fe]:12345, or coap://192.168.1.10:12345)\n
 * @a connectivity_type can be set with extra options.
 * If @a connectivity_type is set without IOTCON_CONNECTIVITY_IPV4_ONLY
 * and IOTCON_CONNECTIVITY_IPV6_ONLY, device informations are discovered by default setting
 * of IoTivity(IPv6 is preferred over IPv4).\n
 * If there are no preference options(PREFER_XXX), it will reply using UDP.
 *
 * @param[in] host_address The host address of remote server
 * @param[in] connectivity_type The connectivity type
 * @param[in] query The query specified as a filter for the device information
 * @param[in] cb The callback function to invoke
 * @param[in] user_data The user data to pass to the function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @pre iotcon_initialize() should be called to initialize.
 * @post iotcon_device_info_cb() will be invoked.
 *
 * @see iotcon_set_device_name()
 * @see iotcon_device_info_cb()
 * @see iotcon_device_info_get_property()
 * @see iotcon_set_timeout()
 */
int iotcon_find_device_info(const char *host_address,
		int connectivity_type,
		iotcon_query_h query,
		iotcon_device_info_cb cb,
		void *user_data);

/**
 * @brief Gets device properties from the device information handle.
 *
 * @since_tizen 3.0
 *
 * @remarks @a value must not be released using free().
 *
 * @param[in] device_info The handle of the device information
 * @param[in] property The properties of the device information
 * @param[out] value The value of the property
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_set_device_name()
 * @see iotcon_device_info_cb()
 * @see iotcon_find_device_info()
 */
int iotcon_device_info_get_property(iotcon_device_info_h device_info,
		iotcon_device_info_e property, char **value);

/**
 * @brief Specifies the type of function passed to iotcon_find_platform_info().
 * @details The @a result could be one of #iotcon_error_e.
 *
 * @since_tizen 3.0
 *
 * @param[in] platform_info The platform information from remote server.
 * @param[in] result The result code (0 on success, other wise a negative error value)
 * @param[in] user_data The user data to pass to the function
 *
 * @return true to continue to find platform information, otherwise false to stop. \n
 * #IOTCON_FUNC_CONTINUE and #IOTCON_FUNC_STOP are more friendly values for the return.
 *
 * @pre iotcon_find_platform_info() will invoke this callback function.
 *
 * @see iotcon_find_platform_info()
 * @see iotcon_platform_info_get_property()
 */
typedef bool (*iotcon_platform_info_cb)(iotcon_platform_info_h platform_info,
		iotcon_error_e result, void *user_data);

/**
 * @brief Finds the platform information of remote server, asynchronously.
 * @details Request platform information to server and pass the information by calling
 * iotcon_platform_info_cb().\n
 * @a host_address could be #IOTCON_MULTICAST_ADDRESS for multicast.\n
 * If succeed to getting platform information, iotcon_platform_info_cb() will be invoked
 * with information.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @remarks @a host_address should be in the format coap://address:port.\n
 * (ex. coap://[fe80::ae5a:14ff:fe24:b8fe]:12345, or coap://192.168.1.10:12345)\n
 * @a connectivity_type can be set with extra options.
 * If @a connectivity_type is set without IOTCON_CONNECTIVITY_IPV4_ONLY
 * and IOTCON_CONNECTIVITY_IPV6_ONLY, platform informations are discovered by default setting
 * of IoTivity(IPv6 is preferred over IPv4).\n
 * If there are no preference options(PREFER_XXX), it will reply using UDP.
 *
 * @param[in] host_address The host address of remote server
 * @param[in] connectivity_type The connectivity type
 * @param[in] query The query specified as a filter for the platform information
 * @param[in] cb The callback function to invoke
 * @param[in] user_data The user data to pass to the function
 *
 * @return  0 on success, otherwise a negative error value.
 *
 * @retval #IOTCON_ERROR_NONE Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @pre iotcon_initialize() should be called to initialize.
 * @post iotcon_platform_info_cb() will be invoked.
 *
 * @see iotcon_platform_info_cb()
 * @see iotcon_platform_info_get_property()
 * @see iotcon_set_timeout()
 */
int iotcon_find_platform_info(const char *host_address,
		int connectivity_type,
		iotcon_query_h query,
		iotcon_platform_info_cb cb,
		void *user_data);

/**
 * @brief Gets platform properties from the platform information handle.
 *
 * @since_tizen 3.0
 *
 * @remarks @a value must not be released using free().
 *
 * @param[in] platform_info The handle of the platform information
 * @param[in] property The properties of the platform information
 * @param[out] value The value of the property
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_platform_info_cb()
 * @see iotcon_find_platform_info()
 */
int iotcon_platform_info_get_property(iotcon_platform_info_h platform_info,
		iotcon_platform_info_e property, char **value);

/**
 * @}
 */

#endif /* __IOTCON_CLIENT_H__ */
