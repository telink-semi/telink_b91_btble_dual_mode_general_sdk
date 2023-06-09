/********************************************************************************************************
 * @file     tlkmmi_sysCtrl.h
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
#ifndef TLKMMI_SYS_CTRL_H
#define TLKMMI_SYS_CTRL_H

#if (TLKMMI_SYSTEM_ENABLE)


typedef struct{
	uint08 isBoot;
	uint08 resv00;
	uint16 heartNumb;
	uint32 heartIntv; //HeartBeat Interval
	uint32 powerIntv;
	uint32 heartTimer;
	uint32 powerTimer;
}tlkmmi_sys_ctrl_t;


int  tlkmmi_sys_ctrlInit(void);

void tlkmmi_sys_ctrlHandler(void);

void tlkmmi_sys_setPowerParam(uint32 offTimeUs, bool isBoot);
void tlkmmi_sys_setHeartParam(uint32 intervalUs);

void tlkmmi_sys_poweroff(void);


#endif //TLKMMI_SYSTEM_ENABLE

#endif //TLKMMI_SYS_CTRL_H

