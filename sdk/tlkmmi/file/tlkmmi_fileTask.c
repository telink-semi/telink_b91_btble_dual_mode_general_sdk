/********************************************************************************************************
 * @file     tlkmmi_fileTask.c
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
#if (TLKMMI_FILE_ENABLE)
#include "tlksys/tsk/tlktsk_stdio.h"
#include "tlkmmi_file.h"
#include "tlkmmi_fileAdapt.h"
#include "tlkmmi_fileTask.h"
#include "tlkmmi_fileMsgInner.h"
#include "tlkmmi_fileMsgOuter.h"


extern int tlkmdi_comm_regCmdCB(uint08 mtype, uint08 taskID);

static int  tlkmmi_file_taskStart(void);
static int  tlkmmi_file_taskPause(void);
static int  tlkmmi_file_taskClose(void);
static int  tlkmmi_file_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen);
static int  tlkmmi_file_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
static uint tlkmmi_file_taskIntvUs(void);
static void tlkmmi_file_taskWakeup(void);
static void tlkmmi_file_taskHandler(void);
static const tlktsk_modinf_t sTlkMmiFileTaskInfs = {
	TLKTSK_TASKID_FILE, //taskID
	tlkmmi_file_taskStart, //int(*Start)
	tlkmmi_file_taskPause, //int(*Pause)
	tlkmmi_file_taskClose, //int(*Close)
	tlkmmi_file_taskInput, //int(*Input)
	tlkmmi_file_taskExtMsg, //int(*ExtMsg)
	tlkmmi_file_taskIntvUs, //uint(*IntvUs)
	tlkmmi_file_taskWakeup, //void(*Wakeup)
	tlkmmi_file_taskHandler, //void(*Handler)
};


int tlkmmi_file_taskInit(void)
{
	tlktsk_mount(TLKMMI_FILE_PROCID, &sTlkMmiFileTaskInfs);
	tlkmdi_comm_regCmdCB(TLKPRT_COMM_MTYPE_FILE, TLKTSK_TASKID_FILE);
	
	return TLK_ENONE;
}




static int tlkmmi_file_taskStart(void)
{
	return TLK_ENONE;
}
static int tlkmmi_file_taskPause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_file_taskClose(void)
{
	return TLK_ENONE;
}
static int tlkmmi_file_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		return tlkmmi_file_innerMsgHandler(msgID, pData, dataLen);
	}else{
		return tlkmmi_file_outerMsgHandler(msgID, pData, dataLen);
	}
}
static int tlkmmi_file_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static uint tlkmmi_file_taskIntvUs(void)
{
	return 0;
}
static void tlkmmi_file_taskWakeup(void)
{
	
}
static void tlkmmi_file_taskHandler(void)
{
	
}



#endif //#if (TLKMMI_FILE_ENABLE)

