/********************************************************************************************************
 * @file     ll_stack.h
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

#ifndef LL_STACK_H_
#define LL_STACK_H_


#include "tlkstk/ble/ble_stack.h"
#include "tlkstk/ble/ble_format.h"
#include "tlkstk/hci/hci_cmd.h"
#include "tlkstk/ble/controller/phy/phy_stack.h"

#include "tlkstk/inner/tlkstk_inner.h"
#include "drivers.h"



/******************************* conn_config start ******************************************************************/
#ifndef			BLMS_MAX_CONN_MASTER_NUM
#define			BLMS_MAX_CONN_MASTER_NUM						2
#endif

#ifndef			BLMS_MAX_CONN_SLAVE_NUM
#define			BLMS_MAX_CONN_SLAVE_NUM							1
#endif

#define			BLMS_MAX_CONN_NUM								(BLMS_MAX_CONN_MASTER_NUM + BLMS_MAX_CONN_SLAVE_NUM)



#define			CONN_IDX_MASTER0								0
#define			CONN_IDX_SLAVE0									BLMS_MAX_CONN_MASTER_NUM






#ifndef			BLMS_CONN_MASTER_EN
#define			BLMS_CONN_MASTER_EN								1
#endif

#ifndef			BLMS_CONN_SLAVE_EN
#define			BLMS_CONN_SLAVE_EN								1
#endif


/******************************* conn_config end ******************************************************************/



/******************************* ll start ***********************************************************************/

#define			BLT_STRUCT_4B_ALIGN_CHECK_EN					1  //disable when release SDK, to clear warning


#define 		MORE_MAINLOOP_IN_SRAM_EN						1


#define 		ADV_LEGACY_MASK									BIT(0)
#define 		ADV_EXTENDED_MASK								BIT(1)
#define 		SCAN_LEGACY_MASK								BIT(2)
#define 		SCAN_EXTENDED_MASK								BIT(3)
#define 		SET_RANDON_ADDR_CMD_MASK						BIT(4)


#define			BLS_LINK_STATE_IDLE								0
#define			BLS_LINK_STATE_ADV								BIT(0)
#define			BLS_LINK_STATE_CONN								BIT(3)


typedef bool (*ll_conn_callback_t)(rf_packet_connect_t *, u8 *);
extern 	ll_conn_callback_t 				 	ll_adv_2_slave_cb;

typedef	struct {
	u8		num;	//not used now
	u8		mask;   //not used now
	u8		wptr;
	u8		rptr;
	u8*		p;
}scan_fifo_t;

typedef struct{
	//Private and DHkey use the same buffer, after obtaining dhkey, the private key content will be overwritten by dhkey
	u8 sc_sk_dhk_own[32];  //  own  private(dhk reuse buffer) key[32]
	u8 sc_pk_own[64];      //  own  public  key[64]
	u8 sc_pk_peer[64];     // peer  public  key[64]
}smp_sc_key_t;


extern _attribute_aligned_(4) scan_fifo_t	scan_priRxFifo;


#if (CONTROLLER_GEN_P256KEY_ENABLE)
extern _attribute_aligned_(4) smp_sc_key_t	smp_sc_key;
ble_sts_t   blt_ll_getP256pubKey(void);
ble_sts_t 	blt_ll_generateDHkey (u8* remote_public_key, bool use_dbg_key);
void 		blt_ll_procGetP256pubKeyEvent (void);
void 		blt_ll_procGenDHkeyEvent (void);
#endif





/******************************* ll end *************************************************************************/










/******************************* ll_system start ***********************************************************************/



enum {
	FLAG_TASK_START						=	BIT(30),
	FLAG_TASK_DONE						=	BIT(29),
	FLAG_IRQ_RX							=	BIT(27),
	FLAG_IRQ_TX							=	BIT(26),

	FLAG_MODULE_MAINLOOP				= 	BIT(25),
	FLAG_MODULE_RESET					= 	BIT(24),
	FLAG_MODULE_SET_HOST_CHM			=	BIT(23),


	//ACL
	FLAG_ACL_SLAVE_CLEAR_SLEEP_LATENCY	=	BIT(15),


	//SCAN
	FLAG_SCAN_DATA_REPORT				=	BIT(18),


	FLAG_TASK_IDX_MASK					=	0x1F,  //0~31
};



typedef 	int (*ll_task_callback_t)(int);
typedef 	int (*ll_task_callback_2_t)(int, void*p);


//extern	ll_task_callback_t						ll_leg_adv_irq_task_cb;
extern	ll_task_callback_t						ll_leg_adv_mlp_task_cb;

extern	ll_task_callback_t						ll_leg_scan_irq_task_cb;
extern	ll_task_callback_t						ll_leg_scan_mlp_task_cb;

extern	ll_task_callback_t						ll_init_mlp_task_cb;

extern	ll_task_callback_t						ll_acl_conn_irq_task_cb;
extern	ll_task_callback_2_t					ll_acl_conn_mlp_task_cb;

extern	ll_task_callback_t						ll_acl_slave_irq_task_cb;
extern	ll_task_callback_t						ll_acl_slave_mlp_task_cb;
extern	ll_task_callback_t						ll_acl_master_irq_task_cb;



typedef u32  (*ll_module_adv_callback_t)(u8);
extern ll_module_adv_callback_t				ll_module_adv_cb;

typedef int  (*ll_adv_task_insert_callback_t)(void);
extern ll_adv_task_insert_callback_t		ll_extadv_task_insert_cb;

extern ll_adv_task_insert_callback_t		ll_prichn_scan_rebuild_cb;


typedef int (*ll_procScanPkt_callback_t)(u8 *, u8 *, u32);
typedef int (*ll_procScanDat_callback_t)(u8 *);
typedef void (*ll_set_scan_callback_t)(int);
typedef bool  (*ll_initPkt_callback_t)(u8 *);

extern ll_procScanPkt_callback_t  ll_procScanPktCb;
extern ll_initPkt_callback_t	  ll_initPkt_cb;



u32  blt_ll_get_rx_packet_tick(u8 rf_len);

/******************************* ll_system end *************************************************************************/






/******************************* ll start **********************************************************************/

//TODO:  all debug MACRO should remove at last
#define			DBG_DECRYPTION_ERR_EN							0



#define DBG_ACL_SALVE_EN										1







#define 	    BLMS_CONN_TIMING_EXTEND							0		//TODO

#define			CONNECT_COMPLETE_CALLBACK_IN_MAINLOOP			1




#define			BLMS_STATE_NONE									0

#define			BLMS_STATE_SCHE_START							BIT(0)

#define			BLMS_STATE_ADV									BIT(1)
#define			BLMS_STATE_EXTADV_START							BIT(2)
#define			BLMS_STATE_EXTADV_POST							BIT(3)

#define			BLMS_STATE_SCAN_START							BIT(4)
#define			BLMS_STATE_SCAN_POST							BIT(5)
#define			BLMS_STATE_SEC_SCAN_START						BIT(6)
#define			BLMS_STATE_SEC_SCAN_POST						BIT(7)
#define			BLMS_STATE_PRICHN_SCAN							BIT(8)  //new design

#define			BLMS_STATE_BTX_S								BIT(10)
#define			BLMS_STATE_BTX_E								BIT(11)
#define			BLMS_STATE_BRX_S								BIT(12)
#define			BLMS_STATE_BRX_E								BIT(13)


#define			BLMS_STATE_CIG_E								BIT(16)
#define			BLMS_STATE_CTX_S								BIT(17)  //CIS BTX Start
#define			BLMS_STATE_CTX_E								BIT(18)  //CIS BTX End
#define			BLMS_STATE_CRX_S								BIT(19)	 //CIS BRX Start
#define			BLMS_STATE_CRX_E								BIT(20)  //CIS BRX End
#define			BLMS_STATE_CRX_GRP_E							BIT(21)




#define			BLMS_STATE_BIG_START							BIT(24)
#define			BLMS_STATE_BIG_POST								BIT(25)
#define			BLMS_STATE_BIS_BCST_S							BIT(26)
#define			BLMS_STATE_BIS_BCST_E							BIT(27)
#define			BLMS_STATE_BIS_SYNC_S							BIT(28)
#define			BLMS_STATE_BIS_SYNC_E							BIT(29)



#define 		BLMS_FLG_RF_CONN_DONE 		 					(FLD_RF_IRQ_CMD_DONE  | FLD_RF_IRQ_FIRST_TIMEOUT | FLD_RF_IRQ_RX_TIMEOUT | FLD_RF_IRQ_RX_CRC_2)



#define 		BLMS_ERR_DEBUG(x)



#define			TSKMSK_ACL_CONN_0								BIT(0)
#define			TSKMSK_ACL_MASTER_0								BIT(0)



extern my_fifo_t	hci_tx_iso_fifo;

_attribute_aligned_(4)
typedef struct {
	u8		macAddress_public[6];
	u8		macAddress_random[6];   //host may set this
}ll_mac_t;
extern ll_mac_t  bltMac;

_attribute_aligned_(4)
typedef struct {

//0x00
	u8		leg_adv_en;      //legacy ADV
	u8		leg_scan_en;	 //legacy Scan
	u8		leg_init_en; 	 //legacy Initiate
	u8		ext_adv_en;


	u8		max_master_num;
	u8		max_slave_num;
	u8		cur_master_num;
	u8		cur_slave_num;

	u8		connectEvt_mask;
	u8		disconnEvt_mask;
	u8		conupdtEvt_mask;  //conn_update
	u8 		phyupdtEvt_mask;

	u8		getP256pubKeyEvtPending;
	u8		getP256pubKeystatus;
	u8		generateDHkeyEvtPending;
	u8		generateDHkeyStatus;

	u8		llblock;
	u8		connSync;				//if more than 8 connections, u8 -> u16
	u8		new_conn_forbidden;
	u8		master_connInter;

	u8		manual_rf_done;
	u8		delay_clear_rf_status;
  	u8		hci_cmd_mask;
	u8		conn_mask;

	u8		controller_stack_param_check;
  	u8		acl_master_en;
  	u8		acl_slave_en;
  	u8		acl_packet_length;//7.8.2 LE Read Buffer Size command [v1]

  	u8		maxRxOct;
  	u8		maxTxOct;
  	u8		maxTxOct_master;
  	u8		maxTxOct_slave;

  	u8		cache_txfifo_used;
  	u8		u8_rsvd2[3];

  	u32		dly_start_tick_clr_rf_sts;
  	u32		acl_rx_dma_buff;
  	u32		scan_rx_pri_chn_dma_buff;
  	u32		connDleSendTimeUs;

} st_llms_para_t;

extern _attribute_aligned_(4)	volatile st_llms_para_t  blmsParam;


_attribute_aligned_(4)
typedef struct {
	u32 rx_irq_tick;
	u32 rx_header_tick;
	u32 rx_timeStamp;
} rx_pkt_sts_t;
extern _attribute_aligned_(4)	rx_pkt_sts_t  bltRxPkt;

_attribute_aligned_(4)
typedef struct {
  	u32		ll_aclRxFifo_set	: 1;
  	u32		ll_aclTxMasFifo_set	: 1;
  	u32		ll_aclTxSlvFifo_set	: 1;
  	u32		ll_aclTxCacheFifo_set	: 1;
  	u32		hci_aclRxFifo_set	: 1;
  	u32		hci_isoRxFifo_set	: 1;
  	u32     ll_cisRxFifo_set	: 1;
  	u32		ll_cisTxFifo_set	: 1;
  	u32		ll_cisRxEvtFifo_set	: 1;
  	u32		ll_bisRxEvtFifo_set : 1;
  	u32		ll_bisTxFifo_set	: 1;
  	u32		ll_bisRxFifo_set	: 1;
  	u32     rfu					: 20;  //attention !!!
}st_ll_temp_para_t;

extern _attribute_aligned_(4)	st_ll_temp_para_t  bltempParam;

_attribute_aligned_(4)
typedef struct {
	u8		gLlChannelMap[5];
	u8		rsvd[3]; //align

	u32		hostMapUptCmdTick;
	u32		hostMapUptCmdPending; //BIT(0): master ACL0, BIT(1): master ACL1, etc.
	u32		hostMapUptExecuted;   //BIT(0): master ACL0, BIT(1): master ACL1, etc.
} st_llm_hostChnClassUpt_t;

extern	_attribute_aligned_(4)	st_llm_hostChnClassUpt_t  blmhostChnClassUpt;
extern				u16 scaPpmTbl[8];
extern	volatile	u8	blt_state;
extern	volatile	u32	blms_state;
extern	volatile    int blm_btxbrx_state;
extern				u8	blms_tx_empty_packet[];

typedef     int (*llms_blt_main_loop_post_callback_t)(void);
typedef 	int (*llms_host_mainloop_callback_t)(void);
typedef 	int (*llms_enc_done_callback_t)(u16 connHandle, u8 status, u8 enc_enable);
typedef 	int (*llms_conn_complete_handler_t)(u16 conn, u8 *p);
typedef 	int (*llms_conn_terminate_handler_t)(u16 conn, u8 *p);

extern llms_host_mainloop_callback_t 	llms_host_main_loop_cb;
extern llms_enc_done_callback_t 	 	llms_encryption_done_cb;
extern llms_conn_complete_handler_t 	llms_connComplete_handler;
extern llms_conn_terminate_handler_t 	llms_connTerminate_handler;




int 		blt_llms_procPendingEvent(void);




void 		blt_llms_registerHostMainloopCallback (llms_host_mainloop_callback_t cb);
void 		blt_llms_registerConnectionEncryptionDoneCallback(llms_enc_done_callback_t  cb);
void 		blt_llms_registerConnectionCompleteHandler(llms_conn_complete_handler_t  handler);
void 		blt_llms_registerConnectionTerminateHandler(llms_conn_terminate_handler_t  handler);

static inline void blt_llms_setRxBlockSts(bool block)
{
	blmsParam.llblock = block;
}

static inline u8 blt_llms_getRxBlockSts(void)
{
	return blmsParam.llblock;
}

u8 			blt_llms_push_hold_data(u16 connHandle);
void  		blt_llms_setEncryptionBusy(u16 connHandle, u8 enc_busy);
int  		blt_llms_isEncryptionBusy(u16 connHandle);
u8 			blt_llms_pushTxfifo_hold (u16 connHandle, u8 *p);  //TODO: used in master SDP
bool 		blt_ll_pushAclChClassUpdPkt(u16 connHandle, u8 *pChm);
u8  		blt_llms_smpPushEncPkt (u16 connHandle, u8 type);
void 		blt_llms_smpSecurityProc(u16 connHandle);

void 		blt_csa1_calculateChannelTable(u8* chm, u8 hop, u8 *ptbl);

void 		blt_ll_procDlePending(u16 connHandle);
u8 			blt_ll_getOwnAddrType(u16 connHandle);
u8*			blt_ll_getOwnMacAddr(u16 connHandle, u8 addr_type);

/******************************* ll end ************************************************************************/












/******************************* ll_pm start ******************************************************************/
#ifndef			BLMS_PM_ENABLE
#define			BLMS_PM_ENABLE									1
#endif


#define 		PPM_IDX_LONG_SLEEP_MIN						3	//300 ppm
#define 		PPM_IDX_SHORT_SLEEP_MIN						5 	//500 ppm
#define 		PPM_IDX_MAX									10  //1000 ppm


typedef struct {
	u8		pm_inited;
	u8		conn_no_suspend;
	u8		slave_no_sleep;
	u8		min_tolerance_us;	// < 256uS, default 0


	u32		long_sleep_disable_tick;
}st_llms_pm_t;

extern _attribute_aligned_(4) st_llms_pm_t  blmsPm;


void 	ble_hw_recover(void);
u32		ble_pm_calculate_timing (int low_power_en, u32 bt_tick, u32 bt_sniff_interval);
int 	ble_pm_process(int type);

/******************************* ll_pm end ********************************************************************/












/******************************* ll_misc start ******************************************************************/
u32 		blt_llms_conn_calc_access_addr_v2(void);

u32 		blt_calBit1Number(u32 dat);

u32 		blt_calBit1NumberV2(u64 num);

u32 		blt_phy_getRfPacketTime_us(u8 rf_len, le_phy_type_t phy, le_coding_ind_t ci);
/******************************* ll_misc end ********************************************************************/




/******************************* ll_device start ******************************************************************/
#if (LL_MULTI_SLAVE_MAC_ENABLE)
	#define			SLAVE_DEV_MAX								3
#else
	#define			SLAVE_DEV_MAX								1
#endif

typedef enum {
	MASTER_DEVICE_INDEX_0     = 0,

	SLAVE_DEVICE_INDEX_0      = 0,
	SLAVE_DEVICE_INDEX_1      = 1,
	SLAVE_DEVICE_INDEX_2      = 2
}local_dev_ind_t;   //local device index
/******************************* ll_device end ********************************************************************/




/******************************* HCI start ******************************************************************/
ble_sts_t  		blc_hci_reset(void);
ble_sts_t 		blc_hci_le_readBufferSize_cmd(u8 *pData);
ble_sts_t 		blc_hci_le_getLocalSupportedFeatures(u8 *features);
ble_sts_t		blc_hci_receiveHostACLData(u16 connHandle, u8 PB_Flag, u8 BC_Flag, u8 *pData );
ble_sts_t 		blc_hci_setTxDataLength (u16 connHandle, u16 tx, u16 txtime);
ble_sts_t 		blc_hci_readSuggestedDefaultTxDataLength (u8 *tx, u8 *txtime);
ble_sts_t 		blc_hci_writeSuggestedDefaultTxDataLength (u16 tx, u16 txtime);
ble_sts_t		blc_hci_readMaximumDataLength(hci_le_readMaxDataLengthCmd_retParam_t  *para);
ble_sts_t 		blc_hci_le_getRemoteSupportedFeatures(u16 connHandle);
ble_sts_t 		blc_hci_le_readChannelMap(u16 connHandle, u8 *returnChannelMap);

/******************************* HCI end ********************************************************************/






#endif /* LL_STACK_H_ */
