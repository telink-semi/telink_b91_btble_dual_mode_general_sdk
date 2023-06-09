/********************************************************************************************************
 * @file     tlkmmi_phoneAdapt.c
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
#if (TLKMMI_PHONE_ENABLE)
#include "tlkmmi_phone.h"
#include "tlkmmi_phoneAdapt.h"
#include "tlksys/tlksys_stdio.h"


static uint08 sTlkMmiPhoneProcID = 0;


/******************************************************************************
 * Function: tlkmmi_phone_adaptInit
 * Descript: Initializes the adapter control parameters.
 * Params: None.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_adaptInit(uint08 procID)
{
	sTlkMmiPhoneProcID = procID;
	return tlksys_adapt_init(sTlkMmiPhoneProcID);
}

/******************************************************************************
 * Function: tlkmmi_phone_adaptHandler
 * Descript: Implement the function of traversing the scheduled task and work
 *           queue in the adapter to ensure the timeliness of execution.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_phone_adaptHandler(void)
{
	tlksys_adapt_handler(sTlkMmiPhoneProcID);
}

/******************************************************************************
 * Function: tlkmmi_phone_adaptInterval
 * Descript: Gets the execution time of the next task in the process.
 * Params: None.
 * Return: The execution time of the next task in the process.
 * Others: None.
*******************************************************************************/
uint tlkmmi_phone_adaptInterval(void)
{
	return tlksys_adapt_interval(sTlkMmiPhoneProcID);
}

/******************************************************************************
 * Function: tlkmmi_phone_adaptInitTimer, tlkmmi_phone_adaptDeinitTimer
 *           tlkmmi_phone_adaptInitQueue, tlkmmi_phone_adaptDeinitQueue
 * Descript: Initializes and deinitializes the timing scheduler and work queue.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 *     @userArg[IN]--Parameters passed in by the user will be returned on the call.
 *     @timeout[IN]--Scheduling interval set by the user. Unit:us.
 *     @timerCB[IN]--The callback interface after the time has arrived.
 *     @queueCB[IN]--The callback interface for the work to be executed.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_adaptInitTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout)
{
	return tlksys_adapt_initTimer(sTlkMmiPhoneProcID, pTimer, timerCB, userArg, timeout);
}
int tlkmmi_phone_adaptInitQueue(tlkapi_queue_t *pProcs, TlkApiQueueCB queueCB, uint32 userArg)
{
	return tlksys_adapt_initQueue(sTlkMmiPhoneProcID, pProcs, queueCB, userArg);
}
void tlkmmi_phone_adaptDeinitTimer(tlkapi_timer_t *pTimer)
{
	tlksys_adapt_deinitTimer(sTlkMmiPhoneProcID, pTimer);
}
void tlkmmi_phone_adaptDeinitQueue(tlkapi_queue_t *pProcs)
{
	tlksys_adapt_deinitQueue(sTlkMmiPhoneProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmmi_phone_adaptIsHaveTimer, tlkmmi_phone_adaptIsHaveQueue
 * Descript: Check whether scheduled tasks and cache work exist in the adaptation.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 * Return: Return TRUE if has, FALSE otherwise.
 * Others: None.
*******************************************************************************/
bool tlkmmi_phone_adaptIsHaveTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_isHaveTimer(sTlkMmiPhoneProcID, pTimer);
}
bool tlkmmi_phone_adaptIsHaveQueue(tlkapi_queue_t *pProcs)
{
	return tlksys_adapt_isHaveQueue(sTlkMmiPhoneProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmmi_phone_adaptAppendQueue, tlkmmi_phone_adaptRemoveQueue
 * Descript: Implement the function of adding and deleting work queues.
 * Params:
 *     @pQueue[IN]--Work queue node.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_adaptAppendQueue(tlkapi_queue_t *pProcs)
{	
	return tlksys_adapt_appendQueue(sTlkMmiPhoneProcID, pProcs);
}
int tlkmmi_phone_adaptRemoveQueue(tlkapi_queue_t *pProcs)
{
	return tlksys_adapt_removeQueue(sTlkMmiPhoneProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmmi_phone_adaptUpdateTimer, tlkmmi_phone_adaptInsertTimer,
 *           tlkmmi_phone_adaptRemoveTimer
 * Descript: Implement the function of adding, deleting and updating timer.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @isUpdate[IN]--True,Timer recount; False,Depending on the state of the
 *       other parameters, the timer may continue the previous counting logic.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_adaptUpdateTimer(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert)
{
	return tlksys_adapt_updateTimer(sTlkMmiPhoneProcID, pTimer, timeout, isInsert);
}
int tlkmmi_phone_adaptInsertTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_insertTimer(sTlkMmiPhoneProcID, pTimer, true);
}
int tlkmmi_phone_adaptRemoveTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_removeTimer(sTlkMmiPhoneProcID, pTimer);
}



#endif //#if (TLKMMI_PHONE_ENABLE)

