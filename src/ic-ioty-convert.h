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
#ifndef __IOTCON_INTERNAL_IOTIVITY_CONVERT_H__
#define __IOTCON_INTERNAL_IOTIVITY_CONVERT_H__

#include <octypes.h>
#include "iotcon-types.h"

/* iotcon -> oic */
OCConnectivityType ic_ioty_convert_connectivity_type(iotcon_connectivity_type_e conn_type,
		int conn_options);
OCMethod ic_ioty_convert_request_type(iotcon_request_type_e req_type);
OCEntityHandlerResult ic_ioty_convert_response_result(iotcon_response_result_e result);
OCQualityOfService ic_ioty_convert_qos(iotcon_qos_e qos);
int ic_ioty_convert_host_address(const char *host_address,
		iotcon_connectivity_type_e conn_type,
		int conn_options,
		OCDevAddr *dev_addr);
uint8_t ic_ioty_convert_policies(uint8_t policies);

int ic_ioty_convert_representation(iotcon_representation_h repr,
		OCPayload **payload);
int ic_ioty_convert_header_options(iotcon_options_h options,
		OCHeaderOption dest[], int dest_size);

#endif /*__IOTCON_INTERNAL_IOTIVITY_CONVERT_H__*/
