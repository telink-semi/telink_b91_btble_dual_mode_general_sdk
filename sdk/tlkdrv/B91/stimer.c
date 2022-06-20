/********************************************************************************************************
 * @file     stimer.c
 *
 * @brief    This is the source file for BTBLE SDK
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

#include "stimer.h"


/**
* @brief     This function serves to set timeout by us.
* @param[in] ref  - reference tick of system timer .
* @param[in] us   - count by us.
* @return    true - timeout, false - not timeout
*/
_attribute_retention_code_ _Bool clock_time_exceed(unsigned int ref, unsigned int us)
{
	return ((unsigned int)(stimer_get_tick() - ref) > us * SYSTEM_TIMER_TICK_1US);
}


/**
* @brief     This function performs to set delay time by us.
* @param[in] microsec - need to delay.
* @return    none
*/
_attribute_retention_code_ void delay_us(u32 microsec)
{
	unsigned long t = stimer_get_tick();
	while(!clock_time_exceed(t, microsec)){
	}
}

/**
* @brief     This function performs to set delay time by ms.
* @param[in] millisec - need to delay.
* @return    none
*/
_attribute_retention_code_ void delay_ms(u32 millisec)
{

	unsigned long t = stimer_get_tick();
	while(!clock_time_exceed(t, millisec*1000)){
	}
}
