/********************************************************************************************************
 * @file	tlksys_proc.c
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
#include "tlksys_mutex.h"
#include "tlksys_proc.h"
#include "tlksys.h"


static tlksys_proc_t *tlksys_proc_getNode(uint08 index);
static tlksys_proc_t *tlksys_proc_getIdleNode(uint08 *pIndex);
static tlksys_proc_t *tlksys_proc_getUsedNode(uint08 index);
static tlksys_proc_t *tlksys_proc_getUsedNodeByID(uint08 procID);
static tlksys_proc_t *tlksys_proc_getUsedNodeByTask(tlksys_task_t *pTask);


static tlksys_proc_t sTlkSysProcList[TLKSYS_PROC_MAX_NUMB];

/******************************************************************************
 * Function: tlksys_task_init
 * Descript: Initializes the system process list.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_proc_init(void)
{
	tmemset(sTlkSysProcList, 0, TLKSYS_PROC_MAX_NUMB*sizeof(tlksys_proc_t));
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_proc_appendTask
 * Descript: Inserts a task into the specified process.
 * Params:
 *     @procID--ID of the process to be operated.
 *     @pTask--The task to be inserted.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_proc_appendTask(uint08 procID, tlksys_task_t *pTask)
{
	tlksys_proc_t *pProcs;

	if(procID == 0 || pTask == nullptr || pTask->pNext != nullptr){
		return -TLK_EPARAM;
	}
	
	pProcs = tlksys_proc_getUsedNode(procID-1);
	if(pProcs == nullptr) return -TLK_EPARAM;
	
	if(pProcs->pTaskList == nullptr){
		pProcs->pTaskList = pTask;
	}else{
		tlksys_task_t *pTemp = pProcs->pTaskList;
		while(pTemp->pNext != nullptr){
			pTemp = pTemp->pNext;
		}
		pTemp->pNext = pTask;
	}
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_proc_appendTask
 * Descript: Remove a task from the specified process.
 * Params:
 *     @procID--ID of the process to be operated.
 *     @pTask--The task to be removed.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: 
 *     1.If procID==0, the system will find the corresponding task from all
 *       processes and remove it.
*******************************************************************************/
int tlksys_proc_removeTask(uint08 procID, tlksys_task_t *pTask)
{
	tlksys_proc_t *pProcs;

	if(pTask == nullptr) return -TLK_EPARAM;
	if(procID == 0){
		pProcs = tlksys_proc_getUsedNodeByTask(pTask);
	}else{
		pProcs = tlksys_proc_getUsedNode(procID);
	}
	if(pProcs == nullptr || pProcs->pTaskList == nullptr){
		return -TLK_ENOOBJECT;
	}
	
	if(pProcs->pTaskList == pTask){
		pProcs->pTaskList = pProcs->pTaskList->pNext;
	}else{
		tlksys_task_t *pTemp = pProcs->pTaskList;
		while(pTemp->pNext != nullptr){
			if(pTemp->pNext == pTask) break;
			pTemp = pTemp->pNext;
		}
		if(pTemp->pNext == nullptr) return -TLK_ENOOBJECT;
		pTemp->pNext = pTemp->pNext->pNext;
	}
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_proc_mount
 * Descript: Mount the process to the system.
 * Params:
 *     @procID--Number of the process to be mounted.
 *     @priority--The execution priority of the process.
 *     @stackSize--The size of the stack occupied by process execution.
 * Return: The positive value is the proc ID, otherwise the mount fails.
 * Others:
 *     1.If procID==0, it is allocated by the system;
 *     2.If procID!=0, and the proc ID is occupied, a failure is returned.
*******************************************************************************/
int tlksys_proc_mount(uint08 procID, uint08 priority, uint16 stackSize)
{
	tlksys_proc_t *pProc;

	if(procID == 0){
		pProc = tlksys_proc_getIdleNode(&procID);
	}else{
		procID -= 1;
		pProc = tlksys_proc_getNode(procID);
		if(pProc == nullptr || pProc->isUsed) pProc = nullptr;
	}
	if(pProc == nullptr) return -TLK_EQUOTA;

	procID += 1;
	pProc->isUsed    = true;
	pProc->priority  = priority;
	pProc->stackSize = stackSize;
	pProc->pTaskList = nullptr;
	return procID;
}

/******************************************************************************
 * Function: tlksys_proc_unmount
 * Descript: Unmount the process from the proc list.
 * Params:
 *     @procID--ID of the process to be uninstalled.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_proc_unmount(uint08 procID)
{
	tlksys_task_t *pTask;
	tlksys_task_t *pTemp;
	tlksys_proc_t *pProc;

	pProc = tlksys_proc_getUsedNodeByID(procID);
	if(pProc == nullptr) return -TLK_EQUOTA;

	pTask = pProc->pTaskList;
	
	pProc->isUsed    = false;
	pProc->priority  = 0;
	pProc->stackSize = 0;
	pProc->pTaskList = nullptr;
	while(pTask != nullptr){
		pTemp = pTask;
		pTask = pTask->pNext;
		pTemp->pNext = nullptr;
		tlksys_task_remove(pTemp);
	}
	return procID;
}

/******************************************************************************
 * Function: tlksys_task_start
 * Descript: Start a system process.
 * Params:
 *     @procID--ID of the task to be operated.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_proc_start(uint08 procID)
{
	tlksys_task_t *pTask;
	tlksys_proc_t *pProc;

	pProc = tlksys_proc_getUsedNodeByID(procID);
	if(pProc == nullptr) return -TLK_EPARAM;
	
	pTask = pProc->pTaskList;
	while(pTask != nullptr){
		if(pTask->pFuncs->Start != nullptr) pTask->pFuncs->Start();
		pTask = pTask->pNext;
	}
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_proc_pause
 * Descript: Pause a system process.
 * Params:
 *     @procID--ID of the process to be operated.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_proc_pause(uint08 procID)
{
	tlksys_task_t *pTask;
	tlksys_proc_t *pProc;

	pProc = tlksys_proc_getUsedNodeByID(procID);
	if(pProc == nullptr) return -TLK_EPARAM;
	
	pTask = pProc->pTaskList;
	while(pTask != nullptr){
		if(pTask->pFuncs->Pause != nullptr) pTask->pFuncs->Pause();
		pTask = pTask->pNext;
	}
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_proc_close
 * Descript: Close the system process.
 * Params:
 *     @procID--ID of the process to be operated.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_proc_close(uint08 procID)
{
	tlksys_task_t *pTask;
	tlksys_proc_t *pProc;

	pProc = tlksys_proc_getUsedNodeByID(procID);
	if(pProc == nullptr) return -TLK_EPARAM;
	
	pTask = pProc->pTaskList;
	while(pTask != nullptr){
		if(pTask->pFuncs->Close != nullptr) pTask->pFuncs->Close();
		pTask = pTask->pNext;
	}
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_proc_handler
 * Descript: Loop through the requirements in the process.
 * Params:
 *     @procID--ID of the process to be operated.
 * Return: None.
 * Others: None.
*******************************************************************************/
int tlksys_proc_handler(uint08 procID)
{
	tlksys_task_t *pTask;
	tlksys_proc_t *pProc;

	pProc = tlksys_proc_getUsedNodeByID(procID);
	if(pProc == nullptr) return -TLK_EPARAM;

	tlksys_adapt_handler(procID);
	pTask = pProc->pTaskList;
	while(pTask != nullptr){
		if(pTask->pFuncs->Handler != nullptr) pTask->pFuncs->Handler();
		pTask = pTask->pNext;
	}
	return TLK_ENONE;
}


static tlksys_proc_t *tlksys_proc_getNode(uint08 index)
{
	if(index >= TLKSYS_PROC_MAX_NUMB) return nullptr;
	return &sTlkSysProcList[index];
}
static tlksys_proc_t *tlksys_proc_getIdleNode(uint08 *pIndex)
{
	uint08 index;
	for(index=0; index<TLKSYS_PROC_MAX_NUMB; index++){
		if(!sTlkSysProcList[index].isUsed) break;
	}
	if(index == TLKSYS_PROC_MAX_NUMB) return nullptr;
	if(pIndex != nullptr) *pIndex = index;
	return &sTlkSysProcList[index];
}
static tlksys_proc_t *tlksys_proc_getUsedNode(uint08 index)
{
	tlksys_proc_t *pProc = tlksys_proc_getNode(index);
	if(pProc == nullptr || !pProc->isUsed) return nullptr;
	return pProc;
}
static tlksys_proc_t *tlksys_proc_getUsedNodeByID(uint08 procID)
{
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB || !sTlkSysProcList[procID-1].isUsed){
		return nullptr;
	}
	return &sTlkSysProcList[procID-1];
}
static tlksys_proc_t *tlksys_proc_getUsedNodeByTask(tlksys_task_t *pTask)
{
	uint08 index;
	tlksys_task_t *pTemp;
	if(pTask == nullptr) return nullptr;
	for(index=0; index<TLKSYS_PROC_MAX_NUMB; index++){
		if(!sTlkSysProcList[index].isUsed) continue;
		pTemp = sTlkSysProcList[index].pTaskList;
		while(pTemp != nullptr){
			if(pTemp == pTask) break;
			pTemp = pTemp->pNext;
		}
		if(pTemp != nullptr) break;
	}
	if(index >= TLKSYS_PROC_MAX_NUMB) return nullptr;
	return &sTlkSysProcList[index];
}


#endif //#if (TLK_CFG_SYS_ENABLE)

