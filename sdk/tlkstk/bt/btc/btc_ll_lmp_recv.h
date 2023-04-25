/********************************************************************************************************
 * @file     btc_ll_lmp_recv.h
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

#ifndef BTC_LL_LMP_RECV_H
#define BTC_LL_LMP_RECV_H

/*
 * STRUCTURES
 ****************************************************************************************
 */
enum{
	BTC_LMP_MSG_ID_IDLE,
	BTC_LMP_MSG_ID_RX,
	BTC_LMP_MSG_ID_TX_ACK,
};

///LMP msg
typedef struct{
    ///message identifier
	uint8_t  msg_id;
    ///Link identifier
    uint8_t  link_id;
    ///length of the LMP PDU
    uint8_t  len;
    ///reserved for future use
    uint8_t  rsvd;
    ///buffer of the LMP PDU
    uint8_t*  pdu;
}btc_lmp_msg_t;



typedef int32_t (*lmp_pdu_func_t)(uint8_t link_id, uint8_t *p, uint8_t len);






int btc_lmp_command_exec(uint8_t link_id, uint8_t *p, uint8_t len);
int btc_lmp_tx_cfm_handler(uint8_t link_id,uint8_t* pdu,uint8_t len);


#endif /* BTC_LL_LMP_RECV_H */

