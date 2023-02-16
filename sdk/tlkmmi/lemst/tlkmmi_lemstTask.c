/********************************************************************************************************
 * @file     tlkmmi_lemstTask.c
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
#if (TLKMMI_LEMST_ENABLE)
#include "tlksys/tsk/tlktsk_stdio.h"
#include "tlkmmi/lemst/tlkmmi_lemst.h"
#include "tlkmmi/lemst/tlkmmi_lemstAdapt.h"
#include "tlkmmi/lemst/tlkmmi_lemstTask.h"
#include "tlkmmi/lemst/tlkmmi_lemstMsgInner.h"
#include "tlkmmi/lemst/tlkmmi_lemstMsgOuter.h"


extern int tlkmdi_comm_regCmdCB(uint08 mtype, uint08 taskID);

static int  tlkmmi_lemst_taskStart(void);
static int  tlkmmi_lemst_taskPause(void);
static int  tlkmmi_lemst_taskClose(void);
static int  tlkmmi_lemst_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen);
static int  tlkmmi_lemst_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
static uint tlkmmi_lemst_taskIntvUs(void);
static void tlkmmi_lemst_taskWakeup(void);
static void tlkmmi_lemst_taskHandler(void);
static const tlktsk_modinf_t sTlkMmiLemstTaskInfs = {
	TLKTSK_TASKID_LEMST, //taskID
	tlkmmi_lemst_taskStart, //int(*Start)
	tlkmmi_lemst_taskPause, //int(*Pause)
	tlkmmi_lemst_taskClose, //int(*Close)
	tlkmmi_lemst_taskInput, //int(*Input)
	tlkmmi_lemst_taskExtMsg, //int(*ExtMsg)
	tlkmmi_lemst_taskIntvUs, //uint(*IntvUs)
	tlkmmi_lemst_taskWakeup, //void(*Wakeup)
	tlkmmi_lemst_taskHandler, //void(*Handler)
};


int tlkmmi_lemst_taskInit(void)
{
	tlktsk_mount(TLKMMI_LEMST_PROCID, &sTlkMmiLemstTaskInfs);
	tlkmdi_comm_regCmdCB(TLKPRT_COMM_MTYPE_LE, TLKTSK_TASKID_LEMST);
	
	return TLK_ENONE;
}




static int tlkmmi_lemst_taskStart(void)
{
	return TLK_ENONE;
}
static int tlkmmi_lemst_taskPause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_lemst_taskClose(void)
{
	return TLK_ENONE;
}
static int tlkmmi_lemst_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		tlkmmi_lemst_innerMsgHandler(msgID, pData, dataLen);
	}else{
		tlkmmi_lemst_outerMsgHandler(msgID, pData, dataLen);
	}
	return TLK_ENONE;
}
static int tlkmmi_lemgr_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static uint tlkmmi_lemst_taskIntvUs(void)
{
	return 0;
}
static void tlkmmi_lemst_taskWakeup(void)
{
	
}
static void tlkmmi_lemst_taskHandler(void)
{
	
}





#endif //TLKMMI_LEMST_ENABLE

