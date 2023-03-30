/********************************************************************************************************
 * @file     tlksys_proc.h
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
#ifndef TLKSYS_PROC_H
#define TLKSYS_PROC_H

#if (TLK_CFG_SYS_ENABLE)


typedef struct tlksys_task_s{
	struct tlksys_task_s *pNext;
	tlksys_funcs_t *pFuncs;
}tlksys_task_t;

typedef struct{
	uint08 isUsed;
	uint08 priority;
	uint16 stackSize;
	tlksys_task_t *pTaskList;
}tlksys_proc_t;



/******************************************************************************
 * Function: tlksys_task_init
 * Descript: Initializes the system process list.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_proc_init(void);

/******************************************************************************
 * Function: tlksys_proc_appendTask
 * Descript: Inserts a task into the specified process.
 * Params:
 *     @procID--ID of the process to be operated.
 *     @pTask--The task to be inserted.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_proc_appendTask(uint08 procID, tlksys_task_t *pTask);

/******************************************************************************
 * Function: tlksys_proc_appendTask
 * Descript: Remove a task from the specified process.
 * Params:
 *     @procID--ID of the process to be operated.
 *     @pTask--The task to be removed.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: 
 *     1.If procID==0, the system will find the corresponding task from all
 *       processes and remove it.
*******************************************************************************/
int tlksys_proc_removeTask(uint08 procID, tlksys_task_t *pTask);

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
int tlksys_proc_mount(uint08 procID, uint08 priority, uint16 stackSize);

/******************************************************************************
 * Function: tlksys_proc_unmount
 * Descript: Unmount the process from the proc list.
 * Params:
 *     @procID--ID of the process to be uninstalled.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_proc_unmount(uint08 procID);

/******************************************************************************
 * Function: tlksys_task_start
 * Descript: Start a system process.
 * Params:
 *     @procID--ID of the task to be operated.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_proc_start(uint08 procID);

/******************************************************************************
 * Function: tlksys_proc_pause
 * Descript: Pause a system process.
 * Params:
 *     @procID--ID of the process to be operated.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_proc_pause(uint08 procID);

/******************************************************************************
 * Function: tlksys_proc_close
 * Descript: Close the system process.
 * Params:
 *     @procID--ID of the process to be operated.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_proc_close(uint08 procID);

/******************************************************************************
 * Function: tlksys_proc_handler
 * Descript: Loop through the requirements in the process.
 * Params:
 *     @procID--ID of the process to be operated.
 * Return: None.
 * Others: None.
*******************************************************************************/
int tlksys_proc_handler(uint08 procID);


#endif //#if (TLK_CFG_SYS_ENABLE)

#endif //TLKSYS_PROC_H

