/********************************************************************************************************
 * @file     adv_stack.h
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

#ifndef ADV_STACK_H_
#define ADV_STACK_H_



#include "tlkstk/hci/hci_cmd.h"
#include "tlkstk/ble/ble_stack.h"
#include "tlkstk/ble/ble_format.h"
#include "tlkstk/ble/controller/csa/csa_stack.h"



/******************************* adv start ******************************************************************/
#define 		BLC_FLAG_STK_ADV								BIT(24)



#define			LEG_ADV_MAX_US									3500


#define 		ADV_DFT_TX_DELAY_US								50
#define 		ADV_TX_PREPARE_US								(LL_ADV_TX_SETTLE + 40 + 2)
//#define 		ADV_1MPHY_TX_PREPARE_US							(LL_ADV_TX_SETTLE + 40 + 2)  //40: 1M PHY 5B*8=40uS; 2M PHY 10B*4=40uS
//#define 		ADV_2MPHY_TX_PREPARE_US							(LL_ADV_TX_STL_2M + 40 + 2)
//#define 		ADV_CODEDPHY_TX_PREPARE_US						(LL_ADV_TX_STL_CODED + 0 + 2)

#define 		ADV_TAIL_MARGIN_US								30

/*
		 *   1M PHY   :    (rf_len + 10) * 8,      // 10 = 1(BLE preamble) + 9(accesscode 4 + crc 3 + header 2)
		 *   2M PHY   :	   (rf_len + 11) * 4	   // 11 = 2(BLE preamble) + 9(accesscode 4 + crc 3 + header 2)
		 *  Coded PHY :    376 + (rf_len*8+43)*S // 376uS = 80uS(preamble) + 256uS(Access Code) + 16uS(CI) + 24uS(TERM1)
*/
#define 		AUX_CONN_REQ_1MPHY_US							352   // (34 + 10) * 8 = 352
#define 		AUX_CONN_REQ_2MPHY_US							495   // (34 + 11) * 4 = 495
#define 		AUX_CONN_REQ_CODEDPHY_S2_US						1006  // 376 + (34*8+43)*2=376 + 315*2 =1006
#define 		AUX_CONN_REQ_CODEDPHY_S8_US						2896  // 376 + (34*8+43)*8=376 + 315*8 =2896
#define 		AUX_CONN_RSP_1MPHY_US							176	  // (12 + 10) * 8 = 176
#define 		AUX_CONN_RSP_2MPHY_US							92    // (12 + 11) * 4 =  92
#define 		AUX_CONN_RSP_CODEDPHY_S2_US						654   // 376 + (12*8+43)*2=376 + 139*2 =654
#define 		AUX_CONN_RSP_CODEDPHY_S8_US						1488  // 376 + (12*8+43)*8=376 + 139*8 =1488




//#define 		RFLEN_255_1MPHY_US								2120
//#define 		RFLEN_255_2MPHY_US								1064
//#define 		RFLEN_255_CODEDPHY_S2_US						4542
//#define 		RFLEN_255_CODEDPHY_S8_US						17040

#define 		RFLEN_255_1MPHY_PKT_US								2430 //2120+300=2420, 81*30=2430, 10uS margin
#define 		RFLEN_255_1MPHY_N_30								81

#define 		RFLEN_255_2MPHY_PKT_US								1380 //1064+300=1364, 46*30=1380, 16uS margin
#define 		RFLEN_255_2MPHY_N_30								46

#define 		RFLEN_255_CODEDPHY_S2_PKT_US						4860 //4542+300=4842, 162*30=4860, 18uS margin
#define 		RFLEN_255_CODEDPHY_S2_N_30							162

#define 		RFLEN_255_CODEDPHY_S8_PKT_US						17370 //17040+300=17340, 579*30=17370, 30uS margin
#define 		RFLEN_255_CODEDPHY_S8_N_30							579



_attribute_aligned_(4)
typedef struct {
	u8		advTxDly_us;
	u8		adv_scanReq_connReq;
	u8		rsvd3;
	u8		rsvd4;

	u8		advChn_idx;
	u8		advChn_cnt;
    u8		T_SCAN_RSP_INTVL;
    u8 		T_AUX_RSP_INTVL; //settle aux_scan_rsp/aux_conn_rsp's IFS 150s


    u32		adv_interval_tick;
	u32		rand_delay_mask;
	u32		rand_delay_tick;
	u32		adv_irq_tick;


}ll_adv_t;
extern ll_adv_t	bltAdv;


extern	u32 	blt_advExpectTime;
extern	u32 	blc_rcvd_connReq_tick;
extern 	u8		blc_adv_channel[];


extern rf_packet_adv_t	pkt_Adv;
extern rf_packet_scan_rsp_t	pkt_scanRsp;
/******************************* adv end ********************************************************************/







/******************************* leg_adv start ******************************************************************/

_attribute_aligned_(4)
typedef struct {
	u8		adv_chn_mask;
	u8		adv_duraton_en;
	u8		adv_type;
	u8 		adv_filterPolicy;

	u8      own_addr_type;
	u8	 	u8_rsvd1;
	u8	 	u8_rsvd2;
	u8	 	u8_rsvd3;

	u16		advInt_min;
	u16 	advInt_max;
	u16		advInt_diff;
	u16		advInt_maxAddRandom;

	u32		adv_duration_us;
	u32		adv_begin_tick;

}ll_legadv_t;



extern _attribute_aligned_(4) ll_legadv_t  bltLegAdv;


u32 	blt_ll_send_adv(u8 sequence);
void			blt_ll_reset_leg_adv(void);
ble_sts_t 		blc_hci_le_setAdvParam(hci_le_setAdvParam_cmdParam_t *para);

int 	blt_leg_adv_interrupt_task (int);
int 	blt_leg_adv_mainloop_task (int);

/******************************* leg_adv end ********************************************************************/












/******************************* ext_adv start **********************************************************************/



/******************************* ext_adv  end ********************************************************************/









#endif /* ADV_STACK_H_ */
