/********************************************************************************************************
 * @file     tlkmmi_testTask.c
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
#if (TLKMMI_TEST_ENABLE)
#include "tlksys/tsk/tlktsk_stdio.h"
#include "tlkmmi_test.h"
#include "tlkmmi_testTask.h"
#include "tlkmmi_testAdapt.h"
#include "tlkmmi_testModinf.h"
#include "tlkmmi_testStdio.h"
#include "tlkmmi_testMsg.h"

extern int  tlkmdi_comm_regCmdCB(uint08 mtype, uint08 taskID);

static int  tlkmmi_test_taskStart(void);
static int  tlkmmi_test_taskPause(void);
static int  tlkmmi_test_taskClose(void);
static int  tlkmmi_test_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen);
static int  tlkmmi_test_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
static uint tlkmmi_test_taskIntvUs(void);
static void tlkmmi_test_taskWakeup(void);
static void tlkmmi_test_taskHandler(void);
static const tlktsk_modinf_t sTlkMmiTestTaskInfs = {
	TLKTSK_TASKID_TEST, //taskID
	tlkmmi_test_taskStart, //int(*Start)
	tlkmmi_test_taskPause, //int(*Pause)
	tlkmmi_test_taskClose, //int(*Close)
	tlkmmi_test_taskInput, //int(*Input)
	tlkmmi_test_taskExtMsg, //int(*ExtMsg)
	tlkmmi_test_taskIntvUs, //uint(*IntvUs)
	tlkmmi_test_taskWakeup, //void(*Wakeup)
	tlkmmi_test_taskHandler, //void(*Handler)
};
extern uint08 gTlkMmiTestWorkMode;


int tlkmmi_test_taskInit(void)
{
	tlktsk_mount(TLKMMI_TEST_PROCID, &sTlkMmiTestTaskInfs);
	tlkmdi_comm_regCmdCB(TLKPRT_COMM_MTYPE_TEST, TLKTSK_TASKID_TEST);
	
	return TLK_ENONE;
}


static int tlkmmi_test_taskStart(void)
{
	tlkmmi_test_start();
	return TLK_ENONE;
}
static int tlkmmi_test_taskPause(void)
{
	tlkmmi_test_pause();
	return TLK_ENONE;
}
static int tlkmmi_test_taskClose(void)
{
	tlkmmi_test_close();
	return TLK_ENONE;
}
static int tlkmmi_test_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	if(mtype != TLKPRT_COMM_MTYPE_TEST) return -TLK_ENOSUPPORT;
	tlkapi_trace(TLKMMI_TEST_DBG_FLAG, TLKMMI_TEST_DBG_SIGN, "mtype->%d msgID->%d", mtype, msgID);
	tlkapi_array(TLKMMI_TEST_DBG_FLAG, TLKMMI_TEST_DBG_SIGN, "Payload", pData, dataLen);
	if(msgID < TLKPRT_COMM_CMDID_TEST_CONTEXT_START){
		tlkmmi_test_recvMsgHandler(msgID, pData, dataLen);
		return TLK_ENONE;
	}else{
		return tlkmmi_test_input(msgID, pData, dataLen);
	}
}
static int tlkmmi_test_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static uint tlkmmi_test_taskIntvUs(void)
{
	return 0;
}
static void tlkmmi_test_taskWakeup(void)
{
	
}
static void tlkmmi_test_taskHandler(void)
{
	tlkmmi_test_handler();
}



#endif //#if (TLKMMI_TEST_ENABLE)

