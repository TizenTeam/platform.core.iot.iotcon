/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <glib.h>
#include <octypes.h>
#include <ocstack.h>
#include <ocprovisioningmanager.h>
#include <oxmjustworks.h>
#include <oxmrandompin.h>
#include <pmutility.h>

#include "iotcon.h"
#include "iotcon-provisioning.h"
#include "ic.h"
#include "ic-utils.h"
#include "ic-ioty.h"
#include "ic-provisioning-struct.h"

#define ICL_PROVISIONING_TIMEOUT_MAX 10

struct icl_provisioning_device {
	bool is_found;
	OCProvisionDev_t *device;
	char *host_address;
	int connectivity_type;
	char *device_id;
	int ref_count;
};

struct icl_provisioning_acl {
	OCProvisionDev_t *device;
	GList *resource_list;
	int permission;
	int ref_count;
};


char* _provisioning_parse_uuid(OicUuid_t *uuid)
{
	char uuid_string[256] = {0};

	snprintf(uuid_string, sizeof(uuid_string),
			"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			(*uuid).id[0], (*uuid).id[1], (*uuid).id[2], (*uuid).id[3],
			(*uuid).id[4], (*uuid).id[5], (*uuid).id[6], (*uuid).id[7],
			(*uuid).id[8], (*uuid).id[9], (*uuid).id[10], (*uuid).id[11],
			(*uuid).id[12], (*uuid).id[13], (*uuid).id[14], (*uuid).id[15]);

	DBG("uuid : %s", uuid_string);

	return strdup(uuid_string);
}


void icl_provisioning_print_uuid(OicUuid_t *uuid)
{
	DBG("uuid : %02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			(*uuid).id[0], (*uuid).id[1], (*uuid).id[2], (*uuid).id[3],
			(*uuid).id[4], (*uuid).id[5], (*uuid).id[6], (*uuid).id[7],
			(*uuid).id[8], (*uuid).id[9], (*uuid).id[10], (*uuid).id[11],
			(*uuid).id[12], (*uuid).id[13], (*uuid).id[14], (*uuid).id[15]);
}


OicUuid_t* icl_provisioning_convert_device_id(const char *device_id)
{
	OicUuid_t *uuid;

	RETV_IF(NULL == device_id, NULL);

	uuid = calloc(1, sizeof(struct OicUuid));
	if (NULL == uuid) {
		ERR("calloc() Fail(%d)", errno);
		return NULL;
	}

	sscanf(&device_id[0], "%2hhx", &uuid->id[0]);
	sscanf(&device_id[2], "%2hhx", &uuid->id[1]);
	sscanf(&device_id[4], "%2hhx", &uuid->id[2]);
	sscanf(&device_id[6], "%2hhx", &uuid->id[3]);
	/* device_id[8] == '-' */
	sscanf(&device_id[9], "%2hhx", &uuid->id[4]);
	sscanf(&device_id[11], "%2hhx", &uuid->id[5]);
	/* device_id[13] == '-' */
	sscanf(&device_id[14], "%2hhx", &uuid->id[6]);
	sscanf(&device_id[16], "%2hhx", &uuid->id[7]);
	/* device_id[18] == '-' */
	sscanf(&device_id[19], "%2hhx", &uuid->id[8]);
	sscanf(&device_id[21], "%2hhx", &uuid->id[9]);
	/* device_id[23] == '-' */
	sscanf(&device_id[24], "%2hhx", &uuid->id[10]);
	sscanf(&device_id[26], "%2hhx", &uuid->id[11]);
	sscanf(&device_id[28], "%2hhx", &uuid->id[12]);
	sscanf(&device_id[30], "%2hhx", &uuid->id[13]);
	sscanf(&device_id[32], "%2hhx", &uuid->id[14]);
	sscanf(&device_id[34], "%2hhx", &uuid->id[15]);

	icl_provisioning_print_uuid(uuid);

	return uuid;
}


bool icl_provisioning_compare_oic_uuid(OicUuid_t *a, OicUuid_t *b)
{
	int i;
	bool ret = true;

	RETV_IF(NULL == a, false);
	RETV_IF(NULL == b, false);

	for (i = 0; i < UUID_LENGTH; i++) {
		if ((*a).id[i] != (*b).id[i]) {
			ret = false;
			break;
		}
	}
	return ret;
}


int icl_provisioning_parse_oic_dev_address(OCDevAddr *dev_addr, int secure_port,
		OCConnectivityType conn_type, char **host_address)
{
	int port;
	char temp[PATH_MAX] = {0};

	RETV_IF(NULL == dev_addr, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == host_address, IOTCON_ERROR_INVALID_PARAMETER);

	if (0 == secure_port)
		port = dev_addr->port;
	else
		port = secure_port;

	if (CT_ADAPTER_IP & conn_type) {
		if (CT_IP_USE_V4 & conn_type) {
			snprintf(temp, sizeof(temp), "%s:%d", dev_addr->addr, port);
		} else if (CT_IP_USE_V6 & conn_type) {
			snprintf(temp, sizeof(temp), "[%s]:%d", dev_addr->addr, port);
		} else {
			ERR("Invalid Connectivity Type(%d)", conn_type);
			return IOTCON_ERROR_INVALID_PARAMETER;
		}
	} else {
		ERR("Invalid Connectivity Type(%d)", conn_type);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	*host_address = strdup(temp);

	return IOTCON_ERROR_NONE;
}


OCProvisionDev_t* icl_provisioning_device_clone(OCProvisionDev_t *src)
{
	FN_CALL;

	OCProvisionDev_t *clone;

	RETV_IF(NULL == src, NULL);

	clone = PMCloneOCProvisionDev(src);
	if (NULL == clone) {
		ERR("PMCloneOCProvisionDev() Fail");
		return NULL;
	}

	if (clone->pstat) {
		if (src->pstat->sm) {
			clone->pstat->sm = calloc(1, sizeof(OicSecDpom_t));
			if (NULL == clone->pstat->sm) {
				ERR("calloc() Fail(%d)", errno);
				return NULL;
			}
			memcpy(clone->pstat->sm, src->pstat->sm, sizeof(OicSecDpom_t));
		}
	}

	if (clone->doxm) {
		if (src->doxm->oxmType) {
			clone->doxm->oxmType = calloc(1, sizeof(OicUrn_t));
			if (NULL == clone->doxm->oxmType) {
				ERR("calloc() Fail(%d)", errno);
				return NULL;
			}
			memcpy(clone->doxm->oxmType, src->doxm->oxmType, sizeof(OicUrn_t));
		}
		if (src->doxm->oxm) {
			clone->doxm->oxm = calloc(1, sizeof(OicSecOxm_t));
			if (NULL == clone->doxm->oxm) {
				ERR("calloc() Fail(%d)", errno);
				return NULL;
			}
			memcpy(clone->doxm->oxm, src->doxm->oxm, sizeof(OicSecOxm_t));
		}
	}

	return clone;
}


static int _provisioning_device_get_host_address(OCProvisionDev_t *device,
		char **host_address, int *connectivity_type)
{
	FN_CALL;
	char host[PATH_MAX] = {0};
	int temp_connectivity_type = CT_DEFAULT;

	if (device->connType & CT_ADAPTER_IP) {
		if (device->connType & CT_IP_USE_V4) {
			temp_connectivity_type = IOTCON_CONNECTIVITY_IPV4;
		} else if (device->connType & CT_IP_USE_V6) {
			temp_connectivity_type = IOTCON_CONNECTIVITY_IPV6;
		} else {
			ERR("Invalid Connectivity Type(%d)", device->connType);
			return IOTCON_ERROR_IOTIVITY;
		}
	} else {
		ERR("Invalid Connectivity Type(%d)", device->connType);
		return IOTCON_ERROR_IOTIVITY;
	}

	switch (temp_connectivity_type) {
	case IOTCON_CONNECTIVITY_IPV6:
		snprintf(host, sizeof(host), "[%s]:%d", device->endpoint.addr,
				device->endpoint.port);
		break;
	case IOTCON_CONNECTIVITY_IPV4:
		snprintf(host, sizeof(host), "%s:%d", device->endpoint.addr,
				device->endpoint.port);
		break;
	default:
		ERR("Invalid Connectivity Type(%d)", device->connType);
		return IOTCON_ERROR_IOTIVITY;
	}

	*host_address = strdup(host);
	*connectivity_type = temp_connectivity_type;

	return IOTCON_ERROR_NONE;
}


int icl_provisioning_device_create(OCProvisionDev_t *device,
		iotcon_provisioning_device_h *ret_device)
{
	FN_CALL;
	int ret;
	iotcon_provisioning_device_h temp;

	RETV_IF(NULL == device, IOTCON_ERROR_INVALID_PARAMETER);

	temp = calloc(1, sizeof(struct icl_provisioning_device));
	if (NULL == temp) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	temp->device = icl_provisioning_device_clone(device);
	if (NULL == temp->device) {
		ERR("icl_provisioning_device_clone() Fail");
		free(temp);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	ret = _provisioning_device_get_host_address(device, &temp->host_address,
			&temp->connectivity_type);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("_provisioning_device_get_host_address() Fail(%d)", ret);
		OCDeleteDiscoveredDevices(temp->device);
		free(temp);
		return ret;
	}

	temp->device_id = _provisioning_parse_uuid(&device->doxm->deviceID);

	temp->ref_count = 1;

	*ret_device = temp;

	return IOTCON_ERROR_NONE;
}


API int iotcon_provisioning_device_clone(iotcon_provisioning_device_h device,
		iotcon_provisioning_device_h *cloned_device)
{
	FN_CALL;
	int ret;
	iotcon_provisioning_device_h temp;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == device, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cloned_device, IOTCON_ERROR_INVALID_PARAMETER);

	ret = icl_provisioning_device_create(device->device, &temp);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_provisioning_device_create() Fail(%d)", ret);
		return ret;
	}

	*cloned_device = temp;

	return IOTCON_ERROR_NONE;
}


iotcon_provisioning_device_h icl_provisioning_device_ref(
		iotcon_provisioning_device_h device)
{
	RETV_IF(NULL == device, NULL);

	device->ref_count++;

	return device;
}


API int iotcon_provisioning_device_destroy(iotcon_provisioning_device_h device)
{
	FN_CALL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == device, IOTCON_ERROR_INVALID_PARAMETER);

	if (true == device->is_found) {
		ERR("It can't be destroyed by user.");
		ERR("host address : %s", device->host_address);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	device->ref_count--;
	if (0 < device->ref_count)
		return IOTCON_ERROR_NONE;

	free(device->host_address);
	free(device->device_id);

	OCDeleteDiscoveredDevices(device->device);

	free(device);

	return IOTCON_ERROR_NONE;
}


void icl_provisioning_device_set_found(iotcon_provisioning_device_h device)
{
	if (NULL == device)
		return;

	device->is_found = true;
}


void icl_provisioning_device_unset_found(iotcon_provisioning_device_h device)
{
	if (NULL == device)
		return;

	device->is_found = false;
}


bool icl_provisioning_device_is_found(iotcon_provisioning_device_h device)
{
	RETV_IF(NULL == device, false);

	return device->is_found;
}


OCProvisionDev_t* icl_provisioning_device_get_device(
		iotcon_provisioning_device_h device)
{
	FN_CALL;
	RETV_IF(NULL == device, NULL);

	return device->device;
}


API int iotcon_provisioning_device_get_host_address(iotcon_provisioning_device_h device,
		char **host_address)
{
	FN_CALL;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == device, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == host_address, IOTCON_ERROR_INVALID_PARAMETER);

	*host_address = device->host_address;

	return IOTCON_ERROR_NONE;
}


API int iotcon_provisioning_device_get_connectivity_type(
		iotcon_provisioning_device_h device,
		iotcon_connectivity_type_e *connectivity_type)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == device, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == connectivity_type, IOTCON_ERROR_INVALID_PARAMETER);

	*connectivity_type = device->connectivity_type;

	return IOTCON_ERROR_NONE;
}


API int iotcon_provisioning_device_get_id(iotcon_provisioning_device_h device,
		char **device_id)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == device, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == device_id, IOTCON_ERROR_INVALID_PARAMETER);

	*device_id = device->device_id;

	return IOTCON_ERROR_NONE;
}


static int _provisioning_parse_oxm(OicSecOxm_t oic_oxm, iotcon_provisioning_oxm_e *oxm)
{
	iotcon_provisioning_oxm_e temp;

	switch (oic_oxm) {
	case OIC_JUST_WORKS:
		temp = IOTCON_OXM_JUST_WORKS;
		break;
	case OIC_RANDOM_DEVICE_PIN:
		temp = IOTCON_OXM_RANDOM_PIN;
		break;
	case OIC_MANUFACTURER_CERTIFICATE:
	default:
		ERR("Invalid oxm(%d)", oic_oxm);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	*oxm = temp;

	return IOTCON_ERROR_NONE;
}


API int iotcon_provisioning_device_get_oxm(iotcon_provisioning_device_h device,
		iotcon_provisioning_oxm_e *oxm)
{
	int ret;
	iotcon_provisioning_oxm_e temp;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == device, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == oxm, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == device->device, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == device->device->doxm, IOTCON_ERROR_INVALID_PARAMETER);

	ret = _provisioning_parse_oxm(device->device->doxm->oxmSel, &temp);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("_provisioning_parse_oxm() Fail(%d)", ret);
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	*oxm = temp;

	return IOTCON_ERROR_NONE;
}


API int iotcon_provisioning_device_is_owned(iotcon_provisioning_device_h device,
		bool *is_owned)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == device, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == device->device, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == device->device->doxm, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == is_owned, IOTCON_ERROR_INVALID_PARAMETER);

	*is_owned = device->device->doxm->owned;

	return IOTCON_ERROR_NONE;
}


void icl_provisioning_device_set_owned(iotcon_provisioning_device_h device)
{
	RET_IF(NULL == device);
	RET_IF(NULL == device->device);
	RET_IF(NULL == device->device->doxm);

	device->device->doxm->owned = true;
}


void icl_provisioning_device_print(iotcon_provisioning_device_h device)
{
	RET_IF(NULL == device);

	INFO("Device Information");
	INFO("host address : %s", device->host_address);
	INFO("device ID : %s", device->device_id);

	if (device->device)
		INFO("security version : %s", device->device->secVer);
}


API int iotcon_provisioning_acl_create(iotcon_provisioning_acl_h *acl)
{
	iotcon_provisioning_acl_h temp;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == acl, IOTCON_ERROR_INVALID_PARAMETER);

	temp = calloc(1, sizeof(struct icl_provisioning_acl));
	if (NULL == temp) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	temp->ref_count = 1;

	*acl = temp;

	return IOTCON_ERROR_NONE;
}


iotcon_provisioning_acl_h icl_provisioning_acl_ref(iotcon_provisioning_acl_h acl)
{
	RETV_IF(NULL == acl, NULL);

	acl->ref_count++;

	return acl;
}


API int iotcon_provisioning_acl_destroy(iotcon_provisioning_acl_h acl)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == acl, IOTCON_ERROR_INVALID_PARAMETER);

	acl->ref_count--;
	if (0 < acl->ref_count)
		return IOTCON_ERROR_NONE;

	OCDeleteDiscoveredDevices(acl->device);
	g_list_free_full(acl->resource_list, free);
	free(acl);

	return IOTCON_ERROR_NONE;
}


static gpointer _provisioning_acl_resource_list_clone(gconstpointer src,
		gpointer data)
{
	return ic_utils_strdup(src);
}


int icl_provisioning_acl_clone(iotcon_provisioning_acl_h acl,
		iotcon_provisioning_acl_h *cloned_acl)
{
	int ret;
	iotcon_provisioning_acl_h temp;

	RETV_IF(NULL == acl, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cloned_acl, IOTCON_ERROR_INVALID_PARAMETER);

	ret = iotcon_provisioning_acl_create(&temp);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_provisioning_acl_create() Fail(%d)", ret);
		return ret;
	}

	temp->device = icl_provisioning_device_clone(acl->device);
	if (NULL == temp->device) {
		ERR("icl_provisioning_device_clone() Fail");
		iotcon_provisioning_acl_destroy(temp);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	temp->resource_list = g_list_copy_deep(acl->resource_list,
			_provisioning_acl_resource_list_clone, NULL);

	temp->permission = acl->permission;

	*cloned_acl = temp;

	return IOTCON_ERROR_NONE;
}


API int iotcon_provisioning_acl_set_subject(iotcon_provisioning_acl_h acl,
		iotcon_provisioning_device_h device)
{
	OCProvisionDev_t *dev;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == acl, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == device, IOTCON_ERROR_INVALID_PARAMETER);

	dev = icl_provisioning_device_clone(device->device);
	if (NULL == dev) {
		ERR("icl_provisioning_device_clone() Fail");
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	OCDeleteDiscoveredDevices(acl->device);

	acl->device = dev;

	return IOTCON_ERROR_NONE;
}


API int iotcon_provisioning_acl_set_all_subject(iotcon_provisioning_acl_h acl)
{
	OCProvisionDev_t *dev;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == acl, IOTCON_ERROR_INVALID_PARAMETER);

	dev = calloc(1, sizeof(OCProvisionDev_t));
	if (NULL == dev) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	dev->doxm = calloc(1, sizeof(OicSecDoxm_t));
	if (NULL == dev->doxm) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	memcpy(&dev->doxm->deviceID, "*", 2);

	OCDeleteDiscoveredDevices(acl->device);

	acl->device = dev;

	return IOTCON_ERROR_NONE;
}


OCProvisionDev_t* icl_provisioning_acl_get_subject(iotcon_provisioning_acl_h acl)
{
	RETV_IF(NULL == acl, NULL);

	return acl->device;
}


API int iotcon_provisioning_acl_add_resource(iotcon_provisioning_acl_h acl,
		const char *uri_path)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == acl, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == uri_path, IOTCON_ERROR_INVALID_PARAMETER);

	acl->resource_list = g_list_append(acl->resource_list, ic_utils_strdup(uri_path));

	return IOTCON_ERROR_NONE;
}


API int iotcon_provisioning_acl_set_permission(iotcon_provisioning_acl_h acl,
		int permission)
{
	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == acl, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(permission <= 0 || IOTCON_PERMISSION_FULL_CONTROL < permission,
			IOTCON_ERROR_INVALID_PARAMETER);

	acl->permission = permission;

	return IOTCON_ERROR_NONE;
}


int icl_provisioning_acl_get_resource_count(iotcon_provisioning_acl_h acl)
{
	RETV_IF(NULL == acl, IOTCON_ERROR_INVALID_PARAMETER);

	return g_list_length(acl->resource_list);
}


char* icl_provisioning_acl_get_nth_resource(iotcon_provisioning_acl_h acl, int index)
{
	RETV_IF(NULL == acl, NULL);

	return g_list_nth_data(acl->resource_list, index);
}


int icl_provisioning_acl_convert_permission(int permission)
{
	int oic_permission = 0;

	if (permission & IOTCON_PERMISSION_CREATE)
		oic_permission |= PERMISSION_CREATE;
	if (permission & IOTCON_PERMISSION_READ)
		oic_permission |= PERMISSION_READ;
	if (permission & IOTCON_PERMISSION_WRITE)
		oic_permission |= PERMISSION_WRITE;
	if (permission & IOTCON_PERMISSION_DELETE)
		oic_permission |= PERMISSION_DELETE;
	if (permission & IOTCON_PERMISSION_NOTIFY)
		oic_permission |= PERMISSION_NOTIFY;

	DBG("permission : %d", oic_permission);

	return oic_permission;
}


int icl_provisioning_acl_get_permission(iotcon_provisioning_acl_h acl)
{
	RETV_IF(NULL == acl, 0);

	return acl->permission;
}

