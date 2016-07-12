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
#include <errno.h>
#include <glib.h>
#include <octypes.h>
#include <ocstack.h>
#include <pmutility.h>
#include <provisioningdatabasemanager.h>

#include "iotcon.h"
#include "iotcon-provisioning.h"
#include "ic.h"
#include "ic-ioty.h"
#include "ic-utils.h"
#include "ic-ioty-parse.h"
#include "ic-provisioning-struct.h"

#define ICL_PROVISIONING_REMOVE_TIMEOUT -1

typedef struct {
	int result;
	int ref_count;
	char *device_id;
	iotcon_provisioning_remove_device_cb cb;
	void *user_data;
	size_t num_of_devices;
	OCUuidList_t *linked_devices;
} icl_provisioning_remove_cb_container_s;

typedef struct {
	int timer_id;
	iotcon_provisioning_device_h dest_device;
	icl_provisioning_remove_cb_container_s *cb_data;
} icl_provisioning_remove_delete_container_s;

static icl_provisioning_remove_cb_container_s* _provisioning_remove_cb_ref(
		icl_provisioning_remove_cb_container_s *container)
{
	RETV_IF(NULL == container, NULL);
	RETV_IF(container->ref_count <= 0, NULL);

	container->ref_count++;

	return container;
}


static void icl_provisioning_free_remove_cb_container(
		icl_provisioning_remove_cb_container_s *container, bool is_complete)
{
	int ret;
	OicUuid_t *uuid;

	RET_IF(NULL == container);

	container->ref_count--;

	if (0 < container->ref_count)
		return;

	if (true == is_complete) {
		uuid = icl_provisioning_convert_device_id(container->device_id);

		ret = PDMDeleteDevice(uuid);
		if (OC_STACK_OK != ret)
			ERR("PDMDeleteDevice() Fail(%d)", ret);

		free(uuid);
	}
	free(container->device_id);

	PDMDestoryOicUuidLinkList(container->linked_devices);

	free(container);
}


static void icl_provisioning_free_remove_delete_container(
		icl_provisioning_remove_delete_container_s *container, bool is_complete)
{
	FN_CALL;

	RET_IF(NULL == container);

	iotcon_provisioning_device_destroy(container->dest_device);
	icl_provisioning_free_remove_cb_container(container->cb_data, is_complete);

	free(container);
}


static gboolean _provisioning_remove_idle_cb(gpointer p)
{
	FN_CALL;
	bool is_complete;
	icl_provisioning_remove_delete_container_s *container;

	RETV_IF(NULL == p, G_SOURCE_REMOVE);

	container = p;

	if (0 == --container->cb_data->num_of_devices)
		is_complete = true;
	else
		is_complete = false;

	if (container->cb_data->cb) {
		container->cb_data->cb(container->cb_data->device_id, container->dest_device,
				IOTCON_ERROR_NONE, is_complete, container->cb_data->user_data);
	}

	icl_provisioning_free_remove_delete_container(container, is_complete);

	return G_SOURCE_REMOVE;
}


static gboolean _provisioning_remove_fail_idle_cb(gpointer p)
{
	FN_CALL;
	bool is_complete;
	int num_of_devices;
	icl_provisioning_remove_delete_container_s *container;

	RETV_IF(NULL == p, G_SOURCE_REMOVE);

	container = p;

	num_of_devices = --container->cb_data->num_of_devices;

	if (IOTCON_ERROR_TIMEOUT == container->cb_data->result) {
		is_complete = true;

		if (num_of_devices < 0) {
			icl_provisioning_free_remove_delete_container(container, is_complete);
			return G_SOURCE_REMOVE;
		}
		if (container->cb_data->cb) {
			container->cb_data->cb(container->cb_data->device_id, NULL,
					IOTCON_ERROR_TIMEOUT, is_complete, container->cb_data->user_data);
		}
	} else {
		if (0 == num_of_devices)
			is_complete = true;
		else
			is_complete = false;

		if (container->cb_data->cb) {
			container->cb_data->cb(container->cb_data->device_id, container->dest_device,
					container->cb_data->result, is_complete, container->cb_data->user_data);
		}
	}

	icl_provisioning_free_remove_delete_container(container, is_complete);

	return G_SOURCE_REMOVE;
}


static OCStackApplicationResult _provisioning_remove_device_delete_cb(void *ctx,
		OCDoHandle handle, OCClientResponse *resp)
{
	int ret;
	OicUuid_t *uuid;
	OicUuid_t device_id = {{0}};
	icl_provisioning_remove_delete_container_s *container = ctx;

	RETV_IF(NULL == container, OC_STACK_DELETE_TRANSACTION);

	if (ICL_PROVISIONING_REMOVE_TIMEOUT == container->timer_id) {
		icl_provisioning_free_remove_delete_container(container, true);
		return OC_STACK_DELETE_TRANSACTION;
	}

	if (container->timer_id) {
		g_source_remove(container->timer_id);
		container->timer_id = 0;
	}

	if (NULL == resp) {
		ERR("resp is NULL");
		container->cb_data->result = IOTCON_ERROR_IOTIVITY;
		g_idle_add(_provisioning_remove_fail_idle_cb, container);
		return OC_STACK_DELETE_TRANSACTION;
	}

	if (OC_STACK_RESOURCE_DELETED != resp->result) {
		ERR("_provisioning_remove_device_delete() Fail(%d)", resp->result);
		container->cb_data->result = IOTCON_ERROR_IOTIVITY;
		g_idle_add(_provisioning_remove_fail_idle_cb, container);
		return OC_STACK_DELETE_TRANSACTION;
	}

	memcpy(device_id.id, resp->identity.id, sizeof(device_id.id));

	uuid = icl_provisioning_convert_device_id(container->cb_data->device_id);

	ret = PDMUnlinkDevices(uuid, &device_id);
	if (OC_STACK_OK != ret) {
		ERR("PDMUnlinkDevices() Fail(%d)", ret);
		container->cb_data->result = IOTCON_ERROR_IOTIVITY;
		g_idle_add(_provisioning_remove_fail_idle_cb, container);
		free(uuid);
		return OC_STACK_DELETE_TRANSACTION;
	}
	free(uuid);

	/* user cb */
	g_idle_add(_provisioning_remove_idle_cb, container);

	return OC_STACK_DELETE_TRANSACTION;
}


static gboolean _provisioning_remove_device_delete_timeout(gpointer p)
{
	icl_provisioning_remove_delete_container_s *container = p;

	RETV_IF(NULL == container, G_SOURCE_REMOVE);
	RETV_IF(NULL == container->cb_data, G_SOURCE_REMOVE);

	container->timer_id = ICL_PROVISIONING_REMOVE_TIMEOUT;

	return G_SOURCE_REMOVE;
}


static int _provisioning_remove_device_delete(
		icl_provisioning_remove_delete_container_s *container)
{
	int ret, timeout;
	char *host_address;
	char uri[PATH_MAX] = {0};
	OCProvisionDev_t *device;
	OCCallbackData cbdata = {0};
	const char *cred_uri = "/oic/sec/cred";
	const char *subject_uuid = "subjectuuid";

	device = icl_provisioning_device_get_device(container->dest_device);

	ret = icl_provisioning_parse_oic_dev_address(&device->endpoint, device->securePort,
			device->connType, &host_address);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_provisioning_parse_oic_dev_address() Fail(%d)", ret);
		return ret;
	}

	snprintf(uri, sizeof(uri), "%s%s%s?%s=%s", IC_IOTY_COAPS, host_address, cred_uri,
			subject_uuid, container->cb_data->device_id);

	free(host_address);

	cbdata.cb = _provisioning_remove_device_delete_cb;
	cbdata.context = container;

	ret = icl_ioty_mutex_lock();
	if (IOTCON_ERROR_NONE != ret) {
		ERR("icl_ioty_mutex_lock() Fail(%d)", ret);
		return ret;
	}
	ret = OCDoResource(NULL, OC_REST_DELETE, uri, &device->endpoint, NULL,
			device->connType, OC_HIGH_QOS, &cbdata, NULL, 0);
	icl_ioty_mutex_unlock();
	if (OC_STACK_OK != ret) {
		ERR("OCDoResource() Fail(%d)", ret);
		return ic_ioty_parse_oic_error(ret);
	}

	/* timeout */
	iotcon_get_timeout(&timeout);
	container->timer_id = g_timeout_add_seconds(timeout,
			_provisioning_remove_device_delete_timeout, container);

	return IOTCON_ERROR_NONE;
}


static bool _provisioning_remove_device_found_cb(iotcon_provisioning_device_h device,
		iotcon_error_e result, void *user_data)
{
	FN_CALL;
	int ret;
	OCUuidList_t *cur;
	OCProvisionDev_t *found_device;
	icl_provisioning_remove_cb_container_s *cb_data = user_data;
	icl_provisioning_remove_delete_container_s *container;

	container = calloc(1, sizeof(icl_provisioning_remove_delete_container_s));
	if (NULL == container) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_FUNC_CONTINUE;
	}
	container->cb_data = _provisioning_remove_cb_ref(cb_data);

	if (IOTCON_ERROR_TIMEOUT == result) {
		container->cb_data->result = IOTCON_ERROR_TIMEOUT;
		g_idle_add(_provisioning_remove_fail_idle_cb, container);
		return IOTCON_FUNC_STOP;
	}

	found_device = icl_provisioning_device_get_device(device);

	for (cur = cb_data->linked_devices; cur; cur = cur->next) {
		if (IC_EQUAL == memcmp(found_device->doxm->deviceID.id, cur->dev.id,
					sizeof(cur->dev.id))) {
			ret = iotcon_provisioning_device_clone(device, &container->dest_device);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("iotcon_provisioning_device_clone() Fail");
				icl_provisioning_free_remove_delete_container(container, false);
				return IOTCON_FUNC_CONTINUE;
			}

			ret = _provisioning_remove_device_delete(container);
			if (IOTCON_ERROR_NONE != ret) {
				ERR("_provisioning_remove_device_delete() Fail(%d)", ret);
				icl_provisioning_free_remove_delete_container(container, false);
				return IOTCON_FUNC_CONTINUE;
			}
		}
	}

	return IOTCON_FUNC_CONTINUE;
}


static gboolean _provisioning_remove_complete_idle_cb(gpointer p)
{
	FN_CALL;
	bool is_complete;
	icl_provisioning_remove_cb_container_s *container;

	RETV_IF(NULL == p, G_SOURCE_REMOVE);

	is_complete = true;

	container = p;

	if (container->cb) {
		container->cb(container->device_id, NULL, IOTCON_ERROR_NONE, is_complete,
				container->user_data);
	}

	icl_provisioning_free_remove_cb_container(container, is_complete);

	return G_SOURCE_REMOVE;
}


API int iotcon_provisioning_remove_device(const char *device_id,
		iotcon_provisioning_remove_device_cb cb, void *user_data)
{
	FN_CALL;
	int ret;
	OicUuid_t *uuid;
	OCUuidList_t *cur;
	icl_provisioning_remove_cb_container_s *container;

	RETV_IF(false == ic_utils_check_ocf_feature(), IOTCON_ERROR_NOT_SUPPORTED);
	RETV_IF(NULL == device_id, IOTCON_ERROR_INVALID_PARAMETER);
	RETV_IF(NULL == cb, IOTCON_ERROR_INVALID_PARAMETER);

	container = calloc(1, sizeof(icl_provisioning_remove_cb_container_s));
	if (NULL == container) {
		ERR("calloc() Fail(%d)", errno);
		return IOTCON_ERROR_OUT_OF_MEMORY;
	}

	container->cb = cb;
	container->user_data = user_data;
	container->device_id = strdup(device_id);
	container->ref_count = 1;

	uuid = icl_provisioning_convert_device_id(container->device_id);

	/* Get Linked devices */
	ret = PDMGetLinkedDevices(uuid, &container->linked_devices,
			&container->num_of_devices);
	if (OC_STACK_OK != ret) {
		ERR("PDMGetLinkedDevices() Fail(%d)", ret);
		free(uuid);
		icl_provisioning_free_remove_cb_container(container, false);
		return ic_ioty_parse_oic_error(ret);
	}

	if (0 == container->num_of_devices) {
		ERR("No Linked devices");
		free(uuid);
		g_idle_add(_provisioning_remove_complete_idle_cb, container);
		return IOTCON_ERROR_NONE;
	}

	/* link status : active -> stale */
	for (cur = container->linked_devices; cur; cur = cur->next) {
		ret = PDMSetLinkStale(&cur->dev, uuid);
		if (OC_STACK_OK != ret) {
			ERR("PDMSetLinkStale() Fail(%d)", ret);
			free(uuid);
			icl_provisioning_free_remove_cb_container(container, false);
			return ic_ioty_parse_oic_error(ret);
		}
	}
	free(uuid);

	/* Find owned devices */
	ret = iotcon_provisioning_find_device(IOTCON_PROVISIONING_FIND_OWNED,
			_provisioning_remove_device_found_cb, container);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_provisioning_find_device() Fail(%d)", ret);
		icl_provisioning_free_remove_cb_container(container, false);
		return ret;
	}

	return IOTCON_ERROR_NONE;
}

