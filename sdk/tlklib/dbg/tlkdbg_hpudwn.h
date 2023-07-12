/********************************************************************************************************
 * @file	tlkdbg_hpudwn.h
 *
 * @brief	This is the header file for BTBLE SDK
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
#ifndef TLKDBG_HPU_DWN_H
#define TLKDBG_HPU_DWN_H

#if (TLKDBG_CFG_HPU_DWN_ENABLE)


//HPU - Hardware Protocol UART
#define TLKDBG_HPUDWN_FAST_SPEED_ENABLE    1

typedef enum{
	TLKDBG_HPUDWN_MSGID_NONE = 0x00,
	TLKDBG_HPUDWN_MSGID_START = 0x01,
	TLKDBG_HPUDWN_MSGID_FLASH = 0x02, //Erase Flash
	TLKDBG_HPUDWN_MSGID_FSYNC = 0x03, //Fast Sync
	TLKDBG_HPUDWN_MSGID_CLOSE = 0x04, //
	TLKDBG_HPUDWN_MSGID_REBOOT = 0x05,
}TLKDBG_HPUDWN_MSGID_ENUM;

typedef enum{
	TLKDBG_HPUDWN_MSTATE_HEAD = 0,
	TLKDBG_HPUDWN_MSTATE_ATTR,
	TLKDBG_HPUDWN_MSTATE_BODY,
	TLKDBG_HPUDWN_MSTATE_CHECK,
	TLKDBG_HPUDWN_MSTATE_TAIL,
	TLKDBG_HPUDWN_MSTATE_READY,
}TLKDBG_HPUDWN_MSTATE_ENUM;



void tlkdbg_hpudwn_init(void);

void tlkdbg_hpudbg_recvCmd(uint08 *pData, uint16 dataLen);

void tlkdbg_hpudwn_handler(void);


void tlkdbg_hpudbg_recvDatDeal(uint16 numb, uint08 *pData, uint16 dataLen);




#endif //#if (TLKDBG_CFG_HPU_DWN_ENABLE)

#endif //TLKDBG_HPU_DWN_H

