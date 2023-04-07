/********************************************************************************************************
 * @file     phy_stack.h
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

#ifndef PHY_STACK_H_
#define PHY_STACK_H_


#include "tlkstk/inner/tlkstk_inner.h"
#include "tlkstk/hci/hci_cmd.h"


/******************************* phy start *************************************************************************/

#define 		RFLEN_255_1MPHY_US								2120
#define 		RFLEN_255_2MPHY_US								1064
#define 		RFLEN_255_CODEDPHY_S2_US						4542
#define 		RFLEN_255_CODEDPHY_S8_US						17040


typedef struct{
	u8	llid;
	u8  rf_len;
	u8	opcode;
	u8	tx_phys;
	u8	rx_phys;
}rf_pkt_ll_phy_req_rsp_t;   //phy_req, phy_rsp

typedef struct{
	u8	llid;
	u8  rf_len;
	u8	opcode;
	u8	m_to_s_phy;
	u8	s_to_m_phy;
	u8 	instant0;
	u8 	instant1;
}rf_pkt_ll_phy_update_ind_t;   //phy_req, phy_rsp

typedef struct {
	u8	dft_tx_prefer_phys;
	u8 	dft_rx_prefer_phys;
	u8	dft_prefer_phy;
	u8	dft_CI;

	//for Extended ADV
	u8	cur_llPhy;
	u8	cur_CI;  //cur_coding_ind
	u8	rsvd1[2];

	u8	tx_stl_adv;
	u8	tx_stl_tifs;
	u8	oneByte_us; //1M: 8uS;  2M: 4uS; Coded S2: 16uS; Coded S8: 64uS
	u8	rsvd2[1];
	/* timing after "access_code"
	 * 1M:		 (rf_len+5)*8 		   = rf_len*8 + 40
	 * 2M:		 (rf_len+5)*4 		   = rf_len*4 + 20
	 * Coded S8: rf_len*64 + 720 - 336 = rf_len*64 + 384
	 * Coded S2: rf_len*16 + 462 - 336 = rf_len*16 + 126
	 *
	 * TX_extra_preamble_cost is different for different PHYs, no matter with MCU
	 * 1M:	  5*8=40uS
	 * 2M: 	  4*4=16uS
	 * Coded:      0uS
	 *
	 * AD_convert_delay + LL_TX_STL_TIFS + TX_extra_preamble_cost + TX_compensation = 150
	 *
	 * AD_convert_delay is different for different MCUs and different PHYs
	 *
	 *
	 * 1M:    TX_compensation = 150 - 40 - LL_TX_STL_TIFS_1M - AD_convert_delay_1M = 110 - LL_TX_STL_TIFS_1M - AD_CONVERT_DLY_1M
	 * 2M:    TX_compensation = 150 - 20 - LL_TX_STL_TIFS_2M - AD_convert_delay_2M = 130 - LL_TX_STL_TIFS_2M - AD_CONVERT_DLY_2M
	 * Coded: TX_compensation = 150 -  0 - LL_TX_STL_TIFS_CODED - AD_convert_delay_Coded = 150 - LL_TX_STL_TIFS_CODED - AD_CONVERT_DLY_CODED
	 *
	 * TIFS_offset_us
	 * 1M: 	     TX_compensation + 40  = 150 - LL_TX_STL_TIFS_1M 	- AD_CONVERT_DLY_1M
	 * 2M: 		 TX_compensation + 20  = 150 - LL_TX_STL_TIFS_2M 	- AD_CONVERT_DLY_2M
	 * Coded S8: TX_compensation + 384 = 534 - LL_TX_STL_TIFS_CODED - AD_CONVERT_DLY_CODED
	 * Coded S2: TX_compensation + 126 = 276 - LL_TX_STL_TIFS_CODED - AD_CONVERT_DLY_CODED
	 */
	u16 TIFS_offset_us;

	/* preamble + access_code:  1M: 5*8=40uS;  2M: 6*4=24uS;  Coded: 80+256=336uS */
	/* AD_convert_delay : timing cost on RF analog to digital convert signal process:
	 * 					Eagle	1M: 20uS	   2M: 10uS;      500K(S2): 14uS    125K(S8):  14uS
	 *					Jaguar	1M: 20uS	   2M: 10uS;      500K(S2): 14uS    125K(S8):  14uS
	 *					data is come from Xuqiang.Zhang
	 *
	 *	prmb_ac_us + AD_convert_delay:
	 *	         1M: 40 + 20 = 60 uS
	 *	         2M: 24 + 10 = 34 uS
	 *	      Coded: 336 + 14 = 350 uS
	 * */
	u16 prmb_ac_us; //
}ll_phy_t;

typedef enum{
	LE_CODED_S2 = 2,
	LE_CODED_S8 = 8,
}le_coding_ind_t;

//do not support Asymmetric PHYs, conn_phys = tx_phys & rx_phys
typedef struct {
	u8	conn_prefer_phys;  // conn_prefer_phys = tx_prefer_phys & rx_prefer_phys
	u8	conn_cur_phy;	   //
	u8	conn_next_phy;	   //
	u8	conn_cur_CI;	   // CI: coding_ind

	u8	conn_next_CI;
	u8	phy_req_trigger;  // 1: means current device triggers phy_req, due to API "blc_ll_setPhy" called by Host or Application
	u8	phy_req_pending;
	u8	phy_update_pending;

	u32	conn_updatePhy;

	#if 0
		u8	tx_prefer_phys;		//phy set
		u8 	rx_prefer_phys;
		u8  tx_next_phys;
		u8 	rx_next_phys;

		u8	cur_tx_phy;		//phy using
		u8	cur_rx_phy;
		u16 rsvd;
	#endif

}ll_conn_phy_t;

typedef int (*llms_conn_phy_update_callback_t)(u16 connHandle);
typedef int (*llms_conn_phy_switch_callback_t)(u16 connHandle);
typedef void (*ll_phy_switch_callback_t)(le_phy_type_t, le_coding_ind_t);

extern 	llms_conn_phy_update_callback_t	llms_conn_phy_update_cb; ///blt_ll_updateConnPhy
extern 	llms_conn_phy_switch_callback_t	llms_conn_phy_swicth_cb; ///blt_ll_switchConnPhy
extern 	ll_phy_switch_callback_t		ll_phy_switch_cb;

extern u8	tx_settle_slave[4];
extern u8	tx_settle_master[4];

extern _attribute_aligned_(4) ll_phy_t		bltPHYs;

void rf_ble_switch_phy(le_phy_type_t phy, le_coding_ind_t coding_ind);

/******************************* phy end ***************************************************************************/






/******************************* phy_test start *************************************************************************/

int 	  blc_phy_test_main_loop(void);
int 	  blc_phytest_cmd_handler (u8 *p, int n);
ble_sts_t blc_phy_reset(void);
ble_sts_t blc_phy_setReceiverTest (u8 rx_chn);
ble_sts_t blc_phy_setTransmitterTest (u8 tx_chn, u8 length, u8 pkt_type);
ble_sts_t blc_phy_setPhyTestEnd(u8 *pkt_num);
void 	  blc_phy_preamble_length_set(unsigned char len);

/******************************* phy_test end ***************************************************************************/



#endif /* PHY_STACK_H_ */


