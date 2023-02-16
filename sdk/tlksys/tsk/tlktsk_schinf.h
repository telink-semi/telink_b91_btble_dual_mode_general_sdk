/********************************************************************************************************
 * @file     tlktsk_schinf.h
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
#ifndef TLKTSK_SCHINF_H
#define TLKTSK_SCHINF_H




typedef struct{
	uint08 priority;
	uint08 reserved;
	uint16 stackSize;
	void(*Process)(void *pUsrArg);
}tlktsk_schinf_t;


int tlktsk_schinf_init(void);
int tlktsk_schinf_mount(uint08 schID, uint08 tskID);

int tlktsk_schinf_start(uint08 schID);
int tlktsk_schinf_pause(uint08 schID);
int tlktsk_schinf_close(uint08 schID);

int tlktsk_schinf_handler(uint08 schID);



#endif //TLKTSK_SCHINF_H

