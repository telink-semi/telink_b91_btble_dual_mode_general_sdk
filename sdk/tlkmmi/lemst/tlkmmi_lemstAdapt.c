/********************************************************************************************************
 * @file	tlkmmi_lemstAdapt.c
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
#include "tlkmmi_lemst.h"
#include "tlkmmi_lemstAdapt.h"
#include "tlksys/tlksys_stdio.h"


static uint08 sTlkMmiLemstProcID = 0;


/******************************************************************************
 * Function: tlkmmi_lemst_adaptInit
 * Descript: Initializes the adapter control parameters.
 * Params: None.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkmmi_lemst_adaptInit(uint08 procID)
{
	sTlkMmiLemstProcID = procID;
	return tlksys_adapt_init(sTlkMmiLemstProcID);
}

/******************************************************************************
 * Function: tlkmmi_lemst_adaptHandler
 * Descript: Implement the function of traversing the scheduled task and work
 *           queue in the adapter to ensure the timeliness of execution.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_lemst_adaptHandler(void)
{
	tlksys_adapt_handler(sTlkMmiLemstProcID);
}

/******************************************************************************
 * Function: tlkmmi_lemst_adaptInterval
 * Descript: Gets the execution time of the next task in the process.
 * Params: None.
 * Return: The execution time of the next task in the process.
 * Others: None.
*******************************************************************************/
uint tlkmmi_lemst_adaptInterval(void)
{
	return tlksys_adapt_interval(sTlkMmiLemstProcID);
}

/******************************************************************************
 * Function: tlkmmi_lemst_adaptInitTimer, tlkmmi_lemst_adaptDeinitTimer
 *           tlkmmi_lemst_adaptInitQueue, tlkmmi_lemst_adaptDeinitQueue
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
int tlkmmi_lemst_adaptInitTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout)
{
	return tlksys_adapt_initTimer(sTlkMmiLemstProcID, pTimer, timerCB, userArg, timeout);
}
int tlkmmi_lemst_adaptInitQueue(tlkapi_queue_t *pProcs, TlkApiQueueCB queueCB, uint32 userArg)
{
	return tlksys_adapt_initQueue(sTlkMmiLemstProcID, pProcs, queueCB, userArg);
}
void tlkmmi_lemst_adaptDeinitTimer(tlkapi_timer_t *pTimer)
{
	tlksys_adapt_deinitTimer(sTlkMmiLemstProcID, pTimer);
}
void tlkmmi_lemst_adaptDeinitQueue(tlkapi_queue_t *pProcs)
{
	tlksys_adapt_deinitQueue(sTlkMmiLemstProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmmi_lemst_adaptIsHaveTimer, tlkmmi_lemst_adaptIsHaveQueue
 * Descript: Check whether scheduled tasks and cache work exist in the adaptation.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 * Return: Return TRUE if has, FALSE otherwise.
 * Others: None.
*******************************************************************************/
bool tlkmmi_lemst_adaptIsHaveTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_isHaveTimer(sTlkMmiLemstProcID, pTimer);
}
bool tlkmmi_lemst_adaptIsHaveQueue(tlkapi_queue_t *pProcs)
{
	return tlksys_adapt_isHaveQueue(sTlkMmiLemstProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmmi_lemst_adaptAppendQueue, tlkmmi_lemst_adaptRemoveQueue
 * Descript: Implement the function of adding and deleting work queues.
 * Params:
 *     @pQueue[IN]--Work queue node.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkmmi_lemst_adaptAppendQueue(tlkapi_queue_t *pProcs)
{	
	return tlksys_adapt_appendQueue(sTlkMmiLemstProcID, pProcs);
}
int tlkmmi_lemst_adaptRemoveQueue(tlkapi_queue_t *pProcs)
{
	return tlksys_adapt_removeQueue(sTlkMmiLemstProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmmi_lemst_adaptUpdateTimer, tlkmmi_lemst_adaptInsertTimer,
 *           tlkmmi_lemst_adaptRemoveTimer
 * Descript: Implement the function of adding, deleting and updating timer.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @isUpdate[IN]--True,Timer recount; False,Depending on the state of the
 *       other parameters, the timer may continue the previous counting logic.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkmmi_lemst_adaptUpdateTimer(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert)
{
	return tlksys_adapt_updateTimer(sTlkMmiLemstProcID, pTimer, timeout, isInsert);
}
int tlkmmi_lemst_adaptInsertTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_insertTimer(sTlkMmiLemstProcID, pTimer, true);
}
int tlkmmi_lemst_adaptRemoveTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_removeTimer(sTlkMmiLemstProcID, pTimer);
}


#endif //#if (TLKMMI_LEMST_ENABLE)

