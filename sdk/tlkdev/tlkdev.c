/********************************************************************************************************
 * @file     tlkdev.c
 *
 * @brief    This is the source file for BTBLE SDK
 *
 * @author	 BTBLE GROUP
 * @date         2,2022
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *******************************************************************************************************/
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdev/tlkdev.h"
#include "tlkdev/sys/tlkdev_serial.h"
#include "tlkdev/tlkdev.h"
#if (TLK_DEV_STORE_ENABLE)
#include "tlkdev/sys/tlkdev_store.h"
#endif
#if (TLK_DEV_SRAM_ENABLE)
#include "tlkdev/sys/tlkdev_sram.h"
#endif
#if (TLK_DEV_LCD_ENABLE)
#include "tlkdev/sys/tlkdev_lcd.h"
#endif
#if (TLK_DEV_SENSOR_ENABLE)
#include "tlkdev/sys/tlkdev_sensor.h"
#endif 
#if (TLK_DEV_TOUCH_ENABLE)
#include "tlkdev/sys/tlkdev_touch.h"
#endif




int tlkdev_init(void)
{
	#if (TLK_DEV_SERIAL_ENABLE)
	tlkdev_serial_init();
	#endif
	#if (TLK_DEV_STORE_ENABLE)
	tlkdev_store_init();
	#endif
	#if (TLK_DEV_SRAM_ENABLE)
	tlkdev_sram_init();
	#endif
	#if (TLK_DEV_LCD_ENABLE)
	tlkdev_lcd_init();
	#endif
	#if (TLK_DEV_SENSOR_ENABLE)
	tlkdev_sensor_init();
	#endif
	#if (TLK_DEV_TOUCH_ENABLE)
	tlkdev_touch_init();
	#endif
	
	return TLK_ENONE;
}
void tlkdev_deinit(void)
{
	
}

