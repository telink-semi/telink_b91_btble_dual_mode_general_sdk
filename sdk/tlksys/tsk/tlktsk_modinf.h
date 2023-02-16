/********************************************************************************************************
 * @file     tlktsk_modinf.h
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
#ifndef TLKTSK_MODINF_H
#define TLKTSK_MODINF_H



typedef struct{
	TLKTSK_TASKID_ENUM taskID; //The value is unique for different tasks
	int(*Start)(void);
	int(*Pause)(void);
	int(*Close)(void);
	int(*Input)(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen); //mtype=0:Inner message; Other for outer message
	int(*ExtMsg)(uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
	uint(*IntvUs)(void);
	void(*Wakeup)(void);
	void(*Handler)(void);
}tlktsk_modinf_t;



int  tlktsk_modinf_init(void);
int  tlktsk_modinf_mount(const tlktsk_modinf_t *pModinf);

int  tlktsk_modinf_start(uint08 taskID);
int  tlktsk_modinf_pause(uint08 taskID);
int  tlktsk_modinf_close(uint08 taskID);
int  tlktsk_modinf_input(uint08 taskID, uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen);
int  tlktsk_modinf_extmsg(uint08 taskID, uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
uint tlktsk_modinf_intvUs(uint08 taskID);
void tlktsk_modinf_wakeup(uint08 taskID);
void tlktsk_modinf_handler(uint08 taskID);

bool tlktsk_modinf_isHave(const tlktsk_modinf_t *pModinf);



#endif //TLKTSK_MODINF_H

