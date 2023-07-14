/********************************************************************************************************
 * @file	tlkapi_adapt.h
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
#ifndef TLKAPI_ADAPT_H
#define TLKAPI_ADAPT_H



#define TLKAPI_TIMEOUT_MAX      (0x03FFFFFF)// BIT(31) >> 4
#define TLKAPI_TIMEOUT_MIN      50  //unit: us



typedef struct tlkapi_timer_s  tlkapi_timer_t;
typedef struct tlkapi_queue_s  tlkapi_queue_t;


/******************************************************************************
 * Function: TlkApiQueueCB, TlkApiTimerCB
 * Descript: Defines the format of the user callback interface.
 * Params:
 *     @pQueue[IN]--The node of Process Control.
 *     @pTimer[IN]--The node of Timer Control.
 *     @pUsrArg[IN]--User Argument.
 * Return: Returning TRUE will automatically start the next round of detection.
 *     If FALSE is returned, the node is automatically removed.
*******************************************************************************/
typedef bool(*TlkApiQueueCB)(tlkapi_queue_t *pQueue, uint32 userArg);
typedef bool(*TlkApiTimerCB)(tlkapi_timer_t *pTimer, uint32 userArg);


struct tlkapi_timer_s{
	uint32 arrival;
	uint32 timeout;
	uint32 userArg;
	TlkApiTimerCB timerCB;
	struct tlkapi_timer_s *pNext;
};
struct tlkapi_queue_s{
	uint32 userArg;
	TlkApiQueueCB queueCB;
	struct tlkapi_queue_s *pNext;
};
typedef struct{
	uint08 isUpdate;
	uint08 reserve0;
	uint16 reserve1;
	tlkapi_queue_t *pQueueList; //Circulation list
	tlkapi_timer_t *pTimerList; //Singly linked list
}tlkapi_adapt_t;



/******************************************************************************
 * Function: tlkapi_adapt_init
 * Descript: Initializes the adapter control parameters.
 * Params:
 *     @pAdapt[IN]--Responsible for integrated management of adapter nodes.
 * Return: Operating results. LSLP_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkapi_adapt_init(tlkapi_adapt_t *pAdapt);

/******************************************************************************
 * Function: tlkapi_adapt_clear
 * Descript: CLear the adapter.
 * Params:
 *     @pAdapt[IN]--Responsible for integrated management of adapter nodes.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapi_adapt_clear(tlkapi_adapt_t *pAdapt);

/******************************************************************************
 * Function: tlkapi_adapt_handler
 * Descript: Implement the function of traversing the scheduled task and work
 *           queue in the adapter to ensure the timeliness of execution.
 * Params:
 *     @pAdapt[IN]--Responsible for integrated management of adapter nodes.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapi_adapt_handler(tlkapi_adapt_t *pAdapt);

/******************************************************************************
 * Function: tlkapi_adapt_interval
 * Descript: Gets the execution time of the next task in the adapter.
 * Params:
 *     @pAdapt[IN]--Responsible for integrated management of adapter nodes.
 * Return: The execution time of the next task in the process.
 * Others: None.
*******************************************************************************/
uint tlkapi_adapt_interval(tlkapi_adapt_t *pAdapt);

/******************************************************************************
 * Function: tlkapi_adapt_initTimer, tlkapi_adapt_deinitTimer
 *           tlkapi_adapt_initQueue, tlkapi_adapt_deinitTimer
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
int  tlkapi_adapt_initTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout);
int  tlkapi_adapt_initQueue(tlkapi_queue_t *pQueue, TlkApiQueueCB queueCB, uint32 userArg);
void tlkapi_adapt_deinitTimer(tlkapi_adapt_t *pAdapt, tlkapi_timer_t *pTimer);
void tlkapi_adapt_deinitQueue(tlkapi_adapt_t *pAdapt, tlkapi_queue_t *pQueue);

/******************************************************************************
 * Function: tlkapi_adapt_isHaveTimer, tlkapi_adapt_isHaveQueue
 * Descript: Check whether scheduled tasks and cache work exist in the adaptation.
 * Params:
 *     @pAdapt[IN]--Responsible for integrated management of adapter nodes.
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 * Return: Return TRUE if has, FALSE otherwise.
 * Others: None.
*******************************************************************************/
bool tlkapi_adapt_isHaveTimer(tlkapi_adapt_t *pAdapt, tlkapi_timer_t *pTimer);
bool tlkapi_adapt_isHaveQueue(tlkapi_adapt_t *pAdapt, tlkapi_queue_t *pQueue);

/******************************************************************************
 * Function: tlkapi_adapt_appendQueue, tlkapi_adapt_removeQueue
 * Descript: Implement the function of adding and deleting work queues.
 * Params:
 *     @pAdapt[IN]--Responsible for integrated management of adapter nodes.
 *     @pQueue[IN]--Work queue node.
 * Return: Operating results. LSLP_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkapi_adapt_appendQueue(tlkapi_adapt_t *pAdapt, tlkapi_queue_t *pQueue);
int  tlkapi_adapt_removeQueue(tlkapi_adapt_t *pAdapt, tlkapi_queue_t *pQueue);

/******************************************************************************
 * Function: tlkapi_adapt_updateTimer, tlkapi_adapt_insertTimer,
 *           tlkapi_adapt_removeTimer
 * Descript: Implement the function of adding, deleting and updating timer.
 * Params:
 *     @pAdapt[IN]--Responsible for integrated management of adapter nodes.
 *     @pTimer[IN]--Timer scheduling node.
 *     @isUpdate[IN]--True,Timer recount; False,Depending on the state of the
 *       other parameters, the timer may continue the previous counting logic.
 * Return: Operating results. LSLP_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkapi_adapt_updateTimer(tlkapi_adapt_t *pAdapt, tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert);
int  tlkapi_adapt_insertTimer(tlkapi_adapt_t *pAdapt, tlkapi_timer_t *pTimer, bool isUpdate);
int  tlkapi_adapt_removeTimer(tlkapi_adapt_t *pAdapt, tlkapi_timer_t *pTimer);

/******************************************************************************
 * Function: tlkapi_adapt_timerInterval
 * Descript: Gets the execution time of the next task in the process.
 * Params:
 *     @adapt[IN]--Responsible for integrated management of adapter nodes.
 * Return: The execution time of the next task in the process.
 * Others: None.
*******************************************************************************/
uint tlkapi_adapt_timerInterval(tlkapi_adapt_t *pAdapt);

/******************************************************************************
 * Function: tlkapi_timer_take_first_timeout
 * Descript: Retrieves the first timer to arrive in the adapter.
 * Params:
 *     @pAdapt[IN]--Responsible for integrated management of adapter nodes.
 * Return: The first timer to arrive.
 * Others: None.
*******************************************************************************/
tlkapi_timer_t *tlkapi_adapt_takeFirstTimer(tlkapi_adapt_t *pAdapt);



#endif //TLKAPI_ADAPT_H

