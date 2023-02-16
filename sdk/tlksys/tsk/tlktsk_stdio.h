/********************************************************************************************************
 * @file     tlktsk_stdio.h
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
#ifndef TLKTSK_STDIO_H
#define TLKTSK_STDIO_H



#include "tlksys/prt/tlkpti_stdio.h"
#include "tlksys/prt/tlkpto_stdio.h"
#include "tlksys/tsk/tlktsk_config.h"
#include "tlksys/tsk/tlktsk_define.h"
#include "tlksys/tsk/tlktsk_modinf.h"
#include "tlksys/tsk/tlktsk_adapt.h"



extern int tlktsk_init(void);

extern int tlktsk_mount(uint08 schID, const tlktsk_modinf_t *pModinf);

extern void tlktsk_start(uint08 schID);
extern void tlktsk_pause(uint08 schID);
extern void tlktsk_close(uint08 schID);

extern void tlktsk_run(void);

extern int tlktsk_sendInnerMsg(TLKTSK_TASKID_ENUM taskID, uint16 msgID, uint08 *pData, uint16 dataLen);
extern int tlktsk_sendOuterMsg(TLKTSK_TASKID_ENUM taskID, uint08 mType, uint16 msgID, uint08 *pData, uint16 dataLen);
extern int tlktsk_sendInnerExtMsg(TLKTSK_TASKID_ENUM taskID, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
extern int tlktsk_sendOuterExtMsg(TLKTSK_TASKID_ENUM taskID, uint08 mType, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);



#endif //TLKTSK_STDIO_H

