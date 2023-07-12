/********************************************************************************************************
 * @file	tlkmdi_battery.h
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
#ifndef TLKMDI_BATTERY_H
#define TLKMDI_BATTERY_H

#if (TLK_MDI_BATTERY_ENABLE)



#define TLKMDI_BAT_CHECK_INTERVAL         3000000 //3s
#define TLKMDI_BAT_LOW_PROTECT_ENABLE     1
#define TLKMDI_BAT_LOW_PROTECT_VOLTAGE    3500
#define TLKMDI_BAT_LOW_RESTORE_VOLTAGE    3700

#define TLKMDI_BAT_ANALOG_REG             0x39 //0x39 ~ 0x3f
#define TLKMDI_BAT_FLAG_LOW_POWER         0x01


typedef struct{
	uint08 level;
	uint16 voltage;
	uint16 preValue;
	tlkapi_timer_t timer;
}tlkmdi_battery_t;


/******************************************************************************
 * Function: tlkmdi_battery_init
 * Descript: Initial the battery's gpio and register the callback.
 * Params: None.
 * Return: TLK_NONE is success.
 * Others: None.
*******************************************************************************/
int tlkmdi_battery_init(void);

/******************************************************************************
 * Function: tlkmdi_battery_check
 * Descript: This function use to check the voltage.
 * Params: None.
 * Return: TLK_NONE is success.
 * Others: None.
*******************************************************************************/
void tlkmdi_battery_check(void);


void tlkmdi_battery_startCheck(void);
void tlkmdi_battery_closeCheck(void);


#endif //TLK_MDI_BATTERY_ENABLE

#endif //TLKMDI_BATTERY_H

