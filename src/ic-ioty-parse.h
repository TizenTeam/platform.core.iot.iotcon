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
#ifndef __IOTCON_INTERNAL_IOTIVITY_PARSE_H__
#define __IOTCON_INTERNAL_IOTIVITY_PARSE_H__

#include <stdbool.h>
#include <glib.h>

#include "iotcon-types.h"
#include "iotcon-errors.h"

/* oic -> iotcon */
iotcon_request_type_e ic_ioty_parse_oic_method(OCMethod method);
iotcon_response_result_e ic_ioty_parse_oic_response_result(OCStackResult result);
iotcon_error_e ic_ioty_parse_oic_error(OCStackResult ret);
iotcon_observe_type_e ic_ioty_parse_oic_action(OCObserveAction oic_action);
int ic_ioty_parse_oic_dev_address(OCDevAddr *dev_addr, char **host_address,
		int *conn_type);

int ic_ioty_parse_oic_header_option(OCHeaderOption *option, int option_size,
		iotcon_options_h *options);
void ic_ioty_free_resource_list(gpointer data);
int ic_ioty_parse_oic_discovery_payload(OCDevAddr *dev_addr,
		iotcon_connectivity_type_e connectivity_type,
		int connectivity_prefer,
		OCDiscoveryPayload *discovered,
		GList **resource_list);
int ic_ioty_parse_oic_device_payload(OCDevicePayload *payload,
		iotcon_device_info_h *device_info);
int ic_ioty_parse_oic_platform_payload(OCPlatformPayload *payload,
		iotcon_platform_info_h *platform_info);
int ic_ioty_parse_oic_presence_payload(OCDevAddr *dev_addr,
		OCPresencePayload *payload,
		OCStackResult result,
		iotcon_presence_response_h *presence_response);
int ic_ioty_parse_oic_rep_payload(OCRepPayload *payload, bool is_parent,
		iotcon_representation_h *representation);

#endif /*__IOTCON_INTERNAL_IOTIVITY_PARSE_H__*/
