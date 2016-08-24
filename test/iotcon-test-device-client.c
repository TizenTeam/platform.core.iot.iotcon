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

#include <glib.h>
#include <iotcon.h>
#include "test.h"

static bool _request_device_info(iotcon_device_info_h info, iotcon_error_e result,
		void *user_data)
{
	int ret;
	char *device_name = NULL;
	char *spec_ver = NULL;
	char *device_id = NULL;
	char *data_model_ver = NULL;

	RETVM_IF(IOTCON_ERROR_NONE != result, IOTCON_FUNC_STOP, "Invalid result (%d)", result);

	ret = iotcon_device_info_get_property(info, IOTCON_DEVICE_INFO_NAME, &device_name);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_device_info_get_property() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_device_info_get_property(info, IOTCON_DEVICE_INFO_SPEC_VER, &spec_ver);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_device_info_get_property() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_device_info_get_property(info, IOTCON_DEVICE_INFO_ID, &device_id);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_device_info_get_property() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_device_info_get_property(info, IOTCON_DEVICE_INFO_DATA_MODEL_VER,
			&data_model_ver);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_device_info_get_property() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}

	INFO("Get Device Info");
	INFO("device_name : %s", device_name);
	INFO("spec_version : %s", spec_ver);
	INFO("device_id : %s", device_id);
	INFO("data_model_version : %s", data_model_ver);
	return IOTCON_FUNC_CONTINUE;
}

static bool _request_platform_info(iotcon_platform_info_h info, iotcon_error_e result,
		void *user_data)
{
	int ret;
	char *platform_id = NULL;
	char *manuf_name = NULL;
	char *manuf_url = NULL;
	char *model_number = NULL;
	char *date_of_manuf = NULL;
	char *platform_ver = NULL;
	char *os_ver = NULL;
	char *hardware_ver = NULL;
	char *firmware_ver = NULL;
	char *support_url = NULL;
	char *system_time = NULL;

	RETVM_IF(IOTCON_ERROR_NONE != result, IOTCON_FUNC_CONTINUE, "Invalid result (%d)", result);

	ret = iotcon_platform_info_get_property(info, IOTCON_PLATFORM_INFO_ID, &platform_id);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_platform_info_get_property() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_platform_info_get_property(info, IOTCON_PLATFORM_INFO_MANUF_NAME,
			&manuf_name);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_platform_info_get_property() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_platform_info_get_property(info, IOTCON_PLATFORM_INFO_MANUF_URL,
			&manuf_url);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_platform_info_get_property() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_platform_info_get_property(info, IOTCON_PLATFORM_INFO_MODEL_NUMBER,
			&model_number);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_platform_info_get_property() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_platform_info_get_property(info, IOTCON_PLATFORM_INFO_DATE_OF_MANUF,
			&date_of_manuf);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_platform_info_get_property() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_platform_info_get_property(info, IOTCON_PLATFORM_INFO_PLATFORM_VER,
			&platform_ver);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_platform_info_get_property() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_platform_info_get_property(info, IOTCON_PLATFORM_INFO_OS_VER, &os_ver);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_platform_info_get_property() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_platform_info_get_property(info, IOTCON_PLATFORM_INFO_HARDWARE_VER,
			&hardware_ver);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_platform_info_get_property() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_platform_info_get_property(info, IOTCON_PLATFORM_INFO_FIRMWARE_VER,
			&firmware_ver);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_platform_info_get_property() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_platform_info_get_property(info, IOTCON_PLATFORM_INFO_SUPPORT_URL,
			&support_url);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_platform_info_get_property() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}

	ret = iotcon_platform_info_get_property(info, IOTCON_PLATFORM_INFO_SYSTEM_TIME,
			&system_time);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_platform_info_get_property() Fail(%d)", ret);
		return IOTCON_FUNC_CONTINUE;
	}

	INFO("Get Platform Info");
	INFO("platform_id : %s", platform_id);
	INFO("manuf_name : %s", manuf_name);
	INFO("manuf_url : %s", manuf_url);
	INFO("model_number : %s", model_number);
	INFO("date_of_manufacture : %s", date_of_manuf);
	INFO("platform_ver : %s", platform_ver);
	INFO("os_ver : %s", os_ver);
	INFO("hardware_ver : %s", hardware_ver);
	INFO("firmware_ver : %s", firmware_ver);
	INFO("support_url : %s", support_url);
	INFO("system_time : %s", system_time);

	return IOTCON_FUNC_CONTINUE;
}

int main(int argc, char **argv)
{
	int ret;
	GMainLoop *loop;

	loop = g_main_loop_new(NULL, FALSE);

	/* initialize iotcon */
	ret = iotcon_initialize("/usr/bin/iotcon-test-svr-db-client.dat");
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_initialize() Fail(%d)", ret);
		return -1;
	}

	ret = iotcon_find_device_info(IOTCON_MULTICAST_ADDRESS, IOTCON_CONNECTIVITY_ALL,
			NULL, _request_device_info, NULL);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_find_device_info() Fail(%d)", ret);
		iotcon_deinitialize();
		return -1;
	}

	ret = iotcon_find_platform_info(IOTCON_MULTICAST_ADDRESS, IOTCON_CONNECTIVITY_ALL,
			NULL, _request_platform_info, NULL);
	if (IOTCON_ERROR_NONE != ret) {
		ERR("iotcon_find_platform_info() Fail(%d)", ret);
		iotcon_deinitialize();
		return -1;
	}

	g_main_loop_run(loop);

	g_main_loop_unref(loop);

	/* deinitialize iotcon */
	iotcon_deinitialize();

	return 0;
}
