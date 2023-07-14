/********************************************************************************************************
 * @file	tlkapp.c
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
#include "tlkstk/tlkstk_stdio.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkmdi/misc/tlkmdi_comm.h"
#include "tlksys/tlksys_stdio.h"

#include "tlkapp_config.h"
#include "tlkapp_irq.h"
#include "tlkapp.h"
#include "tlkapp_pm.h"
#include "drivers.h"



extern void flash_plic_preempt_config(unsigned char preempt_en,unsigned char threshold);
extern void trng_init(void);
#if (TLKMMI_TEST_ENABLE)
extern uint tlkcfg_getWorkMode(void);
#endif
extern void tlkbt_set_workMode(u8 workMode);

extern int tlkdev_init(void);
extern int tlksys_init(void);
extern int tlkcfg_load(void);

static void tlkapp_sysProcInit(void);
static void tlkapp_sysTaskInit(void);


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
	tlkbt_set_workMode(1);
	tlkapi_setSysMemBuffer(false, true, sTlkAppMemBuffer, TLKAPP_MEM_TOTAL_SIZE);

	tlkapp_irq_init();

	tlkcfg_load();
	tlksys_init();
	tlkdev_init();
				
#if(TLK_CFG_PM_ENABLE)
	tlkapp_pm_init();
#endif
#if (TLK_CFG_SYS_ENABLE)
	tlksys_pm_appendBusyCheckCB(tlkapp_pmIsBusy, "tlkapp");
#endif
	
	sTlkAppTimer = clock_time()|1;

	tlkapp_sysProcInit();
	tlkapp_sysTaskInit();	
	
	tlksys_start(0xFF);
	
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
 * Function: tlkapp_handler
 * Descript: BTBLE SDK main loop.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_handler(void)
{
	tlksys_handler();
	
	/*PM entry*/
	#if(TLK_CFG_PM_ENABLE)
	tlkapp_pm_handler();
	#endif
}

#if (TLKMMI_SYSTEM_ENABLE)
extern const tlksys_funcs_t gTlkMmiSystemTask;
#endif
#if (TLKMMI_STACK_ENABLE)
extern const tlksys_funcs_t gTlkMmiStackTask;
#endif
#if (TLKMMI_AUDIO_ENABLE)
extern const tlksys_funcs_t gTlkMmiAudioTask;
#endif
#if (TLKMMI_BTMGR_ENABLE)
extern const tlksys_funcs_t gTlkMmiBtmgrTask;
#endif
#if (TLKMMI_LEMGR_ENABLE)
extern const tlksys_funcs_t gTlkMmiLemgrTask;
#endif
#if (TLKMMI_LEMST_ENABLE)
extern const tlksys_funcs_t gTlkMmiLemstTask;
#endif
#if (TLKMMI_PHONE_ENABLE)
extern const tlksys_funcs_t gTlkMmiPhoneTask;
#endif
#if (TLKMMI_FILE_ENABLE)
extern const tlksys_funcs_t gTlkMmiFileTask;
#endif
#if (TLKMMI_TEST_ENABLE)
extern const tlksys_funcs_t gTlkMmiTestTask;
#endif
#if (TLKMMI_VIEW_ENABLE)
extern const tlksys_funcs_t gTlkMmiViewTask;
#endif
#if (TLKMMI_SENSOR_ENABLE)
extern const tlksys_funcs_t gTlkMmiSensorTask;
#endif
static void tlkapp_sysProcInit(void)
{
	tlksys_proc_mount(TLKSYS_PROCID_SYSTEM, 1, 1024); //System processes must come first
	#if (TLK_CFG_STK_ENABLE)
	tlksys_proc_mount(TLKSYS_PROCID_STACK,  2, 1024);
	#endif
	#if (TLKMMI_TEST_ENABLE)
	if(tlkcfg_getWorkMode() != TLK_WORK_MODE_NORMAL){
		tlksys_proc_mount(TLKSYS_PROCID_TEST,   1, 1024);
		return;
	}
	#endif
	#if (TLKMMI_SENSOR_ENABLE)
	tlksys_proc_mount(TLKSYS_PROCID_DEVICE, 2, 1024);
	#endif
	tlksys_proc_mount(TLKSYS_PROCID_AUDIO,  3, 1024);
	#if (TLK_CFG_FS_ENABLE)
	tlksys_proc_mount(TLKSYS_PROCID_FILEM,  2, 1024);
	#endif
	#if (TLK_CFG_GUI_ENABLE)
	tlksys_proc_mount(TLKSYS_PROCID_VIEW,   1, 1024);
	#endif
}

static void tlkapp_sysTaskInit(void)
{
	#if (TLKMMI_SYSTEM_ENABLE)
	tlksys_task_mount(TLKSYS_PROCID_SYSTEM, TLKSYS_TASKID_SYSTEM, &gTlkMmiSystemTask);
	#endif
	#if (TLKMMI_STACK_ENABLE)
	tlksys_task_mount(TLKSYS_PROCID_STACK,  TLKSYS_TASKID_STACK,  &gTlkMmiStackTask); //This should be first in stack process.
	#endif
	#if (TLKMMI_TEST_ENABLE)
	if(tlkcfg_getWorkMode() != TLK_WORK_MODE_NORMAL){
		tlksys_task_mount(TLKSYS_PROCID_TEST, TLKSYS_TASKID_TEST, &gTlkMmiTestTask);
		return;
	}
	#endif
	#if (TLKMMI_AUDIO_ENABLE)
	tlksys_task_mount(TLKSYS_PROCID_AUDIO,  TLKSYS_TASKID_AUDIO,  &gTlkMmiAudioTask);
	#endif
	#if (TLKMMI_BTMGR_ENABLE)
	tlksys_task_mount(TLKSYS_PROCID_STACK,  TLKSYS_TASKID_BTMGR,  &gTlkMmiBtmgrTask);
	#endif
	#if (TLKMMI_LEMGR_ENABLE)
	tlksys_task_mount(TLKSYS_PROCID_STACK,  TLKSYS_TASKID_LEMGR,  &gTlkMmiLemgrTask);
	#endif
	#if (TLKMMI_LEMST_ENABLE)
	tlksys_task_mount(TLKSYS_PROCID_STACK,  TLKSYS_TASKID_LEMST,  &gTlkMmiLemstTask);
	#endif
	#if (TLKMMI_PHONE_ENABLE)
	tlksys_task_mount(TLKSYS_PROCID_STACK,  TLKSYS_TASKID_PHONE,  &gTlkMmiPhoneTask);
	#endif
	#if (TLKMMI_FILE_ENABLE)
	tlksys_task_mount(TLKSYS_PROCID_FILEM,  TLKSYS_TASKID_FILE,   &gTlkMmiFileTask);
	#endif
	#if (TLKMMI_VIEW_ENABLE)
	tlksys_task_mount(TLKSYS_PROCID_VIEW,   TLKSYS_TASKID_VIEW,   &gTlkMmiViewTask);
	#endif
	#if (TLKMMI_SENSOR_ENABLE)
	tlksys_task_mount(TLKSYS_PROCID_DEVICE, TLKSYS_TASKID_SENSOR, &gTlkMmiSensorTask);
	#endif
}


