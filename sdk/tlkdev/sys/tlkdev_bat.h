/********************************************************************************************************
 * @file     tlkdev_bat.h
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

#ifndef TLKDEV_BAT_H
#define TLKDEV_BAT_H

#if (TLKDEV_SYS_BAT_ENABLE)


#if 0
#define BAT_LEAKAGE_PROTECT_EN        1
#define BAT_DEEP_THRES_MV             2000   // 2000 mV low battery alarm
#define BAT_SUSPEND_THRES_MV          1800   // 1800 mV low battery alarm

#define VBAT_CHANNEL_EN               0

/**
 * @brief analog register below can store infomation when MCU in deepsleep mode
 * 	      store your information in these ana_regs before deepsleep by calling analog_write function
 * 	      when MCU wakeup from deepsleep, read the information by by calling analog_read function
 * 	      Reset these analog registers only by power cycle
 */
#define DEEP_ANA_REG0                 0x39 //initial value =0x00
#define DEEP_ANA_REG1                 0x3a //initial value =0x00
#define DEEP_ANA_REG2                 0x3b //initial value =0x00
#define DEEP_ANA_REG3                 0x3c //initial value =0x00
#define DEEP_ANA_REG4                 0x3d //initial value =0x00
#define DEEP_ANA_REG5                 0x3e //initial value =0x00
#define DEEP_ANA_REG6                 0x3f //initial value =0x0f


void battery_set_detect_enable (int en);
int  battery_get_detect_enable (void);

#if (BAT_LEAKAGE_PROTECT_EN)
_attribute_ram_code_ void app_battery_power_check(u16 threshold_deep_vol_mv, u16 threshold_suspend_vol_mv);
#else
void app_battery_power_check(u16 alram_vol_mv);
#endif

#endif


int tlkdev_bat_init(uint32 gpio, uint32 adcGpio);
int tlkdev_bat_getVoltage(uint16 *pVoltage);




#endif //TLKDEV_SYS_BAT_ENABLE

#endif //TLKDEV_BAT_H

