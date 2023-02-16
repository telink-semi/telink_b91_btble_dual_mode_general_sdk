/********************************************************************************************************
 * @file     tlkmmi_phoneTask.c
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
#if (TLKMMI_PHONE_ENABLE)
#include "tlksys/tsk/tlktsk_stdio.h"
#include "tlkmmi/phone/tlkmmi_phone.h"
#include "tlkmmi/phone/tlkmmi_phoneAdapt.h"
#include "tlkmmi/phone/tlkmmi_phoneTask.h"
#include "tlkmmi/phone/tlkmmi_phoneMsgInner.h"
#include "tlkmmi/phone/tlkmmi_phoneMsgOuter.h"


extern int  tlkmdi_comm_regCmdCB(uint08 mtype, uint08 taskID);
extern void tlkmmi_phone_hfCallEvt(uint08 msgID, uint08 *pData, uint08 dataLen);
extern void tlkmmi_phone_cmdHandler(uint08 msgID, uint08 *pData, uint08 dataLen);

static int  tlkmmi_phone_taskStart(void);
static int  tlkmmi_phone_taskPause(void);
static int  tlkmmi_phone_taskClose(void);
static int  tlkmmi_phone_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen);
static int  tlkmmi_phone_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
static uint tlkmmi_phone_taskIntvUs(void);
static void tlkmmi_phone_taskWakeup(void);
static void tlkmmi_phone_taskHandler(void);
static const tlktsk_modinf_t sTlkMmiPhoneTaskInfs = {
	TLKTSK_TASKID_PHONE, //taskID
	tlkmmi_phone_taskStart, //int(*Start)
	tlkmmi_phone_taskPause, //int(*Pause)
	tlkmmi_phone_taskClose, //int(*Close)
	tlkmmi_phone_taskInput, //int(*Input)
	tlkmmi_phone_taskExtMsg, //int(*ExtMsg)
	tlkmmi_phone_taskIntvUs, //uint(*IntvUs)
	tlkmmi_phone_taskWakeup, //void(*Wakeup)
	tlkmmi_phone_taskHandler, //void(*Handler)
};


int tlkmmi_phone_taskInit(void)
{
	tlktsk_mount(TLKMMI_PHONE_PROCID, &sTlkMmiPhoneTaskInfs);
	tlkmdi_comm_regCmdCB(TLKPRT_COMM_MTYPE_CALL, TLKTSK_TASKID_PHONE);
	
	return TLK_ENONE;
}




static int tlkmmi_phone_taskStart(void)
{
	return TLK_ENONE;
}
static int tlkmmi_phone_taskPause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_phone_taskClose(void)
{
	return TLK_ENONE;
}
static int tlkmmi_phone_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		return tlkmmi_phone_innerMsgHandler(msgID, pData, dataLen);
	}else{
		return tlkmmi_phone_outerMsgHandler(msgID, pData, dataLen);
	}
}
static int tlkmmi_phone_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static uint tlkmmi_phone_taskIntvUs(void)
{
	return 0;
}
static void tlkmmi_phone_taskWakeup(void)
{
	
}
static void tlkmmi_phone_taskHandler(void)
{
	
}



#endif //#if (TLKMMI_PHONE_ENABLE)

