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
#include <stdint.h>
#include <limits.h>

#include <octypes.h>
#include <ocpayload.h>

#include "iotcon.h"
#include "ic.h"
#include "ic-utils.h"
#include "ic-remote-resource.h"
#include "ic-device.h"
#include "ic-presence.h"
#include "ic-types.h"
#include "ic-representation.h"
#include "ic-ioty-parse.h"

static int _ioty_parse_oic_rep_payload_value(OCRepPayloadValue *val,
		iotcon_attributes_h *attributes);


iotcon_observe_type_e ic_ioty_parse_oic_action(OCObserveAction oic_action)
{
	switch (oic_action) {
	case OC_OBSERVE_REGISTER:
		return IOTCON_OBSERVE_REGISTER;
	case OC_OBSERVE_DEREGISTER:
		return IOTCON_OBSERVE_DEREGISTER;
	case OC_OBSERVE_NO_OPTION:
	default:
		ERR("Unknown action (%d)", oic_action);
	}
	return IOTCON_OBSERVE_NO_TYPE;
}

iotcon_error_e ic_ioty_parse_oic_error(OCStackResult ret)
{
	switch (ret) {
	case OC_STACK_OK:
		return IOTCON_ERROR_NONE;
	case OC_STACK_NO_MEMORY:
		return IOTCON_ERROR_OUT_OF_MEMORY;
	case OC_STACK_NO_RESOURCE:
		return IOTCON_ERROR_NO_DATA;
	default:
		WARN("Unknown result(%d)", ret);
	}
	return IOTCON_ERROR_IOTIVITY;
}


static void _ioty_parse_oic_transport(OCTransportAdapter adapter, OCTransportFlags flag,
		iotcon_connectivity_type_e *conn_type, int *conn_options)
{
	/* Need to consider to allow various connectivity types */
	switch (adapter) {
	case OC_ADAPTER_IP:
		*conn_options = IC_UTILS_CONNECTIVITY_UDP;
		if (OC_IP_USE_V4 & flag)
			*conn_options |= IC_UTILS_CONNECTIVITY_IPV4;
		else if (OC_IP_USE_V6 & flag)
			*conn_options |= IC_UTILS_CONNECTIVITY_IPV6;
		break;
	case OC_ADAPTER_TCP:
		*conn_options = IC_UTILS_CONNECTIVITY_TCP;
		if (OC_IP_USE_V4 & flag)
			*conn_options |= IC_UTILS_CONNECTIVITY_IPV4;
		else if (OC_IP_USE_V6 & flag)
			*conn_options |= IC_UTILS_CONNECTIVITY_IPV6;
		break;
	default:
		ERR("Invalid Adapter(%d) / Flag(%d)", adapter, flag);
		return;
	}

	*conn_type = IOTCON_CONNECTIVITY_IP;
}


int ic_ioty_parse_oic_dev_address(OCDevAddr *dev_addr, char **host_address,
		int *conn_type)
{
	int conn_options;
	char host_addr[PATH_MAX] = {0};
	const char *coap_str;
	iotcon_connectivity_type_e temp_conn_type;

	RETV_IF(NULL == host_address, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == conn_type, IOTCON_ERROR_INVALID_PARAMETER);

	_ioty_parse_oic_transport(dev_addr->adapter, dev_addr->flags, &temp_conn_type,
			&conn_options);

	if (OC_FLAG_SECURE & dev_addr->flags) {
		if (OC_ADAPTER_TCP & dev_addr->adapter)
			coap_str = IC_COAPS_TCP;
		else
			coap_str = IC_COAPS;
	} else {
		if (OC_ADAPTER_TCP & dev_addr->adapter)
			coap_str = IC_COAP_TCP;
		else
			coap_str = IC_COAP;
	}

	switch (temp_conn_type) {
	case IOTCON_CONNECTIVITY_IP:
		if (IC_UTILS_CONNECTIVITY_IPV6 & conn_options) {
			snprintf(host_addr, sizeof(host_addr), "%s[%s]:%d", coap_str, dev_addr->addr,
					dev_addr->port);
		} else {
			snprintf(host_addr, sizeof(host_addr), "%s%s:%d", coap_str, dev_addr->addr,
					dev_addr->port);
		}
		break;
	case IOTCON_CONNECTIVITY_ALL:
	default:
		ERR("Invalid Connectivity Type(%d)", temp_conn_type);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	*host_address = strdup(host_addr);

	*conn_type = temp_conn_type;

	return IOTCON_ERROR_NONE;
}

iotcon_response_result_e ic_ioty_parse_oic_response_result(OCStackResult result)
{
	switch (result) {
	case OC_STACK_OK:
		return IOTCON_RESPONSE_OK;
	case OC_STACK_RESOURCE_CREATED:
		return IOTCON_RESPONSE_RESOURCE_CREATED;
	case OC_STACK_RESOURCE_DELETED:
		return IOTCON_RESPONSE_RESOURCE_DELETED;
	case OC_STACK_RESOURCE_CHANGED:
		return IOTCON_RESPONSE_RESOURCE_CHANGED;
	case OC_STACK_UNAUTHORIZED_REQ:
		return IOTCON_RESPONSE_FORBIDDEN;
	case OC_STACK_ERROR:
		return IOTCON_RESPONSE_ERROR;
	default:
		WARN("Unknown result(%d)", result);
	}
	return IOTCON_RESPONSE_ERROR;
}

static iotcon_presence_trigger_e _ioty_parse_oic_trigger(OCPresenceTrigger src)
{
	switch (src) {
	case OC_PRESENCE_TRIGGER_CREATE:
		return IOTCON_PRESENCE_RESOURCE_CREATED;
	case OC_PRESENCE_TRIGGER_CHANGE:
		return IOTCON_PRESENCE_RESOURCE_UPDATED;
	case OC_PRESENCE_TRIGGER_DELETE:
		return IOTCON_PRESENCE_RESOURCE_DESTROYED;
	default:
		ERR("Unknown trigger(%d)", src);
	}
	return IOTCON_PRESENCE_RESOURCE_CREATED;
}


static uint8_t _ioty_parse_oic_policies(uint8_t oic_policies)
{
	uint8_t policies = IOTCON_RESOURCE_NO_POLICY;

	if (OC_DISCOVERABLE & oic_policies)
		policies |= IOTCON_RESOURCE_DISCOVERABLE;

	if (OC_OBSERVABLE & oic_policies)
		policies |= IOTCON_RESOURCE_OBSERVABLE;

	if (OC_ACTIVE & oic_policies)
		policies |= IOTCON_RESOURCE_ACTIVE;

	if (OC_SLOW & oic_policies)
		policies |= IOTCON_RESOURCE_SLOW;

	if (OC_SECURE & oic_policies)
		policies |= IOTCON_RESOURCE_SECURE;

	if (OC_EXPLICIT_DISCOVERABLE & oic_policies)
		policies |= IOTCON_RESOURCE_EXPLICIT_DISCOVERABLE;

	return policies;
}

iotcon_request_type_e ic_ioty_parse_oic_method(OCMethod method)
{
	switch (method) {
	case OC_REST_GET:
		return IOTCON_REQUEST_GET;
	case OC_REST_PUT:
		return IOTCON_REQUEST_PUT;
	case OC_REST_POST:
		return IOTCON_REQUEST_POST;
	case OC_REST_DELETE:
		return IOTCON_REQUEST_DELETE;
	default:
		ERR("Unknown method(%d)", method);
		return IOTCON_REQUEST_UNKNOWN;
	}
}

void ic_ioty_free_resource_list(gpointer data)
{
	iotcon_remote_resource_h resource = data;

	resource->is_found = false;

	iotcon_remote_resource_destroy(resource);
}

static int _parse_remote_resource(OCDevAddr *dev_addr,
		OCTransportAdapter adapter,
		int port,
		uint8_t policies,
		char *uri,
		iotcon_resource_interfaces_h ifaces,
		iotcon_resource_types_h types,
		char *device_id,
		char *device_name,
		iotcon_remote_resource_h *resource)
{
	int ret;
	int conn_options;
	const char *coap_str = IC_COAP;
	char host_address[PATH_MAX] = {0};
	iotcon_remote_resource_h temp;
	iotcon_connectivity_type_e conn_type;

	DBG("dev_addr: adapter [%d], flags [%d], port [%d], addr [%s]",
		dev_addr->adapter, dev_addr->flags, dev_addr->port, dev_addr->addr);

	_ioty_parse_oic_transport(adapter, dev_addr->flags, &conn_type, &conn_options);

	switch (conn_type) {
	case IOTCON_CONNECTIVITY_IP:
		if (IC_UTILS_CONNECTIVITY_TCP & conn_options) {
			if (IOTCON_RESOURCE_SECURE & policies)
				coap_str = IC_COAPS_TCP;
			else
				coap_str = IC_COAP_TCP;
		} else if (IC_UTILS_CONNECTIVITY_UDP & conn_options) {
			if (IOTCON_RESOURCE_SECURE & policies)
				coap_str = IC_COAPS;
			else
				coap_str = IC_COAP;
		}

		if (IC_UTILS_CONNECTIVITY_IPV6 & conn_options) {
			snprintf(host_address, sizeof(host_address), "%s[%s]:%d", coap_str,
					dev_addr->addr, port);
		} else {
			snprintf(host_address, sizeof(host_address), "%s%s:%d", coap_str,
					dev_addr->addr, port);
		}
		break;
	case IOTCON_CONNECTIVITY_ALL:
	default:
		snprintf(host_address, sizeof(host_address), "%s%s", coap_str, dev_addr->addr);
	}

	/* remote resource */
	ret = iotcon_remote_resource_create(host_address, conn_type, uri, policies, types,
			ifaces, &temp);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_remote_resource_create() Fail(%d)", ret);
		return ret;
	}

	temp->device_id = strdup(device_id);
	if (NULL == temp->device_id) {
		ERR("strdup(device_id) Fail(%d)", errno);
		iotcon_remote_resource_destroy(temp);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	if (device_name)
		temp->device_name = strdup(device_name);
	else
		temp->device_name = strdup("");
	if (NULL == temp->device_name) {
		ERR("strdup(device_name) Fail(%d)", errno);
		iotcon_remote_resource_destroy(temp);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	*resource = temp;

	return IOTCON_ERROR_NONE;
}

int ic_ioty_parse_oic_discovery_payload(OCDevAddr *dev_addr,
		iotcon_connectivity_type_e connectivity_type,
		int connectivity_prefer,
		OCDiscoveryPayload *payload,
		GList **resource_list)
{
	int ret;
	GList *res_list = NULL;
	struct OCResourcePayload *res_payload;

	RETV_IF(NULL == payload, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == dev_addr, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == resource_list, IOTCON_ERROR_INVALID_PARAMETER);

	for (res_payload = payload->resources; res_payload; res_payload = res_payload->next) {
		int port;
		bool udp_on;
		bool tcp_on;
		uint8_t policies;
		iotcon_remote_resource_h res;
		iotcon_resource_interfaces_h ifaces;
		iotcon_resource_types_h types;
		OCStringLL *node;

		/* uri path */
		if (NULL == res_payload->uri) {
			ERR("res_payload uri is NULL");
			g_list_free_full(res_list, ic_ioty_free_resource_list);
			return IOTCON_ERROR_IOTIVITY;
		}

		/* Resource Types */
		node = res_payload->types;
		if (NULL == node) {
			ERR("res_payload types is NULL");
			g_list_free_full(res_list, ic_ioty_free_resource_list);
			return IOTCON_ERROR_IOTIVITY;
		}

		ret = iotcon_resource_types_create(&types);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_resource_types_create() Fail(%d)", ret);
			g_list_free_full(res_list, ic_ioty_free_resource_list);
			return ret;
		}

		while (node) {
			ret = iotcon_resource_types_add(types, node->value);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_resource_types_add() Fail(%d)", ret);
				iotcon_resource_types_destroy(types);
				g_list_free_full(res_list, ic_ioty_free_resource_list);
				return ret;
			}
			node = node->next;
		}

		/* Resource Interfaces */
		iotcon_resource_interfaces_create(&ifaces);
		node = res_payload->interfaces;
		if (NULL == node) {
			ERR("res_payload interfaces is NULL");
			iotcon_resource_types_destroy(types);
			continue;
		}
		for (; node; node = node->next)
			iotcon_resource_interfaces_add(ifaces, node->value);

		/* Resource Policies */
		policies = _ioty_parse_oic_policies(res_payload->bitmap);
		if (res_payload->secure)
			policies |= IOTCON_RESOURCE_SECURE;

		/* preference is not set */
		tcp_on = true;
		udp_on = true;

		if ((IOTCON_CONNECTIVITY_PREFER_UDP | IOTCON_CONNECTIVITY_PREFER_TCP)
				& connectivity_prefer) {
			udp_on = IOTCON_CONNECTIVITY_PREFER_UDP & connectivity_prefer;
			tcp_on = IOTCON_CONNECTIVITY_PREFER_TCP & connectivity_prefer;
		}

		/* UDP */
		if (udp_on || (0 == res_payload->tcpPort)) {
			/* port */
			port = (res_payload->port) ? res_payload->port : dev_addr->port;

			ret = _parse_remote_resource(dev_addr, dev_addr->adapter, port, policies,
					res_payload->uri, ifaces, types, payload->sid, payload->name, &res);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("_parse_remote_resource() Fail(%d)", ret);
				iotcon_resource_interfaces_destroy(ifaces);
				iotcon_resource_types_destroy(types);
				continue;
			}

			res_list = g_list_append(res_list, res);
		}

		/* TCP */
		if (tcp_on && res_payload->tcpPort) {
			/* port */
			port = res_payload->tcpPort;

			ret = _parse_remote_resource(dev_addr, OC_ADAPTER_TCP, port, policies,
					res_payload->uri, ifaces, types, payload->sid, payload->name, &res);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("_parse_remote_resource() Fail(%d)", ret);
				iotcon_resource_interfaces_destroy(ifaces);
				iotcon_resource_types_destroy(types);
				continue;
			}

			res_list = g_list_append(res_list, res);
		}

		iotcon_resource_interfaces_destroy(ifaces);
		iotcon_resource_types_destroy(types);
	}

	*resource_list = res_list;

	return IOTCON_ERROR_NONE;
}

int ic_ioty_parse_oic_device_payload(OCDevicePayload *payload,
		iotcon_device_info_h *device_info)
{
	struct icl_device_info *info = NULL;

	RETV_IF(NULL == device_info, IOTCON_ERROR_INVALID_PARAMETER);

	info = calloc(1, sizeof(struct icl_device_info));
	if (NULL == info) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	if (payload->deviceName)
		info->device_name = strdup(payload->deviceName);
	else
		info->device_name = strdup("");
	if (NULL == info->device_name)
		ERR("strdup(device_name) Fail(%d)", errno);

	info->spec_ver = ic_utils_strdup(payload->specVersion);
	info->device_id = ic_utils_strdup(payload->sid);
	info->data_model_ver = OCCreateString(payload->dataModelVersions);

	*device_info = info;

	return IOTCON_ERROR_NONE;
}

int ic_ioty_parse_oic_platform_payload(OCPlatformPayload *payload,
		iotcon_platform_info_h *platform_info)
{
	struct icl_platform_info *info = NULL;

	RETV_IF(NULL == payload, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == platform_info, IOTCON_ERROR_INVALID_PARAMETER);

	info = calloc(1, sizeof(struct icl_platform_info));
	if (NULL == info) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	info->platform_id  = ic_utils_strdup(payload->info.platformID);
	info->manuf_name = ic_utils_strdup(payload->info.manufacturerName);
	if (payload->info.manufacturerUrl)
		info->manuf_url = ic_utils_strdup(payload->info.manufacturerUrl);
	if (payload->info.modelNumber)
		info->model_number = ic_utils_strdup(payload->info.modelNumber);
	if (payload->info.dateOfManufacture)
		info->date_of_manuf = ic_utils_strdup(payload->info.dateOfManufacture);
	if (payload->info.platformVersion)
		info->platform_ver = ic_utils_strdup(payload->info.platformVersion);
	if (payload->info.operatingSystemVersion)
		info->os_ver = ic_utils_strdup(payload->info.operatingSystemVersion);
	if (payload->info.hardwareVersion)
		info->hardware_ver = ic_utils_strdup(payload->info.hardwareVersion);
	if (payload->info.firmwareVersion)
		info->firmware_ver = ic_utils_strdup(payload->info.firmwareVersion);
	if (payload->info.supportUrl)
		info->support_url = ic_utils_strdup(payload->info.supportUrl);
	if (payload->info.systemTime)
		info->system_time = ic_utils_strdup(payload->info.systemTime);

	*platform_info = info;

	return IOTCON_ERROR_NONE;
}

int ic_ioty_parse_oic_presence_payload(OCDevAddr *dev_addr,
		OCPresencePayload *payload,
		OCStackResult result,
		iotcon_presence_response_h *presence_response)
{
	FN_CALL;
	int ret;
	struct icl_presence_response *resp = NULL;

	RETV_IF(NULL == dev_addr, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == payload, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == presence_response, IOTCON_ERROR_INVALID_PARAMETER);

	resp = calloc(1, sizeof(struct icl_presence_response));
	if (NULL == resp) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	ret = ic_ioty_parse_oic_dev_address(dev_addr, &(resp->host_address),
			(int*)&(resp->connectivity_type));
	if (IOTCON_ERROR_NONE != ret) {
		ERR("ic_ioty_parse_oic_dev_address() Fail(%d)", ret);
		icl_destroy_presence_response(resp);
		return ret;
	}

	switch (result) {
	case OC_STACK_OK:
		resp->result = IOTCON_PRESENCE_OK;
		resp->trigger = _ioty_parse_oic_trigger(payload->trigger);
		break;
	case OC_STACK_PRESENCE_STOPPED:
		resp->result = IOTCON_PRESENCE_STOPPED;
		break;
	case OC_STACK_PRESENCE_TIMEOUT:
		resp->result = IOTCON_PRESENCE_TIMEOUT;
		break;
	case OC_STACK_ERROR:
	default:
		DBG("Presence error(%d)", result);
		resp->result = IOTCON_ERROR_IOTIVITY;
	}
	if (payload->resourceType)
		resp->resource_type = strdup(payload->resourceType);

	*presence_response = resp;

	return IOTCON_ERROR_NONE;
}

static int _icl_ioty_parse_oic_rep_payload_value_array_attr(
		OCRepPayloadValueArray *arr, int len, int index, iotcon_list_h *list)
{
	int i, ret;
	iotcon_list_h l = NULL;

	RETV_IF(NULL == arr, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == list, IOTCON_ERROR_INVALID_PARAMETER);

	switch (arr->type) {
	case OCREP_PROP_INT:
		ret = iotcon_list_create(IOTCON_TYPE_INT, &l);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_list_create(IOTCON_TYPE_INT) Fail(%d)", ret);
			return ret;
		}
		for (i = 0; i < len; i++) {
			ret = iotcon_list_add_int(l, arr->iArray[index + i], -1);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_list_add_int() Fail(%d)", ret);
				iotcon_list_destroy(l);
				return ret;
			}
		}
		break;
	case OCREP_PROP_BOOL:
		ret = iotcon_list_create(IOTCON_TYPE_BOOL, &l);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_list_create(IOTCON_TYPE_BOOL) Fail(%d)", ret);
			return ret;
		}
		for (i = 0; i < len; i++) {
			ret = iotcon_list_add_bool(l, arr->bArray[index + i], -1);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_list_add_bool() Fail(%d)", ret);
				iotcon_list_destroy(l);
				return ret;
			}
		}
		break;
	case OCREP_PROP_DOUBLE:
		ret = iotcon_list_create(IOTCON_TYPE_DOUBLE, &l);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_list_create(IOTCON_TYPE_DOUBLE) Fail(%d)", ret);
			return ret;
		}
		for (i = 0; i < len; i++) {
			ret = iotcon_list_add_double(l, arr->dArray[index + i], -1);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_list_add_double() Fail(%d)", ret);
				iotcon_list_destroy(l);
				return ret;
			}
		}
		break;
	case OCREP_PROP_STRING:
		ret = iotcon_list_create(IOTCON_TYPE_STR, &l);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_list_create(IOTCON_TYPE_STR) Fail(%d)", ret);
			return ret;
		}
		for (i = 0; i < len; i++) {
			ret = iotcon_list_add_str(l, arr->strArray[index + i], -1);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_list_add_str() Fail(%d)", ret);
				iotcon_list_destroy(l);
				return ret;
			}
		}
		break;
	case OCREP_PROP_BYTE_STRING:
		ret = iotcon_list_create(IOTCON_TYPE_BYTE_STR, &l);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_list_create(IOTCON_TYPE_BYTE_STR) Fail(%d)", ret);
			return ret;
		}
		for (i = 0; i < len; i++) {
			ret = iotcon_list_add_byte_str(l, arr->ocByteStrArray[index + i].bytes,
					arr->ocByteStrArray[index + i].len, -1);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_list_add_byte_str() Fail(%d)", ret);
				iotcon_list_destroy(l);
				return ret;
			}
		}
		break;
	case OCREP_PROP_OBJECT:
		ret = iotcon_list_create(IOTCON_TYPE_ATTRIBUTES, &l);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_list_create(IOTCON_TYPE_ATTRIBUTES) Fail(%d)", ret);
			return ret;
		}
		for (i = 0; i < len; i++) {
			iotcon_attributes_h attributes;
			ret = _ioty_parse_oic_rep_payload_value(arr->objArray[index + i]->values,
					&attributes);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("_ioty_parse_oic_rep_payload_value(%d)", ret);
				iotcon_list_destroy(l);
				return ret;
			}

			ret = iotcon_list_add_attributes(l, attributes, -1);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_list_add_attributes() Fail(%d)", ret);
				iotcon_attributes_destroy(attributes);
				iotcon_list_destroy(l);
				return ret;
			}
			iotcon_attributes_destroy(attributes);
		}
		break;
	case OCREP_PROP_ARRAY:
	case OCREP_PROP_NULL:
	default:
		break;
	}

	*list = l;
	return IOTCON_ERROR_NONE;
}

static int _icl_ioty_parse_oic_rep_payload_value_array(
		OCRepPayloadValueArray *arr, int depth, int len, int index, iotcon_list_h *list)
{
	int i, ret, next_len;
	iotcon_list_h l;

	RETV_IF(NULL == arr, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == list, IOTCON_ERROR_INVALID_PARAMETER);

	if ((MAX_REP_ARRAY_DEPTH - 1) == depth || 0 == arr->dimensions[depth + 1]) {
		ret = _icl_ioty_parse_oic_rep_payload_value_array_attr(arr, len, index, &l);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("_icl_ioty_parse_oic_rep_payload_value_array_attr() Fail(%d)", ret);
			return ret;
		}
		*list = l;
		return IOTCON_ERROR_NONE;
	}

	i = len + index;

	next_len = len / arr->dimensions[depth];
	index -= next_len;

	ret = iotcon_list_create(IOTCON_TYPE_LIST, &l);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_list_create() Fail(%d)", ret);
		return ret;
	}

	while ((index += next_len) < i) {
		iotcon_list_h l_child = NULL;
		ret = _icl_ioty_parse_oic_rep_payload_value_array(arr, depth + 1, next_len, index,
				&l_child);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("_icl_ioty_parse_oic_rep_payload_value_array() Fail(%d)", ret);
			iotcon_list_destroy(l);
			return ret;
		}

		ret = iotcon_list_add_list(l, l_child, -1);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_list_add_list() Fail(%d)", ret);
			iotcon_list_destroy(l_child);
			iotcon_list_destroy(l);
			return ret;
		}
		iotcon_list_destroy(l_child);
	}
	*list = l;
	return IOTCON_ERROR_NONE;
}

static int _ioty_parse_oic_rep_payload_value(OCRepPayloadValue *val,
		iotcon_attributes_h *attributes)
{
	int ret, total_len;
	iotcon_attributes_h s;
	iotcon_list_h list;
	iotcon_attributes_h s_obj;

	RETV_IF(NULL == val, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);

	ret = iotcon_attributes_create(&s);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_attributes_create() Fail(%d)", ret);
		return ret;
	}

	while (val) {
		switch (val->type) {
		case OCREP_PROP_INT:
			ret = iotcon_attributes_add_int(s, val->name, val->i);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_attributes_add_int() Fail(%d)", ret);
				iotcon_attributes_destroy(s);
				return ret;
			}
			break;
		case OCREP_PROP_BOOL:
			ret = iotcon_attributes_add_bool(s, val->name, val->b);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_attributes_add_bool() Fail(%d)", ret);
				iotcon_attributes_destroy(s);
				return ret;
			}
			break;
		case OCREP_PROP_DOUBLE:
			ret = iotcon_attributes_add_double(s, val->name, val->d);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_attributes_add_double() Fail(%d)", ret);
				iotcon_attributes_destroy(s);
				return ret;
			}
			break;
		case OCREP_PROP_STRING:
			ret = iotcon_attributes_add_str(s, val->name, val->str);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_attributes_add_str() Fail(%d)", ret);
				iotcon_attributes_destroy(s);
				return ret;
			}
			break;
		case OCREP_PROP_BYTE_STRING:
			ret = iotcon_attributes_add_byte_str(s, val->name, val->ocByteStr.bytes,
					val->ocByteStr.len);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_attributes_add_byte_str() Fail(%d)", ret);
				iotcon_attributes_destroy(s);
				return ret;
			}
			break;
		case OCREP_PROP_NULL:
			ret = iotcon_attributes_add_null(s, val->name);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_attributes_add_null() Fail(%d)", ret);
				iotcon_attributes_destroy(s);
				return ret;
			}
			break;
		case OCREP_PROP_ARRAY:
			total_len = calcDimTotal(val->arr.dimensions);
			ret = _icl_ioty_parse_oic_rep_payload_value_array(&val->arr, 0, total_len, 0,
					&list);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("_icl_ioty_parse_oic_rep_payload_value_array() Fail(%d)", ret);
				iotcon_attributes_destroy(s);
				return ret;
			}
			ret = iotcon_attributes_add_list(s, val->name, list);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_attributes_add_list() Fail(%d)", ret);
				iotcon_list_destroy(list);
				iotcon_attributes_destroy(s);
				return ret;
			}
			iotcon_list_destroy(list);
			break;
		case OCREP_PROP_OBJECT:
			ret = _ioty_parse_oic_rep_payload_value(val->obj->values, &s_obj);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("_ioty_parse_oic_rep_payload_value() Fail(%d)", ret);
				iotcon_attributes_destroy(s);
				return ret;
			}
			ret = iotcon_attributes_add_attributes(s, val->name, s_obj);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_attributes_add_attributes() Fail(%d)", ret);
				iotcon_attributes_destroy(s_obj);
				iotcon_attributes_destroy(s);
				return ret;
			}
			iotcon_attributes_destroy(s_obj);
			break;
		default:
			ERR("Invalid Type(%d)", val->type);
		}
		val = val->next;
	}

	*attributes = s;

	return IOTCON_ERROR_NONE;
}

int ic_ioty_parse_oic_rep_payload(OCRepPayload *payload, bool is_parent,
		iotcon_representation_h *representation)
{
	int ret = 0;
	OCStringLL *node;
	OCRepPayload *child_node;
	iotcon_resource_interfaces_h ifaces;
	iotcon_representation_h repr;
	iotcon_attributes_h attributes = NULL;
	iotcon_resource_types_h types = NULL;

	RETV_IF(NULL == payload, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == representation, IOTCON_ERROR_INVALID_PARAMETER);

	ret = iotcon_representation_create(&repr);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_create() Fail(%d)", ret);
		return ret;
	}

	/* uri path */
	if (payload->uri)
		repr->uri_path = strdup(payload->uri);

	/* resource types */
	if (payload->types) {
		node = payload->types;
		while (node) {
			if (NULL == types) {
				ret = iotcon_resource_types_create(&types);
				if (IOTCON_ERROR_NONE != ret) {
					ERR("iotcon_resource_types_add() Fail(%d)", ret);
					iotcon_representation_destroy(repr);
					return ret;
				}
			}
			ret = iotcon_resource_types_add(types, node->value);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_resource_types_add() Fail(%d)", ret);
				iotcon_resource_types_destroy(types);
				iotcon_representation_destroy(repr);
				return ret;
			}
			node = node->next;
		}
		ret = iotcon_representation_set_resource_types(repr, types);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_representation_set_resource_types() Fail(%d)", ret);
			iotcon_resource_types_destroy(types);
			iotcon_representation_destroy(repr);
			return ret;
		}
		iotcon_resource_types_destroy(types);
	}

	iotcon_resource_interfaces_create(&ifaces);

	/* resource interfaces */
	node = payload->interfaces;
	if (node) {
		while (node) {
			iotcon_resource_interfaces_add(ifaces, node->value);
			node = node->next;
		}
	} else {
		// TODO: verify spec
		iotcon_resource_interfaces_add(ifaces, IOTCON_INTERFACE_DEFAULT);
	}

	ret = iotcon_representation_set_resource_interfaces(repr, ifaces);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_representation_set_resource_interfaces() Fail(%d)", ret);
		iotcon_resource_interfaces_destroy(ifaces);
		iotcon_representation_destroy(repr);
		return ret;
	}
	iotcon_resource_interfaces_destroy(ifaces);

	/* attributes */
	if (payload->values) {
		ret = _ioty_parse_oic_rep_payload_value(payload->values, &attributes);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_representation_set_resource_types() Fail(%d)", ret);
			iotcon_representation_destroy(repr);
			return ret;
		}
		ret = iotcon_representation_set_attributes(repr, attributes);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_representation_set_attributes() Fail(%d)", ret);
			iotcon_attributes_destroy(attributes);
			iotcon_representation_destroy(repr);
			return ret;
		}
		iotcon_attributes_destroy(attributes);
	}

	/* children */
	child_node = payload->next;
	while (is_parent && child_node) {
		/* generate recursively */
		iotcon_representation_h repr_child;

		ret = ic_ioty_parse_oic_rep_payload(child_node, false, &repr_child);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("ic_ioty_parse_oic_rep_payload() Fail(%d)", ret);
			iotcon_representation_destroy(repr);
			return ret;
		}

		ret = iotcon_representation_add_child(repr, repr_child);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("iotcon_representation_add_child() Fail(%d)", ret);
			iotcon_representation_destroy(repr_child);
			iotcon_representation_destroy(repr);
			return ret;
		}
		iotcon_representation_destroy(repr_child);

		child_node = child_node->next;
	}

	*representation = repr;

	return IOTCON_ERROR_NONE;
}

int ic_ioty_parse_oic_header_option(OCHeaderOption *option, int option_size,
		iotcon_options_h *options)
{
	int i, ret;
	iotcon_options_h op;

	RETV_IF(NULL == options, IOTCON_ERROR_INVALID_PARAMETER);

	if (option_size <= 0) {
		*options = NULL;
		return IOTCON_ERROR_NONE;
	}

	ret = iotcon_options_create(&op);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_options_create() Fail(%d)", ret);
		return ret;
	}

	for (i = 0; i < option_size; i++)
		iotcon_options_add(op, option[i].optionID, (char *)option[i].optionData);

	*options = op;

	return IOTCON_ERROR_NONE;
}

