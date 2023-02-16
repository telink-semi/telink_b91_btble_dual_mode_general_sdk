/********************************************************************************************************
 * @file     tlkmmi_lemgrTask.c
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
#if (TLKMMI_LEMGR_ENABLE)
#include "tlksys/tsk/tlktsk_stdio.h"
#include "tlkmmi_lemgr.h"
#include "tlkmmi_lemgrAdapt.h"
#include "tlkmmi_lemgrTask.h"
#include "tlkmmi_lemgrAcl.h"
#include "tlkmmi_lemgrMsgInner.h"
#include "tlkmmi_lemgrMsgOuter.h"


extern int tlkmdi_comm_regCmdCB(uint08 mtype, uint08 taskID);

static int  tlkmmi_lemgr_taskStart(void);
static int  tlkmmi_lemgr_taskPause(void);
static int  tlkmmi_lemgr_taskClose(void);
static int  tlkmmi_lemgr_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen);
static int  tlkmmi_lemgr_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
static uint tlkmmi_lemgr_taskIntvUs(void);
static void tlkmmi_lemgr_taskWakeup(void);
static void tlkmmi_lemgr_taskHandler(void);
static const tlktsk_modinf_t sTlkMmiLemgrTaskInfs = {
	TLKTSK_TASKID_LEMGR, //taskID
	tlkmmi_lemgr_taskStart, //int(*Start)
	tlkmmi_lemgr_taskPause, //int(*Pause)
	tlkmmi_lemgr_taskClose, //int(*Close)
	tlkmmi_lemgr_taskInput, //int(*Input)
	tlkmmi_lemgr_taskExtMsg, //int(*ExtMsg)
	tlkmmi_lemgr_taskIntvUs, //uint(*IntvUs)
	tlkmmi_lemgr_taskWakeup, //void(*Wakeup)
	tlkmmi_lemgr_taskHandler, //void(*Handler)
};


int tlkmmi_lemgr_taskInit(void)
{
	tlktsk_mount(TLKMMI_LEMGR_PROCID, &sTlkMmiLemgrTaskInfs);
	tlkmdi_comm_regCmdCB(TLKPRT_COMM_MTYPE_LE, TLKTSK_TASKID_LEMGR);
	
	return TLK_ENONE;
}




static int tlkmmi_lemgr_taskStart(void)
{
	tlkmmi_lemgr_startAdv(0, 0);
	return TLK_ENONE;
}
static int tlkmmi_lemgr_taskPause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_lemgr_taskClose(void)
{
	return TLK_ENONE;
}
static int tlkmmi_lemgr_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		tlkmmi_lemgr_innerMsgHandler(msgID, pData, dataLen);
	}else{
		tlkmmi_lemgr_outerMsgHandler(msgID, pData, dataLen);
	}
	return TLK_ENONE;
}
static int tlkmmi_lemgr_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static uint tlkmmi_lemgr_taskIntvUs(void)
{
	return 0;
}
static void tlkmmi_lemgr_taskWakeup(void)
{
	
}
static void tlkmmi_lemgr_taskHandler(void)
{
	
}





#endif //TLKMMI_LEMGR_ENABLE

