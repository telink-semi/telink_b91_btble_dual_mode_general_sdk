/********************************************************************************************************
 * @file     tlkmmi_audioTask.c
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
#include "string.h"
#include "tlkapi/tlkapi_stdio.h"
#if (TLKMMI_AUDIO_ENABLE)
#include "tlksys/tsk/tlktsk_stdio.h"
#include "tlkmmi_audio.h"
#include "tlkmmi_audioAdapt.h"
#include "tlkmmi_audioCtrl.h"
#include "tlkmmi_audioMsgOuter.h"
#include "tlkmmi_audioMsgInner.h"
#include "tlkmdi/aud/tlkmdi_audio.h"


extern int tlkmdi_comm_regCmdCB(uint08 mtype, uint08 taskID);

static int  tlkmmi_audio_taskStart(void);
static int  tlkmmi_audio_taskPause(void);
static int  tlkmmi_audio_taskClose(void);
static int  tlkmmi_audio_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen);
static int  tlkmmi_audio_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
static uint tlkmmi_audio_taskIntvUs(void);
static void tlkmmi_audio_taskWakeup(void);
static void tlkmmi_audio_taskHandler(void);
static const tlktsk_modinf_t sTlkMmiAudioTaskInfs = {
	TLKTSK_TASKID_AUDIO, //taskID
	tlkmmi_audio_taskStart, //int(*Start)
	tlkmmi_audio_taskPause, //int(*Pause)
	tlkmmi_audio_taskClose, //int(*Close)
	tlkmmi_audio_taskInput, //int(*Input)
	tlkmmi_audio_taskExtMsg, //int(*ExtMsg)
	tlkmmi_audio_taskIntvUs, //uint(*IntvUs)
	tlkmmi_audio_taskWakeup, //void(*Wakeup)
	tlkmmi_audio_taskHandler, //void(*Handler)
};


int tlkmmi_audio_taskInit(void)
{
	tlktsk_mount(TLKMMI_AUDIO_PROCID, &sTlkMmiAudioTaskInfs);
	tlkmdi_comm_regCmdCB(TLKPRT_COMM_MTYPE_AUDIO, TLKTSK_TASKID_AUDIO);
	
	return TLK_ENONE;
}




static int tlkmmi_audio_taskStart(void)
{
	uint08 volume = tlkmdi_audio_getVoiceBtpVolume();
	tlktsk_sendInnerMsg(TLKTSK_TASKID_BTMGR, TLKPTI_BT_MSGID_SET_HFP_VOLUME, &volume, 1);
	return TLK_ENONE;
}
static int tlkmmi_audio_taskPause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_audio_taskClose(void)
{
	return TLK_ENONE;
}
static int tlkmmi_audio_taskInput(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		return tlkmmi_audio_innerMsgHandler(msgID, pData, dataLen);
	}else{
		return tlkmmi_audio_outerMsgHandler(msgID, pData, dataLen);
	}
}
static int tlkmmi_audio_taskExtMsg(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static uint tlkmmi_audio_taskIntvUs(void)
{
	return 0;
}
static void tlkmmi_audio_taskWakeup(void)
{
	
}
static void tlkmmi_audio_taskHandler(void)
{
	
}




#endif //#if (TLKMMI_AUDIO_ENABLE)

