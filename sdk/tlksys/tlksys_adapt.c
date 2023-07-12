/********************************************************************************************************
 * @file	tlksys_adapt.c
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
#if (TLK_CFG_SYS_ENABLE)
#include "tlksys_stdio.h"
#include "tlksys_adapt.h"
#if (TLK_CFG_OS_ENABLE)
#include "tlksys_mutex.h"
#endif


typedef struct{
	tlkapi_adapt_t adapt;
}tlktsk_adapt_t;
static tlktsk_adapt_t sTlkSysAdaptList[TLKSYS_PROC_MAX_NUMB] = {0};


/******************************************************************************
 * Function: tlksys_adapt_init
 * Descript: Initializes the adapter control parameters.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_adapt_init(uint08 procID)
{
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	return tlkapi_adapt_init(&sTlkSysAdaptList[procID-1].adapt);
}

/******************************************************************************
 * Function: tlktsk_adapt_run_once
 * Descript: Implement the function of traversing the scheduled task and work
 *           queue in the adapter to ensure the timeliness of execution.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlksys_adapt_handler(uint08 procID)
{
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	tlkapi_adapt_handler(&sTlkSysAdaptList[procID-1].adapt);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
}

/******************************************************************************
 * Function: tlksys_adapt_interval
 * Descript: Gets the execution time of the next task in the process.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 * Return: The execution time of the next task in the process.
 * Others: None.
*******************************************************************************/
uint tlksys_adapt_interval(uint08 procID)
{
	uint ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return 0xFFFFFFFF;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_interval(&sTlkSysAdaptList[procID-1].adapt);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}

/******************************************************************************
 * Function: tlksys_adapt_initTimer, tlksys_adapt_deinitTimer
 *           tlksys_adapt_initQueue, tlksys_adapt_deinitTimer
 * Descript: Initializes and deinitializes the timing scheduler and work queue.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 *     @userArg[IN]--Parameters passed in by the user will be returned on the call.
 *     @timeout[IN]--Scheduling interval set by the user. Unit:us.
 *     @timerCB[IN]--The callback interface after the time has arrived.
 *     @queueCB[IN]--The callback interface for the work to be executed.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_adapt_initTimer(uint08 procID, tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout)
{
	int ret = TLK_ENONE;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	if(tlksys_adapt_isHaveTimer(procID, pTimer)){
		ret = -TLK_EEXIST;
	}
	if(ret == TLK_ENONE){
		ret = tlkapi_adapt_initTimer(pTimer, timerCB, userArg, timeout);
	}
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}
int tlksys_adapt_initQueue(uint08 procID, tlkapi_queue_t *pQueue, TlkApiQueueCB queueCB, uint32 userArg)
{
	int ret = TLK_ENONE;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	if(tlksys_adapt_isHaveQueue(procID, pQueue)){
		ret = -TLK_EEXIST;
	}
	if(ret == TLK_ENONE){
		ret = tlkapi_adapt_initQueue(pQueue, queueCB, userArg);
	}
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}
void tlksys_adapt_deinitTimer(uint08 procID, tlkapi_timer_t *pTimer)
{
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	tlkapi_adapt_deinitTimer(&sTlkSysAdaptList[procID-1].adapt, pTimer);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
}
void tlksys_adapt_deinitQueue(uint08 procID, tlkapi_queue_t *pQueue)
{
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	tlkapi_adapt_deinitQueue(&sTlkSysAdaptList[procID-1].adapt, pQueue);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
}

/******************************************************************************
 * Function: tlksys_adapt_isHaveTimer, tlksys_adapt_isHaveQueue
 * Descript: Check whether scheduled tasks and cache work exist in the adaptation.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 * Return: Return TRUE if has, FALSE otherwise.
 * Others: None.
*******************************************************************************/
bool tlksys_adapt_isHaveTimer(uint08 procID, tlkapi_timer_t *pTimer)
{
	int ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return false;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_isHaveTimer(&sTlkSysAdaptList[procID-1].adapt, pTimer);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}
bool tlksys_adapt_isHaveQueue(uint08 procID, tlkapi_queue_t *pQueue)
{
	int ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return false;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_isHaveQueue(&sTlkSysAdaptList[procID-1].adapt, pQueue);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}

/******************************************************************************
 * Function: tlksys_adapt_appendQueue, tlksys_adapt_removeQueue
 * Descript: Implement the function of adding and deleting work queues.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 *     @pQueue[IN]--Work queue node.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_adapt_appendQueue(uint08 procID, tlkapi_queue_t *pQueue)
{
	int ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_appendQueue(&sTlkSysAdaptList[procID-1].adapt, pQueue);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}
int tlksys_adapt_removeQueue(uint08 procID, tlkapi_queue_t *pQueue)
{
	int ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_removeQueue(&sTlkSysAdaptList[procID-1].adapt, pQueue);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}

/******************************************************************************
 * Function: tlksys_adapt_updateTimer, tlksys_adapt_insertTimer,
 *           tlksys_adapt_removeTimer
 * Descript: Implement the function of adding, deleting and updating timer.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 *     @pTimer[IN]--Timer scheduling node.
 *     @isUpdate[IN]--True,Timer recount; False,Depending on the state of the
 *       other parameters, the timer may continue the previous counting logic.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_adapt_updateTimer(uint08 procID, tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert)
{
	int ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_updateTimer(&sTlkSysAdaptList[procID-1].adapt, pTimer, timeout, isInsert);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}
int tlksys_adapt_insertTimer(uint08 procID, tlkapi_timer_t *pTimer, bool isUpdate)
{
	int ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_insertTimer(&sTlkSysAdaptList[procID-1].adapt, pTimer, true);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}
int tlksys_adapt_removeTimer(uint08 procID, tlkapi_timer_t *pTimer)
{
	int ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_removeTimer(&sTlkSysAdaptList[procID-1].adapt, pTimer);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}


#endif //#if (TLK_CFG_SYS_ENABLE)

