/********************************************************************************************************
 * @file     tlksys_task.h
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
#ifndef TLKSYS_TASK_H
#define TLKSYS_TASK_H

#if (TLK_CFG_SYS_ENABLE)


/******************************************************************************
 * Function: tlksys_task_init
 * Descript: Initializes the system task list.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_task_init(void);

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
 *     2.If taskID! =0, and the change ID is occupied, a failure is returned.
*******************************************************************************/
int tlksys_task_mount(uint08 procID, uint08 taskID, const tlksys_funcs_t *pFuncs);

/******************************************************************************
 * Function: tlksys_task_unmount
 * Descript: Unmount the task from the task list.
 * Params:
 *     @taskID--ID of the task to be uninstalled.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_task_unmount(uint08 taskID);
int tlksys_task_remove(tlksys_task_t *pTask);

/******************************************************************************
 * Function: tlksys_task_start
 * Descript: Start a system task.
 * Params:
 *     @taskID--ID of the task to be uninstalled.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_task_start(uint08 taskID);

/******************************************************************************
 * Function: tlksys_task_start
 * Descript: Pause a system task.
 * Params:
 *     @taskID--ID of the task to be uninstalled.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_task_pause(uint08 taskID);

/******************************************************************************
 * Function: tlksys_task_close
 * Descript: Close the system task.
 * Params:
 *     @taskID--ID of the task to be uninstalled.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlksys_task_close(uint08 taskID);

/******************************************************************************
 * Function: tlksys_task_input
 * Descript: Send a message to a task.
 * Params:
 *     @taskID--ID of the task to be uninstalled.
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
	uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: tlksys_task_handler
 * Descript: Loop through the requirements in the task.
 * Params:
 *     @taskID--ID of the task to be uninstalled.
 * Return: None.
 * Others: 
 *     1.Do not use the modified interface unless necessary. The modified 
 *       interface will be discarded later.
 *     2.Users can replace the interface with scheduled and queued tasks in 
 *       the adapter.
*******************************************************************************/
void tlksys_task_handler(uint08 taskID);


#endif //#if (TLK_CFG_SYS_ENABLE)

#endif //TLKSYS_TASK_H

