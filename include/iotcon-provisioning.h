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
#ifndef __IOTCON_PROVISIONING_H__
#define __IOTCON_PROVISIONING_H__

#include <iotcon-errors.h>

#define IOTCON_ERROR_AUTHENTICATION_FAILURE (TIZEN_ERROR_IOTCON | 0xF001)

typedef struct icl_provisioning_device* iotcon_provisioning_device_h;
typedef struct icl_provisioning_acl* iotcon_provisioning_acl_h;

typedef enum {
	IOTCON_PROVISIONING_FIND_OWNED,
	IOTCON_PROVISIONING_FIND_UNOWNED,
	IOTCON_PROVISIONING_FIND_ALL,
} iotcon_provisioning_find_e;

typedef enum {
	IOTCON_PERMISSION_CREATE = (1 << 0),
	IOTCON_PERMISSION_READ = (1 << 1),
	IOTCON_PERMISSION_WRITE = (1 << 2),
	IOTCON_PERMISSION_DELETE = (1 << 3),
	IOTCON_PERMISSION_NOTIFY = (1 << 4),
	IOTCON_PERMISSION_FULL_CONTROL = (IOTCON_PERMISSION_CREATE | IOTCON_PERMISSION_READ
			| IOTCON_PERMISSION_WRITE | IOTCON_PERMISSION_DELETE | IOTCON_PERMISSION_NOTIFY),
} iotcon_provisioning_permission_e;

typedef enum {
	IOTCON_OXM_JUST_WORKS,
	IOTCON_OXM_RANDOM_PIN,
} iotcon_provisioning_oxm_e;

/* initialize */
int iotcon_provisioning_initialize(const char *file_path, const char *db_path);

typedef char* (*iotcon_provisioning_randompin_cb)(void *user_data);

int iotcon_provisioning_set_randompin_cb(iotcon_provisioning_randompin_cb cb,
		void *user_data);

/* discover */
typedef bool (*iotcon_provisioning_found_device_cb)(iotcon_provisioning_device_h device,
		iotcon_error_e result, void *user_data);

int iotcon_provisioning_find_device(iotcon_provisioning_find_e owned,
		iotcon_provisioning_found_device_cb cb, void *user_data);


/* register */
typedef void (*iotcon_provisioning_ownership_transfer_cb)(
		iotcon_provisioning_device_h device,
		iotcon_error_e result,
		void *user_data);

int iotcon_provisioning_register_unowned_device(
		iotcon_provisioning_device_h device,
		iotcon_provisioning_ownership_transfer_cb cb,
		void *user_data);

/* cred */
typedef void (*iotcon_provisioning_provision_cred_cb)(
		iotcon_provisioning_device_h device1,
		iotcon_provisioning_device_h device2,
		iotcon_error_e result,
		void *user_data);

int iotcon_provisioning_provision_cred(iotcon_provisioning_device_h device1,
		iotcon_provisioning_device_h device2,
		iotcon_provisioning_provision_cred_cb cb,
		void *user_data);

/* acl */
typedef void (*iotcon_provisioning_provision_acl_cb)(
		iotcon_provisioning_device_h device,
		iotcon_provisioning_acl_h acl,
		iotcon_error_e result,
		void *user_data);

int iotcon_provisioning_provision_acl(iotcon_provisioning_device_h device,
		iotcon_provisioning_acl_h acl,
		iotcon_provisioning_provision_acl_cb cb,
		void *user_data);

/* pairwise */
typedef void (*iotcon_provisioning_pairwise_devices_cb)(
		iotcon_provisioning_device_h device1,
		iotcon_provisioning_device_h device2,
		iotcon_error_e result,
		void *user_data);

int iotcon_provisioning_pairwise_devices(iotcon_provisioning_device_h device1,
		iotcon_provisioning_acl_h acl1,
		iotcon_provisioning_device_h device2,
		iotcon_provisioning_acl_h acl2,
		iotcon_provisioning_pairwise_devices_cb cb,
		void *user_data);

/* unlink */
typedef void (*iotcon_provisioning_unlink_pairwise_cb)(
		iotcon_provisioning_device_h device1,
		iotcon_provisioning_device_h device2,
		iotcon_error_e result,
		void *user_data);

int iotcon_provisioning_unlink_pairwise(iotcon_provisioning_device_h device1,
		iotcon_provisioning_device_h device2,
		iotcon_provisioning_unlink_pairwise_cb cb,
		void *user_data);

/* remove */
typedef void (*iotcon_provisioning_remove_device_cb)(
		const char *device_id,
		iotcon_provisioning_device_h dest_device,
		iotcon_error_e result,
		bool is_complete,
		void *user_data);

int iotcon_provisioning_remove_device(const char *device_id,
		iotcon_provisioning_remove_device_cb cb, void *user_data);

/* struct */
int iotcon_provisioning_device_clone(iotcon_provisioning_device_h device,
		iotcon_provisioning_device_h *cloned_device);
int iotcon_provisioning_device_destroy(iotcon_provisioning_device_h device);
int iotcon_provisioning_device_get_host_address(iotcon_provisioning_device_h device,
		char **host_address);
int iotcon_provisioning_device_get_connectivity_type(iotcon_provisioning_device_h device,
		iotcon_connectivity_type_e *connectivity_type);
int iotcon_provisioning_device_get_id(iotcon_provisioning_device_h device,
		char **device_id);
int iotcon_provisioning_device_get_oxm(iotcon_provisioning_device_h device,
		iotcon_provisioning_oxm_e *oxm);
int iotcon_provisioning_device_is_owned(iotcon_provisioning_device_h device,
		bool *is_owned);

int iotcon_provisioning_acl_create(iotcon_provisioning_acl_h *acl);
int iotcon_provisioning_acl_set_all_subject(iotcon_provisioning_acl_h acl);
int iotcon_provisioning_acl_set_subject(iotcon_provisioning_acl_h acl,
		iotcon_provisioning_device_h subject);
int iotcon_provisioning_acl_add_resource(iotcon_provisioning_acl_h acl,
		const char *uri_path);
int iotcon_provisioning_acl_set_permission(iotcon_provisioning_acl_h acl,
		int permission);
int iotcon_provisioning_acl_destroy(iotcon_provisioning_acl_h acl);

#endif /* __IOTCON_PROVISIONING_H__ */
