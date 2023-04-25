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



typedef enum{
	TLKAPI_CHIP_CLOCK_96M = 1,
	TLKAPI_CHIP_CLOCK_48M = 2,
	TLKAPI_CHIP_CLOCK_32M = 3,
}TLKAPI_CHIP_CLOCK_ENUM;


void tlkapi_chip_switchClock(TLKAPI_CHIP_CLOCK_ENUM clock);


void tlkapi_chip_stackInit(void);
uint tlkapi_chip_stackCheck(void);
uint tlkapi_chip_stackDepth(void);
bool tlkapi_chip_stackOverflow(void);


void tlkapi_random(uint08 *pBuff, uint16 buffLen);


extern void core_reboot(void);
//wakeupSrc refer to "pm_sleep_wakeup_src_e".
extern void core_enter_deep(uint wakeupSrc);
extern void core_enter_critical(void);
extern void core_leave_critical(void);
extern unsigned int core_disable_interrupt(void);
extern void core_restore_interrupt(unsigned int value);
extern void core_interrupt_disable(void);
extern void core_interrupt_restore(void);


#endif 

