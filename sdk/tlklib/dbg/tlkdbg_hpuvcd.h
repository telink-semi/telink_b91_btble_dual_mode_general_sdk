/********************************************************************************************************
 * @file	tlkdbg_hpuvcd.h
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
#ifndef TLKDBG_HPU_VCD_H
#define TLKDBG_HPU_VCD_H

#if (TLKDBG_CFG_HPU_VCD_ENABLE)


//HPU - Hardware Protocol UART


#define TLKDBG_HPU_VCD_BUFFER_SIZE       2048
#define TLKDBG_HPU_VCD_CACHE_SIZE        64

#define TLKDBG_HPU_VCD_HEAD_SIGN         0xFA
#define TLKDBG_HPU_VCD_ESCAPE_SIGN       0xFD


typedef enum{
	TLKDBG_HPU_VCD_TYPE_SYNC = 0, //None ID
	TLKDBG_HPU_VCD_TYPE_TICK, //id=1~255
	TLKDBG_HPU_VCD_TYPE_EVENT, //id=0~255
	TLKDBG_HPU_VCD_TYPE_LEVEL, //id=0~127
	TLKDBG_HPU_VCD_TYPE_BYTE, //id=0~255
	TLKDBG_HPU_VCD_TYPE_WORD, //id=0~255
	TLKDBG_HPU_VCD_TYPE_MAX,
}TLKDBG_HPU_VCD_TYPE_ENUM;


void tlkdbg_hpuvcd_init(void);
void tlkdbg_hpuvcd_reset(void);
bool tlkdbg_hpuvcd_isBusy(void);
void tlkdbg_hpuvcd_handler(void);

void tlkdbg_hpuvcd_sync(void);
void tlkdbg_hpuvcd_tick(uint08 id);
void tlkdbg_hpuvcd_level(uint08 id, uint08 level);
void tlkdbg_hpuvcd_event(uint08 id);
void tlkdbg_hpuvcd_byte(uint08 id, uint08 value);
void tlkdbg_hpuvcd_word(uint08 id, uint16 value);

void tlkdbg_hpuvcd_setOpen(bool isOpen);

#endif //#if (TLKDBG_CFG_HPU_VCD_ENABLE)

#endif //TLKDBG_HPU_VCD_H

