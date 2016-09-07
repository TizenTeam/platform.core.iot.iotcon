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
#include "ic-ioty-parse.h"
#include "ic-provisioning-struct.h"

#define ICL_PROVISIONING_TIMEOUT_MAX 10

struct icl_provisioning_randompin_cb_container {
	iotcon_provisioning_randompin_cb cb;
	void *user_data;
};

struct icl_provisioning_ownership_transfer_cb_container {
	int result;
	iotcon_provisioning_device_h device;
	iotcon_provisioning_ownership_transfer_cb cb;
	void *user_data;
};

struct icl_provisioning_provision_cred_cb_container {
	iotcon_provisioning_device_h device1;
	iotcon_provisioning_device_h device2;
	iotcon_error_e result;
	iotcon_provisioning_provision_cred_cb cb;
	void *user_data;
};

struct icl_provisioning_provision_acl_cb_container {
	iotcon_provisioning_device_h device;
	iotcon_provisioning_acl_h acl;
	iotcon_error_e result;
	iotcon_provisioning_provision_acl_cb cb;
	void *user_data;
};

struct icl_provisioning_pairwise_devices_cb_container {
	iotcon_provisioning_device_h device1;
	iotcon_provisioning_acl_h acl1;
	iotcon_provisioning_device_h device2;
	iotcon_provisioning_acl_h acl2;
	iotcon_error_e result;
	iotcon_provisioning_pairwise_devices_cb cb;
	void *user_data;
};

struct icl_provisioning_unlink_pairwise_cb_container {
	iotcon_provisioning_device_h device1;
	iotcon_provisioning_device_h device2;
	iotcon_error_e result;
	iotcon_provisioning_unlink_pairwise_cb cb;
	void *user_data;
};

static OTMCallbackData_t icl_justworks_otmcb;
static OTMCallbackData_t icl_pinbased_otmcb;
static struct icl_provisioning_randompin_cb_container icl_randompin_cb_container;

static iotcon_error_e _provisioning_parse_oic_error(OCStackResult ret)
{
	switch (ret) {
	case OC_STACK_RESOURCE_CREATED:
	case OC_STACK_RESOURCE_DELETED:
	case OC_STACK_RESOURCE_CHANGED:
		return IOTCON_ERROR_NONE;
	case OC_STACK_AUTHENTICATION_FAILURE:
		return IOTCON_ERROR_AUTHENTICATION_FAILURE;
	default:
		return ic_ioty_parse_oic_error(ret);
	}
}


static void _provisioning_set_justworks()
{
	icl_justworks_otmcb.loadSecretCB = LoadSecretJustWorksCallback;
	icl_justworks_otmcb.createSecureSessionCB = CreateSecureSessionJustWorksCallback;
	icl_justworks_otmcb.createSelectOxmPayloadCB = CreateJustWorksSelectOxmPayload;
	icl_justworks_otmcb.createOwnerTransferPayloadCB = CreateJustWorksOwnerTransferPayload;
}


static void _provisioning_set_randompin()
{
	icl_pinbased_otmcb.loadSecretCB = InputPinCodeCallback;
	icl_pinbased_otmcb.createSecureSessionCB = CreateSecureSessionRandomPinCallback;
	icl_pinbased_otmcb.createSelectOxmPayloadCB = CreatePinBasedSelectOxmPayload;
	icl_pinbased_otmcb.createOwnerTransferPayloadCB = CreatePinBasedOwnerTransferPayload;
}


API int iotcon_provisioning_initialize(const char *file_path, const char *db_path)
{
	FN_CALL;
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(false == ic_utils_check_permission((IC_PERMISSION_INTERNET
					|IC_PERMISSION_NETWORK_GET)), IOTCON_ERROR_PERMISSION_DENIED);
	RETV_IF(NULL == file_path, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == db_path, IOTCON_ERROR_INVALID_PARAMETER);

	ret = icl_initialize(file_path, true);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_initialize() Fail(%d)", ret);
		return ret;
	}

	ret = access(db_path, R_OK | W_OK);
	if (-1 != ret)
		DBG("Provisioning DB File already exists.");
	else
		DBG("No provisioning DB File, creating new.");

	ret = icl_ioty_mutex_lock();
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_mutex_lock() Fail(%d)", ret);
		iotcon_deinitialize();
		return ret;
	}

	ret = OCInitPM(db_path);
	if (OC_STACK_OK != ret) {
		ERR("OCInitPM() Fail(%d)", ret);
		ic_utils_mutex_unlock(IC_UTILS_MUTEX_IOTY);
		iotcon_deinitialize();
		return _provisioning_parse_oic_error(ret);
	}

	_provisioning_set_justworks();

	ret = OCSetOwnerTransferCallbackData(OIC_JUST_WORKS, &icl_justworks_otmcb);
	icl_ioty_mutex_unlock();
	if (OC_STACK_OK != ret) {
		ERR("OCSetOwnerTransferCallbackData() Fail(%d)", ret);
		iotcon_deinitialize();
		return _provisioning_parse_oic_error(ret);
	}

	return IOTCON_ERROR_NONE;
}


static void _provisioning_input_pin_cb(char *pin, size_t len)
{
	FN_CALL;
	char *temp;

	RET_IF(NULL == icl_randompin_cb_container.cb);
	RET_IF(NULL == pin);
	RET_IF(len <= OXM_RANDOM_PIN_SIZE);

	temp = icl_randompin_cb_container.cb(icl_randompin_cb_container.user_data);
	if ((NULL == temp) || (len <= strlen(temp))) {
		ERR("Invalid Pin Number");
		return;
	}

	strncpy(pin, temp, len);
}


API int iotcon_provisioning_set_randompin_cb(iotcon_provisioning_randompin_cb cb,
		void *user_data)
{
	FN_CALL;
	int ret;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	_provisioning_set_randompin();

	ret = icl_ioty_mutex_lock();
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_mutex_lock() Fail(%d)", ret);
		return ret;
	}

	ret = OCSetOwnerTransferCallbackData(OIC_RANDOM_DEVICE_PIN, &icl_pinbased_otmcb);
	icl_ioty_mutex_unlock();
	if (OC_STACK_OK != ret) {
		ERR("OCSetOwnerTransferCallbackData() Fail(%d)", ret);
		return _provisioning_parse_oic_error(ret);
	}

	icl_randompin_cb_container.cb = cb;
	icl_randompin_cb_container.user_data = user_data;

	SetInputPinCB(_provisioning_input_pin_cb);

	return IOTCON_ERROR_NONE;
}


static void _provisioning_ownership_transfer_cb_container_destroy(
		struct icl_provisioning_ownership_transfer_cb_container *container)
{
	if (container->device) {
		iotcon_provisioning_device_destroy(container->device);
		container->device = NULL;
	}

	free(container);
}


static gboolean _provisioning_ownership_transfer_idle_cb(gpointer p)
{
	struct icl_provisioning_ownership_transfer_cb_container *container = p;

	icl_provisioning_device_set_owned(container->device);

	if (container->cb)
		container->cb(container->device, container->result, container->user_data);

	_provisioning_ownership_transfer_cb_container_destroy(container);

	return G_SOURCE_REMOVE;
}


static void _provisioning_ownership_transfer_cb(void *ctx, int n_of_res,
		OCProvisionResult_t *arr, bool has_error)
{
	FN_CALL;
	struct icl_provisioning_ownership_transfer_cb_container *container;

	RET_IF(NULL == ctx);

	container = ctx;

	container->result = _provisioning_parse_oic_error(arr[0].res);
	DBG("result : %d", container->result);

	g_idle_add(_provisioning_ownership_transfer_idle_cb, container);

	return;
}


API int iotcon_provisioning_register_unowned_device(
		iotcon_provisioning_device_h device,
		iotcon_provisioning_ownership_transfer_cb cb,
		void *user_data)
{
	FN_CALL;
	int ret;
	OCProvisionDev_t *dev_list;
	struct icl_provisioning_ownership_transfer_cb_container *container;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == device, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	if (true == icl_provisioning_device_is_found(device)) {
		ERR("The device should be cloned");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	container = calloc(1, sizeof(struct icl_provisioning_ownership_transfer_cb_container));
	if (NULL == container) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	container->cb = cb;
	container->user_data = user_data;

	container->device = icl_provisioning_device_ref(device);

	dev_list = icl_provisioning_device_get_device(device);

	ret = icl_ioty_mutex_lock();
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_mutex_lock() Fail(%d)", ret);
		_provisioning_ownership_transfer_cb_container_destroy(container);
		return ret;
	}

	ret = OCDoOwnershipTransfer(container, dev_list, _provisioning_ownership_transfer_cb);
	icl_ioty_mutex_unlock();
	if (OC_STACK_OK != ret) {
		ERR("OCDoOwnershipTransfer() Fail(%d)", ret);
		_provisioning_ownership_transfer_cb_container_destroy(container);
		return _provisioning_parse_oic_error(ret);
	}

	return IOTCON_ERROR_NONE;
}


static void _provisioning_provision_cred_cb_container_destroy(
		struct icl_provisioning_provision_cred_cb_container *container)
{
	FN_CALL;

	if (container->device1) {
		iotcon_provisioning_device_destroy(container->device1);
		container->device1 = NULL;
	}
	if (container->device2) {
		iotcon_provisioning_device_destroy(container->device2);
		container->device2 = NULL;
	}

	free(container);
}


static gboolean _provisioning_provision_cred_idle_cb(gpointer p)
{
	struct icl_provisioning_provision_cred_cb_container *container = p;

	if (container->cb) {
		container->cb(container->device1, container->device2, container->result,
				container->user_data);
	}

	_provisioning_provision_cred_cb_container_destroy(container);

	return G_SOURCE_REMOVE;
}


static void _provisioning_provision_cred_cb(void *ctx, int n_of_res,
		OCProvisionResult_t *arr, bool has_error)
{
	FN_CALL;
	int i;
	struct icl_provisioning_provision_cred_cb_container *container = ctx;

	if (true == has_error)
		DBG("provision cred has error");

	container->result = IOTCON_ERROR_NONE;

	for (i = 0; i < n_of_res; i++) {
		DBG("arr[%d].res : %d", i, arr[i].res);
		if (OC_STACK_OK != arr[i].res)
			container->result = _provisioning_parse_oic_error(arr[i].res);
	}
	g_idle_add(_provisioning_provision_cred_idle_cb, container);
}


API int iotcon_provisioning_provision_cred(iotcon_provisioning_device_h device1,
		iotcon_provisioning_device_h device2,
		iotcon_provisioning_provision_cred_cb cb,
		void *user_data)
{
	FN_CALL;
	int ret;
	size_t key_size;
	struct icl_provisioning_provision_cred_cb_container *container;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == device1, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == device2, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	if (true == icl_provisioning_device_is_found(device1)) {
		ERR("The device should be cloned(device1)");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	if (true == icl_provisioning_device_is_found(device2)) {
		ERR("The device should be cloned(device2)");
		return IOTCON_ERROR_INVALID_PARAMETER;
	}

	key_size = OWNER_PSK_LENGTH_256; /* or OWNER_PSK_LENGTH_128 */

	container = calloc(1, sizeof(struct icl_provisioning_provision_cred_cb_container));
	if (NULL == container) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	container->device1 = icl_provisioning_device_ref(device1);
	container->device2 = icl_provisioning_device_ref(device2);
	container->cb = cb;
	container->user_data = user_data;

	ret = icl_ioty_mutex_lock();
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_mutex_lock() Fail(%d)", ret);
		_provisioning_provision_cred_cb_container_destroy(container);
		return ret;
	}

	ret = OCProvisionCredentials(container,
			SYMMETRIC_PAIR_WISE_KEY,
			key_size,
			icl_provisioning_device_get_device(container->device1),
			icl_provisioning_device_get_device(container->device2),
			_provisioning_provision_cred_cb);
	icl_ioty_mutex_unlock();
	if (OC_STACK_OK != ret) {
		ERR("OCProvisionCredentails() Fail(%d)", ret);
		_provisioning_provision_cred_cb_container_destroy(container);
		return _provisioning_parse_oic_error(ret);
	}

	return IOTCON_ERROR_NONE;
}


static void _provisioning_provision_acl_cb_container_destroy(
		struct icl_provisioning_provision_acl_cb_container *container)
{
	FN_CALL;

	if (container->device) {
		iotcon_provisioning_device_destroy(container->device);
		container->device = NULL;
	}

	free(container);
}


static void _provisioning_ace_add_resource(OicSecAce_t *ace, OicSecRsrc_t *resource)
{
	OicSecRsrc_t *current;

	current = ace->resources;

	if (NULL == current) {
		ace->resources = resource;
		return;
	}

	while (current->next)
		current = current->next;

	current->next = resource;
}


static OicSecAcl_t* _provisioning_convert_acl(iotcon_provisioning_device_h device,
		iotcon_provisioning_acl_h acl)
{
	int i;
	int permission;
	int num_of_resources;
	char *uri_path;
	OicSecAcl_t *oic_acl;
	OicSecAce_t *ace;
	OicSecRsrc_t *resource;
	OCProvisionDev_t *subject, *oic_device;

	oic_acl = calloc(1, sizeof(OicSecAcl_t));
	if (NULL == oic_acl) {
		ERR("calloc() Fail(%d)", errno);
		return NULL;
	}
	ace = calloc(1, sizeof(OicSecAce_t));
	if (NULL == ace) {
		ERR("calloc() Fail(%d)", errno);
		OCDeleteACLList(oic_acl);
		return NULL;
	}
	oic_acl->aces = ace;

	subject = icl_provisioning_acl_get_subject(acl);

	memcpy(&ace->subjectuuid, &subject->doxm->deviceID, 128/8);

	icl_provisioning_print_uuid(&ace->subjectuuid);

	num_of_resources = icl_provisioning_acl_get_resource_count(acl);

	for (i = 0; i < num_of_resources; i++) {
		resource = calloc(1, sizeof(OicSecRsrc_t));
		if (NULL == resource) {
			ERR("calloc() Fail(%d)", errno);
			OCDeleteACLList(oic_acl);
			return NULL;
		}

		uri_path = icl_provisioning_acl_get_nth_resource(acl, i);
		if (NULL == uri_path) {
			ERR("icl_provisioning_acl_get_nth_resource() Fail(%d)", errno);
			free(resource);
			OCDeleteACLList(oic_acl);
			return NULL;
		}
		resource->href = ic_utils_strdup(uri_path);

		// TODO: resource types & resource interfaces
		// temp code
		resource->types = calloc(1, sizeof(char*));
		if (NULL == resource->types) {
			ERR("calloc() Fail(%d)", errno);
			return NULL;
		}
		resource->types[0] = strdup(" ");
		resource->typeLen = 1;

		resource->interfaces = calloc(1, sizeof(char*));
		if (NULL == resource->interfaces) {
			ERR("calloc() Fail(%d)", errno);
			return NULL;
		}
		resource->interfaces[0] = strdup(OC_RSRVD_INTERFACE_DEFAULT);
		resource->interfaceLen = 1;

		_provisioning_ace_add_resource(ace, resource);
	}

	permission = icl_provisioning_acl_get_permission(acl);
	ace->permission = icl_provisioning_acl_convert_permission(permission);

	oic_device = icl_provisioning_device_get_device(device);

	memcpy(&oic_acl->rownerID, &oic_device->doxm->deviceID, sizeof(OicUuid_t));

	icl_provisioning_print_uuid(&oic_acl->rownerID);

	return oic_acl;
}


static gboolean _provisioning_provision_acl_idle_cb(void *p)
{
	struct icl_provisioning_provision_acl_cb_container *container = p;

	if (container->cb) {
		container->cb(container->device, container->acl, container->result,
				container->user_data);
	}

	_provisioning_provision_acl_cb_container_destroy(container);

	return G_SOURCE_REMOVE;
}


static void _provisioning_provision_acl_cb(void *ctx, int n_of_res,
		OCProvisionResult_t *arr, bool has_error)
{
	FN_CALL;
	int i;
	struct icl_provisioning_provision_acl_cb_container *container = ctx;

	if (true == has_error)
		DBG("provision acl has error");

	container->result = IOTCON_ERROR_NONE;

	for (i = 0; i < n_of_res; i++) {
		DBG("arr[%d].res : %d", i, arr[i].res);
		if (OC_STACK_OK != arr[i].res)
			container->result = _provisioning_parse_oic_error(arr[i].res);
	}
	g_idle_add(_provisioning_provision_acl_idle_cb, container);
}


API int iotcon_provisioning_provision_acl(iotcon_provisioning_device_h device,
		iotcon_provisioning_acl_h acl,
		iotcon_provisioning_provision_acl_cb cb,
		void *user_data)
{
	FN_CALL;
	int ret;
	OicSecAcl_t *oic_acl;
	struct icl_provisioning_provision_acl_cb_container *container;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == device, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == acl, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	container = calloc(1, sizeof(struct icl_provisioning_provision_acl_cb_container));
	if (NULL == container) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	container->device = icl_provisioning_device_ref(device);
	container->acl = icl_provisioning_acl_ref(acl);

	container->cb = cb;
	container->user_data = user_data;

	oic_acl = _provisioning_convert_acl(device, acl);

	ret = icl_ioty_mutex_lock();
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_mutex_lock() Fail(%d)", ret);
		_provisioning_provision_acl_cb_container_destroy(container);
		return ret;
	}

	ret = OCProvisionACL(container,
			icl_provisioning_device_get_device(container->device),
			oic_acl,
			_provisioning_provision_acl_cb);
	icl_ioty_mutex_unlock();
	if (OC_STACK_OK != ret) {
		ERR("OCProvisionACL() Fail(%d)", ret);
		_provisioning_provision_acl_cb_container_destroy(container);
		return _provisioning_parse_oic_error(ret);
	}

	return IOTCON_ERROR_NONE;
}


static void _provisioning_pairwise_devices_cb_container_destroy(
		struct icl_provisioning_pairwise_devices_cb_container *container)
{
	FN_CALL;

	if (container->device1) {
		iotcon_provisioning_device_destroy(container->device1);
		container->device1 = NULL;
	}
	if (container->acl1) {
		iotcon_provisioning_acl_destroy(container->acl1);
		container->acl1 = NULL;
	}
	if (container->device2) {
		iotcon_provisioning_device_destroy(container->device2);
		container->device2 = NULL;
	}
	if (container->acl2) {
		iotcon_provisioning_acl_destroy(container->acl2);
		container->acl2 = NULL;
	}

	free(container);
}


static gboolean _provisioning_pairwise_devices_idle_cb(void *p)
{
	struct icl_provisioning_pairwise_devices_cb_container *container = p;

	if (container->cb) {
		container->cb(container->device1, container->device2, container->result,
				container->user_data);
	}

	_provisioning_pairwise_devices_cb_container_destroy(container);

	return G_SOURCE_REMOVE;
}


static void _provisioning_pairwise_devices_cb(void *ctx, int n_of_res,
		OCProvisionResult_t *arr, bool has_error)
{
	FN_CALL;
	int i;
	struct icl_provisioning_pairwise_devices_cb_container *container = ctx;

	if (true == has_error)
		DBG("pairwise devices has error");

	container->result = IOTCON_ERROR_NONE;

	for (i = 0; i < n_of_res; i++) {
		DBG("arr[%d].res : %d", i, arr[i].res);
		if (OC_STACK_OK != arr[i].res)
			container->result = _provisioning_parse_oic_error(arr[i].res);
	}
	g_idle_add(_provisioning_pairwise_devices_idle_cb, container);
}


API int iotcon_provisioning_pairwise_devices(iotcon_provisioning_device_h device1,
		iotcon_provisioning_acl_h acl1,
		iotcon_provisioning_device_h device2,
		iotcon_provisioning_acl_h acl2,
		iotcon_provisioning_pairwise_devices_cb cb,
		void *user_data)

{
	FN_CALL;
	int ret;
	size_t key_size;
	OicSecAcl_t *oic_acl1 = NULL;
	OicSecAcl_t *oic_acl2 = NULL;
	struct icl_provisioning_pairwise_devices_cb_container *container;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == device1, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == device2, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	key_size = OWNER_PSK_LENGTH_256; /* or OWNER_PSK_LENGTH_256 */

	container = calloc(1, sizeof(struct icl_provisioning_pairwise_devices_cb_container));
	if (NULL == container) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	container->device1 = icl_provisioning_device_ref(device1);

	if (acl1) {
		container->acl1 = icl_provisioning_acl_ref(acl1);
		oic_acl1 = _provisioning_convert_acl(container->device1, container->acl1);
	}

	container->device2 = icl_provisioning_device_ref(device2);

	if (acl2) {
		container->acl2 = icl_provisioning_acl_ref(acl2);
		oic_acl2 = _provisioning_convert_acl(container->device2, container->acl2);
	}

	container->cb = cb;
	container->user_data = user_data;

	ret = icl_ioty_mutex_lock();
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_mutex_lock() Fail(%d)", ret);
		_provisioning_pairwise_devices_cb_container_destroy(container);
		return ret;
	}

	ret = OCProvisionPairwiseDevices(container,
			SYMMETRIC_PAIR_WISE_KEY,
			key_size,
			icl_provisioning_device_get_device(container->device1),
			oic_acl1,
			icl_provisioning_device_get_device(container->device2),
			oic_acl2,
			_provisioning_pairwise_devices_cb);
	icl_ioty_mutex_unlock();
	if (OC_STACK_OK != ret) {
		ERR("OCProvisionPairwiseDevces() Fail(%d)", ret);
		_provisioning_pairwise_devices_cb_container_destroy(container);
		return _provisioning_parse_oic_error(ret);
	}

	return IOTCON_ERROR_NONE;
}


static void _provisioning_unlink_pairwise_cb_container_destroy(
		struct icl_provisioning_unlink_pairwise_cb_container *container)
{
	if (container->device1) {
		iotcon_provisioning_device_destroy(container->device1);
		container->device1 = NULL;
	}
	if (container->device2) {
		iotcon_provisioning_device_destroy(container->device2);
		container->device2 = NULL;
	}

	free(container);
}


static gboolean _provisioning_unlink_pairwise_idle_cb(gpointer p)
{
	struct icl_provisioning_unlink_pairwise_cb_container *container = p;

	if (container->cb) {
		container->cb(container->device1, container->device2, container->result,
				container->user_data);
	}

	_provisioning_unlink_pairwise_cb_container_destroy(container);

	return G_SOURCE_REMOVE;
}


static void _provisioning_unlink_pairwise_cb(void *ctx, int n_of_res,
		OCProvisionResult_t *arr, bool has_error)
{
	FN_CALL;
	int i;
	struct icl_provisioning_unlink_pairwise_cb_container *container = ctx;

	if (true == has_error)
		DBG("unlink pairwise has error");

	for (i = 0; i < n_of_res; i++) {
		DBG("arr[%d].res : %d", i, arr[i].res);
		if (OC_STACK_OK != arr[i].res)
			container->result = _provisioning_parse_oic_error(arr[i].res);
	}

	g_idle_add(_provisioning_unlink_pairwise_idle_cb, container);
}


API int iotcon_provisioning_unlink_pairwise(iotcon_provisioning_device_h device1,
		iotcon_provisioning_device_h device2,
		iotcon_provisioning_unlink_pairwise_cb cb,
		void *user_data)
{
	FN_CALL;
	int ret;
	struct icl_provisioning_unlink_pairwise_cb_container *container;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == device1, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == device2, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	container = calloc(1, sizeof(struct icl_provisioning_unlink_pairwise_cb_container));
	if (NULL == container) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	container->device1 = icl_provisioning_device_ref(device1);
	container->device2 = icl_provisioning_device_ref(device2);

	container->cb = cb;
	container->user_data = user_data;

	ret = icl_ioty_mutex_lock();
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_mutex_lock() Fail(%d)", ret);
		_provisioning_unlink_pairwise_cb_container_destroy(container);
		return ret;
	}

	ret = OCUnlinkDevices(container,
			icl_provisioning_device_get_device(container->device1),
			icl_provisioning_device_get_device(container->device2),
			_provisioning_unlink_pairwise_cb);
	icl_ioty_mutex_unlock();
	if (OC_STACK_OK != ret) {
		ERR("OCUnlinkDevices() Fail(%d)", ret);
		_provisioning_unlink_pairwise_cb_container_destroy(container);
		return _provisioning_parse_oic_error(ret);
	}

	return IOTCON_ERROR_NONE;
}
