/********************************************************************************************************
 * @file     tlkapp.c
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
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkmdi/tlkmdi.h"
#include "tlkmdi/misc/tlkmdi_comm.h"
#include "tlksys/tsk/tlktsk_stdio.h"

#include "tlkstk/ble/ble.h"
#include "tlkapp_config.h"
#include "tlkapp_irq.h"
#include "tlkapp_pm.h"
#include "tlkapp.h"



extern void flash_plic_preempt_config(unsigned char preempt_en,unsigned char threshold);
extern void trng_init(void);


extern int tlkdev_init(void);
extern int tlktsk_init(void);
extern int tlkmmi_init(void);


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
  	g_plic_preempt_en = 1; //Interrupt nesting is mandatory, core_enter_critical/core_leave_critical
	flash_plic_preempt_config(1, 1);
	trng_init();
	tlksdk_mode_select(1,0);
	tlkapi_setSysMemBuffer(false, sTlkAppMemBuffer, TLKAPP_MEM_TOTAL_SIZE);

	tlkapp_irq_init();
	
	tlkdev_init();
	tlktsk_init();
	tlkstk_init();
	tlkmdi_init();
	tlkmmi_init();
		
#if(TLK_CFG_PM_ENABLE)
	tlkapp_pm_init();
#endif
	
	sTlkAppTimer = clock_time()|1;
	
	tlktsk_start(0xFF);
	
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

/******************************************************************************
 * Function: tlkapp_pmIsBusy
 * Descript: user check is step into deepSleep_retention Mode or not.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
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
	tlktsk_run();
	
	/*PM entry*/
	#if(TLK_CFG_PM_ENABLE)
	tlkapp_pm_handler();
	#endif
}


