/********************************************************************************************************
 * @file     app.c
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
#include "tlkstk/tlkstk_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlksys/prt/tlkpto_stdio.h"
#include "tlkmdi/tlkmdi.h"
#include "tlkdev/tlkdev.h"
#include "tlklib/usb/tlkusb.h"

#include "tlkstk/ble/ble.h"
#include "tlkapp_config.h"

#include "tlkapp_debug.h"
#include "tlkapp_irq.h"

#include "tlkapp.h"


extern void trng_init(void);



/******************************************************************************
 * Function: tlkapp_init
 * Descript: user initialization when MCU power on or wake_up from deepSleep mode.
 * Params: None.
 * Return: TLK_ENONE is success.
 * Others: None.
*******************************************************************************/
int tlkapp_init(void)
{
  	g_plic_preempt_en = 1;
	trng_init();
	tlksdk_mode_select(0,1);
	tlkapp_irq_init();
	tlkdev_init();
	#if (TLK_CFG_DBG_ENABLE)
	tlkdbg_init();
	#endif
	tlkstk_init();
	tlkapp_debug_init();

	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapp_process
 * Descript: BTBLE SDK main loop.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_process(void)
{
	tlkdbg_handler();
	tlkusb_process();
	#if (TLK_DEV_HCIUART_ENABLE)
	tlkdev_hciuart_handler();
	#endif
}


