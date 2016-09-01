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
#ifndef __IOTCON_SERVER_OBSERVERS_H__
#define __IOTCON_SERVER_OBSERVERS_H__

#include <iotcon-types.h>

/**
 * @file iotcon-observers.h
 */

/**
 * @ingroup CAPI_IOT_CONNECTIVITY_SERVER_MODULE
 * @defgroup CAPI_IOT_CONNECTIVITY_SERVER_OBSERVERS_MODULE Observers
 *
 * @brief IoTCon Observers provides API to manage client observing a resource.
 *
 * @section CAPI_IOT_CONNECTIVITY_SERVER_OBSERVERS_MODULE_HEADER Required Header
 *  \#include <iotcon.h>
 *
 * @section CAPI_IOT_CONNECTIVITY_SERVER_OBSERVERS_MODULE_OVERVIEW Overview
 * The iotcon overview API provides methods for managing observe id.
 *
 * Example :
 * @code
#include <iotcon.h>
static iotcon_observers_h _observers;
static void _request_handler(iotcon_resource_h resource, iotcon_request_h request,
		void *user_data)
{
	int ret, observe_id;
	iotcon_request_type_e type;
	iotcon_observe_type_e observe_type;
	iotcon_representation_h repr = NULL;

	...
	ret = iotcon_request_get_request_type(request, &type);
	if (IOTCON_ERROR_NONE != ret)
		return;
	...

	if (IOTCON_REQUEST_PUT == type) {
		iotcon_attributes_h attributes = NULL;
		iotcon_representation_h repr = NULL;
		...

		ret = iotcon_representation_create(&repr);
		if (IOTCON_ERROR_NONE != ret)
			return;

		ret = iotcon_attributes_create(&attributes);
		if (IOTCON_ERROR_NONE != ret) {
			iotcon_representation_destroy(repr);
			return;
		}
		...
		ret = iotcon_resource_notify(resource, repr, _observers, IOTCON_QOS_HIGH);
		if (IOTCON_ERROR_NONE != ret) {
			iotcon_attributes_destroy(attributes);
			iotcon_representation_destroy(repr);
			return;
		}

		iotcon_attributes_destroy(attributes);
		iotcon_representation_destroy(repr);
	}

	ret = iotcon_request_get_observe_type(request, &observe_type);
	if (IOTCON_ERROR_NONE != ret)
		return;

	if (IOTCON_OBSERVE_REGISTER == observe_type) {
		ret = iotcon_request_get_observe_id(request, &observe_id);
		if (IOTCON_ERROR_NONE != ret)
			return;

		if (NULL == _observers) {
			ret = iotcon_observers_create(&_observers);
			if (IOTCON_ERROR_NONE != ret)
				return;
		}
		ret = iotcon_observers_add(_observers, observe_id);
		if (IOTCON_ERROR_NONE != ret)
			return;
		...
	} else if (IOTCON_OBSERVE_DEREGISTER == observe_type) {
		ret = iotcon_request_get_observe_id(request, &observe_id);
		if (IOTCON_ERROR_NONE != ret)
			return;

		if (NULL == _observers)
			return;

		ret = iotcon_observers_remove(_observers, observe_id);
		if (IOTCON_ERROR_NONE != ret)
			return;
		...
	}
	...
}
 * @endcode
 *
 * @section CAPI_IOT_CONNECTIVITY_SERVER_OBSERVERS_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/iot.ocf\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature Element</b>.</a>
 *
 * @{
 */

/**
 * @brief Creates a new observers handle.
 *
 * @since_tizen 3.0
 *
 * @remarks You must destroy @a observers by calling iotcon_observers_destroy()
 * if @a observers is no longer needed.
 *
 * @param[out] observers A newly allocated list of observers handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #IOTCON_ERROR_OUT_OF_MEMORY  Out of memory
 *
 * @see iotcon_observers_destroy()
 * @see iotcon_observers_add()
 * @see iotcon_observers_remove()
 */
int iotcon_observers_create(iotcon_observers_h *observers);

/**
 * @brief Destroys a observers handle.
 *
 * @since_tizen 3.0
 *
 * @param[in] observers The handle of the observers
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_observers_create()
 * @see iotcon_observers_add()
 * @see iotcon_observers_remove()
 */
int iotcon_observers_destroy(iotcon_observers_h observers);

/**
 * @brief Adds a observer id into the observers handle.
 *
 * @since_tizen 3.0
 *
 * @param[in] observers The handle of the observers
 * @param[in] obs_id The id to be appended to observers
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_observers_create()
 * @see iotcon_observers_destroy()
 * @see iotcon_observers_remove()
 */
int iotcon_observers_add(iotcon_observers_h observers, int obs_id);

/**
 * @brief Removes id from the observers.
 *
 * @since_tizen 3.0
 *
 * @param[in] observers observers The handle of the observers
 * @param[in] obs_id The id to be removed from observers
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #IOTCON_ERROR_NONE  Successful
 * @retval #IOTCON_ERROR_NOT_SUPPORTED  Not supported
 * @retval #IOTCON_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see iotcon_observers_create()
 * @see iotcon_observers_destroy()
 * @see iotcon_observers_add()
 */
int iotcon_observers_remove(iotcon_observers_h observers, int obs_id);

/**
 * @}
 */

#endif /* __IOTCON_SERVER_OBSERVERS_H__ */
