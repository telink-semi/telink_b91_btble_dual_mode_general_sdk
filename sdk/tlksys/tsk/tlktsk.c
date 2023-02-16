/********************************************************************************************************
 * @file     tlktsk.c
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
#include "tlktsk_stdio.h"
#include "tlktsk_adapt.h"
#include "tlktsk_mutex.h"
#include "tlktsk_modinf.h"
#include "tlktsk_schinf.h"
#include "tlktsk.h"
#if (TLK_CFG_OS_ENABLE)
#include "tlkapi/tlkapi_os.h"
#endif
#include "drivers.h"


#if (TLK_CFG_OS_ENABLE)
static int tlktsk_osInit(void);
#endif //#if (TLK_CFG_OS_ENABLE)

#if (TLK_CFG_OS_ENABLE)
static tlkos_task_t sTlkTskSysTask;
static tlkos_task_t sTlkTskAudioTask;
static tlkos_task_t sTlkTskStackTask;
#endif


int tlktsk_init(void)
{
	int index;

	#if (TLK_CFG_OS_ENABLE)
	tlktsk_osInit();
	tlktsk_mutex_init();
	#endif
	for(index=0; index<TLKTSK_PROCID_MAX; index++){
		tlktsk_adapt_init(index);
	}
	tlktsk_modinf_init();
	tlktsk_schinf_init();
	
	return TLK_ENONE;
}


int tlktsk_mount(uint08 procID, const tlktsk_modinf_t *pModinf)
{
	int ret;

	if(procID >= TLKTSK_PROCID_MAX || pModinf == nullptr || pModinf->taskID == 0 
		|| pModinf->taskID >= TLKTSK_TASKID_MAX){
		return -TLK_EPARAM;
	}

	ret = tlktsk_modinf_mount(pModinf);
	if(ret != TLK_ENONE) return ret;

	ret = tlktsk_schinf_mount(procID, pModinf->taskID);
	if(ret != TLK_ENONE) return ret;
	
	return TLK_ENONE;
}

void tlktsk_start(uint08 procID)
{
	if(procID == 0xFF){
		uint08 index;
		for(index=0; index<TLKTSK_TASKID_MAX; index++){
			tlktsk_modinf_start(index);
		}
		for(index=0; index<TLKTSK_PROCID_MAX; index++){
			tlktsk_schinf_start(index);
		}
	}else{
		tlktsk_schinf_start(procID);
	}
}
void tlktsk_pause(uint08 procID)
{
	if(procID == 0xFF){
		uint08 index;
		for(index=0; index<TLKTSK_PROCID_MAX; index++){
			tlktsk_schinf_pause(index);
		}
	}else{
		tlktsk_schinf_pause(procID);
	}
}
void tlktsk_close(uint08 procID)
{
	if(procID == 0xFF){
		uint08 index;
		for(index=0; index<TLKTSK_PROCID_MAX; index++){
			tlktsk_schinf_close(index);
		}
	}else{
		tlktsk_schinf_close(procID);
	}
}

int tlktsk_sendInnerMsg(TLKTSK_TASKID_ENUM taskID, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	int ret;
	core_enter_critical();
	ret = tlktsk_modinf_input(taskID, TLKPRT_COMM_MTYPE_NONE, msgID, pData, dataLen);
	core_leave_critical();
	return ret;
}
int tlktsk_sendOuterMsg(TLKTSK_TASKID_ENUM taskID, uint08 mType, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	int ret;
	core_enter_critical();
	ret = tlktsk_modinf_input(taskID, mType, msgID, pData, dataLen);
	core_leave_critical();
	return ret;
}
int tlktsk_sendInnerExtMsg(TLKTSK_TASKID_ENUM taskID, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	int ret;
	core_enter_critical();
	ret = tlktsk_modinf_extmsg(taskID, TLKPRT_COMM_MTYPE_NONE, msgID, pHead, headLen, pData, dataLen);
	core_leave_critical();
	return ret;
}
int tlktsk_sendOuterExtMsg(TLKTSK_TASKID_ENUM taskID, uint08 mType, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	int ret;
	core_enter_critical();
	ret = tlktsk_modinf_extmsg(taskID, mType, msgID, pHead, headLen, pData, dataLen);
	core_leave_critical();
	return ret;
}


void tlktsk_run(void)
{
	#if (TLK_CFG_OS_ENABLE)
	tlkos_start_run();
	#else
	uint08 index;
	for(index=0; index<TLKTSK_PROCID_MAX; index++){
		tlktsk_adapt_handler(index);
		tlktsk_schinf_handler(index);
	}
	#endif
}



#if (TLK_CFG_OS_ENABLE)
#include "drivers.h"

static void tlktsk_systemProc(void *pUsrArg);
static void tlktsk_audioProc(void *pUsrArg);
static int tlktsk_osInit(void)
{
	int ret;
	
	tlkos_init();

	ret = tlkos_task_create(&sTlkTskSysTask, "SystemTask", 2, 1024*6, tlktsk_systemProc, nullptr);
	if(ret != TLK_ENONE){
		tlkapi_trace(TLKTSK_DBG_FLAG, TLKTSK_DBG_SIGN, "tlkos_task_create[System]: failure");
	}
	ret = tlkos_task_create(&sTlkTskAudioTask, "AudioTask", 3, 1024*8, tlktsk_audioProc, nullptr);
	if(ret != TLK_ENONE){
		tlkapi_trace(TLKTSK_DBG_FLAG, TLKTSK_DBG_SIGN, "tlkos_task_create[Audio]: failure");
	}
	
	return TLK_ENONE;
}

extern uint tlkos_task_getStackValid(tlkos_task_t *pTask);
static void tlktsk_systemProc(void *pUsrArg)
{
	uint08 index;
	uint32 gTimer = clock_time();
	
	while(true){
		if(clock_time_exceed(gTimer, 2000000)){
			gTimer = clock_time();
			tlkapi_trace(TLKTSK_DBG_FLAG, TLKTSK_DBG_SIGN, "tlktsk_systemProc: validStack-%d",
				tlkos_task_getStackValid(&sTlkTskSysTask));
		}
		for(index=0; index<TLKTSK_PROCID_MAX; index++){
			if(index == TLKTSK_PROCID_AUDIO) continue;
			tlktsk_adapt_handler(index);
			tlktsk_schinf_handler(index);
		}
	}
}
static void tlktsk_audioProc(void *pUsrArg)
{
	uint32 interval;
	uint32 gTimer = clock_time();

	while(true){
		if(clock_time_exceed(gTimer, 300000)){
			gTimer = clock_time();
			tlkapi_trace(TLKTSK_DBG_FLAG, TLKTSK_DBG_SIGN, "tlktsk_audioProc: validStack-%d",
				tlkos_task_getStackValid(&sTlkTskAudioTask));
		}
		tlktsk_adapt_handler(TLKTSK_PROCID_AUDIO);
		tlktsk_schinf_handler(TLKTSK_PROCID_AUDIO);
		interval = tlktsk_adapt_interval(TLKTSK_PROCID_AUDIO);
		if(interval < 500) interval = 0;
		else if(interval > 3000000) interval = 3000000;
		if(interval != 0){
			interval = (interval+500)/1000;
			tlkos_task_delayMs(&sTlkTskAudioTask, interval);
		}
	}
}


#endif //#if (TLK_CFG_OS_ENABLE)

