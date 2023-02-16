/********************************************************************************************************
 * @file     tlkmmi_viewTask.c
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
#if (TLKMMI_VIEW_ENABLE)
#include "tlksys/tsk/tlktsk_stdio.h"
#include "tlkmmi/view/tlkmmi_view.h"
#include "tlkmmi/view/tlkmmi_viewTask.h"
#if (TLK_MDI_GUI_ENABLE)
#include "tlkmdi/gui/tlkmdi_gui.h"
#endif


extern int  tlkmdi_comm_regCmdCB(uint08 mtype, uint08 taskID);

static int  tlkmmi_view_taskStart(void);
static int  tlkmmi_view_taskPause(void);
static int  tlkmmi_view_taskClose(void);
static int  tlkmmi_view_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen);
static int  tlkmmi_view_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
static uint tlkmmi_view_taskIntvUs(void);
static void tlkmmi_view_taskWakeup(void);
static void tlkmmi_view_taskHandler(void);
static const tlktsk_modinf_t sTlkMmiViewTaskInfs = {
	TLKTSK_TASKID_VIEW, //taskID
	tlkmmi_view_taskStart, //int(*Start)
	tlkmmi_view_taskPause, //int(*Pause)
	tlkmmi_view_taskClose, //int(*Close)
	tlkmmi_view_taskInput, //int(*Input)
	tlkmmi_view_taskExtMsg, //int(*ExtMsg)
	tlkmmi_view_taskIntvUs, //uint(*IntvUs)
	tlkmmi_view_taskWakeup, //void(*Wakeup)
	tlkmmi_view_taskHandler, //void(*Handler)
};


int tlkmmi_view_taskInit(void)
{
	tlktsk_mount(TLKMMI_VIEW_PROCID, &sTlkMmiViewTaskInfs);
	tlkmdi_comm_regCmdCB(TLKPRT_COMM_MTYPE_VIEW, TLKTSK_TASKID_VIEW);
	
	return TLK_ENONE;
}




static int tlkmmi_view_taskStart(void)
{
	return TLK_ENONE;
}
static int tlkmmi_view_taskPause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_view_taskClose(void)
{
	return TLK_ENONE;
}
static int tlkmmi_view_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	
	return TLK_ENONE;
}
static int tlkmmi_view_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static uint tlkmmi_view_taskIntvUs(void)
{
	return 0;
}
static void tlkmmi_view_taskWakeup(void)
{
	
}
static void tlkmmi_view_taskHandler(void)
{
	tlkmdi_gui_handler();
}


#endif //#if (TLKMMI_VIEW_ENABLE)

