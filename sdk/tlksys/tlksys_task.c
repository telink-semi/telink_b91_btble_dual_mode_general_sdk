/********************************************************************************************************
 * @file     tlksys_task.c
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


static bool tlksys_task_isExist(const tlksys_funcs_t *pFuncs);
static tlksys_task_t *tlksys_tesk_getItem(uint08 index);
static tlksys_task_t *tlksys_tesk_getIdleItem(uint08 *pIndex);
static tlksys_task_t *tlksys_tesk_getUsedItemByID(uint08 taskID);


static tlksys_task_t sTlkSysTaskList[TLKSYS_TASK_MAX_NUMB] = {0};


/******************************************************************************
 * Function: tlksys_task_init
 * Descript: Initializes the system task list.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_task_init(void)
{
	tmemset(sTlkSysTaskList, 0, TLKSYS_TASK_MAX_NUMB*sizeof(tlksys_task_t));
	return TLK_ENONE;
}


/******************************************************************************
 * Function: tlksys_task_mount
 * Descript: Mount the specified task to the task list and obtain the task ID.
 * Params:
 *     @procID--Number of the process to be mounted.
 *     @taskID--The desired task ID of the user.
 *     @pFuncs--Interface for mounting the task.
 * Return: The positive value is the task ID, otherwise the mount fails.
 * Others:
 *     1.If taskID==0, it is allocated by the system;
 *     2.If taskID!=0, and the task ID is occupied, a failure is returned.
*******************************************************************************/
int tlksys_task_mount(uint08 procID, uint08 taskID, const tlksys_funcs_t *pFuncs)
{
	int ret;
	tlksys_task_t *pTask;

	if(pFuncs == nullptr || pFuncs->Init == nullptr) return -TLK_EPARAM;
	if(tlksys_task_isExist(pFuncs)) return -TLK_EEXIST;

	if(taskID == 0){
		pTask = tlksys_tesk_getIdleItem(&taskID);
	}else{
		taskID -= 1;
		pTask = tlksys_tesk_getItem(taskID);
		if(pTask != nullptr && pTask->pFuncs != nullptr) pTask = nullptr;
	}
	if(pTask == nullptr) return -TLK_EQUOTA;

	taskID += 1;
	pTask->pNext = nullptr;
	pTask->pFuncs = (tlksys_funcs_t*)pFuncs;
	ret = tlksys_proc_appendTask(procID, pTask);
	if(ret != TLK_ENONE){
		pTask->pNext  = nullptr;
		pTask->pFuncs = nullptr;
		return -TLK_EFAIL;
	}
	if(pFuncs->Init(procID, taskID) != TLK_ENONE){
		tlksys_proc_removeTask(procID, pTask);
		pTask->pNext  = nullptr;
		pTask->pFuncs = nullptr;
		return -TLK_EFAIL;
	}
	
	return taskID;
}
/******************************************************************************
 * Function: tlksys_task_unmount
 * Descript: Unmount the task from the task list.
 * Params:
 *     @taskID--ID of the task to be uninstalled.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_task_unmount(uint08 taskID)
{
	tlksys_task_t *pTask;
	
	pTask = tlksys_tesk_getUsedItemByID(taskID);
	if(pTask == nullptr) return -TLK_ENOOBJECT;
	
	tlksys_proc_removeTask(0, pTask);
	if(pTask->pFuncs->Close != nullptr){
		pTask->pFuncs->Close();
	}
	pTask->pNext  = nullptr;
	pTask->pFuncs = nullptr;
	return TLK_ENONE;
}
int tlksys_task_remove(tlksys_task_t *pTask)
{
	uint08 index;
	if(pTask == nullptr) return -TLK_EPARAM;
	for(index=0; index<TLKSYS_TASK_MAX_NUMB; index++){
		if(&sTlkSysTaskList[index] == pTask) break;
	}
	if(index == TLKSYS_TASK_MAX_NUMB) return nullptr;

	sTlkSysTaskList[index].pNext  = nullptr;
	sTlkSysTaskList[index].pFuncs = nullptr;
	if(pTask->pFuncs->Close != nullptr){
		pTask->pFuncs->Close();
	}
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_task_start
 * Descript: Start a system task.
 * Params:
 *     @taskID--ID of the task to be operated.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_task_start(uint08 taskID)
{
	tlksys_task_t *pTask = tlksys_tesk_getUsedItemByID(taskID);
	if(pTask == nullptr) return -TLK_EPARAM;
	if(pTask->pFuncs->Start == nullptr) return -TLK_ENOSUPPORT;
	return pTask->pFuncs->Start();
}

/******************************************************************************
 * Function: tlksys_task_start
 * Descript: Pause a system task.
 * Params:
 *     @taskID--ID of the task to be operated.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_task_pause(uint08 taskID)
{
	tlksys_task_t *pTask = tlksys_tesk_getUsedItemByID(taskID);
	if(pTask == nullptr) return -TLK_EPARAM;
	if(pTask->pFuncs->Pause == nullptr) return -TLK_ENOSUPPORT;
	return pTask->pFuncs->Pause();
}

/******************************************************************************
 * Function: tlksys_task_close
 * Descript: Close the system task.
 * Params:
 *     @taskID--ID of the task to be operated.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_task_close(uint08 taskID)
{
	tlksys_task_t *pTask = tlksys_tesk_getUsedItemByID(taskID);
	if(pTask == nullptr) return -TLK_EPARAM;
	if(pTask->pFuncs->Close == nullptr) return -TLK_ENOSUPPORT;
	return pTask->pFuncs->Close();
}

/******************************************************************************
 * Function: tlksys_task_input
 * Descript: Send a message to a task.
 * Params:
 *     @taskID--ID of the task to be operated.
 *     @mtype--The type of the message. 0-Inner msg, others for outer message.
 *     @msgID--The ID of the message under the message type.
 *     @pHead--Message header data.
 *     @headLen--Message header length.
 *     @pData--Message body data.
 *     @dataLen--Message body length.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others:
 *     1.For message interactions that are not in the same process, the "pHead"
 *       and HeadLen fields must be set to NULL.
*******************************************************************************/
int tlksys_task_input(uint08 taskID, uint08 mtype, uint16 msgID, 
	uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen) 
{
	tlksys_task_t *pTask = tlksys_tesk_getUsedItemByID(taskID);
	if(pTask == nullptr) return -TLK_EPARAM;
	if(pTask->pFuncs->Input == nullptr) return -TLK_ENOSUPPORT;
	return pTask->pFuncs->Input(mtype, msgID, pHead, headLen, pData, dataLen);
}

/******************************************************************************
 * Function: tlksys_task_handler
 * Descript: Loop through the requirements in the task.
 * Params:
 *     @taskID--ID of the task to be operated.
 * Return: None.
 * Others: 
 *     1.Do not use the modified interface unless necessary. The modified 
 *       interface will be discarded later.
 *     2.Users can replace the interface with scheduled and queued tasks in 
 *       the adapter.
*******************************************************************************/
void tlksys_task_handler(uint08 taskID)
{
	tlksys_task_t *pTask = tlksys_tesk_getUsedItemByID(taskID);
	if(pTask == nullptr || pTask->pFuncs->Handler == nullptr) return;
	pTask->pFuncs->Handler();
}



static bool tlksys_task_isExist(const tlksys_funcs_t *pFuncs)
{
	uint08 index;
	if(pFuncs == nullptr) return false;
	for(index=0; index<TLKSYS_TASK_MAX_NUMB; index++){
		if(sTlkSysTaskList[index].pFuncs != nullptr && sTlkSysTaskList[index].pFuncs == (tlksys_funcs_t*)pFuncs){
			break;
		}
	}
	if(index == TLKSYS_TASK_MAX_NUMB) return false;
	return true;
}

static tlksys_task_t *tlksys_tesk_getItem(uint08 index)
{
	if(index >= TLKSYS_TASK_MAX_NUMB) return nullptr;
	else return &sTlkSysTaskList[index];
}
static tlksys_task_t *tlksys_tesk_getIdleItem(uint08 *pIndex)
{
	uint08 index;
	for(index=0; index<TLKSYS_TASK_MAX_NUMB; index++){
		if(sTlkSysTaskList[index].pFuncs == nullptr) break;
	}
	if(index == TLKSYS_TASK_MAX_NUMB) return nullptr;
	if(pIndex != nullptr) *pIndex = index;
	return &sTlkSysTaskList[index];
}
static tlksys_task_t *tlksys_tesk_getUsedItemByID(uint08 taskID)
{
	if(taskID == 0 || taskID > TLKSYS_TASK_MAX_NUMB) return nullptr;
	if(sTlkSysTaskList[taskID-1].pFuncs == nullptr) return nullptr;
	return &sTlkSysTaskList[taskID-1];
}

#endif //#if (TLK_CFG_SYS_ENABLE)

