/********************************************************************************************************
 * @file	tlkmmi_sysAdapt.h
 *
 * @brief	This is the header file for BTBLE SDK
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
#ifndef TLKMMI_ADAPT_H
#define TLKMMI_ADAPT_H

#if (TLKMMI_SYSTEM_ENABLE)



/******************************************************************************
 * Function: tlkmmi_sys_adaptInit
 * Descript: Initializes the adapter control parameters.
 * Params: None.
 * Return: Operating results. LSLP_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkmmi_sys_adaptInit(uint08 procID);

/******************************************************************************
 * Function: tlkmmi_sys_adaptHandler
 * Descript: Implement the function of traversing the scheduled task and work
 *           queue in the adapter to ensure the timeliness of execution.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_sys_adaptHandler(void);

/******************************************************************************
 * Function: tlkmmi_sys_adaptInterval
 * Descript: Gets the execution time of the next task in the process.
 * Params: None.
 * Return: The execution time of the next task in the process.
 * Others: None.
*******************************************************************************/
uint tlkmmi_sys_adaptInterval(void);

/******************************************************************************
 * Function: tlkmmi_sys_adaptInitTimer, tlkmmi_sys_adaptDeinitTimer
 *           tlkmmi_sys_adaptInitQueue, tlkmmi_sys_adaptDeinitQueue
 * Descript: Initializes and deinitializes the timing scheduler and work queue.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 *     @userArg[IN]--Parameters passed in by the user will be returned on the call.
 *     @timeout[IN]--Scheduling interval set by the user. Unit:us.
 *     @timerCB[IN]--The callback interface after the time has arrived.
 *     @queueCB[IN]--The callback interface for the work to be executed.
 * Return: Operating results. LSLP_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkmmi_sys_adaptInitTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout);
int  tlkmmi_sys_adaptInitQueue(tlkapi_queue_t *pProcs, TlkApiQueueCB queueCB, uint32 userArg);
void tlkmmi_sys_adaptDeinitTimer(tlkapi_timer_t *pTimer);
void tlkmmi_sys_adaptDeinitQueue(tlkapi_queue_t *pProcs);

/******************************************************************************
 * Function: tlkmmi_sys_adaptIsHaveTimer, tlkmmi_sys_adaptIsHaveQueue
 * Descript: Check whether scheduled tasks and cache work exist in the adaptation.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 * Return: Return TRUE if has, FALSE otherwise.
 * Others: None.
*******************************************************************************/
bool tlkmmi_sys_adaptIsHaveTimer(tlkapi_timer_t *pTimer);
bool tlkmmi_sys_adaptIsHaveQueue(tlkapi_queue_t *pProcs);

/******************************************************************************
 * Function: tlkmmi_sys_adaptAppendQueue, tlkmmi_sys_adaptRemoveQueue
 * Descript: Implement the function of adding and deleting work queues.
 * Params:
 *     @pQueue[IN]--Work queue node.
 * Return: Operating results. LSLP_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkmmi_sys_adaptAppendQueue(tlkapi_queue_t *pProcs);
int  tlkmmi_sys_adaptRemoveQueue(tlkapi_queue_t *pProcs);

/******************************************************************************
 * Function: tlkmmi_sys_adaptUpdateTimer, tlkmmi_sys_adaptInsertTimer,
 *           tlkmmi_sys_adaptRemoveTimer
 * Descript: Implement the function of adding, deleting and updating timer.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @isUpdate[IN]--True,Timer recount; False,Depending on the state of the
 *       other parameters, the timer may continue the previous counting logic.
 * Return: Operating results. LSLP_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkmmi_sys_adaptUpdateTimer(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert);
int  tlkmmi_sys_adaptInsertTimer(tlkapi_timer_t *pTimer);
int  tlkmmi_sys_adaptRemoveTimer(tlkapi_timer_t *pTimer);




#endif //#if (TLKMMI_SYSTEM_ENABLE)

#endif //TLKMMI_ADAPT_H

