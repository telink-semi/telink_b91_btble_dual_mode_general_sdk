/********************************************************************************************************
 * @file	tlkmmi_sys.c
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
#if (TLKMMI_SYSTEM_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi_sys.h"
#include "tlkmmi_sysAdapt.h"
#include "tlkmmi_sysCtrl.h"
#include "tlkmmi_sysDev.h"
#include "tlkmmi_sysDbg.h"
#include "tlkmmi_sysMsgInner.h"
#include "tlkmmi_sysMsgDbg.h"
#include "tlkmmi_sysMsgSys.h"

#include "tlkmdi/misc/tlkmdi_usb.h"
#include "tlkmdi/misc/tlkmdi_comm.h"
#include "tlkmdi/misc/tlkmdi_debug.h"
#include "tlkmdi/misc/tlkmdi_key.h"
#include "tlkmdi/misc/tlkmdi_led.h"


static void tlkmmi_system_datCB(uint08 datID, uint16 number, uint08 *pData, uint08 dataLen);

TLKSYS_MMI_TASK_DEFINE(system, System);



static int tlkmmi_system_init(uint08 procID, uint08 taskID)
{
	tlkapi_timer_init();

	#if (TLK_CFG_COMM_ENABLE)
	tlkmdi_comm_init();
	#endif
	#if (TLK_MDI_DEBUG_ENABLE)
	tlkmdi_debug_init();
	#endif
	#if (TLK_MDI_KEY_ENABLE)
	tlkmdi_key_init();
	#endif
	#if (TLK_MDI_LED_ENABLE)
	tlkmdi_led_init();
	#endif
	#if (TLK_MDI_USB_ENABLE)
	tlkmdi_usb_init();
	#endif
	#if (TLK_MDI_BATTERY_ENABLE)
	tlkmdi_battery_init();
	#endif

	#if (TLK_CFG_COMM_ENABLE)
	tlkmdi_comm_regCmdCB(TLKPRT_COMM_MTYPE_SYS, TLKSYS_TASKID_SYSTEM);
	tlkmdi_comm_regCmdCB(TLKPRT_COMM_MTYPE_DBG, TLKSYS_TASKID_SYSTEM);
	tlkmdi_comm_regDatCB(TLKPRT_COMM_SYS_DAT_PORT, tlkmmi_system_datCB, true);
	#endif

	tlkmmi_sys_adaptInit(procID);
	tlkmmi_sys_ctrlInit();
	tlkmmi_sys_dbgInit();
	tlkmmi_sys_devInit();

	return TLK_ENONE;
}
static int tlkmmi_system_start(void)
{
	#if (TLK_CFG_COMM_ENABLE)
	tlkmdi_comm_clear();
	#endif

	#if (TLK_MDI_BATTERY_ENABLE)
	tlkmdi_battery_startCheck();
	#endif
	tlkmmi_sys_sendCommEvt(TLKPRT_COMM_EVTID_SYS_READY, nullptr, 0);
	
	return TLK_ENONE;
}
static int tlkmmi_system_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_system_close(void)
{
	#if (TLK_MDI_BATTERY_ENABLE)
	tlkmdi_battery_closeCheck();
	#endif
	return TLK_ENONE;
}
static int tlkmmi_system_input(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen,
	uint08 *pData, uint16 dataLen)
{
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		return tlkmmi_sys_innerMsgHandler(msgID, pHead, headLen, pData, dataLen);
	}else if(mtype == TLKPRT_COMM_MTYPE_SYS){
		return tlkmmi_sys_sysMsgHandler(msgID, pData, dataLen);
	}else if(mtype == TLKPRT_COMM_MTYPE_DBG){
		return tlkmmi_sys_dbgMsgHandler(msgID, pData, dataLen);
	}
	return -TLK_ENOSUPPORT;
}


static void tlkmmi_system_handler(void)
{
	#if (TLK_MDI_USB_ENABLE)
	tlkmdi_usb_handler();
	#endif
	#if (TLK_MDI_DEBUG_ENABLE)
	tlkmdi_debug_handler();
	#endif
	#if (TLK_CFG_COMM_ENABLE)
	tlkmdi_comm_handler();
	#endif
	tlkmmi_sys_ctrlHandler();
}


static void tlkmmi_system_datCB(uint08 datID, uint16 number, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_system_datCB 01");
	
}



#endif //#if (TLKMMI_SYSTEM_ENABLE)

