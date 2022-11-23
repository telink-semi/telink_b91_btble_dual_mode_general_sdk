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
#include "tlkdev/tlkdev_stdio.h"
#include "tlkdev/sys/tlkdev_serial.h"
#if (TLK_DEV_HCIUART_ENABLE)
#include "tlkdev/sys/tlkdev_hciuart.h"
#endif
#include "tlkdev/tlkdev.h"
#if (TLK_DEV_STORE_ENABLE)
#include "tlkdev/sys/tlkdev_store.h"
#endif

extern void tlkusb_process(void);


int tlkdev_init(void)
{
	#if (TLK_DEV_SERIAL_ENABLE)
	tlkdev_serial_init();
	#endif
	#if (TLK_DEV_HCIUART_ENABLE)
	tlkdev_hciuart_init();
	#endif
	#if (TLK_DEV_STORE_ENABLE)
	tlkdev_store_init();
	#endif
	
	return TLK_ENONE;
}
void tlkdev_deinit(void)
{
	
}

