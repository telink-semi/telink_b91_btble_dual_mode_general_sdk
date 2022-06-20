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
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi/tlkmmi_stdio.h"
#include "tlkprt/tlkprt_comm.h"
#include "tlklib/fs/filesystem.h"
#include "tlkmdi/tlkmdi.h"

#include "tlkstk/ble/ble.h"
#include "tlkapp_config.h"
#include "tlkapp_system.h"
#include "tlkapp_battery.h"
#include "tlkapp_adapt.h"
#include "tlkapp_debug.h"
#include "tlkapp_irq.h"
#include "tlkapp_pm.h"
#include "tlkapp.h"


extern void flash_plic_preempt_config(unsigned char preempt_en,unsigned char threshold);
extern void trng_init(void);



static uint32 sTlkAppTimer; 
static uint08 sTlkAppMemBuffer[TLKAPP_MEM_TOTAL_SIZE] = {0};


/******************************************************************************
 * Function: tlkapp_init
 * Descript: user initialization when MCU power on or wake_up from deepSleep mode.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/  
int tlkapp_init(void)
{
	/* random number generator must be initiated here( in the beginning of user_init_nromal).
	 * When deepSleep retention wakeUp, no need initialize again */
  	g_plic_preempt_en = 1;
	flash_plic_preempt_config(1, 1);
	trng_init(); //random_generator_init();  //this is must
	sdk_mode_select(1,0);
	tlkapi_setSysMemBuffer(false, sTlkAppMemBuffer, TLKAPP_MEM_TOTAL_SIZE);
	
	tlkapp_irq_init();
		
	tlkapi_init();
	tlkdev_init();
	tlkstk_init();
	tlkmdi_init();
	tlkmmi_init();
	tlkmdi_clear_event();
	tlkmdi_comm_sendSysEvt(TLKPRT_COMM_EVTID_SYS_READY, nullptr, 0);

	tlkapp_adapt_init();
	tlkapp_debug_init();
	#if (TLKAPP_CFG_BAT_CHECK_ENABLE)
	tlkapp_battery_init();
	#endif
	
	tlkapp_system_init();
	
#if(TLK_CFG_PM_ENABLE)
	tlkapp_pm_init();
#endif

	sTlkAppTimer = clock_time()|1;

	return TLK_ENONE;
}



/******************************************************************************
 * Function: tlkapp_deepInit
 * Descript: user initialization when MCU wake_up from deepSleep_retention mode.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
void tlkapp_deepInit(void)
{
	
}

bool tlkapp_pmIsBusy(void)
{
	if(sTlkAppTimer != 0){
		if(!clock_time_exceed(sTlkAppTimer, 5000000)) return true;
		sTlkAppTimer = 0;
	}

	return false;
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
	tlkapi_process();
	tlkdev_process();
	tlkstk_process();
	tlkmdi_process();
	tlkmmi_process();

	tlkapp_adapt_handler();
	tlkapp_system_handler();
	
	////////////////////////////////////// PM entry   /////////////////////////////////
	#if(TLK_CFG_PM_ENABLE)
	tlkapp_pm_handler();
	#endif
}


