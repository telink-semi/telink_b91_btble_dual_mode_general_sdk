/********************************************************************************************************
 * @file	btc_core.h
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
#ifndef BTC_CORE_H
#define BTC_CORE_H


typedef struct{
	uint08 inq_en;
	uint08 inq_mode;
	uint08 inqscan_type;
	uint08 inqscan_use_fec;

	uint08 inq_lap[3];
	uint08 max_link_num;

	uint16 inqscan_inv;
	uint16 inqscan_win;

	uint32 page_to;
	uint32 ext_page_to;

	uint16 pagescan_inv;
	uint16 pagescan_win;

	uint16 sniff_inv_max;
	uint16 sniff_inv_min;

	uint32 classofdevice;

	uint08 local_bdaddr[6];
	uint08 pagescan_type;
	uint08 pagescan_rep_mode;

	uint08 local_name[32];
	uint08 scan_en;
	uint08 page_en;
}btc_stack_param_t;


void btc_core_init(void);


void btc_context_store(void);
void btc_context_restore(void);


void bt_modem_reg_restore(void);
void bt_modem_reg_store(void);

uint08 btc_core_allocLtAddr(void);
void btc_core_freeLtAddr(uint08 LtAddr);



#endif //BTC_CORE_H

