/********************************************************************************************************
 * @file	btc_ll_afh_chn_cls.h
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
#ifndef _BT_LL_CHN_CLS_H_
#define _BT_LL_CHN_CLS_H_

#include <stdint.h>
#include "tlkstk/bt/incs/co_bt_defines.h"
#include "tlkstk/bt/incs/em_map_config.h"

/// CLASSIFICATION state
typedef enum{
    BTC_AFH_CLS_STATE_IDLE,
    BTC_AFH_CLS_STATE_WAIT_INTERVAL,
	BTC_AFH_CLS_STATE_WAIT_DONE,
}BTC_AFH_CLS_STATE_ENUM;
typedef enum{
	BTC_AFH_CLS_MSGID_IDLE,
	BTC_AFH_CLS_MSGID_HCI_SET_AFH_HOST_CH_CLASS,
	BTC_AFH_CLS_MSGID_DISABLE_CLS,
	BTC_AFH_CLS_MSGID_LMP_REQ,
	BTC_AFH_CLS_MSGID_LMP_CH_CLS_REQ,
	BTC_AFH_CLS_MSGID_LMP_CH_CLS,
}BTC_AFH_CLS_MSGID_ENUM;


/// Channel assessment data
typedef struct bt_afh_ch_assess_data {
    /// Timestamp of last received packet for each frequency
    uint32_t timestamp[AFH_NB_CHANNEL_MAX];

    /// Channel quality level for each frequency
    int8_t level[AFH_NB_CHANNEL_MAX];
} bt_afh_ch_assess_data_t;


/// AFH parameters structure
typedef struct bt_afh_tag {

    uint8_t channel_assess_en;

    /// AFH state
    uint8_t active;

    /// AFH temporary state (state saved during Role switch)
    uint8_t temp_en;

    uint8_t rsvd;

    /// Channel classification from Host
    struct chnl_map host_ch_class;

    /// Master channel map (map used for all master links with AFH enabled)
    struct chnl_map master_ch_map;

    struct bt_afh_ch_assess_data afh_ch_assess;

}  __attribute__ ((__packed__)) __attribute__ ((aligned (4))) bt_afh_tag_t;

extern bt_afh_tag_t bt_afh_env;




typedef int(*btc_afh_cls_task_callback_t)(uint08 linkId, uint08 msgId, uint08 *pData, uint08 dataLen);

extern btc_afh_cls_task_callback_t gBtcAfhClsSysMsgCB;
int btc_timer_afh_cls_interval_to_func(uint32_t data);
void btc_afh_cls_init(void);

#endif /* _BT_LL_CHN_CLS_H_ */
