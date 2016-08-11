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
#ifndef __IOTCON_ERRORS_H__
#define __IOTCON_ERRORS_H__

#include <tizen.h>

#ifndef TZ_VER_3
#define TIZEN_ERROR_IOTCON -0x01C80000
#endif

/**
 * @file iotcon-errors.h
 */

/**
 * @ingroup CAPI_IOT_CONNECTIVITY_MODULE
 * @brief Enumeration for IoTCon error code.
 * @since_tizen 3.0
 *
 * @{
 */
typedef enum {
	IOTCON_ERROR_NONE = TIZEN_ERROR_NONE, /**< Successful */
	IOTCON_ERROR_IO_ERROR = TIZEN_ERROR_IO_ERROR, /**< I/O error */
	IOTCON_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY, /**< Out of memory */
	IOTCON_ERROR_PERMISSION_DENIED = TIZEN_ERROR_PERMISSION_DENIED, /**< Permission denied */
	IOTCON_ERROR_NOT_SUPPORTED = TIZEN_ERROR_NOT_SUPPORTED, /**< Not supported */
	IOTCON_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER, /**< Invalid parameter */
	IOTCON_ERROR_NO_DATA = TIZEN_ERROR_NO_DATA, /**< No data available */
	IOTCON_ERROR_TIMEOUT = TIZEN_ERROR_TIMED_OUT, /**< Time out */
	IOTCON_ERROR_IOTIVITY = TIZEN_ERROR_IOTCON | 0x01, /**< IoTivity errors */
	IOTCON_ERROR_REPRESENTATION = TIZEN_ERROR_IOTCON | 0x02, /**< Representation errors */
	IOTCON_ERROR_INVALID_TYPE = TIZEN_ERROR_IOTCON | 0x03, /**< Invalid type */
	IOTCON_ERROR_ALREADY = TIZEN_ERROR_IOTCON | 0x04, /**< Already */
	IOTCON_ERROR_SYSTEM = TIZEN_ERROR_IOTCON | 0x06, /**< System errors */
} iotcon_error_e;

/*
 * IOTCON_ERROR_NONE 0
 * IOTCON_ERROR_IO_ERROR -5
 * IOTCON_ERROR_OUT_OF_MEMORY -12
 * IOTCON_ERROR_PERMISSION_DENIED -13
 * IOTCON_ERROR_NOT_SUPPORTED -1073741822
 * IOTCON_ERROR_INVALID_PARAMETER -22
 * IOTCON_ERROR_NO_DATA -61
 * IOTCON_ERROR_TIMEOUT -1073741823
 * IOTCON_ERROR_IOTIVITY -29884415
 * IOTCON_ERROR_REPRESENTATION -29884414
 * IOTCON_ERROR_INVALID_TYPE -29884413
 * IOTCON_ERROR_ALREADY -29884412
 * IOTCON_ERROR_SYSTEM -29884410
 */

/**
 * @}
 */

#endif /* __IOTCON_ERRORS_H__ */
