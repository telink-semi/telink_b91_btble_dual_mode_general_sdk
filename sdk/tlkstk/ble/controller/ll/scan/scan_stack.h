/********************************************************************************************************
 * @file     scan_stack.h
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

#ifndef SCAN_STACK_H_
#define SCAN_STACK_H_

#include "tlkstk/hci/hci_cmd.h"



#define			SCAN_DEBUG_EN									1
#define 		SCAN_ERR_DEBUG(x)								do{irq_disable();write_reg32(0x40000, (x));while(1);}while(0)



/******************************* scan start ******************************************************************/

extern u8 scan_pri_chn_rx_fifo[];

extern rf_packet_scan_req_t	pkt_scanReq;



#define SCAN_PRICHN_RXFIFO_SIZE				64  //37+24(maybe 17, 24 is come from 826x,) 16B align(dma rx size)
#define SCAN_PRICHN_RXFIFO_NUM				8   //must be 2^n
#define SCAN_PRICHN_RXFIFO_MASK				7


#define SCAN_SECCHN_RXFIFO_SIZE				288
#define SCAN_SECCHN_RXFIFO_NUM				8   //must be 2^n
#define SCAN_SECCHN_RXFIFO_MASK				7


int irq_scan_rx_primary_channel(void);

/******************************* scan end ********************************************************************/









/******************************* leg_scan start ******************************************************************/

#define	  		BLMS_SCAN_SLOT							  		56  //56*0.625 = 35mS
#define			BLMS_SCAN_INTERVL								( BLMS_SCAN_SLOT * SYSTEM_TIMER_TICK_625US)

#define			BLMS_PERCENT_MAX								128


_attribute_aligned_(4)
typedef struct {

//0x00
	u8		scan_type;
	u8		scan_filterPolicy;
	u8		filter_dup;
	u8		scan_percent;  //to optimize code running effect, Denominator set to 128, "*128" -> "<<7",  "/128" -> ">>7"

	u8		scanDevice_num;
	u8		scanRspDevice_num;
	u8		scan_extension_mask;
	u8 		rsvd;

	u32		scan_itvl_tick;
	u32		scan_win_tick;
	u32		scan_end_tick;

}st_scan_t;

extern	_attribute_aligned_(4) st_scan_t  blmsScn;



int 	blt_leg_scan_interrupt_task (int);
int 	blt_leg_scan_mainloop_task (int);

void 	blt_reset_leg_scan(void);

u32 	blt_ll_isr_legacy_scan(uint8_t sequence, int clock);
void	blt_leg_scan_post(void);

void 	blt_ll_switchScanChannel (int set_chn);
int  	blt_ll_procScanPkt(u8 *raw_pkt, u8 *new_pkt, u32 tick_now);
int  	blt_ll_procLegacyScanData(void);


/******************************* leg_scan end ********************************************************************/










/******************************* ext_scan start ******************************************************************/
#define 		EXTSCAN_PREPARE_US								80
#define 		EXTSCAN_TAIL_US									20
_attribute_aligned_(4)
typedef struct aux_task_t{
	u8	 type_oft;
	u8	 type_idx;
	u8	 type_flg;
	u8	 rsvd;

	u32	 aux_tickStart;
	u32	 aux_tickDuration;

//	struct aux_task_t  *pre;
	struct aux_task_t  *next;
} aux_task_t;




typedef struct {
	/* special design begin, for fast parameters check */
	u8		extHdrFlg;
	u8		aux_oftUnit;
	u8		aux_chnIdx;
	u8		aux_secPhy;

	u16		adi_info;
	u16		rsvd;
	/* special design end */
}ext_pkt_info_t;

_attribute_aligned_(4)
typedef struct {
	u8 		occupied;
	u8		u8_rsvd1;
	u8		u8_rsvd2;
	u8		u8_rsvd3;

	ext_pkt_info_t	extPkt_info;


	u8		adrType;
	u8		addr[6];



	//Attention: if not 4B aligned, task scheduler will collapse. SO variable must 4B aligned above !!!
	aux_task_t	auxTsk;

}ll_extscn_t;







_attribute_aligned_(4)
typedef struct {
	u8		pri_phy;
	u8 		cur_phy; //

	u8      auxList_taskNum;

}extscn_mng_t;

extern extscn_mng_t		bltExtScn;



int irq_scan_rx_secondary_channel(void);

int  blt_ll_procExtScanData(void);



/******************************* ext_scan end ********************************************************************/


















#endif /* SCAN_STACK_H_ */
