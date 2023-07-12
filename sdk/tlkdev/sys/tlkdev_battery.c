/********************************************************************************************************
 * @file	tlkdev_battery.c
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
#if (TLK_DEV_BATTERY_ENABLE)
#include "tlkdev/tlkdev.h"
#include "tlkdrv/ext/battery/tlkdrv_battery.h"
#include "tlkdev_battery.h"
#include "drivers.h"



int tlkdev_battery_init(void)
{
	return tlkdrv_battery_init();
}

int tlkdev_battery_getVoltage(uint16 *pVoltage)
{
	return tlkdrv_battery_getVoltage(pVoltage);
}



#endif //TLK_DEV_BATTERY_ENABLE

