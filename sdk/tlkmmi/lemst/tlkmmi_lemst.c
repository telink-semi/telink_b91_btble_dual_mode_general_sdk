/********************************************************************************************************
 * @file	tlkmmi_lemst.c
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
#if (TLKMMI_LEMST_ENABLE)
#include "tlkmdi/misc/tlkmdi_comm.h"
#include "tlkmmi_lemst.h"
#include "tlkmmi_lemstAdapt.h"
#include "tlkmmi_lemstCtrl.h"
#include "tlkmmi_lemstAcl.h"
#include "tlkmmi_lemstTask.h"
#include "tlkmmi_lemstMsgInner.h"
#include "tlkmmi_lemstMsgOuter.h"


TLKSYS_MMI_TASK_DEFINE(lemst, Lemst);


static int tlkmmi_lemst_init(uint08 procID, uint08 taskID)
{
	#if (TLK_CFG_COMM_ENABLE)
	tlkmdi_comm_regCmdCB(TLKPRT_COMM_MTYPE_LE, TLKSYS_TASKID_LEMST);
	#endif
	
	tlkmmi_lemst_adaptInit(procID);
	tlkmmi_lemst_ctrlInit();
	tlkmmi_lemst_aclInit();

	return TLK_ENONE;
}
static int tlkmmi_lemst_start(void)
{
	
	return TLK_ENONE;
}
static int tlkmmi_lemst_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_lemst_close(void)
{
	
	return TLK_ENONE;
}
static int tlkmmi_lemst_input(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen,
	uint08 *pData, uint16 dataLen)
{
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		tlkmmi_lemst_innerMsgHandler(msgID, pData, dataLen);
	}else{
		tlkmmi_lemst_outerMsgHandler(msgID, pData, dataLen);
	}
	return TLK_ENONE;
}
static void tlkmmi_lemst_handler(void)
{
	
}






#endif //TLKMMI_LEMST_ENABLE

