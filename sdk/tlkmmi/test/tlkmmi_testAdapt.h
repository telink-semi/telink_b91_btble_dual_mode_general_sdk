/********************************************************************************************************
 * @file	tlkmmi_testAdapt.h
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
#ifndef TLKMMI_TEST_ADAPT_H
#define TLKMMI_TEST_ADAPT_H

#if (TLKMMI_TEST_ENABLE)



/******************************************************************************
 * Function: tlkmmi_test_adaptInit
 * Descript: Initializes the adapter control parameters.
 * Params: None.
 * Return: Operating results. LSLP_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkmmi_test_adaptInit(uint08 procID);

/******************************************************************************
 * Function: tlkmmi_test_adaptHandler
 * Descript: Implement the function of traversing the scheduled task and work
 *           queue in the adapter to ensure the timeliness of execution.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_test_adaptHandler(void);

/******************************************************************************
 * Function: tlkmmi_test_adaptInterval
 * Descript: Gets the execution time of the next task in the process.
 * Params: None.
 * Return: The execution time of the next task in the process.
 * Others: None.
*******************************************************************************/
uint tlkmmi_test_adaptInterval(void);

/******************************************************************************
 * Function: tlkmmi_test_adaptInitTimer, tlkmmi_test_adaptDeinitTimer
 *           tlkmmi_test_adaptInitQueue, tlkmmi_test_adaptDeinitQueue
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
int  tlkmmi_test_adaptInitTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout);
int  tlkmmi_test_adaptInitQueue(tlkapi_queue_t *pProcs, TlkApiQueueCB queueCB, uint32 userArg);
void tlkmmi_test_adaptDeinitTimer(tlkapi_timer_t *pTimer);
void tlkmmi_test_adaptDeinitQueue(tlkapi_queue_t *pProcs);

/******************************************************************************
 * Function: tlkmmi_test_adaptIsHaveTimer, tlkmmi_test_adaptIsHaveQueue
 * Descript: Check whether scheduled tasks and cache work exist in the adaptation.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 * Return: Return TRUE if has, FALSE otherwise.
 * Others: None.
*******************************************************************************/
bool tlkmmi_test_adaptIsHaveTimer(tlkapi_timer_t *pTimer);
bool tlkmmi_test_adaptIsHaveQueue(tlkapi_queue_t *pProcs);

/******************************************************************************
 * Function: tlkmmi_test_adaptAppendQueue, tlkmmi_test_adaptRemoveQueue
 * Descript: Implement the function of adding and deleting work queues.
 * Params:
 *     @pQueue[IN]--Work queue node.
 * Return: Operating results. LSLP_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkmmi_test_adaptAppendQueue(tlkapi_queue_t *pProcs);
int  tlkmmi_test_adaptRemoveQueue(tlkapi_queue_t *pProcs);

/******************************************************************************
 * Function: tlkmmi_test_adaptUpdateTimer, tlkmmi_test_adaptInsertTimer,
 *           tlkmmi_test_adaptRemoveTimer
 * Descript: Implement the function of adding, deleting and updating timer.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @isUpdate[IN]--True,Timer recount; False,Depending on the state of the
 *       other parameters, the timer may continue the previous counting logic.
 * Return: Operating results. LSLP_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkmmi_test_adaptUpdateTimer(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert);
int  tlkmmi_test_adaptInsertTimer(tlkapi_timer_t *pTimer);
int  tlkmmi_test_adaptRemoveTimer(tlkapi_timer_t *pTimer);




#endif //#if (TLKMMI_TEST_ENABLE)

#endif //TLKMMI_TEST_ADAPT_H

