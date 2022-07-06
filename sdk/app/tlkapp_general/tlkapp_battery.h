/********************************************************************************************************
 * @file     tlkapp_battery.h
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

#ifndef TLKAPP_BATTERY_H
#define TLKAPP_BATTERY_H

#if (TLKAPP_CFG_BAT_CHECK_ENABLE)


// TLKAPP_CFG_BAT_CHANNEL=0: Internal battery voltage. The battery voltage sample range is 1.8~3.5V 
// TLKAPP_CFG_BAT_CHANNEL=GPIO_Pxx: External battery voltage. if the battery voltage > 3.6V, should take some external voltage divider.
#define TLKAPP_BAT_GPIO_PIN               GPIO_PB7//0//GPIO_PB0
#define TLKAPP_BAT_ADC_PIN                ADC_GPIO_PB7

#define TLKAPP_BAT_CHECK_INTERVAL         1000000 //3s
#define TLKAPP_BAT_LOW_PROTECT_ENABLE     1
#define TLKAPP_BAT_LOW_PROTECT_VOLTAGE    3500
#define TLKAPP_BAT_LOW_RESTORE_VOLTAGE    3700

#define TLKAPP_BAT_ANALOG_REG             0x39 //0x39 ~ 0x3f
#define TLKAPP_BAT_FLAG_LOW_POWER         0x01


/******************************************************************************
 * Function: tlkapp_battery_init
 * Descript: Initial the battery's gpio and register the callback.
 * Params: None.
 * Return: TLK_NONE is success.
 * Others: None.
*******************************************************************************/
int tlkapp_battery_init(void);

/******************************************************************************
 * Function: tlkapp_battery_check
 * Descript: This function use to check the voltage.
 * Params: None.
 * Return: TLK_NONE is success.
 * Others: None.
*******************************************************************************/
int tlkapp_battery_check(void);



#endif //#if (TLKAPP_CFG_BAT_CHECK_ENABLE)

#endif //TLKAPP_BATTERY_H

