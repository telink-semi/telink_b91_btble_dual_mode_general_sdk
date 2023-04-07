/********************************************************************************************************
 * @file     tlkdrv_battery.h
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
#ifndef TLKDRV_BATTERY_H
#define TLKDRV_BATTERY_H
#if (TLK_DEV_BATTERY_ENABLE)


// TLKAPP_CFG_BAT_CHANNEL=0: Internal battery voltage. The battery voltage sample range is 1.8~3.5V 
// TLKAPP_CFG_BAT_CHANNEL=GPIO_Pxx: External battery voltage. if the battery voltage > 3.6V, should take some external voltage divider.
#define TLKDRV_BATTERY_GPIO_PIN        GPIO_PB7//0//GPIO_PB0
#define TLKDRV_BATTERY_ADC_PIN         ADC_GPIO_PB7


int tlkdrv_battery_init(void);
int tlkdrv_battery_getVoltage(uint16 *pVoltage);



#endif //#if (TLK_DEV_BATTERY_ENABLE)
#endif //TLKDRV_BATTERY_H

