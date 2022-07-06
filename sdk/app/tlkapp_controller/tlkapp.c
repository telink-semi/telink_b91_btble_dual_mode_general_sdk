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
#include "tlkdev/tlkdev_stdio.h"
#include "tlkstk/tlkstk_stdio.h"
#include "tlkprt/tlkprt_comm.h"
#include "tlklib/fs/filesystem.h"
#include "tlkmdi/tlkmdi.h"

#include "tlkstk/ble/ble.h"
#include "tlkapp_config.h"

#include "tlkapp_debug.h"
#include "tlkapp_irq.h"

#include "tlkapp.h"


extern void trng_init(void);


volatile u32 AAAA_test001 = 0;
volatile u32 AAAA_test002 = 0;
volatile u32 AAAA_test003 = 0;
volatile u32 AAAA_test004 = 0;
volatile u32 AAAA_test005 = 0;
volatile u32 AAAA_test006 = 0;
volatile u32 AAAA_test007 = 0;
volatile u32 AAAA_test008 = 0;
volatile u32 AAAA_test009 = 0;
volatile u32 AAAA_test00A = 0;


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
	AAAA_test001 ++;
	tlkapp_irq_init();
	tlkapi_init();
	AAAA_test002 ++;
	tlkdev_init();
	AAAA_test003 ++;
	tlkstk_init();
	AAAA_test004 ++;
	tlkapp_debug_init();
	AAAA_test008 ++;

	return TLK_ENONE;
}




volatile u32 AAAA_test101 = 0;
volatile u32 AAAA_test102 = 0;
volatile u32 AAAA_test103 = 0;
volatile u32 AAAA_test104 = 0;


/******************************************************************************
 * Function: tlkapp_process
 * Descript: BTBLE SDK main loop.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_process(void)
{
	AAAA_test101 ++;	
	tlkapi_process();
	AAAA_test102 ++;
	tlkdev_process();
	AAAA_test103 ++;
	tlkstk_process();
	AAAA_test104 ++;

}


