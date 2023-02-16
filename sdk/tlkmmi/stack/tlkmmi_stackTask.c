/********************************************************************************************************
 * @file     tlkmmi_stackTask.c
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
#include "tlkmdi/tlkmdi_stdio.h"
#if (TLKMMI_STACK_ENABLE)
#include "tlkstk/tlkstk_stdio.h"
#include "tlksys/tsk/tlktsk_stdio.h"
#include "tlkmmi_stack.h"
#include "tlkmmi_stackTask.h"
#include "tlkmmi_stackAdapt.h"


extern int  tlkmdi_comm_regCmdCB(uint08 mtype, uint08 taskID);

static int  tlkmmi_stack_taskStart(void);
static int  tlkmmi_stack_taskPause(void);
static int  tlkmmi_stack_taskClose(void);
static int  tlkmmi_stack_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen);
static int  tlkmmi_stack_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
static uint tlkmmi_stack_taskIntvUs(void);
static void tlkmmi_stack_taskWakeup(void);
static void tlkmmi_stack_taskHandler(void);
static const tlktsk_modinf_t sTlkMmiStackTaskInfs = {
	TLKTSK_TASKID_STACK, //taskID
	tlkmmi_stack_taskStart, //int(*Start)
	tlkmmi_stack_taskPause, //int(*Pause)
	tlkmmi_stack_taskClose, //int(*Close)
	tlkmmi_stack_taskInput, //int(*Input)
	tlkmmi_stack_taskExtMsg, //int(*ExtMsg)
	tlkmmi_stack_taskIntvUs, //uint(*IntvUs)
	tlkmmi_stack_taskWakeup, //void(*Wakeup)
	tlkmmi_stack_taskHandler, //void(*Handler)
};


int tlkmmi_stack_taskInit(void)
{
	tlktsk_mount(TLKMMI_STACK_PROCID, &sTlkMmiStackTaskInfs);
	
	return TLK_ENONE;
}


static int tlkmmi_stack_taskStart(void)
{
	
	return TLK_ENONE;
}
static int tlkmmi_stack_taskPause(void)
{
	
	return TLK_ENONE;
}
static int tlkmmi_stack_taskClose(void)
{
	
	return TLK_ENONE;
}
static int tlkmmi_stack_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	return TLK_ENONE;
}
static int tlkmmi_stack_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static uint tlkmmi_stack_taskIntvUs(void)
{
	return 0;
}
static void tlkmmi_stack_taskWakeup(void)
{
	
}
static void tlkmmi_stack_taskHandler(void)
{
	tlkstk_process();
}



#endif //#if (TLKMMI_STACK_ENABLE)

