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
#if (TLK_CFG_SYS_ENABLE)
#include "tlksys.h"
#include "tlksys_stdio.h"
#include "tlksys_adapt.h"
#include "tlksys_mutex.h"
#include "tlksys_proc.h"
#include "tlksys_task.h"
#include "tlksys_pm.h"
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


int tlksys_init(void)
{
	int index;

	tlksys_pm_init();
	#if (TLK_CFG_OS_ENABLE)
	tlktsk_osInit();
	tlksys_mutex_init();
	#endif
	for(index=1; index<=TLKSYS_PROC_MAX_NUMB; index++){
		tlksys_adapt_init(index);
	}
	tlksys_mutex_init();
	tlksys_proc_init();
	tlksys_task_init();
	
	return TLK_ENONE;
}

void tlksys_start(uint08 procID)
{
	if(procID == 0xFF || procID == 0x00){
		uint08 index;
		for(index=1; index<=TLKSYS_PROC_MAX_NUMB; index++){
			tlksys_proc_start(index);
		}
	}else{
		tlksys_proc_start(procID);
	}
}
void tlksys_pause(uint08 procID)
{
	if(procID == 0xFF || procID == 0x00){
		uint08 index;
		for(index=1; index<=TLKSYS_PROC_MAX_NUMB; index++){
			tlksys_proc_pause(index);
		}
	}else{
		tlksys_proc_pause(procID);
	}
}
void tlksys_close(uint08 procID)
{
	if(procID == 0xFF || procID == 0x00){
		uint08 index;
		for(index=1; index<=TLKSYS_PROC_MAX_NUMB; index++){
			tlksys_proc_close(index);
		}
	}else{
		tlksys_proc_close(procID);
	}
}

void tlksys_handler(void)
{
	#if (TLK_CFG_OS_ENABLE)
	tlkos_start_run();
	#else
	uint08 index;
	for(index=1; index<=TLKSYS_PROC_MAX_NUMB; index++){
		tlksys_proc_handler(index);
	}
	#endif
}


int tlksys_sendInnerMsg(uint08 taskID, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	int ret;
	#if (TLK_CFG_OS_ENABLE)
	core_enter_critical();
	#endif
	ret = tlksys_task_input(taskID, TLKPRT_COMM_MTYPE_NONE, msgID, nullptr, 0, pData, dataLen);
	#if (TLK_CFG_OS_ENABLE)
	core_leave_critical();
	#endif
	return ret;
}
int tlksys_sendOuterMsg(uint08 taskID, uint08 mType, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	int ret;
	#if (TLK_CFG_OS_ENABLE)
	core_enter_critical();
	#endif
	ret = tlksys_task_input(taskID, mType, msgID, nullptr, 0, pData, dataLen);
	#if (TLK_CFG_OS_ENABLE)
	core_leave_critical();
	#endif
	return ret;
}
int tlksys_sendInnerExtMsg(uint08 taskID, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	int ret;
	#if (TLK_CFG_OS_ENABLE)
	core_enter_critical();
	#endif
	ret = tlksys_task_input(taskID, TLKPRT_COMM_MTYPE_NONE, msgID, pHead, headLen, pData, dataLen);
	#if (TLK_CFG_OS_ENABLE)
	core_leave_critical();
	#endif
	return ret;
}
int tlksys_sendOuterExtMsg(uint08 taskID, uint08 mType, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	int ret;
	#if (TLK_CFG_OS_ENABLE)
	core_enter_critical();
	#endif
	ret = tlksys_task_input(taskID, mType, msgID, pHead, headLen, pData, dataLen);
	#if (TLK_CFG_OS_ENABLE)
	core_leave_critical();
	#endif
	return ret;
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
		for(index=1; index<=TLKSYS_PROC_MAX_NUMB; index++){
			if(index == TLKSYS_PROCID_AUDIO) continue;
			tlksys_proc_handler(index+1);
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
		tlksys_proc_handler(TLKSYS_PROCID_AUDIO);
		interval = tlksys_adapt_interval(TLKSYS_PROCID_AUDIO);
		if(interval < 500) interval = 0;
		else if(interval > 3000000) interval = 3000000;
		if(interval != 0){
			interval = (interval+500)/1000;
			tlkos_task_delayMs(&sTlkTskAudioTask, interval);
		}
	}
}


#endif //#if (TLK_CFG_OS_ENABLE)

#endif //#if (TLK_CFG_SYS_ENABLE)

