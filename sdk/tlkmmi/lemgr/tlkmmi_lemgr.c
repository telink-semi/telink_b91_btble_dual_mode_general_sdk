/********************************************************************************************************
 * @file     tlkmmi_lemgr.c
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
#include "tlksys/tlksys_stdio.h"
#include "tlkmdi/misc/tlkmdi_comm.h"
#include "tlkmmi_lemgr.h"
#include "tlkmmi_lemgrAdapt.h"
#include "tlkmmi_lemgrCtrl.h"
#include "tlkmmi_lemgrAcl.h"
#include "tlkmmi_lemgrMsgInner.h"
#include "tlkmmi_lemgrMsgOuter.h"


TLKSYS_MMI_TASK_DEFINE(lemgr, Lemgr);


static int tlkmmi_lemgr_init(uint08 procID, uint08 taskID)
{
	#if (TLK_CFG_COMM_ENABLE)
	tlkmdi_comm_regCmdCB(TLKPRT_COMM_MTYPE_LE, TLKSYS_TASKID_LEMGR);
	#endif
	
	tlkmmi_lemgr_adaptInit(procID);
	tlkmmi_lemgr_ctrlInit();
	tlkmmi_lemgr_aclInit();
	return TLK_ENONE;
}
static int tlkmmi_lemgr_start(void)
{
	tlkmmi_lemgr_startAdv(0, 0);
	return TLK_ENONE;
}
static int tlkmmi_lemgr_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_lemgr_close(void)
{
	
	return TLK_ENONE;
}
static int tlkmmi_lemgr_input(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen,
	uint08 *pData, uint16 dataLen)
{
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		tlkmmi_lemgr_innerMsgHandler(msgID, pData, dataLen);
	}else{
		tlkmmi_lemgr_outerMsgHandler(msgID, pData, dataLen);
	}
	return TLK_ENONE;
}
static void tlkmmi_lemgr_handler(void)
{
#if BLE_IOS_ANCS_ENABLE
	extern void app_ancsTask();
	app_ancsTask();
#endif

#if BLE_IOS_AMS_ENABLE
	extern void app_amsTask();
	app_amsTask();
#endif
}



#endif //TLKMMI_LEMGR_ENABLE

