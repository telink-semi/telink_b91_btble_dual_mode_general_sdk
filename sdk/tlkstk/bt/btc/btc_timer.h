/********************************************************************************************************
 * @file     bt_ll_timer.h
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

#ifndef BT_LL_TIMER_H_
#define BT_LL_TIMER_H_


enum TIMER_TASK
{
	BT_TIMER_LMPTO_EVENT,
	BT_TIMER_RESET_EVENT,
	BT_TIMER_WAIT_HOST_ACCEPT_CON_REQ_TO_EVENT,
	BT_TIMER_WAIT_DISCONNECTION_TO_EVENT,
	BT_TIMER_WAIT_RSW_WAIT_ACCEPTED_TO_EVENT,
	BT_TIMER_WAIT_RSW_ACCEPTED_ACK_TO_EVENT,
	BT_TIMER_WAIT_RSW_SCH_START_TO_EVENT,
	BT_TIMER_WAIT_RSW_TDD_SWITCH_TO_EVENT,
	BT_TIMER_WAIT_RSW_INSTANT_TO_EVENT,
	BT_TIMER_WAIT_AFH_MASTER_REFRESH_MAP_TO_EVENT,
	BT_TIMER_WAIT_AFH_INSTANT_TO_EVENT,
	BT_TIMER_WAIT_AFH_CLS_INTERVAL_TO_EVENT,
	BT_TIMER_WAIT_POWER_CONTROL_REQ_TO_EVENT,
	BT_TIMER_WAIT_SNIFF_PRE_ENTER_TO_EVENT,
	BT_TIMER_EVENT_MAX,
};
typedef int (*btc_timer_callback_t)(uint32_t data);


int btc_timer_init(void);

bool btc_timer_isPend(void);

bool btc_timer_start(uint8_t link_id, btc_timer_callback_t cb, uint32_t data, uint32_t t_us);
void btc_timer_close(uint8_t link_id, btc_timer_callback_t cb);
void btc_timer_handler(void);


void bt_ll_clear_all_timer_event(uint8_t link_id);
int bt_ll_timer_reset_func(uint32_t data);
int bt_ll_timer_lmp_to_func(uint32_t data);
int bt_ll_timer_host_accept_con_func(uint32_t data);
int bt_ll_timer_disconnection_to_func(uint32_t data);


#endif /* BT_LL_TIMER_H_ */
