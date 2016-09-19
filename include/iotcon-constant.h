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
#ifndef __IOTCON_CONSTANT_H__
#define __IOTCON_CONSTANT_H__

/**
 * @file iotcon-constant.h
 */

/**
 * @addtogroup CAPI_IOT_CONNECTIVITY_COMMON_MODULE
 *
 * @{
 */

/**
 * @brief The IP Address for multicast.
 *
 * @since_tizen 3.0
 */
#define IOTCON_MULTICAST_ADDRESS NULL

/**
 * @brief Use this value as the return value to stop foreach function.
 *
 * @since_tizen 3.0
 */
#define IOTCON_FUNC_STOP false

/**
 * @brief Use this value as the return value to continue foreach function.
 *
 * @since_tizen 3.0
 */
#define IOTCON_FUNC_CONTINUE true

/**
 * @brief Default Interface.
 *
 * @since_tizen 3.0
 */
#define IOTCON_INTERFACE_DEFAULT "oic.if.baseline"

/**
 * @brief List Links Interface which is used to list the references to other resources
 * contained in a resource.
 *
 * @since_tizen 3.0
 */
#define IOTCON_INTERFACE_LINK "oic.if.ll"

/**
 * @brief Batch Interface which is used to manipulate (GET, PUT, POST, DELETE) on other
 * resource contained in a resource.
 *
 * @since_tizen 3.0
 */
#define IOTCON_INTERFACE_BATCH "oic.if.b"

/**
 * @brief Group Interface which is used to manipulate (GET, PUT, POST) a group of remote
 * resources.
 *
 * @since_tizen 3.0
 */
#define IOTCON_INTERFACE_GROUP "oic.mi.grp"

/**
 * @brief Read-Only Interface which is used to limit the methods that can be applied to a
 * resource to GET only.
 *
 * @since_tizen 3.0
 */
#define IOTCON_INTERFACE_READONLY "oic.if.r"

/**
 * @brief Enumeration for type of observation.
 *
 * @since_tizen 3.0
 */
typedef enum {
	IOTCON_OBSERVE_NO_TYPE = 0, /**< Indicates no option */
	IOTCON_OBSERVE_REGISTER = 1, /**< Indicates action of registering observation*/
	IOTCON_OBSERVE_DEREGISTER = 2, /**< Indicates action of unregistering observation */
} iotcon_observe_type_e;

/**
 * @brief Enumeration for policy of observation.
 *
 * @since_tizen 3.0
 */
typedef enum {
	IOTCON_OBSERVE_IGNORE_OUT_OF_ORDER = 0, /**< Indicates observation request for most up-to-date notifications only */
	IOTCON_OBSERVE_ACCEPT_OUT_OF_ORDER = 1 /**< Indicates observation request for all notifications including state notifications */
} iotcon_observe_policy_e;

/**
 * @brief Enumeration for of connectivities which can be held in a resource.
 *
 * @remarks IOTCON_CONNECTIVITY_PREFER_UDP and IOTCON_CONNECTIVITY_PREFER_TCP should be
 * mutually exclusive.
 * @remarks IOTCON_CONNECTIVITY_IPV4_ONLY and IOTCON_CONNECTIVITY_IPV6_ONLY should be
 * mutually exclusive.
 *
 * @since_tizen 3.0
 */
typedef enum {
	IOTCON_CONNECTIVITY_ALL = 0, /**< Indicates all connectivities */
	IOTCON_CONNECTIVITY_IP = (1 << 0), /**< Indicates Internet Protocol connectivity */
	/* Extra options for Connectivity */
	IOTCON_CONNECTIVITY_PREFER_UDP = (1 << 16), /**< It is related to IOTCON_CONNECTIVITY_IP, and it indicates UDP is preferred */
	IOTCON_CONNECTIVITY_PREFER_TCP = (1 << 17), /**< It is related to IOTCON_CONNECTIVITY_IP, and it indicates TCP is preferred */
	IOTCON_CONNECTIVITY_IPV4_ONLY = (1 << 18), /**< When this bit is set with IOTCON_CONNECTIVITY_IP, resources are discovered for IPv4 */
	IOTCON_CONNECTIVITY_IPV6_ONLY = (1 << 19), /**< When this bit is set with IOTCON_CONNECTIVITY_IP, resources are discovered for IPv6 */
} iotcon_connectivity_type_e;

/**
 * @brief Enumeration for policy which can be held in a resource.
 *
 * @since_tizen 3.0
 */
typedef enum {
	IOTCON_RESOURCE_NO_POLICY = 0, /**< Indicates resource uninitialized */
	IOTCON_RESOURCE_DISCOVERABLE = (1 << 0), /**< Indicates resource that is allowed to be discovered */
	IOTCON_RESOURCE_OBSERVABLE = (1 << 1), /**< Indicates resource that is allowed to be observed */
	IOTCON_RESOURCE_ACTIVE = (1 << 2), /**< Indicates resource initialized and activated */
	IOTCON_RESOURCE_SLOW = (1 << 3), /**< Indicates resource which takes some delay to respond */
	IOTCON_RESOURCE_SECURE = (1 << 4), /**< Indicates secure resource */
	IOTCON_RESOURCE_EXPLICIT_DISCOVERABLE = (1 << 5), /**< When this bit is set, the resource is allowed to be discovered only if discovery request contains an explicit querystring. */
} iotcon_resource_policy_e;

/**
 * @brief Enumeration for type of request.
 *
 * @since_tizen 3.0
 */
typedef enum {
	IOTCON_REQUEST_UNKNOWN = 0, /**< Indicates none */
	IOTCON_REQUEST_GET = 1, /**< Indicates get method of request */
	IOTCON_REQUEST_PUT = 2, /**< Indicates put method of request */
	IOTCON_REQUEST_POST = 3, /**< Indicates post method of request */
	IOTCON_REQUEST_DELETE = 4, /**< Indicates delete method of request */
} iotcon_request_type_e;

/**
 * @brief Enumeration for result of response.
 *
 * @since_tizen 3.0
 */
typedef enum {
	IOTCON_RESPONSE_OK = 0, /**< Indicates result of response for success */
	IOTCON_RESPONSE_ERROR, /**< Indicates result of response for something error */
	IOTCON_RESPONSE_RESOURCE_CREATED, /**< Indicates result of response for resource has created */
	IOTCON_RESPONSE_RESOURCE_DELETED, /**< Indicates result of response for resource has deleted */
	IOTCON_RESPONSE_RESOURCE_CHANGED, /**< Indicates result of response for resource has changed */
	IOTCON_RESPONSE_SLOW, /**< Indicates result of response for slow resource */
	IOTCON_RESPONSE_FORBIDDEN, /**< Indicates result of response for accessing unauthorized resource */
} iotcon_response_result_e;

/**
 * @brief Enumeration for result of presence.
 *
 * @since_tizen 3.0
 */
typedef enum {
	IOTCON_PRESENCE_OK = 0, /**< Indicates for successful action of presence */
	IOTCON_PRESENCE_STOPPED, /**< Indicates for stopped action of presence */
	IOTCON_PRESENCE_TIMEOUT, /**< Indicates for no response of presence for some time */
} iotcon_presence_result_e;

/**
 * @brief Enumeration for operation of presence response.
 *
 * @since_tizen 3.0
 */
typedef enum  {
	IOTCON_PRESENCE_RESOURCE_CREATED, /**< Indicates for resource creation operation of server */
	IOTCON_PRESENCE_RESOURCE_UPDATED, /**< Indicates for resource update operation of server */
	IOTCON_PRESENCE_RESOURCE_DESTROYED, /**< Indicates for resource destruction operation of server */
} iotcon_presence_trigger_e;

/**
 * @brief Enumeration for types of representation that is able to have.
 *
 * @since_tizen 3.0
 */
typedef enum {
	IOTCON_TYPE_NONE = 0, /**< Indicates for representation that have no type */
	IOTCON_TYPE_INT, /**< Indicates for representation that have int type */
	IOTCON_TYPE_BOOL, /**< Indicates for representation that have bool type */
	IOTCON_TYPE_DOUBLE, /**< Indicates for representation that have double type */
	IOTCON_TYPE_STR, /**< Indicates for representation that have string type */
	IOTCON_TYPE_BYTE_STR, /**< Indicates for representation that have byte string type */
	IOTCON_TYPE_NULL, /**< Indicates for representation that have null type */
	IOTCON_TYPE_LIST, /**< Indicates for representation that have list type */
	IOTCON_TYPE_ATTRIBUTES, /**< Indicates for representation that have another representation type */
} iotcon_type_e;

/**
 * @brief Enumeration for properties of device information.
 *
 * @since_tizen 3.0
 */
typedef enum {
	IOTCON_DEVICE_INFO_NAME = 0, /**< Indicates human friendly name for device */
	IOTCON_DEVICE_INFO_SPEC_VER, /**< Indicates spec version of the core specification */
	IOTCON_DEVICE_INFO_ID, /**< Indicates unique identifier for OCF device */
	IOTCON_DEVICE_INFO_DATA_MODEL_VER, /**< Indicates version of the specs this device data model is implemented to */
} iotcon_device_info_e;

/**
 * @brief Enumeration for properties of platform information.
 *
 * @since_tizen 3.0
 */
typedef enum {
	IOTCON_PLATFORM_INFO_ID = 0, /**< Indicates platform identifier */
	IOTCON_PLATFORM_INFO_MANUF_NAME, /**< Indicates name of manufacturer */
	IOTCON_PLATFORM_INFO_MANUF_URL, /**< Indicates URL to manufacturer */
	IOTCON_PLATFORM_INFO_MODEL_NUMBER, /**< Indicates model number as designated by manufacturer */
	IOTCON_PLATFORM_INFO_DATE_OF_MANUF, /**< Indicates manufacturing date of device */
	IOTCON_PLATFORM_INFO_PLATFORM_VER, /**< Indicates version of platform defined by manufacturer */
	IOTCON_PLATFORM_INFO_OS_VER, /**< Indicates version of platform resident OS */
	IOTCON_PLATFORM_INFO_HARDWARE_VER, /**< Indicates version of platform hardware */
	IOTCON_PLATFORM_INFO_FIRMWARE_VER, /**< Indicates version of device firmware */
	IOTCON_PLATFORM_INFO_SUPPORT_URL, /**< Indicates URL that points to support information from manufacturer */
	IOTCON_PLATFORM_INFO_SYSTEM_TIME, /**< Indicates reference time for the device */
} iotcon_platform_info_e;

/**
 * @brief Enumeration for states of remote resource.
 *
 * @since_tizen 3.0
 */
typedef enum {
	IOTCON_REMOTE_RESOURCE_ALIVE, /**< Indicates remote resource is alive */
	IOTCON_REMOTE_RESOURCE_LOST_SIGNAL, /**< Indicates remote resource is lost */
} iotcon_remote_resource_state_e;

/**
 * @brief Enumeration for quality of service.
 *
 * @since_tizen 3.0
 */
typedef enum {
	IOTCON_QOS_LOW, /**< Indicates low quality of service */
	IOTCON_QOS_HIGH, /**< Indicates high quality of service  */
} iotcon_qos_e;

/**
 * @}
 */

#endif /* __IOTCON_CONSTANT_H__ */
