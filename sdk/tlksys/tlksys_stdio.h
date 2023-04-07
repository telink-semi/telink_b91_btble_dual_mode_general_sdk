/********************************************************************************************************
 * @file     tlksys_stdio.h
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
#ifndef TLKSYS_STDIO_H
#define TLKSYS_STDIO_H

#if (TLK_CFG_SYS_ENABLE)


#define TLKSYS_PROC_MAX_NUMB        8
#define TLKSYS_TASK_MAX_NUMB        16


typedef enum{
	TLKSYS_PROCID_NONE = 0,
	TLKSYS_PROCID_SYSTEM, //System processes must come first
	TLKSYS_PROCID_AUDIO,
	TLKSYS_PROCID_FILEM,
	TLKSYS_PROCID_STACK,
	TLKSYS_PROCID_TEST,
	TLKSYS_PROCID_VIEW,
}TLKSYS_PROCID_ENUM;

typedef enum{
	TLKSYS_TASKID_NONE = 0,
	TLKSYS_TASKID_SYSTEM, //System task must come first
	TLKSYS_TASKID_STACK,
	TLKSYS_TASKID_AUDIO,
	TLKSYS_TASKID_BTMGR,
	TLKSYS_TASKID_LEMGR,
	TLKSYS_TASKID_LEMST,
	TLKSYS_TASKID_PHONE,
	TLKSYS_TASKID_FILE,
	TLKSYS_TASKID_TEST,
	TLKSYS_TASKID_VIEW,
}TLKSYS_TASKID_ENUM;


typedef struct{
	int(*Init)(uint08 procID, uint08 taskID);
	int(*Start)(void);
	int(*Pause)(void);
	int(*Close)(void);
	int(*Input)(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen); //mtype=0:Inner message; Other for outer message
	void(*Handler)(void);
}tlksys_funcs_t;


#include "tlksys/prt/tlkpti_stdio.h"
#include "tlksys/prt/tlkpto_stdio.h"
#include "tlksys/tlksys_adapt.h"
#include "tlksys/tlksys_proc.h"
#include "tlksys/tlksys_task.h"
#include "tlksys/tlksys_pm.h"


#define TLKSYS_MMI_TASK_DEFINE(name0, name1)       \
	static int tlkmmi_##name0##_init(uint08 procID, uint08 taskID);\
	static int tlkmmi_##name0##_start(void);\
	static int tlkmmi_##name0##_pause(void);\
	static int tlkmmi_##name0##_close(void);\
	static int tlkmmi_##name0##_input(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);\
	static void tlkmmi_##name0##_handler(void);\
	const tlksys_funcs_t gTlkMmi##name1##Task = {\
		tlkmmi_##name0##_init,\
		tlkmmi_##name0##_start,\
		tlkmmi_##name0##_pause,\
		tlkmmi_##name0##_close,\
		tlkmmi_##name0##_input,\
		tlkmmi_##name0##_handler\
	}\


extern int  tlksys_init(void);
extern void tlksys_start(uint08 procID);
extern void tlksys_pause(uint08 procID);
extern void tlksys_close(uint08 procID);
extern void tlksys_handler(void);


extern int tlksys_sendInnerMsg(uint08 taskID, uint16 msgID, uint08 *pData, uint16 dataLen);
extern int tlksys_sendOuterMsg(uint08 taskID, uint08 mType, uint16 msgID, uint08 *pData, uint16 dataLen);
extern int tlksys_sendInnerExtMsg(uint08 taskID, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
extern int tlksys_sendOuterExtMsg(uint08 taskID, uint08 mType, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);


#endif //#if (TLK_CFG_SYS_ENABLE)

#endif //TLKSYS_STDIO_H

