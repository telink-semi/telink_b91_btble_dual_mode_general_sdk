/********************************************************************************************************
 * @file     tlkmmi_sysCtrl.c
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
#if (TLKMMI_SYSTEM_ENABLE)
#include "tlksys/prt/tlkpto_stdio.h"
#include "tlkmdi/misc/tlkmdi_comm.h"
#include "tlkmmi_sys.h"
#include "tlkmmi_sysCtrl.h"
#include "tlkmmi_sysMsgInner.h"
#include "drivers.h"

static tlkmmi_sys_ctrl_t sTlkMmiSysCtrl;



/******************************************************************************
 * Function: tlkmmi_sys_ctrlInit
 * Descript: This function is used to initialize system parameters and configuration.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
int tlkmmi_sys_ctrlInit(void)
{
	tmemset(&sTlkMmiSysCtrl, 0, sizeof(tlkmmi_sys_ctrl_t));
		
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_sys_commHandler
 * Descript: This function to control the mechine power off or restart 
 *           and register the heart beat.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
void tlkmmi_sys_ctrlHandler(void)
{
	if(sTlkMmiSysCtrl.powerTimer != 0 && clock_time_exceed(sTlkMmiSysCtrl.powerTimer, sTlkMmiSysCtrl.powerIntv)){
		if(sTlkMmiSysCtrl.isBoot) core_reboot();
		else tlkmmi_sys_poweroff();
		sTlkMmiSysCtrl.powerTimer = 0;
	}
	if(sTlkMmiSysCtrl.heartTimer != 0 && clock_time_exceed(sTlkMmiSysCtrl.heartTimer, sTlkMmiSysCtrl.heartIntv)){
		sTlkMmiSysCtrl.heartTimer = clock_time()|1;
		tlkmmi_sys_sendCommEvt(TLKPRT_COMM_EVTID_SYS_HEART_BEAT, (uint08*)&sTlkMmiSysCtrl.heartNumb, 2);
		sTlkMmiSysCtrl.heartNumb ++;
	}	
}

void tlkmmi_sys_setPowerParam(uint32 offTimeUs, bool isBoot)
{
	if(sTlkMmiSysCtrl.powerTimer == 0 || sTlkMmiSysCtrl.isBoot != isBoot){
		sTlkMmiSysCtrl.powerTimer = clock_time()|1;
		sTlkMmiSysCtrl.powerIntv = offTimeUs;
		sTlkMmiSysCtrl.isBoot = isBoot;
	}
}
void tlkmmi_sys_setHeartParam(uint32 intervalUs)
{
	if(intervalUs != 0 && (sTlkMmiSysCtrl.heartTimer == 0 || sTlkMmiSysCtrl.heartIntv != intervalUs)){
		sTlkMmiSysCtrl.heartTimer = clock_time()|1;
		sTlkMmiSysCtrl.heartIntv = intervalUs;
	}else if(intervalUs == 0 && sTlkMmiSysCtrl.heartTimer != 0){
		sTlkMmiSysCtrl.heartTimer = 0;
	}
}

/******************************************************************************
 * Function: tlkmmi_sys_poweroff
 * Descript: This Function for power off the meachine.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
void tlkmmi_sys_poweroff(void)
{
	usb_dp_pullup_en(0);

	core_enter_deep(0);

//	gpio_input_dis(TLKAPP_WAKEUP_PIN);
//	gpio_output_dis(TLKAPP_WAKEUP_PIN);
//	gpio_set_up_down_res(TLKAPP_WAKEUP_PIN,GPIO_PIN_PULLDOWN_100K);
//	pm_set_gpio_wakeup(TLKAPP_WAKEUP_PIN, WAKEUP_LEVEL_HIGH, 0);
}



#endif //#if (TLKMMI_SYSTEM_ENABLE)

