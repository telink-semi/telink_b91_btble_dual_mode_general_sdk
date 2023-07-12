/********************************************************************************************************
 * @file	tlkdev_sensor.c
 *
 * @brief	This is the source file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#include "tlkapi/tlkapi_stdio.h"
#if (TLK_DEV_SENSOR_ENABLE)
#include "tlkdev/tlkdev_stdio.h"
#include "tlkdev/sys/tlkdev_sensor.h"
#include "tlkdrv/ext/sensor/tlkdrv_sensor.h"



#define TLKDEV_SENSOR_DEV        TLKDRV_SENSOR_DEV_DSDA217



bool tlkdev_sensor_isOpen(void)
{
	return tlkdrv_sensor_isOpen(TLKDEV_SENSOR_DEV);
}

int tlkdev_sensor_init(void)
{
	return tlkdrv_sensor_init(TLKDEV_SENSOR_DEV);
}
int tlkdev_sensor_open(void)
{
	return tlkdrv_sensor_open(TLKDEV_SENSOR_DEV);
}
int tlkdev_sensor_close(void)
{
	return tlkdrv_sensor_close(TLKDEV_SENSOR_DEV);
}
int tlkdev_sensor_reset(void)
{
	return tlkdrv_sensor_reset(TLKDEV_SENSOR_DEV);
}






#endif //#if (TLK_DEV_SENSOR_ENABLE)

