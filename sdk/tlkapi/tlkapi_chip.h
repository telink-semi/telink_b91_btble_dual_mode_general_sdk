/********************************************************************************************************
 * @file     tlkapi_chip.h
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

#ifndef TLKAPI_CHIP_H
#define TLKAPI_CHIP_H


#define TLKAPI_CHIP_STACK_CHECK_ENABLE           0


typedef enum{
	TLKAPI_CHIP_CLOCK_96M = 1,
	TLKAPI_CHIP_CLOCK_48M = 2,
}TLKAPI_CHIP_CLOCK_ENUM;


void tlkapi_chip_switchClock(TLKAPI_CHIP_CLOCK_ENUM clock);


#if (TLKAPI_CHIP_STACK_CHECK_ENABLE)
void tlkapi_chip_stackInit(void);
uint tlkapi_chip_stackCheck(void);
uint tlkapi_chip_stackDepth(void);
bool tlkapi_chip_stackOverflow(void);
#endif

void tlkapi_random(uint08 *pBuff, uint16 buffLen);


#endif 

