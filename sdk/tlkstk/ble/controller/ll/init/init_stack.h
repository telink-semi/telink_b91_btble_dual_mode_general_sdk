/********************************************************************************************************
 * @file     init_stack.h
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

#ifndef INIT_STACK_H_
#define INIT_STACK_H_




//can not be "1", will conflict with " blms_create_connection & CONN_IDX_MASK"
#define			BLMS_CONNECTION_REQ						BIT(15)
#define			BLMS_CONNECTION_REQ_PENDING				BIT(14)

#define  		BLMS_WINSIZE							7


// 150us(T_ifs) + 352us(conn_req) = 502 us,  sub some margin 22(RX IRQ delay/irq_handler delay)
// real test data: 470 uS,  beginning is "u32 tick_now = rx_tick_now = clock_time();" in irq_acl_conn_rx
//						    ending is    "while( !(reg_rf_irq_status & FLD_RF_IRQ_TX));" in
//                          "irq_handler" to "u32 tick_now = rx_tick_now = clock_time();" is 4 uS
#define			PKT_INIT_AFTER_RX_TICK							( 480 *SYSTEM_TIMER_TICK_1US)


_attribute_aligned_(4)
typedef struct {
	u8	conn_policy;
	u8	conn_advType;

	u8	conn_mac[6];

	u8  own_addr_type;
	u8  createConnectCmdEn;
	u8  isDisableInitTimeout;
	u8  rsvd;

	u32	scan_itvl_tick;
	u32 scan_win_tick;

}st_llms_init_t;

extern	_attribute_aligned_(4) st_llms_init_t  blmsInit;

extern	volatile	int blms_create_connection;
extern	u32 blms_timeout_connectDevice;

extern	rf_packet_ll_init_t	pkt_llms_init;

void 	blt_ll_setInitEnable (int init_en);
void 	blt_ll_initInitiatingCommon(void);
void	blt_ll_procInitiateConnectionTimeout(void);

int 	blt_init_mainloop_task (int);


#endif /* INIT_STACK_H_ */
