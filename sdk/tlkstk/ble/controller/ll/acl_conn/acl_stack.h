/********************************************************************************************************
 * @file     acl_stack.h
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

#ifndef ACL_STACK_H_
#define ACL_STACK_H_


#include "tlkstk/ble/ble_common.h"
#include "tlkstk/ble/ble_stack.h"
#include "tlkstk/ble/ble_config.h"
#include "tlkstk/ble/ble_format.h"
#include "tlkstk/ble/controller/phy/phy_stack.h"
#include "tlkalg/crypt/aes/tlkalg_aes_ccm.h"


#ifndef			FIX_BOUNDARY_RX_NO_DMALEN_REWRITE_CAUSE_SW_CRC_ERROR
#define			FIX_BOUNDARY_RX_NO_DMALEN_REWRITE_CAUSE_SW_CRC_ERROR		1
#endif




/******************************* acl_conn start ******************************************************************/
#define			ACL_TXFIFO_4K_LIMITATION_WORKAROUND				1

#define			BLM_CONN_HANDLE									BIT(7)
#define			BLS_CONN_HANDLE									BIT(6)
#define 		BLT_ACL_CONN_MASTER_HANDLE						BLM_CONN_HANDLE
#define 		BLT_ACL_CONN_SLAVE_HANDLE						BLS_CONN_HANDLE
#define			BLT_ACL_CONN_HANDLE								(BLT_ACL_CONN_MASTER_HANDLE | BLT_ACL_CONN_SLAVE_HANDLE)


#define			CONN_IDX_MASK									15    //15 is very safe. cause total connection number will not exceed 15




/////////////////////////////////////////////////////////////////////////////
#define			HANDLE_STK_FLAG									BIT(15)
#define 		BLMS_STACK_USED_TX_FIFO_NUM      				2
/////////////////////////////////////////////////////////////////////////////

#define			MAX_OCTETS_DATA_LEN_27							27
#define			MAX_OCTETS_DATA_LEN_EXTENSION					251


#define			LL_PACKET_OCTET_TIME(n)							((n) * 8 + 112)

#define 		DATA_LENGTH_REQ_PENDING							1
#define 		DATA_LENGTH_RSP_PENDING							2

/**
 *  @brief  Definition for LLID of Data Physical Channel PDU header field
 */
#define 				LLID_RESERVED				0x00
#define 				LLID_DATA_CONTINUE			0x01
#define 				LLID_DATA_START				0x02
#define 				LLID_CONTROL				0x03


//////////////////////////////// LL ENC ///////////////////////////////////
#define			MS_LL_ENC_OFF									0
#define			MS_LL_ENC_REQ									1
#define			MS_LL_ENC_RSP_T									2
#define			MS_LL_ENC_START_REQ_T							3
#define			MS_LL_ENC_START_REQ								4
#define			MS_LL_ENC_START_RSP_T							5
#define			MS_LL_ENC_PAUSE_REQ								6
#define			MS_LL_ENC_PAUSE_RSP_T							7
#define			MS_LL_ENC_PAUSE_RSP								8
#define			MS_LL_REJECT_IND_T								9
#define         MS_LL_ENC_SMP_INFO_S                			10
#define         MS_LL_ENC_SMP_INFO_E                			11

#define			MS_CONN_ENC_CHANGE								BIT(7)
#define			MS_CONN_ENC_REFRESH								BIT(6)
#define			MS_CONN_ENC_REFRESH_T							BIT(5)





#define		 	CONN_STATUS_DISCONNECT							0   //disconnect must be "0"
#define			CONN_STATUS_COMPLETE							1
#define			CONN_STATUS_ESTABLISH							2


#define 		BLMS_CONN_CREATE_RX_MAX_TRY_NUM					4
#define 		BLMS_CONN_UPDATE_BRX_MAX_TRY_NUM				4





#define			CONN_CHANNEL_MAP								1
#define			CONN_UPDATE_PARAM								2
#define         CONN_PHY_UPDATE_IND_CMD                         3

#define			CONN_UPDATE_CMD									BIT(1)
#define			CONN_UPDATE_PENDING								BIT(2)
#define			CONN_UPDATE_NEARBY								BIT(3)
#define			CONN_UPDATE_SYNC								BIT(4)




#define			DATA_MARK_UPDATE_POTENTIAL						BIT(0)


#define 		FSM_STATE_IDLE									0
#define 		FSM_STATE_START									1



typedef	struct {
	u16		size;
	u8		size_div_16;
	u8		num;

	u8		mask;
	u8		wptr;
	u8		rptr;
	u8		rsvd;
	u8*		p_base;
}	acl_rx_fifo_t;


typedef	struct {
	u16		size;
	u16		conn_full_size;
	u8		depth;
	u8		real_num;
	u8		logic_num;
	u8		mask;
	u8*		p_base;
}	acl_tx_fifo_t;

extern  acl_rx_fifo_t			blt_rxfifo;
extern	acl_tx_fifo_t			blt_m_txfifo;
extern	acl_tx_fifo_t			blt_s_txfifo;


#if (ACL_TXFIFO_4K_LIMITATION_WORKAROUND)

typedef	struct {
	u16		size;
	u8		num;
	u8		mask;
	u8		wptr;
	u8		rptr;
	u8		rsvd1;
	u8		rsvd2;
	u8*		p;
}	acl_cache_txfifo_t;

extern  acl_cache_txfifo_t			blt_cache_txFifo;

bool blt_acl_pushCacheTxfifo(u16 connHandle, u8 *p);
void blt_acl_cache_tx_fifo_to_hw_tx_fifo(void);

#endif



_attribute_aligned_(4)
typedef struct {
	u16		connEffectiveMaxRxOctets;
	u16		connEffectiveMaxTxOctets;
	u16 	connMaxRxOctets;
	u16 	connMaxTxOctets;
	u16		connRemoteMaxRxOctets;
	u16 	connRemoteMaxTxOctets;
	u16		supportedMaxRxOctets;
	u16		supportedMaxTxOctets;

	u8	 	connInitialMaxTxOctets;  //u8 is enough
	u8		connMaxTxRxOctets_req;
	u8		connRxDiff100;
	u8		connTxDiff100;
}ll_data_extension_t;



typedef union {
	struct{
		u8		update_cmd;
		u8		update_param;
		u8		update_map;
		u8      update_phy;
	};
	u32 update_pack;
}conn_updt_t;

typedef union {
	struct{
		u8 		peer_terminate;
		u8 		local_terminate;
		u8		terminate_reason;
		u8		rsvd;
	};
	u32 termin_pack;
}conn_trmt_t;


typedef union {
	struct{
		u8		connect_evt;		//Event triggered by IRQ
		u8		disconn_evt;		//Event triggered by IRQ
		u8		conn_update_evt;	//Event triggered by IRQ
		u8		phy_update_evt;		//Event triggered by IRQ
	};
	u32 irqevt1_pack;
}irq_evt_1_t;


_attribute_aligned_(4)
typedef struct {
	conn_updt_t		conn_update_union;
	irq_evt_1_t		irq_event1_union;
	conn_trmt_t		conn_termin_union;

	u8		tx_wptr;
	u8		tx_rptr;
	u8		tx_num;
	u8		conn_fifo_flag;

	u8		max_fifo_num;
	u8		save_flg;
	u8		local_sn;
	u8		local_nesn;

	u8      conn_dma_tx_rptr;
	u8		peer_last_rfLen;
	u8		peer_last_sn;
	u8		role;

	u8		connState;			 // 0/Conn_Complete/Conn_Establish
	u8		chn_idx;
	u8		conn_chn;
	u8		conn_sca;

	u8		acl_conn_Index;
	u8		connUpt_inst_jump;
	u8		conn_updateEvt_pending; //hold this event until new conne_param_used
	u8      blt_tx_pkt_hold;

	u8		conn_receive_packet;
	u8		conn_receive_new_packet;
	u8  	connTermTxFifoWptr;
	u8		conn_chn_hop;

	u8      ll_enc_busy;
	u8      encryption_evt;		//Event triggered by MainLoop
	u8		encryption_tmp_st;
	u8 		ll_ver_exg_flg;

	u8		sync_timing;
	u8		sync_num;
	u8		peer_adr_type;
	u8     	conn_established;

	u8      conn_winsize_next;		//u8 is enough
	u8      btxbrx_busy;
	u8  	sentLlOpcode;
	u8 		remoteFeatureReq;	//Only used by slave role

	u8		rcvdLlOpcode;		//not used now
	u8 		interval_type;
	u8		task_skip_cnt;
	u8		fsm_state;

	u8		sequence_cur;
	u8		sequence_last;
	u8		priority_cur;
	u8		tight_1st_pkt;  //first packet timing is tight, may not complete TX & RX

	u16		conn_mark_inst;
	u8		conn_mark_chn;
	u8		readRemoteVersion;


	u16		conn_interval_n_1m25;   //u8: max interval 320mS, not enough for big interval such as 4S;  u16: max 81.9S
	u16		conn_interval_next;		//u8: max interval 320mS, not enough for big interval such as 4S;  u16: max 81.9S
	u16     conn_offset_next;		//u8: max interval 320mS, not enough for big interval such as 4S;  u16: max 81.9S
	u16		pm_error_us;

	u16		conn_inst_next;
	u16     conn_phy_inst_next;
	u16		conn_param_inst_next;
	u16		conn_map_inst_next;	    //update conn_param & update_map should separate, update map may delay to process due to BRX slot dropped

	u16		conn_latency;
	u16		conn_latency_next;
	u16		conn_timeout_next;     //Note: unit 10mS
	u16		connHandle;

	u16		conn_inst;
	u16		enc_ediv;       //EDIV

	u16		rx_tifs_tx_us;
	u16		conn_rx_overflow_evt;

	u32		conn_terminate_tick;
	u32 	conn_interval_tick;
	u32		conn_timeout;
	u32     conn_complete_tick;
	u32     conn_established_tick;  
	u32		conn_software_timeout;
	u32		conn_irq_tick;
	u32		tick_1st_rx;

	u32		conn_tick;
	u32		aclAccessAddr;
	u32		aclCrcInit;

	u32     conn_crc_revert; //revert crc24 init value
	u32 	ll_remoteFeature;

	u32		connMasterExpectTime;
	u32		conn_anchorTick;

#if (BLUETOOTH_VER == BLUETOOTH_VER_5_2)
	u32 	ll_remoteFeature1; //for core5.2 featureSet
#endif

	u32		conn_inst_u32;


	u8		peer_adr[6];
	u8		conn_chn_map[5];
	u8		conn_chn_map_next[5];


	u8		enc_random[8];  //RANDOM

	u32		enc_ivm;       //master IVm
	u8 		enc_skdm[8];   //master SKDm
	u32		enc_ivs;       //slave  IVs
	u8 		enc_skds[8];   //slave  SKDs

	//0xA8, 168

	u8		chn_tbl[40];
	u8		chn_new_tbl[40];
	ble_crypt_para_t	crypt;   //40 Byte

	//0x120, 288


#if (LL_FEATURE_ENABLE_LE_2M_PHY | LL_FEATURE_ENABLE_LE_CODED_PHY)
  	ll_conn_phy_t		connPhyCtrl;
#endif


#if (LL_FEATURE_ENABLE_LE_DATA_LENGTH_EXTENSION)
	ll_data_extension_t ext_data;
#endif


#if(LL_FEATURE_SUPPORT_CHANNEL_SELECTION_ALGORITHM2)
	u8 channel_used_num;
	u8 peer_chnSel;
	u8 conn_chnsel;
	u8 channel_new_used_num;

	u16 channel_id;
	u16 reserve;
#endif


#if (BLS_PROC_MASTER_UPDATE_REQ_IN_IRQ_ENABLE)
	u32		conn_pkt_rcvd;
	u32		conn_pkt_rcvd_no;
	u8 		*conn_pkt_dec_pending;
#endif

} st_ll_conn_t;

extern _attribute_aligned_(4)	volatile st_ll_conn_t	AA_blms[];   // AA_ just for debug
#define blms	AA_blms


_attribute_aligned_(4)
typedef struct {
	u8		conn_rx_num;
	u8		conn_tx_num;
	u8		updateCmd_pending;
	u8		conn_notAck_num;

	u32 	tick_connectDevice;
} acl_conn_para_t;  //ACL connection parameters

extern	_attribute_aligned_(4)	acl_conn_para_t		aclConn_param;



extern st_ll_conn_t  *blms_pconn;
extern volatile	int		blms_conn_sel;






typedef int (*blms_LTK_req_callback_t)(u16 handle, u8* rand, u16 ediv);
//extern blms_LTK_req_callback_t blt_llms_ltk_request;

typedef bool (*ll_push_fifo_handler_t) (u16, u8 *);
extern  ll_push_fifo_handler_t				ll_push_tx_fifo_handler;



/**
 * @brief      get ACL connection control block.
 * @param[in]  connHandle - ACL connection handle.
 * @return     return the ACL connection control block pointer
 */
void*		blt_ll_getAclConnPtr(u16 connHandle);

int 		blt_acl_conn_interrupt_task (int flag);
int 		blt_acl_conn_mainloop_task (int flag, void *p);


int 		blms_connect_common(st_ll_conn_t *pc, rf_packet_connect_t * pInit);
int 		blms_start_common_1  (st_ll_conn_t *pc);
int 		blms_start_common_2  (st_ll_conn_t *pc);

int 		blms_post_common_1(st_ll_conn_t *pc);
int 		blms_post_common_no_terminate(void);

void 		irq_acl_conn_rx(void);
int 		irq_acl_conn_tx(void);

int 		blt_ll_acl_conn_sync_process(int sync_ok);

void 		blt_conn_inst_chn_update(st_ll_conn_t *pc, u16 increase);

int 		blt_ll_procConnectionEvent(u16 connHandle,  st_ll_conn_t* pc); //


u8  		blt_llms_get_tx_fifo_max_num (u16 connHandle);

void 		blt_llms_pushToHWfifo(void);
void 		blt_llms_saveTxfifoRptr(void);

bool 		blt_llms_pushTxfifo(u16 connHandle, u8 *p);
void 		blt_llms_push_fifo_hw(void);
void 		blt_llms_update_fifo_sw(void);
bool 		blt_llmsPushLlCtrlPkt(u16 connHandle, u8 opcode, u8*pkt);

bool		blt_llms_unknownRsp(u16 connHandle, u8 unknownType);
bool		blt_llms_rejectInd(u16 connHandle, u8 opCode, u8 errCode, u8 extRejectInd);
int			blt_llms_main_loop_data (u16 connHandle, u8 *raw_pkt);


void 		blt_llms_registerLtkReqEvtCb(blms_LTK_req_callback_t evtCbFunc);


int  		blt_llms_isConnectionEncrypted(u16 connHandle);

int 		blt_llms_startEncryption(u16 connHandle ,u16 ediv, u8* random, u8* ltk);

u8 			blt_llms_getConnState(u16 connHandle);

void		blt_ll_acl_conn_mainloop(void);

void		blt_ll_reset_acl_conn(void);

void		blt_ll_rstDmaChnForRF(void);

void		blt_ll_set_conn_soft_timeout (st_ll_conn_t *pc, u16 conn_interval);

static inline u8 blt_llms_get_connEffectiveMaxTxOctets_by_connIdx(u16 conn_idx)
{
	#if (LL_FEATURE_ENABLE_LE_DATA_LENGTH_EXTENSION)
		return blms[conn_idx].ext_data.connEffectiveMaxTxOctets;
	#else
		return 27;
	#endif
}

static inline u8 blt_llms_get_connEffectiveMaxRxOctets_by_connIdx(u16 conn_idx)
{
	#if (LL_FEATURE_ENABLE_LE_DATA_LENGTH_EXTENSION)
		return blms[conn_idx].ext_data.connEffectiveMaxRxOctets;
	#else
		return 27;
	#endif
}

ble_sts_t   blt_hci_ltkRequestReply(u16 connHandle,  u8*ltk);
ble_sts_t   blt_hci_ltkRequestNegativeReply(u16 connHandle);
ble_sts_t   blt_ll_exchangeDataLength (u16 connHandle, u8 opcode, u16 maxTxOct);


/******************************* acl_conn end   ******************************************************************/







/******************************* acl_slave start ******************************************************************/
#if (MCU_CORE_TYPE == MCU_CORE_9518)
	#define	AES_CCM_DEC_US										200		//timing for running "aes_ll_ccm_decryption"
#else //kite/vulture
	#error "feed tested value"
#endif


#define			SLAVE_SYNC_CONN_CREATE							BIT(0)
#define			SLAVE_SYNC_CONN_UPDATE							BIT(1)
#define			SLAVE_SYNC_CONN_UPT_FAIL						BIT(2)
#define			MASTER_SYNC_CONN_CREATE							BIT(3)
#define			MASTER_SYNC_CONN_UPDATE							BIT(4)

#define			BRX_LEFT_EARLY_TICK	   							(200 * SYSTEM_TIMER_TICK_1US)
#define			BRX_TAIL_MARGIN_TICK	   						( 50 * SYSTEM_TIMER_TICK_1US)
#define			BTX_LEFT_EARLY_TICK	   							(150 * SYSTEM_TIMER_TICK_1US)
#define			BTX_TAIL_MARGIN_TICK	   						( 50 * SYSTEM_TIMER_TICK_1US)
#define 		PAYLOAD_27B_TIFS_27B_1MPHY_US					806   // (328*2 + 150) = 656 + 150 = 806(contain 4B mic len)


#define 		CONN_INTERVAL_SML								1
#define			CONN_INTERVAL_MID								2
#define			CONN_INTERVAL_BIG								3









_attribute_aligned_(4)
typedef struct{
	s8		brx_pkt_rcvd;
	s8		brx_pkt_miss;
	u8		blt_brx_synced ;
	u8 		rsvd;
	u32		last_brx;
	int		connection_offset;
	int		brx_ref;
}st_ll_timing_t;


_attribute_aligned_(4)
typedef struct {
	u8		acl_slv_Index;
	s8		brx_pkt_miss;
	u8		brx_cal_synced;
	u8		brx_cal_long_sleep_synced;


	s8      ppm_cal_idx_cur;
	s8      ppm_cal_idx_last;
	s8 		ppm_idx;
	u8		u8_rsvd1[1];

	u16 	conn_tolerance_us;
	u16		tolerance_max_us;

	s32		conn_offset_tick;
	u32		tick_last_1st_rx;
  	u32		sleep_sys_ms;
  	u32		sleep_32k_rc;




	u8		interval_level;
	u8		conn_new_param;
	u8		latency_wakeup_flg;
	u8		slave_sleep_flg;

	u16		latency_used;
	u16		latency_available;


	u32		brx_mark_tick;
	u32		connExpectTime;
	u32		conn_start_time;
	u32		conn_duration;
  	u32		latency_wakeup_tick;

#if (BLS_PROC_MASTER_UPDATE_REQ_IN_IRQ_ENABLE)
	u8		blt_buff_conn[24];
#endif

} st_lls_conn_t;

extern	_attribute_aligned_(4)	st_lls_conn_t	blmsSlave[];
extern	st_lls_conn_t	*bls_pconn;
extern	int				bls_conn_sel;




typedef int (*ll_acl_slave_terminate_irq_callback_t)(void);


int 	blt_acl_slave_interrupt_task (int flag);
int 	blt_acl_slave_mainloop_task (int flag);

bool 	blt_s_connect (rf_packet_connect_t * pInit, u8 *raw_pkt);

u32 	blt_brx_start (int link);
int 	blt_brx_post(int link);

void 	blt_brx_timing_init(void);
void 	blt_brx_timing_update(void);

int	 	blt_ll_set_slave_conn_interval_level (st_ll_conn_t *pc, st_lls_conn_t *ps, u16 conn_interval);

void  	blt_ll_reset_acl_slave(void);

void 	blt_set_fsmTimeout_intLevel(st_lls_conn_t *pcs, u16 interval);

void 	bls_ll_setNewConnection_timing(st_lls_conn_t *pcs, u16 interval, u8 winsize, u16 winOffset);


u8  	blt_ll_getRealTxFifoNumber (u16 connHandle);

void 	blt_acl_slave_rx_procUpdateReq(u8 *raw_pkt);
void 	blt_acl_slave_tx_procUpdateReq(void);
int 	blt_ll_conn_chn_phy_update(st_ll_conn_t* pc, u8 *pkt);
void 	blt_acl_slave_slotgap_procUpdateReq(void);
/******************************* acl_slave end   ******************************************************************/









/******************************* acl_master start ******************************************************************/

#define			ACL_MASTER_FIFONUM								12

_attribute_aligned_(4)
typedef struct {

	u16		evtCnt_mark_btx;
	u16		u16_rsvd;

	u32     sche_tick_conn;

} st_llm_conn_t;

extern	_attribute_aligned_(4)	st_llm_conn_t	blmsMaster[];
extern	st_llm_conn_t	*blm_pconn;

typedef int (*ll_acl_master_terminate_irq_callback_t)(void);

int 	blt_acl_master_interrupt_task (int flag);
int 	blt_ll_ctrlAclChClassUpd(unsigned char *pChm);
bool 	blms_m_connect (rf_packet_connect_t * pInit, u8 *raw_pkt);
u32 	blt_btx_start (int link);
int 	blt_btx_post (int);

void 	blt_btx_end_schedule(int link, u8 priority);

void 	blt_ll_registerAclMasterTerminateIrqCb(ll_acl_master_terminate_irq_callback_t  cb);

/******************************* acl_master end   ******************************************************************/




/******************************* acl_phy start ******************************************************************/

void blt_ll_sendPhyReq(u16 connHandle);
void blt_ll_sendPhyUpdateInd(u16 connHandle);
int  blt_ll_updateConnPhy(u16 connHandle);
int  blt_ll_switchConnPhy(u16 connHandle);
int  blt_ll_resetConnPhy(ll_conn_phy_t* pconn_phy);

/**
 * @brief       this function is used to set PHY type for connection, same as API: blc_ll_setPhy
 * @param[in]	connHandle - Connection_Handle Range:0x0000 to 0x0EFF
 * @param[in]	all_phys - preference PHY for TX & RX
 * @param[in]	tx_phys - preference PHY for TX
 * @param[in]	rx_phys - preference PHY for RX
 * @param[in]	phy_options - LE coding indication prefer
 * @return     status, 0x00:  succeed
 * 					   other: failed
 */
ble_sts_t 	blc_hci_le_setPhy(hci_le_setPhyCmd_param_t* para);

/******************************* acl_phy end   ******************************************************************/





#endif /* ACL_STACK_H_ */
