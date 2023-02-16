/********************************************************************************************************
 * @file     tlktsk.h
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
#ifndef TLKTSK_H
#define TLKTSK_H


#define TLKTSK_DBG_FLAG       ((TLK_MAJOR_DBGID_SYS << 24) | (TLK_MINOR_DBGID_SYS_TSK << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKTSK_DBG_SIGN       "[TSK]"


int tlktsk_init(void);

int tlktsk_mount(uint08 procID, const tlktsk_modinf_t *pModinf);

void tlktsk_start(uint08 procID);
void tlktsk_pause(uint08 procID);
void tlktsk_close(uint08 procID);

void tlktsk_run(void);


int tlktsk_sendInnerMsg(TLKTSK_TASKID_ENUM taskID, uint16 msgID, uint08 *pData, uint16 dataLen);
int tlktsk_sendOuterMsg(TLKTSK_TASKID_ENUM taskID, uint08 mType, uint16 msgID, uint08 *pData, uint16 dataLen);
int tlktsk_sendInnerExtMsg(TLKTSK_TASKID_ENUM taskID, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
int tlktsk_sendOuterExtMsg(TLKTSK_TASKID_ENUM taskID, uint08 mType, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);



#endif //TLKTSK_H
