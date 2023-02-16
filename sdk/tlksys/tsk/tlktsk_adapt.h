/********************************************************************************************************
 * @file     tlktsk_adapt.h
 *
 * @brief    This is the header file for BTBLE SDK
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
#ifndef TLKTSK_ADAPT_H
#define TLKTSK_ADAPT_H



/******************************************************************************
 * Function: tlktsk_adapt_init
 * Descript: Initializes the adapter control parameters.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int  tlktsk_adapt_init(uint08 procID);

/******************************************************************************
 * Function: tlktsk_adapt_run_once
 * Descript: Implement the function of traversing the scheduled task and work
 *           queue in the adapter to ensure the timeliness of execution.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlktsk_adapt_handler(uint08 procID);

/******************************************************************************
 * Function: tlktsk_adapt_interval
 * Descript: Gets the execution time of the next task in the process.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 * Return: The execution time of the next task in the process.
 * Others: None.
*******************************************************************************/
uint tlktsk_adapt_interval(uint08 procID);

/******************************************************************************
 * Function: tlktsk_adapt_initTimer, tlktsk_adapt_deinitTimer
 *           tlktsk_adapt_initQueue, tlktsk_adapt_deinitTimer
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
int  tlktsk_adapt_initTimer(uint08 procID, tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout);
int  tlktsk_adapt_initQueue(uint08 procID, tlkapi_queue_t *pQueue, TlkApiQueueCB queueCB, uint32 userArg);
void tlktsk_adapt_deinitTimer(uint08 procID, tlkapi_timer_t *pTimer);
void tlktsk_adapt_deinitQueue(uint08 procID, tlkapi_queue_t *pQueue);

/******************************************************************************
 * Function: tlktsk_adapt_isHaveTimer, tlktsk_adapt_isHaveQueue
 * Descript: Check whether scheduled tasks and cache work exist in the adaptation.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 * Return: Return TRUE if has, FALSE otherwise.
 * Others: None.
*******************************************************************************/
bool tlktsk_adapt_isHaveTimer(uint08 procID, tlkapi_timer_t *pTimer);
bool tlktsk_adapt_isHaveQueue(uint08 procID, tlkapi_queue_t *pQueue);

/******************************************************************************
 * Function: tlktsk_adapt_appendQueue, tlktsk_adapt_removeQueue
 * Descript: Implement the function of adding and deleting work queues.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 *     @pQueue[IN]--Work queue node.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int  tlktsk_adapt_appendQueue(uint08 procID, tlkapi_queue_t *pQueue);
int  tlktsk_adapt_removeQueue(uint08 procID, tlkapi_queue_t *pQueue);

/******************************************************************************
 * Function: tlktsk_adapt_updateTimer, tlktsk_adapt_insertTimer,
 *           tlktsk_adapt_removeTimer
 * Descript: Implement the function of adding, deleting and updating timer.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 *     @pTimer[IN]--Timer scheduling node.
 *     @isUpdate[IN]--True,Timer recount; False,Depending on the state of the
 *       other parameters, the timer may continue the previous counting logic.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int  tlktsk_adapt_updateTimer(uint08 procID, tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert);
int  tlktsk_adapt_insertTimer(uint08 procID, tlkapi_timer_t *pTimer, bool isUpdate);
int  tlktsk_adapt_removeTimer(uint08 procID, tlkapi_timer_t *pTimer);




#endif //TLKTSK_ADAPT_H

