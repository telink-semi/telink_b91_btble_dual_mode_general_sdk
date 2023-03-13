/********************************************************************************************************
 * @file     tlkmmi_sysTask.c
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
#include "tlksys/tsk/tlktsk_stdio.h"
#include "tlkmmi_sys.h"
#include "tlkmmi_sysAdapt.h"
#include "tlkmmi_sysCtrl.h"
#include "tlkmmi_sysTask.h"
#include "tlkmmi_sysMsgSys.h"
#include "tlkmmi_sysMsgDbg.h"
#include "tlkmmi_sysMsgInner.h"

#include "tlkmdi/misc/tlkmdi_usb.h"
#include "tlkmdi/misc/tlkmdi_comm.h"
#include "tlkmdi/misc/tlkmdi_debug.h"


#if (TLK_CFG_COMM_ENABLE)
extern int tlkmdi_comm_regCmdCB(uint08 mtype, uint08 taskID);
extern int tlkmdi_comm_regDatCB(uint08 datID, tlkmdi_comm_datCB datCB, bool isForce);
#endif

static int  tlkmmi_sys_taskStart(void);
static int  tlkmmi_sys_taskPause(void);
static int  tlkmmi_sys_taskClose(void);
static int  tlkmmi_sys_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen);
static int  tlkmmi_sys_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
static uint tlkmmi_sys_taskIntvUs(void);
static void tlkmmi_sys_taskWakeup(void);
static void tlkmmi_sys_taskHandler(void);
static const tlktsk_modinf_t sTlkMmiSysTaskInfs = {
	TLKTSK_TASKID_SYSTEM, //taskID
	tlkmmi_sys_taskStart, //int(*Start)
	tlkmmi_sys_taskPause, //int(*Pause)
	tlkmmi_sys_taskClose, //int(*Close)
	tlkmmi_sys_taskInput, //int(*Input)
	tlkmmi_sys_taskExtMsg, //int(*ExtMsg)
	tlkmmi_sys_taskIntvUs, //uint(*IntvUs)
	tlkmmi_sys_taskWakeup, //void(*Wakeup)
	tlkmmi_sys_taskHandler, //void(*Handler)
};

static void tlkmmi_sys_datCB(uint08 datID, uint16 number, uint08 *pData, uint08 dataLen);

int tlkmmi_sys_taskInit(void)
{
	tlktsk_mount(TLKMMI_SYS_PROCID, &sTlkMmiSysTaskInfs);
	tlkmdi_comm_regCmdCB(TLKPRT_COMM_MTYPE_SYS, TLKTSK_TASKID_SYSTEM);
	tlkmdi_comm_regCmdCB(TLKPRT_COMM_MTYPE_DBG, TLKTSK_TASKID_SYSTEM);
	tlkmdi_comm_regDatCB(TLKPRT_COMM_SYS_DAT_PORT, tlkmmi_sys_datCB, true);
	
	return TLK_ENONE;
}


static int tlkmmi_sys_taskStart(void)
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
static int tlkmmi_sys_taskPause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_sys_taskClose(void)
{
	#if (TLK_MDI_BATTERY_ENABLE)
	tlkmdi_battery_closeCheck();
	#endif

	return TLK_ENONE;
}
static int tlkmmi_sys_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		return tlkmmi_sys_innerMsgHandler(msgID, pData, dataLen);
	}else if(mtype == TLKPRT_COMM_MTYPE_SYS){
		return tlkmmi_sys_sysMsgHandler(msgID, pData, dataLen);
	}else if(mtype == TLKPRT_COMM_MTYPE_DBG){
		return tlkmmi_sys_dbgMsgHandler(msgID, pData, dataLen);
	}
	return -TLK_ENOSUPPORT;
}
static int tlkmmi_sys_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_taskExtMsg: mtype[%d]", mtype);
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		return tlkmmi_sys_innerExtMsgHandler(msgID, pHead, headLen, pData, dataLen);
	}
	return -TLK_ENOSUPPORT;
}
static uint tlkmmi_sys_taskIntvUs(void)
{
	return 0;
}
static void tlkmmi_sys_taskWakeup(void)
{
	
}
static void tlkmmi_sys_taskHandler(void)
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


static void tlkmmi_sys_datCB(uint08 datID, uint16 number, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_datCB 01");
	
}


#endif //#if (TLKMMI_SYSTEM_ENABLE)

