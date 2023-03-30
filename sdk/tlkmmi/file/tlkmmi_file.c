/********************************************************************************************************
 * @file     tlkmmi_file.c
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
#include "tlksys/tlksys_stdio.h"
#include "tlkmdi/misc/tlkmdi_file.h"
#include "tlkmdi/misc/tlkmdi_comm.h"
#include "tlkmmi_file.h"
#include "tlkmmi_fileAdapt.h"
#include "tlkmmi_fileCtrl.h"
#include "tlkmmi_fileMsgInner.h"
#include "tlkmmi_fileMsgOuter.h"
#if (TLK_MDI_FILE_ENABLE)
#include "tlkmdi/misc/tlkmdi_file.h"
#endif
#if (TLK_MDI_FS_ENABLE)
#include "tlkmdi/misc/tlkmdi_fs.h"
#endif


extern bool tlkmmi_file_dfuIsStart(void);




TLKSYS_MMI_TASK_DEFINE(file, File);


static int tlkmmi_file_init(uint08 procID, uint08 taskID)
{
	#if (TLK_CFG_COMM_ENABLE)
	tlkmdi_comm_regCmdCB(TLKPRT_COMM_MTYPE_FILE, TLKSYS_TASKID_FILE);
	#endif
	#if (TLK_MDI_FS_ENABLE)
	tlkmdi_fs_init();
	#endif
	#if (TLK_MDI_FILE_ENABLE)
	tlkmdi_file_init();
	#endif
	#if (TLK_CFG_SYS_ENABLE)
	tlksys_pm_appendBusyCheckCB(tlkmmi_file_dfuIsStart);
	#endif
	
	tlkmmi_file_adaptInit(procID);
	tlkmmi_file_ctrlInit();
	
	return TLK_ENONE;
}
static int tlkmmi_file_start(void)
{
	
	return TLK_ENONE;
}
static int tlkmmi_file_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_file_close(void)
{
	
	return TLK_ENONE;
}
static int tlkmmi_file_input(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen,
	uint08 *pData, uint16 dataLen)
{
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		return tlkmmi_file_innerMsgHandler(msgID, pData, dataLen);
	}else{
		return tlkmmi_file_outerMsgHandler(msgID, pData, dataLen);
	}
}
static void tlkmmi_file_handler(void)
{
	
}




#endif //#if (TLKMMI_FILE_ENABLE)

