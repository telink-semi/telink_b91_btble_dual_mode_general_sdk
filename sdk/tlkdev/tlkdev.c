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
#include "tlkdev/tlkdev.h"
#if (TLK_DEV_XTSD01G_ENABLE)
#include "tlkdev/ext/xtx/tlkdev_xtsd01g.h"
#endif
#if (TLK_DEV_XT26G0X_ENABLE)
#include "tlkdev/ext/xtx/tlkdev_xt26g0x.h"
#endif
//#include "drivers.h"


extern void tlkusb_process(void);


int tlkdev_init(void)
{
#if TLKAPP_GENERAL_ENABLE
	#if (TLK_DEV_XTSD01G_ENABLE)
		tlkdev_xtsd01g_init();
	#endif
	#if (TLK_DEV_XT26G0X_ENABLE)
		tlkdev_xt26g0x_init();
	#endif
	#if (TLK_CFG_WDG_ENABLE)
	wd_set_interval_ms(3000);
	wd_start();
	#endif
#endif
#if TLKAPP_CONTROLLER_ENABLE&&TLKAPP_HCI_UART_MODE
	tlkdev_serial_init();
#endif

	return TLK_ENONE;
}


void tlkdev_process(void)
{
	#if !TLKAPP_HCI_USB_MODE
	tlkdev_serial_handler();
	#endif
	
	#if (TLK_CFG_WDG_ENABLE)
	wd_clear();
	#endif
}

bool tlkdev_pmIsBusy(void)
{
	#if (TLK_DEV_SERIAL_ENABLE)
	return tlkdev_serial_isBusy();
	#else
	return false;
	#endif
}

