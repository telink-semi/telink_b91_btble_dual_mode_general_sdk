/********************************************************************************************************
 * @file     tlkdev_spk.h
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

#ifndef TLKDEV_SPK_H
#define TLKDEV_SPK_H







int  tlkdev_spk_init(void);
int  tlkdev_spk_open(void);
void tlkdev_spk_close(void);

void tlkdev_spk_mute(void);
bool tlkdev_spk_play(uint08 *pPcmData, uint16 dataLen);

void tlkdev_spk_reset(void);

uint32 tlkdev_spk_idleLen(void);
uint32 tlkdev_spk_dataLen(void);

//"dataLen == 0x0000":Fills all idle buffers; "dataLen == 0xFFFF":Fill all buffers.
void tlkdev_spk_fillZero(uint16 dataLen, bool isInc);
bool tlkdev_spk_fillBuff(uint08 *pData, uint16 dataLen);
void tlkdev_spk_backReadData(uint08 *pBuffer, uint16 buffLen, uint16 offset, bool isBack);



#endif //TLKDEV_SPK_H

