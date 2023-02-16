/********************************************************************************************************
 * @file     tlkmmi_btmgrTask.c
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
#if (TLKMMI_BTMGR_ENABLE)
#include "tlksys/tsk/tlktsk_stdio.h"
#include "tlkmmi_btmgr.h"
#include "tlkmmi_btmgrAdapt.h"
#include "tlkmmi_btmgrTask.h"
#include "tlkmmi_btmgrMsgInner.h"
#include "tlkmmi_btmgrMsgOuter.h"
#include "tlkmdi/bt/tlkmdi_bt.h"
#if (TLK_MDI_BTIAP_ENABLE)
#include "tlkmdi/bt/tlkmdi_btiap.h"
#endif


extern int tlkmdi_comm_regCmdCB(uint08 mtype, uint08 taskID);

static int  tlkmmi_btmgr_taskStart(void);
static int  tlkmmi_btmgr_taskPause(void);
static int  tlkmmi_btmgr_taskClose(void);
static int  tlkmmi_btmgr_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen);
static int  tlkmmi_btmgr_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
static uint tlkmmi_btmgr_taskIntvUs(void);
static void tlkmmi_btmgr_taskWakeup(void);
static void tlkmmi_btmgr_taskHandler(void);
static const tlktsk_modinf_t sTlkMmiBtmgrTaskInfs = {
	TLKTSK_TASKID_BTMGR, //taskID
	tlkmmi_btmgr_taskStart, //int(*Start)
	tlkmmi_btmgr_taskPause, //int(*Pause)
	tlkmmi_btmgr_taskClose, //int(*Close)
	tlkmmi_btmgr_taskInput, //int(*Input)
	tlkmmi_btmgr_taskExtMsg, //int(*ExtMsg)
	tlkmmi_btmgr_taskIntvUs, //uint(*IntvUs)
	tlkmmi_btmgr_taskWakeup, //void(*Wakeup)
	tlkmmi_btmgr_taskHandler, //void(*Handler)
};


int tlkmmi_btmgr_taskInit(void)
{
	tlktsk_mount(TLKMMI_BTMGR_PROCID, &sTlkMmiBtmgrTaskInfs);
	tlkmdi_comm_regCmdCB(TLKPRT_COMM_MTYPE_BT, TLKTSK_TASKID_BTMGR);
	
	return TLK_ENONE;
}




static int tlkmmi_btmgr_taskStart(void)
{
	return TLK_ENONE;
}
static int tlkmmi_btmgr_taskPause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_btmgr_taskClose(void)
{
	return TLK_ENONE;
}
static int tlkmmi_btmgr_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		return tlkmmi_btmgr_innerMsgHandler(msgID, pData, dataLen);
	}else{
		return tlkmmi_btmgr_outerMsgHandler(msgID, pData, dataLen);
	}
}
static int tlkmmi_btmgr_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static uint tlkmmi_btmgr_taskIntvUs(void)
{
	return 0;
}
static void tlkmmi_btmgr_taskWakeup(void)
{
	
}
static void tlkmmi_btmgr_taskHandler(void)
{
	tlkmdi_bt_handler();
	#if (TLK_MDI_BTIAP_ENABLE)
	tlkmdi_btiap_handler();
	#endif
}




#endif //#if (TLKMMI_BTMGR_ENABLE)

