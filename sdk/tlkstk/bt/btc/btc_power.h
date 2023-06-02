/********************************************************************************************************
 * @file     bt_ll_power_control.h
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

#ifndef _BT_LL_POWER_CONTROL_H_
#define _BT_LL_POWER_CONTROL_H_

#include <stdint.h>
#include "btc_ll.h"

#define RSSI_40dB_THRHLD -40;
#define RSSI_60dB_THRHLD -60;

/// Enumeration of RF modulations
enum BTC_RF_MODE_ENUM
{
    BTC_RF_MODE_GFSK  = 0x01,
    BTC_RF_MODE_DQPSK = 0x02,
    BTC_RF_MODE_8DPSK = 0x03,
};
enum BTC_POWER_STATE_ENUM
{
	BTC_POWER_STATE_IDLE,
	BTC_POWER_STATE_INC_MAX,
	BTC_POWER_STATE_INC_STEP,
	BTC_POWER_STATE_DEC,
	BTC_POWER_STATE_MAX,
};



int btc_power_init(void);

uint08 btc_power_getMaxValue(void);
uint08 btc_power_getMinValue(void);

uint08 btc_power_getTxDbm(uint08 tx_pwr, uint08 mod);
uint08 btc_power_getTxLevel(uint08 link_id, uint08 mod);


#endif /* _BT_LL_POWER_CONTROL_H_ */
