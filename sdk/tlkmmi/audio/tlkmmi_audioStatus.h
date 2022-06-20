/********************************************************************************************************
 * @file     tlkmmi_audioStatus.h
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

#ifndef TLKMMI_AUDIO_STATUS_H
#define TLKMMI_AUDIO_STATUS_H

#if (TLKMMI_AUDIO_ENABLE)



#define TLKMMI_AUDIO_STATUS_ITEM_NUMB       4 //>=2

#define TLK_INVALID_HANDLE           0xFFFF



typedef enum{
	TLKMMI_AUDIO_PRIORITY_LEVEL0 = 0,
	TLKMMI_AUDIO_PRIORITY_LEVEL1,
	TLKMMI_AUDIO_PRIORITY_LEVEL2,
	TLKMMI_AUDIO_PRIORITY_LEVEL3,
	TLKMMI_AUDIO_PRIORITY_LEVEL4,
	TLKMMI_AUDIO_PRIORITY_LEVEL5,
	TLKMMI_AUDIO_PRIORITY_LEVEL6,
	TLKMMI_AUDIO_PRIORITY_LOWEST = TLKMMI_AUDIO_PRIORITY_LEVEL0,
	
	TLKMMI_AUDIO_SCO_PRIORITY  = TLKMMI_AUDIO_PRIORITY_LEVEL6,
	TLKMMI_AUDIO_HFP_PRIORITY  = TLKMMI_AUDIO_PRIORITY_LEVEL5,
	TLKMMI_AUDIO_SRC_PRIORITY  = TLKMMI_AUDIO_PRIORITY_LEVEL2,
	TLKMMI_AUDIO_SNK_PRIORITY  = TLKMMI_AUDIO_PRIORITY_LEVEL1,
	TLKMMI_AUDIO_TONE_PRIORITY = TLKMMI_AUDIO_PRIORITY_LEVEL4,
	TLKMMI_AUDIO_PLAY_PRIORITY = TLKMMI_AUDIO_PRIORITY_LEVEL0,
}TLKMMI_AUDIO_PRIORITY_ENUM;


typedef struct{
	uint08 optype; //Refer to TLKMMI_AUDIO_OPTYPE_ENUM.
	uint08 priority; //Refer to TLKMMI_AUDIO_PRIORITY_ENUM.
	uint08 isMutex; //If TRUE, it will exit automatically when inserted by a higher-priority task.
	uint08 isFirst; //If TRUE, inserts into non-first positions are allowed; Otherwise, it will exit automatically after the first position cannot be inserted
}tlkmmi_audio_statusFunc_t;
typedef struct{
	uint08 optype; //Refer to TLKMMI_SYSTEM_OPTYPE_ENUM.
	uint08 priority;
	uint16 handle;
}tlkmmi_audio_statusItem_t;
typedef struct{
	uint08 nowNumb;
	tlkmmi_audio_statusItem_t item[TLKMMI_AUDIO_STATUS_ITEM_NUMB];
}tlkmmi_audio_statusCtrl_t;



/******************************************************************************
 * Function: tlkmmi_audio_statusInit
 * Descript: Initial all of audio status control block. 
 * Params: None.
 * Return: Return TLK_ENONE is success and other value is filure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_statusInit(void);

/******************************************************************************
 * Function: tlkmmi_audio_statusIndex
 * Descript: Get the current audio status control block index. 
 * Params:
 *         @handle[IN]--The handle
 *         @optype[IN]--The audio operate type.
 * Return: The audio status control block index.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_statusIndex(uint16 handle, uint08 optype);
int tlkmmi_audio_statusIndexByOptype(uint08 optype);

/******************************************************************************
 * Function: tlkmmi_audio_insertStatus
 * Descript: Suspend the current audio task when a new audio task insert 
 *           and start running.
 * Params:
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_insertStatus(uint16 handle, uint08 optype);

/******************************************************************************
 * Function: tlkmmi_audio_removeStatus
 * Descript: Suspend the current task and delete it.Then resume the task 
 *           which been suspend before.
 * Params:
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_removeStatus(uint16 handle, uint08 optype);

bool tlkmmi_audio_statusIsSwitch(uint08 dstOptype, uint08 srcOptype);

/******************************************************************************
 * Function: tlkmmi_audio_removeStatusByOptype
 * Descript: Remove the special audio status control block by it's opreate type.
 * Params: @optype[IN]--The operate type.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_removeStatusByOptype(uint08 optype);

/******************************************************************************
 * Function: tlkmmi_audio_removeStatusByHandle
 * Descript: Remove the special audio status control block by it's handle.
 * Params: @handle[IN]--The handle.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_removeStatusByHandle(uint16 handle);


void tlkmmi_audio_printfStatus(void);

/******************************************************************************
 * Function: tlkmmi_audio_getCurOptype
 * Descript: Get the operate type of the Current of audio status control block 
 *           in it's array. 
 * Params: None.
 * Return: Retur the operate type.
 * Others: None.
*******************************************************************************/
int  tlkmmi_audio_getCurOptype(void);

/******************************************************************************
 * Function: tlkmmi_audio_isHaveOptype
 * Descript: Check the audio status control block array have match 
 *           the special operate type.
 * Params: None.
 * Return: Retur true(there exist a audio control block's 
 *         opertate type equal the optye).
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_isHaveOptype(uint08 optype);

/******************************************************************************
 * Function: tlkmmi_audio_getCurStatus
 * Descript: Get the current audio status control block.
 * Params: None.
 * Return: Retur current audio control block.
 * Others: None.
*******************************************************************************/
tlkmmi_audio_statusItem_t *tlkmmi_audio_getCurStatus(void);

/******************************************************************************
 * Function: tlkmmi_audio_getSndStatus
 * Descript: Get the 2nd audio status control block.
 * Params: None.
 * Return: Retur 2nd audio control block.
 * Others: None.
*******************************************************************************/
tlkmmi_audio_statusItem_t *tlkmmi_audio_getSndStatus(void);


#endif //#if (TLKMMI_AUDIO_ENABLE)

#endif //TLKMMI_AUDIO_STATUS_H

