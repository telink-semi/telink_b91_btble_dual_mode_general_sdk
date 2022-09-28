/********************************************************************************************************
 * @file     tlkos_task.c
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
#if (TLK_CFG_OS_ENABLE)
#include "tlklib/os/tlkos_stdio.h"
#include "tlklib/os/tlkos_task.h"
#if (TLK_OS_FREERTOS_ENABLE)
#include "tlklib/os/freertos/include/FreeRTOS.h"
#include "tlklib/os/freertos/include/task.h"
#endif


/******************************************************************************
 * Function: tlkos_task_create
 * Descript: Creating a Task.
 * Params:
 *     @pTask[OUT]--Task control structure.
 *     @pName[IN]--The Name of the task to be created.
 *     @priority[IN]--The priority of the task to be created.
 *     @stackSize[IN]--The stack size of the task to be created.
 *     @enter[IN]--The callback function of the task to be created.
 *     @pUsrArg[IN]--The user argument of the task to be created.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
 * Others: pTask must not be nullptr.
*******************************************************************************/
int tlkos_task_create(tlkos_task_t *pTask, char *pName, uint08 priority, uint16 stackSize, 
	TlkOsTaskEnterCB enter, void *pUsrArg)
{
	if(pTask == nullptr || stackSize == 0 || enter == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	ulong handle = 0;
	BaseType_t ret;
	ret = xTaskCreate((TaskFunction_t)enter, pName, stackSize, pUsrArg, priority, (TaskHandle_t*)&handle);
	if(ret != pdPASS || handle == 0) return -TLK_EFAIL;
	pTask->handle = handle;
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_task_destory
 * Descript: Destory a Task.
 * Params:
 *     @pTask[IN]--Task control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_task_destory(tlkos_task_t *pTask)
{
	if(pTask == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTask->handle == 0) return -TLK_ENOREADY;
	vTaskDelete((TaskHandle_t)(pTask->handle));
	pTask->handle = 0;
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_task_suspend
 * Descript: Suspend a Task.
 * Params:
 *     @pTask[IN]--Task control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_task_suspend(tlkos_task_t *pTask)
{
	if(pTask == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTask->handle == 0) return -TLK_ENOREADY;
	vTaskSuspend((TaskHandle_t)(pTask->handle));
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_task_suspend
 * Descript: Resume a Task.
 * Params:
 *     @pTask[OUT]--Task control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_task_resume(tlkos_task_t *pTask)
{
	if(pTask == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTask->handle == 0) return -TLK_ENOREADY;
	vTaskResume((TaskHandle_t)(pTask->handle));
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_task_suspend
 * Descript: Suspend all Task.
 * Params: None.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_task_suspendAll(void)
{
#if (TLK_OS_FREERTOS_ENABLE)
	vTaskSuspendAll();
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif

}

/******************************************************************************
 * Function: tlkos_task_resumeAll
 * Descript: Resume all Task.
 * Params: None.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_task_resumeAll(void)
{
#if (TLK_OS_FREERTOS_ENABLE)
	xTaskResumeAll();
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_task_getState
 * Descript: Gets the current status of the task.
 * Params:
 *     @pTask[IN]--Task control structure.
 * Return: Return the current status of the task, refer to TLKOS_TASK_STATE_ENUM.
*******************************************************************************/
uint tlkos_task_getState(tlkos_task_t *pTask)
{
	if(pTask == nullptr) return TLKOS_TASK_STATE_NONE;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTask->handle == 0) return TLKOS_TASK_STATE_NONE;
	eTaskState state = eTaskGetState((const TaskHandle_t)(pTask->handle));
	if(state == eRunning) return TLKOS_TASK_STATE_RUNNING;
    else if(state == eReady) return TLKOS_TASK_STATE_READY;
    else if(state == eBlocked) return TLKOS_TASK_STATE_BLOCK;
    else if(state == eSuspended) return TLKOS_TASK_STATE_SUSPEND;
    else if(state == eDeleted) return TLKOS_TASK_STATE_DELETED;
    else if(state == eInvalid) return TLKOS_TASK_STATE_INVALID;
	else return TLKOS_TASK_STATE_NONE;
#else
	return -TLK_ENOSUPPORT;
#endif		
}

/******************************************************************************
 * Function: tlkos_task_getState
 * Descript: Gets the current priority of the task.
 * Params:
 *     @pTask[IN]--Task control structure.
 * Return: Return the current priority of the task.
*******************************************************************************/
uint tlkos_task_getPriority(tlkos_task_t *pTask)
{
	if(pTask == nullptr) return 0;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTask->handle == 0) return 0;
	return uxTaskPriorityGet((const TaskHandle_t)(pTask->handle));
#else
	return -TLK_ENOSUPPORT;
#endif	
}

/******************************************************************************
 * Function: tlkos_task_getState
 * Descript: Sets the priority of the task.
 * Params:
 *     @pTask[IN]--Task control structure.
 *     @priority[IN]--The priority of the task to be set.
 * Return: None.
*******************************************************************************/
void tlkos_task_setPriority(tlkos_task_t *pTask, uint priority)
{
	if(pTask == nullptr) return;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTask->handle == 0) return;
	vTaskPrioritySet((TaskHandle_t)(pTask->handle), priority);
	return;
#else
	return;
#endif	
}



#endif //#if (TLK_CFG_OS_ENABLE)

