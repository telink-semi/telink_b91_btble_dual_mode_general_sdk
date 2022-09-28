/********************************************************************************************************
 * @file     tlkos_task.h
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
#ifndef TLKOS_TASK_H
#define TLKOS_TASK_H

#if (TLK_CFG_OS_ENABLE)


typedef void(*TlkOsTaskEnterCB)(void *pUsrArg);

typedef enum{
	TLKOS_TASK_STATE_NONE = 0,
	TLKOS_TASK_STATE_READY, /* The task being queried is in a read or pending ready list. */
	TLKOS_TASK_STATE_BLOCK, /* The task being queried is in the Blocked state. */
	TLKOS_TASK_STATE_RUNNING, /* A task is querying the state of itself, so must be running. */
	TLKOS_TASK_STATE_SUSPEND, /* The task being queried is in the Suspended state, or is in the Blocked state with an infinite time out. */
	TLKOS_TASK_STATE_DELETED, /* The task being queried has been deleted, but its TCB has not yet been freed. */
	TLKOS_TASK_STATE_INVALID, /* Used as an 'invalid state' value. */
}TLKOS_TASK_STATE_ENUM;



typedef struct{
	ulong handle;
}tlkos_task_t;



/******************************************************************************
 * Function: tlkos_task_create
 * Descript: Creating a Task.
 * Params:
 *     @pTask[OUT]--Task control structure.
 *     @pName[IN]--The Name of the task to be created.
 *     @priority[IN]--The priority of the task to be created.
 *     @stackSize[IN]--The stack size of the task to be created.
 *     @enter[IN]--The enter callback of the task to be created.
 *     @pUsrArg[IN]--The user argument of the task to be created.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
 * Others: pTask must not be nullptr.
*******************************************************************************/
int tlkos_task_create(tlkos_task_t *pTask, char *pName, uint08 priority, uint16 stackSize, 
	TlkOsTaskEnterCB enter, void *pUsrArg);

/******************************************************************************
 * Function: tlkos_task_destory
 * Descript: Destory a Task.
 * Params:
 *     @pTask[IN]--Task control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_task_destory(tlkos_task_t *pTask);

/******************************************************************************
 * Function: tlkos_task_suspend
 * Descript: Suspend a Task.
 * Params:
 *     @pTask[IN]--Task control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_task_suspend(tlkos_task_t *pTask);

/******************************************************************************
 * Function: tlkos_task_suspend
 * Descript: Resume a Task.
 * Params:
 *     @pTask[OUT]--Task control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_task_resume(tlkos_task_t *pTask);

/******************************************************************************
 * Function: tlkos_task_suspend
 * Descript: Suspend all Task.
 * Params: None.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_task_suspendAll(void);

/******************************************************************************
 * Function: tlkos_task_resumeAll
 * Descript: Resume all Task.
 * Params: None.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_task_resumeAll(void);

/******************************************************************************
 * Function: tlkos_task_getState
 * Descript: Gets the current status of the task.
 * Params:
 *     @pTask[IN]--Task control structure.
 * Return: Return the current status of the task, refer to TLKOS_TASK_STATE_ENUM.
*******************************************************************************/
uint tlkos_task_getState(tlkos_task_t *pTask);

/******************************************************************************
 * Function: tlkos_task_getState
 * Descript: Gets the current priority of the task.
 * Params:
 *     @pTask[IN]--Task control structure.
 * Return: Return the current priority of the task.
*******************************************************************************/
uint tlkos_task_getPriority(tlkos_task_t *pTask);

/******************************************************************************
 * Function: tlkos_task_getState
 * Descript: Sets the priority of the task.
 * Params:
 *     @pTask[IN]--Task control structure.
 *     @priority[IN]--The priority of the task to be set.
 * Return: None.
*******************************************************************************/
void tlkos_task_setPriority(tlkos_task_t *pTask, uint priority);


#endif //#if (TLK_CFG_OS_ENABLE)

#endif //TLKOS_TASK_H

