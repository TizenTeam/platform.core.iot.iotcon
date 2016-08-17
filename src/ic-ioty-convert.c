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
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <glib.h>

#include <octypes.h>
#include <ocpayload.h>

#include "iotcon.h"
#include "ic.h"
#include "ic-list.h"
#include "ic-value.h"
#include "ic-options.h"
#include "ic-representation.h"
#include "ic-resource-types.h"
#include "ic-resource-interfaces.h"
#include "ic-ioty-convert.h"

struct icl_attributes_list_s {
	OCRepPayloadPropType type;
	size_t dimensions[MAX_REP_ARRAY_DEPTH];
	GList *list;
};

static int _icl_ioty_fill_oic_rep_payload_value(OCRepPayload *payload,
		iotcon_attributes_h attributes);


OCConnectivityType ic_ioty_convert_connectivity_type(
		iotcon_connectivity_type_e conn_type)
{
	int connectivity_type = conn_type;

	switch (connectivity_type) {
	case IOTCON_CONNECTIVITY_IPV4:
		return CT_ADAPTER_IP | CT_IP_USE_V4;
	case IOTCON_CONNECTIVITY_IPV6:
		return CT_ADAPTER_IP | CT_IP_USE_V6;
	case IOTCON_CONNECTIVITY_ALL:
		return CT_DEFAULT;
	default:
		ERR("Invalid conn_type(%d)", conn_type);
	}
	return CT_DEFAULT;
}


static void _ic_ioty_conn_type_to_oic_transport_type(int conn_type,
		OCTransportAdapter *adapter, OCTransportFlags *flag)
{
	RET_IF(NULL == adapter);
	RET_IF(NULL == flag);

	switch (conn_type) {
	case IOTCON_CONNECTIVITY_IPV4:
		*adapter = OC_ADAPTER_IP;
		*flag = OC_IP_USE_V4;
		break;
	case IOTCON_CONNECTIVITY_IPV6:
		*adapter = OC_ADAPTER_IP;
		*flag = OC_IP_USE_V6;
		break;
	case IOTCON_CONNECTIVITY_ALL:
	default:
		*adapter = OC_DEFAULT_ADAPTER;
		*flag = OC_DEFAULT_FLAGS;
	}
}


int ic_ioty_convert_connectivity(const char *host_address, int conn_type,
		OCDevAddr *dev_addr)
{
	char host[PATH_MAX] = {0};
	char *dev_host, *ptr = NULL;

	RETV_IF(NULL == dev_addr, IOTCON_ERROR_INVALID_PARAMETER);

	snprintf(host, sizeof(host), "%s", host_address);

	switch (conn_type) {
	case IOTCON_CONNECTIVITY_IPV4:
		dev_host = strtok_r(host, ":", &ptr);
		snprintf(dev_addr->addr, sizeof(dev_addr->addr), "%s", dev_host);
		dev_addr->port = atoi(strtok_r(NULL, ":", &ptr));
		break;
	case IOTCON_CONNECTIVITY_IPV6:
		dev_host = strtok_r(host, "]", &ptr);
		snprintf(dev_addr->addr, sizeof(dev_addr->addr), "%s", dev_host + 1);
		dev_addr->port = atoi(strtok_r(NULL, ":", &ptr));
		break;
	default:
		ERR("Invalid Connectivity Type(%d)", conn_type);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	_ic_ioty_conn_type_to_oic_transport_type(conn_type, &(dev_addr->adapter),
			&(dev_addr->flags));

	return IOTCON_ERROR_NONE;
}

OCEntityHandlerResult ic_ioty_convert_response_result(
		iotcon_response_result_e result)
{
	switch (result) {
	case IOTCON_RESPONSE_OK:
		return OC_EH_OK;
	case IOTCON_RESPONSE_RESOURCE_CREATED:
		return OC_EH_RESOURCE_CREATED;
	case IOTCON_RESPONSE_RESOURCE_DELETED:
		return OC_EH_RESOURCE_DELETED;
	case IOTCON_RESPONSE_RESOURCE_CHANGED:
		return OC_EH_CHANGED;
	case IOTCON_RESPONSE_FORBIDDEN:
		return OC_EH_FORBIDDEN;
	case IOTCON_RESPONSE_ERROR:
		return OC_EH_ERROR;
	default:
		WARN("Unknown result(%d)", result);
	}
	return OC_EH_ERROR;
}

OCMethod ic_ioty_convert_request_type(iotcon_request_type_e type)
{
	switch (type) {
	case IOTCON_REQUEST_GET:
		return OC_REST_GET;
	case IOTCON_REQUEST_PUT:
		return OC_REST_PUT;
	case IOTCON_REQUEST_POST:
		return OC_REST_POST;
	case IOTCON_REQUEST_DELETE:
		return OC_REST_DELETE;
	default:
		ERR("Unknown Type(%d)", type);
	}
	return OC_REST_NOMETHOD;
}

OCQualityOfService ic_ioty_convert_qos(iotcon_qos_e qos)
{
	switch (qos) {
	case IOTCON_QOS_HIGH:
		return OC_HIGH_QOS;
	case IOTCON_QOS_LOW:
		return OC_LOW_QOS;
	default:
		ERR("Unknown qos(%d)", qos);
	}
	return OC_NA_QOS;
}

uint8_t ic_ioty_convert_policies(uint8_t policies)
{
	uint8_t oic_policies = OC_RES_PROP_NONE;

	if (IOTCON_RESOURCE_DISCOVERABLE & policies)
		oic_policies |= OC_DISCOVERABLE;

	if (IOTCON_RESOURCE_OBSERVABLE & policies)
		oic_policies |= OC_OBSERVABLE;

	if (IOTCON_RESOURCE_ACTIVE & policies)
		oic_policies |= OC_ACTIVE;

	if (IOTCON_RESOURCE_SLOW & policies)
		oic_policies |= OC_SLOW;

	if (IOTCON_RESOURCE_SECURE & policies)
		oic_policies |= OC_SECURE;

	if (IOTCON_RESOURCE_EXPLICIT_DISCOVERABLE & policies)
		oic_policies |= OC_EXPLICIT_DISCOVERABLE;

	return oic_policies;
}

int ic_ioty_convert_header_options(iotcon_options_h options,
		OCHeaderOption dest[], int dest_size)
{
	int i = 0, src_size;
	GHashTableIter iter;
	gpointer option_id, option_data;

	RETV_IF(NULL == options, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == options->hash, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == dest, IOTCON_ERROR_INVALID_PARAMETER);

	src_size = g_hash_table_size(options->hash);

	if (dest_size < src_size) {
		ERR("Exceed Size(%d)", src_size);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	g_hash_table_iter_init(&iter, options->hash);
	while (g_hash_table_iter_next(&iter, &option_id, &option_data)) {
		dest[i].protocolID = OC_COAP_ID;
		dest[i].optionID = GPOINTER_TO_INT(option_id);
		dest[i].optionLength = strlen(option_data) + 1;
		memcpy(dest[i].optionData, option_data, dest[i].optionLength);
		i++;
	}
	return IOTCON_ERROR_NONE;
}

static int _icl_ioty_attributes_list_to_value_list(iotcon_list_h list,
		struct icl_attributes_list_s *value_list, int depth)
{
	int ret;
	GList *c;

	RETV_IF(NULL == list, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == value_list, IOTCON_ERROR_INVALID_PARAMETER);

	value_list->dimensions[depth] = g_list_length(list->list);

	switch (list->type) {
	case IOTCON_TYPE_BOOL:
		value_list->type = OCREP_PROP_BOOL;
		for (c = list->list; c; c = c->next)
			value_list->list = g_list_append(value_list->list, c->data);
		break;
	case IOTCON_TYPE_INT:
		value_list->type = OCREP_PROP_INT;
		for (c = list->list; c; c = c->next)
			value_list->list = g_list_append(value_list->list, c->data);
		break;
	case IOTCON_TYPE_DOUBLE:
		value_list->type = OCREP_PROP_DOUBLE;
		for (c = list->list; c; c = c->next)
			value_list->list = g_list_append(value_list->list, c->data);
		break;
	case IOTCON_TYPE_STR:
		value_list->type = OCREP_PROP_STRING;
		for (c = list->list; c; c = c->next)
			value_list->list = g_list_append(value_list->list, c->data);
		break;
	case IOTCON_TYPE_BYTE_STR:
		value_list->type = OCREP_PROP_BYTE_STRING;
		for (c = list->list; c; c = c->next)
			value_list->list = g_list_append(value_list->list, c->data);
		break;
	case IOTCON_TYPE_ATTRIBUTES:
		value_list->type = OCREP_PROP_OBJECT;
		for (c = list->list; c; c = c->next)
			value_list->list = g_list_append(value_list->list, c->data);
		break;
	case IOTCON_TYPE_LIST:
		for (c = list->list; c; c = c->next) {
			ret = _icl_ioty_attributes_list_to_value_list(((icl_val_list_s *)c->data)->list,
					value_list, depth + 1);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("_icl_ioty_attributes_list_to_value_list() Fail(%d)", ret);
				return ret;
			}
		}
		break;
	case IOTCON_TYPE_NULL:
	default:
		ERR("Invalid type (%d)", list->type);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	return IOTCON_ERROR_NONE;
}

static int _icl_ioty_fill_oic_rep_payload_value_array(OCRepPayload *payload,
		const char *key, struct icl_attributes_list_s *list)
{
	int i, j, len, ret;
	bool *b_arr;
	double *d_arr;
	char **str_arr;
	int64_t *i_arr;
	OCByteString *byte_arr;
	struct OCRepPayload **attributes_arr;
	GList *node;

	RETV_IF(NULL == payload, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == key, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == list, IOTCON_ERROR_INVALID_PARAMETER);

	if (NULL == list->list || 0 == g_list_length(list->list)) {
		DBG("list is empty");
		return IOTCON_ERROR_NONE;
	}

	len = calcDimTotal(list->dimensions);

	switch (list->type) {
	case OCREP_PROP_BOOL:
		b_arr = calloc(len, sizeof(bool));
		if (NULL == b_arr) {
			ERR("calloc() Fail(%d)", errno);
			return IOTCON_ERROR_OUT_OF_MEMORY;
		}
		for (node = list->list, i = 0; node; node = node->next, i++)
			b_arr[i] = ((icl_basic_s*)node->data)->val.b;
		OCRepPayloadSetBoolArrayAsOwner(payload, key, b_arr, list->dimensions);
		break;
	case OCREP_PROP_INT:
		i_arr = calloc(len, sizeof(int64_t));
		if (NULL == i_arr) {
			ERR("calloc() Fail(%d)", errno);
			return IOTCON_ERROR_OUT_OF_MEMORY;
		}
		for (node = list->list, i = 0; node; node = node->next, i++)
			i_arr[i] = ((icl_basic_s*)node->data)->val.b;
		OCRepPayloadSetIntArrayAsOwner(payload, key, i_arr, list->dimensions);
		break;
	case OCREP_PROP_DOUBLE:
		d_arr = calloc(len, sizeof(double));
		if (NULL == d_arr) {
			ERR("calloc() Fail(%d)", errno);
			return IOTCON_ERROR_OUT_OF_MEMORY;
		}
		for (node = list->list, i = 0; node; node = node->next, i++)
			d_arr[i] = ((icl_basic_s*)node->data)->val.d;
		OCRepPayloadSetDoubleArrayAsOwner(payload, key, d_arr, list->dimensions);
		break;
	case OCREP_PROP_STRING:
		str_arr = calloc(len, sizeof(char *));
		if (NULL == str_arr) {
			ERR("calloc() Fail(%d)", errno);
			return IOTCON_ERROR_OUT_OF_MEMORY;
		}
		for (node = list->list, i = 0; node; node = node->next, i++)
			str_arr[i] = ((icl_basic_s*)node->data)->val.s;

		OCRepPayloadSetStringArray(payload, key, (const char **)str_arr, list->dimensions);
		free(str_arr);
		break;
	case OCREP_PROP_BYTE_STRING:
		byte_arr = calloc(len, sizeof(OCByteString));
		if (NULL == byte_arr) {
			ERR("calloc() Fail(%d)", errno);
			return IOTCON_ERROR_OUT_OF_MEMORY;
		}
		for (node = list->list, i = 0; node; node = node->next, i++) {
			byte_arr[i].bytes = ((icl_val_byte_str_s*)node->data)->s;
			byte_arr[i].len = ((icl_val_byte_str_s*)node->data)->len;
		}
		OCRepPayloadSetByteStringArray(payload, key, byte_arr, list->dimensions);
		free(byte_arr);
		break;
	case OCREP_PROP_OBJECT:
		attributes_arr = calloc(len, sizeof(struct OCRepPayload *));
		if (NULL == attributes_arr) {
			ERR("calloc() Fail(%d)", errno);
			return IOTCON_ERROR_OUT_OF_MEMORY;
		}
		for (node = list->list, i = 0; node; node = node->next, i++) {
			attributes_arr[i] = OCRepPayloadCreate();
			if (NULL == attributes_arr[i]) {
				ERR("OCRepPayloadCreate() Fail");
				free(attributes_arr);
				return ret;
			}
			ret = _icl_ioty_fill_oic_rep_payload_value(attributes_arr[i],
					((icl_val_attributes_s*)node->data)->attributes);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("_icl_ioty_fill_oic_rep_payload_value() Fail(%d)", ret);
				for (j = 0; j <= i; j++)
					OCRepPayloadDestroy(attributes_arr[j]);
				free(attributes_arr);
				return ret;
			}
		}
		OCRepPayloadSetPropObjectArrayAsOwner(payload, key, attributes_arr, list->dimensions);
		break;
	case OCREP_PROP_ARRAY:
	case OCREP_PROP_NULL:
	default:
		ERR("Invalid Type(%d)", list->type);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}
	return IOTCON_ERROR_NONE;
}

static int _icl_ioty_fill_oic_rep_payload_value(OCRepPayload *payload,
		iotcon_attributes_h attributes)
{
	FN_CALL;
	int ret;
	GHashTableIter iter;
	gpointer key, value;
	OCRepPayload *repr_payload;
	OCByteString byte_string;
	struct icl_value_s *attributes_value;
	struct icl_attributes_list_s value_list = {0};

	RETV_IF(NULL == attributes, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == payload, IOTCON_ERROR_INVALID_PARAMETER);

	g_hash_table_iter_init(&iter, attributes->hash_table);
	while (g_hash_table_iter_next(&iter, &key, &value)) {
		attributes_value = (struct icl_value_s *)value;
		if (NULL == attributes_value) {
			ERR("attributes_value(%s) is NULL", key);
			continue;
		}

		switch (attributes_value->type) {
		case IOTCON_TYPE_BOOL:
			OCRepPayloadSetPropBool(payload, key, ((icl_basic_s*)attributes_value)->val.b);
			break;
		case IOTCON_TYPE_INT:
			OCRepPayloadSetPropInt(payload, key, ((icl_basic_s*)attributes_value)->val.i);
			break;
		case IOTCON_TYPE_DOUBLE:
			OCRepPayloadSetPropDouble(payload, key, ((icl_basic_s*)attributes_value)->val.d);
			break;
		case IOTCON_TYPE_STR:
			OCRepPayloadSetPropString(payload, key, ((icl_basic_s*)attributes_value)->val.s);
			break;
		case IOTCON_TYPE_NULL:
			OCRepPayloadSetNull(payload, key);
			break;
		case IOTCON_TYPE_BYTE_STR:
			byte_string.bytes = ((icl_val_byte_str_s*)attributes_value)->s;
			byte_string.len = ((icl_val_byte_str_s*)attributes_value)->len;
			OCRepPayloadSetPropByteString(payload, key, byte_string);
			break;
		case IOTCON_TYPE_LIST:
			ret = _icl_ioty_attributes_list_to_value_list(((icl_val_list_s*)attributes_value)->list,
					&value_list, 0);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("_icl_ioty_attributes_list_to_value_list() Fail(%d)", ret);
				return ret;
			}
			ret = _icl_ioty_fill_oic_rep_payload_value_array(payload, key, &value_list);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("_icl_ioty_fill_oic_rep_payload_value_array() Fail(%d)", ret);
				g_list_free(value_list.list);
				return ret;
			}
			g_list_free(value_list.list);
			value_list.list = NULL;
			break;
		case IOTCON_TYPE_ATTRIBUTES:
			repr_payload = OCRepPayloadCreate();
			ret = _icl_ioty_fill_oic_rep_payload_value(repr_payload,
					((icl_val_attributes_s*)attributes_value)->attributes);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("_icl_ioty_fill_oic_rep_payload_value() Fail(%d)", ret);
				OCRepPayloadDestroy(repr_payload);
				return ret;
			}
			OCRepPayloadSetPropObjectAsOwner(payload, key, repr_payload);
			break;
		case IOTCON_TYPE_NONE:
		default:
			ERR("Invalid Type(%d)", attributes_value->type);
			return IOTCON_ERROR_INVALID_PARAMETER;
		}
	}
	return IOTCON_ERROR_NONE;
}

int ic_ioty_convert_representation(iotcon_representation_h repr,
		OCPayload **payload)
{
	FN_CALL;
	int ret;
	GList *c;
	OCRepPayload *repr_payload, *cur;

	RETV_IF(NULL == payload, IOTCON_ERROR_INVALID_PARAMETER);

	repr_payload = OCRepPayloadCreate();
	if (NULL == repr_payload) {
		ERR("OCRepPayloadCreate() Fail");
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	if (NULL == repr) {
		*payload = (OCPayload *)repr_payload;
		return IOTCON_ERROR_NONE;
	}

	/* uri path */
	if (repr->uri_path)
		OCRepPayloadSetUri(repr_payload, repr->uri_path);

	/* interfaces */
	if (repr->interfaces) {
		for (c = repr->interfaces->iface_list; c; c = c->next)
			OCRepPayloadAddInterface(repr_payload, c->data);
	} else {
		OCRepPayloadAddInterface(repr_payload, IOTCON_INTERFACE_DEFAULT);
	}

	/* resource types */
	if (repr->res_types) {
		for (c = repr->res_types->type_list; c; c = c->next)
			OCRepPayloadAddResourceType(repr_payload, c->data);
	}

	/* attributes */
	if (repr->attributes) {
		ret = _icl_ioty_fill_oic_rep_payload_value(repr_payload, repr->attributes);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("_icl_ioty_fill_oic_rep_payload_value() Fail(%d)", ret);
			OCRepPayloadDestroy(repr_payload);
			return ret;
		}
	}

	/* children */
	cur = repr_payload;
	for (c = repr->children; c; c = c->next) {
		OCPayload *child = NULL;
		ret = ic_ioty_convert_representation(c->data, &child);
		if (IOTCON_ERROR_NONE != ret) {
			ERR("ic_ioty_convert_representation() Fail(%d)", ret);
			OCRepPayloadDestroy(repr_payload);
			return IOTCON_ERROR_IOTIVITY;
		}
		cur->next = (OCRepPayload *)child;
		cur = cur->next;
	}

	*payload = (OCPayload *)repr_payload;
	return IOTCON_ERROR_NONE;
}

