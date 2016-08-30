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
#ifndef __IOTCON_SERVER_RESOURCE_H__
#define __IOTCON_SERVER_RESOURCE_H__

#include <stdint.h>
#include <iotcon-types.h>

/**
 * @file iotcon-resource.h
 */

/**
 * @ingroup CAPI_IOT_CONNECTIVITY_SERVER_MODULE
 * @defgroup CAPI_IOT_CONNECTIVITY_SERVER_RESOURCE_MODULE Resource
 *
 * @brief IoTCon Resource provides API to manage resource.
 *
 * @section CAPI_IOT_CONNECTIVITY_SERVER_RESOURCE_MODULE_HEADER Required Header
 *  \#include <iotcon.h>
 *
 * @section CAPI_IOT_CONNECTIVITY_SERVER_RESOURCE_MODULE_OVERVIEW Overview
 * The iotcon resource API provides methods for managing handle and resource information.
 *
 * Example :
 * @code
#include <iotcon.h>

static iotcon_resource_h _resource_room;

static void _room_request_handler(iotcon_resource_h resource, iotcon_request_h request, void *user_data)
{
	// handle request
	...
}

static void _door_request_handler(iotcon_resource_h resource, iotcon_request_h request, void *user_data)
{
	// handle request
	...
}

static void _create_resource()
{
	int ret;
	uint8_t policies;
	iotcon_resource_interfaces_h resource_ifaces = NULL;
	iotcon_resource_types_h resource_types = NULL;
	iotcon_resource_h resource_door = NULL;

	// 1. create room resource
	policies = IOTCON_RESOURCE_DISCOVERABLE | IOTCON_RESOURCE_OBSERVABLE;

	ret = iotcon_resource_types_create(&resource_types);
	if (IOTCON_ERROR_NONE != ret)
		return;

	ret = iotcon_resource_types_add(resource_types, "org.tizen.room");
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

	ret = iotcon_resource_interfaces_add(resource_ifaces, IOTCON_INTERFACE_LINK);
	if (IOTCON_ERROR_NONE != ret) {
		iotcon_resource_interfaces_destroy(resource_ifaces);
		iotcon_resource_types_destroy(resource_types);
		return;
	}

	ret = iotcon_resource_interfaces_add(resource_ifaces, IOTCON_INTERFACE_BATCH);
	if (IOTCON_ERROR_NONE != ret) {
		iotcon_resource_interfaces_destroy(resource_ifaces);
		iotcon_resource_types_destroy(resource_types);
		return;
	}

	ret = iotcon_resource_create("/room/1", resource_types, resource_ifaces,
			policies, _room_request_handler, NULL, &_resource_room);
	if (IOTCON_ERROR_NONE != ret) {
		iotcon_resource_interfaces_destroy(resource_ifaces);
		iotcon_resource_types_destroy(resource_types);
		return;
	}
	iotcon_resource_interfaces_destroy(resource_ifaces);
	iotcon_resource_types_destroy(resource_types);

	// 2. create door resource
	policies = IOTCON_RESOURCE_OBSERVABLE;

	ret = iotcon_resource_types_create(&resource_types);
	if (IOTCON_ERROR_NONE != ret) {
		iotcon_resource_destroy(_resource_room);
		_resource_room = NULL;
		return;
	}

	ret = iotcon_resource_types_add(resource_types, "org.tizen.door");
	if (IOTCON_ERROR_NONE != ret) {
		iotcon_resource_types_destroy(resource_types);
		iotcon_resource_destroy(_resource_room);
		_resource_room = NULL;
		return;
	}

	ret = iotcon_resource_interfaces_create(&resource_ifaces);
	if (IOTCON_ERROR_NONE != ret)
		iotcon_resource_types_destroy(resource_types);
		iotcon_resource_destroy(_resource_room);
		return;

	ret = iotcon_resource_interfaces_add(resource_ifaces, IOTCON_INTERFACE_DEFAULT);
	if (IOTCON_ERROR_NONE != ret) {
		iotcon_resource_interfaces_destroy(resource_ifaces);
		iotcon_resource_types_destroy(resource_types);
		iotcon_resource_destroy(_resource_room);
		return;
	}

	ret = iotcon_resource_create("/door/1", resource_types, resource_ifaces,
			policies, _door_request_handler, NULL, &resource_door);
	if (IOTCON_ERROR_NONE != ret) {
		iotcon_resource_interfaces_destroy(resource_ifaces);
		iotcon_resource_types_destroy(resource_types);
		iotcon_resource_destroy(_resource_room);
		_resource_room = NULL;
		return;
	}
	iotcon_resource_interfaces_destroy(resource_ifaces);
	iotcon_resource_types_destroy(resource_types);

	// 3. bind door resource to room resource
	ret = iotcon_resource_bind_child_resource(_resource_room, resource_door);
	if (IOTCON_ERROR_NONE != ret) {
		iotcon_resource_destroy(resource_door);
		iotcon_resource_destroy(_resource_room);
		_resource_room = NULL;
		return;
	}
}
 * @endcode
 *
 * @section CAPI_IOT_CONNECTIVITY_SERVER_RESOURCE_MODULE_FEATURE Related Features
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
 * @brief Specifies the type of function passed to iotcon_resource_create() and
 * iotcon_resource_set_request_handler().
 * @details Called when server receive request from the client.
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The resource requested
 * @param[in] request The request from client
 * @param[in] user_data The user data to pass to the function
 *
 * @pre The callback must be registered using iotcon_resource_create()
 *
 * @see iotcon_resource_create()
 * @see iotcon_resource_set_request_handler()
 */
typedef void (*iotcon_request_handler_cb)(iotcon_resource_h resource,
		iotcon_request_h request, void *user_data);

/**
 * @brief Creates a resource handle and registers the resource in server.
 * @details Registers a resource specified by @a uri_path, @a res_types, @a ifaces which have
 * @a properties in IoTCon server.\n
 * When client find the registered resource, iotcon_request_handler_cb() will be called automatically.\n
 * @a uri_path format would be relative URI path like '/a/light'\n
 * @a res_types is a list of resource types. Create a iotcon_resource_types_h handle and
 * add types string to it.\n
 * @a ifaces is a list of resource interfaces. Create a iotcon_resource_interfaces_h handle and
 * add interfaces string to it.\n
 * @a policies also can contain multiple policies like
 * IOTCON_RESOURCE_DISCOVERABLE | IOTCON_RESOURCE_OBSERVABLE.\n
 * iotcon_request_handler_cb() will be called when receive CRUD request to the registered
 * resource.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @remarks @a uri_path length must be less than 128.\n
 * You must destroy @a resource by calling iotcon_resource_destroy()
 * if @a resource is no longer needed.
 *
 * @param[in] uri_path The URI path of the resource
 * @param[in] res_types The list of type of the resource
 * @param[in] ifaces The list of interface of the resource
 * @param[in] policies The policies of the resource\n Set of #iotcon_resource_policy_e
 * @param[in] cb The request handler callback function
 * @param[in] user_data The user data to pass to the callback function
 * @param[out] resource_handle The handle of the resource
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
 * @post When the resource receive CRUD request, iotcon_request_handler_cb() will be called.
 *
 * @see iotcon_resource_destroy()
 * @see iotcon_resource_bind_interface()
 * @see iotcon_resource_bind_type()
 * @see iotcon_resource_set_request_handler()
 * @see iotcon_resource_bind_child_resource()
 * @see iotcon_resource_unbind_child_resource()
 * @see iotcon_request_handler_cb()
 */
int iotcon_resource_create(const char *uri_path,
		iotcon_resource_types_h res_types,
		iotcon_resource_interfaces_h ifaces,
		uint8_t policies,
		iotcon_request_handler_cb cb,
		void *user_data,
		iotcon_resource_h *resource_handle);

/**
 * @brief Destroys the resource and releases its data.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @remarks When a normal variable is used, there are only permission denied error.
 * If the errors of this API are not handled, then you must check an application have
 * the privileges for the API.
 *
 * @param[in] resource_handle The handle of the resource to be unregistered
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @pre iotcon_initialize() should be called to initialize.
 *
 * @see iotcon_resource_create()
 * @see iotcon_resource_bind_interface()
 * @see iotcon_resource_bind_type()
 * @see iotcon_resource_set_request_handler()
 * @see iotcon_resource_bind_child_resource()
 * @see iotcon_resource_unbind_child_resource()
 * @see iotcon_request_handler_cb()
 */
int iotcon_resource_destroy(iotcon_resource_h resource_handle);

/**
 * @brief Binds an interface to the resource.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @remarks @a iface could be a value such as #IOTCON_INTERFACE_DEFAULT.
 *
 * @param[in] resource The handle of the resource
 * @param[in] iface The interface to be bound to the resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 * @retval #IOTCON_ERROR_ALREADY  Already done
 *
 * @pre iotcon_initialize() should be called to initialize.
 *
 * @see iotcon_resource_create()
 * @see iotcon_resource_destroy()
 * @see iotcon_resource_bind_type()
 * @see iotcon_resource_set_request_handler()
 * @see iotcon_resource_bind_child_resource()
 * @see iotcon_resource_unbind_child_resource()
 * @see iotcon_request_handler_cb()
 */
int iotcon_resource_bind_interface(iotcon_resource_h resource, const char *iface);

/**
 * @brief Binds a type to the resource.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @remarks The length of @a resource_type should be less than or equal to 61.\n
 * The @a resource_type must start with a lowercase alphabetic character, followed by a sequence
 * of lowercase alphabetic, numeric, ".", or "-" characters, and contains no white space.
 *
 * @param[in] resource_handle The handle of the resource
 * @param[in] resource_type The type to be bound to the resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 * @retval #IOTCON_ERROR_ALREADY  Already done
 *
 * @pre iotcon_initialize() should be called to initialize.
 *
 * @see iotcon_resource_create()
 * @see iotcon_resource_destroy()
 * @see iotcon_resource_bind_interface()
 * @see iotcon_resource_set_request_handler()
 * @see iotcon_resource_bind_child_resource()
 * @see iotcon_resource_unbind_child_resource()
 * @see iotcon_request_handler_cb()
 */
int iotcon_resource_bind_type(iotcon_resource_h resource_handle,
		const char *resource_type);

/**
 * @brief Sets a request handler to the resource.
 * @details When the resource receive CRUD request, iotcon_request_handler_cb() will be
 * called.
 *
 * @since_tizen 3.0
 *
 * @remarks Registered callback function will be replaced with the new @a cb.
 *
 * @param[in] resource The handle of the resource
 * @param[in] cb The request handler to be bound to the resource
 * @param[in] user_data The user data to pass to the callback function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_resource_create()
 * @see iotcon_resource_destroy()
 * @see iotcon_resource_bind_interface()
 * @see iotcon_resource_bind_type()
 * @see iotcon_resource_bind_child_resource()
 * @see iotcon_resource_unbind_child_resource()
 * @see iotcon_request_handler_cb()
 */
int iotcon_resource_set_request_handler(iotcon_resource_h resource,
		iotcon_request_handler_cb cb, void *user_data);

/**
 * @brief Binds a child resource into the parent resource.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] parent The handle of the parent resource
 * @param[in] child The handle of the child resource to be added to the parent resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_ALREADY  Already done
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @pre iotcon_initialize() should be called to initialize.
 *
 * @see iotcon_resource_create()
 * @see iotcon_resource_destroy()
 * @see iotcon_resource_bind_interface()
 * @see iotcon_resource_bind_type()
 * @see iotcon_resource_set_request_handler()
 * @see iotcon_resource_unbind_child_resource()
 * @see iotcon_request_handler_cb()
 */
int iotcon_resource_bind_child_resource(iotcon_resource_h parent,
		iotcon_resource_h child);

/**
 * @brief Unbinds a child resource from the parent resource.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] parent The handle of the parent resource
 * @param[in] child The handle of the child resource to be unbound from the parent resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_SYSTEM System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 * @retval #IOTCON_ERROR_NO_DATA No data available
 *
 * @pre iotcon_initialize() should be called to initialize.
 *
 * @see iotcon_resource_create()
 * @see iotcon_resource_destroy()
 * @see iotcon_resource_bind_interface()
 * @see iotcon_resource_bind_type()
 * @see iotcon_resource_set_request_handler()
 * @see iotcon_resource_bind_child_resource()
 * @see iotcon_request_handler_cb()
 */
int iotcon_resource_unbind_child_resource(iotcon_resource_h parent,
		iotcon_resource_h child);

/**
 * @brief Notifies specific clients that resource's attributes have changed.
 * @details If @a observers is @c NULL, the @a msg will notify to all observers.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/internet
 *
 * @param[in] resource The handle of the resource
 * @param[in] repr The handle of the representation
 * @param[in] observers The handle of the observers.
 * @param[in] qos The quality of service for message transfer.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_REPRESENTATION  Representation error
 * @retval #IOTCON_ERROR_SYSTEM  System error
 * @retval #IOTCON_ERROR_PERMISSION_DENIED Permission denied
 *
 * @pre iotcon_initialize() should be called to initialize.
 *
 * @see iotcon_remote_resource_observe_cb()
 * @see iotcon_remote_resource_observe_register()
 * @see iotcon_remote_resource_observe_deregister()
 * @see iotcon_observers_create()
 * @see iotcon_observers_destroy()
 * @see iotcon_observers_add()
 * @see iotcon_observers_remove()
 */
int iotcon_resource_notify(iotcon_resource_h resource, iotcon_representation_h repr,
		iotcon_observers_h observers, iotcon_qos_e qos);

/**
 * @brief Gets the number of child resources of the resource.
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the resource
 * @param[out] count The number of child resources
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_resource_get_nth_child()
 * @see iotcon_resource_get_uri_path()
 * @see iotcon_resource_get_types()
 * @see iotcon_resource_get_interfaces()
 * @see iotcon_resource_get_policies()
 */
int iotcon_resource_get_child_count(iotcon_resource_h resource,
		unsigned int *count);

/**
 * @brief Gets the child resource at the given index in the parent resource.
 *
 * @since_tizen 3.0
 *
 * @remarks @a child must not be released using iotcon_resource_destroy().
 *
 * @param[in] parent The handle of the parent resource
 * @param[in] index The index of the child resource
 * @param[out] child The child resource at the index
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_NO_DATA No data available
 *
 * @see iotcon_resource_get_child_count()
 * @see iotcon_resource_get_uri_path()
 * @see iotcon_resource_get_types()
 * @see iotcon_resource_get_interfaces()
 * @see iotcon_resource_get_policies()
 */
int iotcon_resource_get_nth_child(iotcon_resource_h parent, int index,
		iotcon_resource_h *child);

/**
 * @brief Gets an URI path of the resource.
 *
 * @since_tizen 3.0
 *
 * @remarks @a uri_path must not be released using free().
 *
 * @param[in] resource The handle of the resource
 * @param[out] uri_path The URI path of the resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_resource_get_child_count()
 * @see iotcon_resource_get_nth_child()
 * @see iotcon_resource_get_types()
 * @see iotcon_resource_get_interfaces()
 * @see iotcon_resource_get_policies()
 */
int iotcon_resource_get_uri_path(iotcon_resource_h resource, char **uri_path);

/**
 * @brief Gets the list of types in the resource.
 *
 * @since_tizen 3.0
 *
 * @remarks @a types must not be released using iotcon_resource_types_destroy().
 *
 * @param[in] resource The handle of the resource
 * @param[out] types The types of the resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_resource_get_child_count()
 * @see iotcon_resource_get_nth_child()
 * @see iotcon_resource_get_uri_path()
 * @see iotcon_resource_get_interfaces()
 * @see iotcon_resource_get_policies()
 */
int iotcon_resource_get_types(iotcon_resource_h resource, iotcon_resource_types_h *types);

/**
 * @brief Gets the interfaces of the resource.
 *
 * @since_tizen 3.0
 *
 * @remarks @a ifaces must not be released using iotcon_resource_interfaces_destroy().
 *
 * @param[in] resource The handle of the resource
 * @param[out] ifaces The interfaces of the resource
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_resource_get_child_count()
 * @see iotcon_resource_get_nth_child()
 * @see iotcon_resource_get_uri_path()
 * @see iotcon_resource_get_types()
 * @see iotcon_resource_get_policies()
 */
int iotcon_resource_get_interfaces(iotcon_resource_h resource,
		iotcon_resource_interfaces_h *ifaces);

/**
 * @brief Gets the policies in the resource.
 * @details @a policies can contain multiple policies like
 * IOTCON_RESOURCE_DISCOVERABLE | IOTCON_RESOURCE_OBSERVABLE.
 *
 * @since_tizen 3.0
 *
 * @param[in] resource The handle of the resource
 * @param[out] policies The policies of resource\n Set of #iotcon_resource_policy_e
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_resource_get_child_count()
 * @see iotcon_resource_get_nth_child()
 * @see iotcon_resource_get_uri_path()
 * @see iotcon_resource_get_types()
 * @see iotcon_resource_get_interfaces()
 */
int iotcon_resource_get_policies(iotcon_resource_h resource, uint8_t *policies);

/**
 * @}
 */

#endif /* __IOTCON_SERVER_RESOURCE_H__ */
