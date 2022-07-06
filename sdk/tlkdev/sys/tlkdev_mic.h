/********************************************************************************************************
 * @file     tlkdev_mic.h
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

#ifndef TLKDEV_MIC_H
#define TLKDEV_MIC_H



int  tlkdev_mic_init(void);
int  tlkdev_mic_open(void);
void tlkdev_mic_close(void);


void tlkdev_mic_reset(void);

bool tlkdev_mic_readData(uint08 *pBuffer, uint16 buffLen, uint16 *pOffset);



#endif //TLKDEV_MIC_H

