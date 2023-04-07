/********************************************************************************************************
 * @file     tlkmmi_lemgrAcl.c
 *
 * @brief    This is the source file for BTBLE SDK
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
#include "tlkapi/tlkapi_stdio.h"
#if (TLKMMI_LEMGR_ENABLE)
#include "tlkstk/ble/ble.h"
#include "tlkmdi/le/tlkmdi_le_common.h"
#include "tlkmdi/le/tlkmdi_le_device_manage.h"
#include "tlkmdi/le/tlkmdi_le_simple_sdp.h"
#include "tlkmdi/le/tlkmdi_le_custom_pair.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrAdapt.h"
#include "tlkmmi/lemgr/tlkmmi_lemgr.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrMsgOuter.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrCtrl.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrAtt.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrAcl.h"

#if BLE_IOS_ANCS_ENABLE
	#include "tlklib/ios_service/ancs/ancs.h"
	#include "tlklib/ios_service/ancs/ancsDemo.h"
#endif

#if BLE_IOS_AMS_ENABLE
	#include "tlklib/ios_service/ams/ams.h"
	#include "tlklib/ios_service/ams/amsDemo.h"
#endif



static int tlkmmi_lemgr_coreEventCB(uint32 evtID, uint08 *pData, int dataLen);
static int tlkmmi_lemgr_hostEventCB(uint32 evtID, uint08 *pData, int dataLen);
static int tlkmmi_lemgr_connectCompleteEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_lemgr_disconnCompleteEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_lemgr_gattDataCB(uint16 connHandle, uint08 *pkt);
#if (TLKMMI_LEMGR_OTA_SERVER_ENABLE)
static void tlkmmi_lemgr_enterOTAEvt(void);
static void tlkmmi_lemgr_leaveOTAEvt(int result);
#endif

extern uint32 flash_sector_mac_address;

/***************** ACL connection L2CAP layer MTU TX & RX data FIFO allocation, End **********************************/

/**
 * @brief	ACL RX buffer. size & number defined in app_buffer.h
 * ACL RX FIFO is shared by all connections to hold LinkLayer RF RX data, user should define this buffer
 * if either ACl connection master role or ACl connection slave role is used.
 */
//_attribute_ble_data_retention_
_attribute_data_no_init_  
static uint08 sTlkmmiLemgrAclRxFifo[TLKMMI_LEMGR_ACL_RX_FIFO_SIZE * TLKMMI_LEMGR_ACL_RX_FIFO_NUM] = {0};

/**
 * @brief	ACL TX buffer. size & number defined in app_buffer.h
 *  ACL MASTER TX buffer should be defined only when ACl connection master role is used.
 *  ACL SLAVE  TX buffer should be defined only when ACl connection slave role is used.
 */
//_attribute_ble_data_retention_ uint08	tlkmmiLemgrAclMstTxFifo[TLKMMI_LEMGR_ACL_MASTER_TX_FIFO_SIZE * TLKMMI_LEMGR_ACL_MASTER_TX_FIFO_NUM * TLKMMI_LEMGR_MASTER_MAX_NUM] = {0}; //no BLE master in this project
_attribute_ble_data_retention_
static uint08 sTlkmmiLemgrAclSlvTxFifo[TLKMMI_LEMGR_ACL_SLAVE_TX_FIFO_SIZE * TLKMMI_LEMGR_ACL_SLAVE_TX_FIFO_NUM * TLKMMI_LEMGR_SLAVE_MAX_NUM] = {0};

/**
 * @brief	ACL TX cache FIFO, used to workaround problem caused by
 *          "(ACL_xxx_TX_FIFO_SIZE * (ACL_xxx_TX_FIFO_NUM - 1)) must be less than 4096 (4K)"
 *          CAL_LL_ACL_TX_FIFO_SIZE(230) = 240, CAL_LL_ACL_TX_FIFO_SIZE(231) = 256
 *          when ACL_xxx_MAX_TX_OCTETS > 230, ACL_xxx_TX_FIFO_SIZE >= 256, ACL_xxx_TX_FIFO_NUM can only be 9
 *          if you want TX FIFO 16 or 32, can use Cache TX FIFO to extend this function.
 *
 */
#if (TLKMMI_LEMGR_WORKAROUND_TX_FIFO_4K_LIMITATION_EN && (TLKMMI_LEMGR_MASTER_MAX_TX_OCTETS > 230 || TLKMMI_LEMGR_SLAVE_MAX_TX_OCTETS > 230))
	//_attribute_ble_data_retention_  //iRam not enough now, add later
static uint08 sTlkmmiLemgrAclCacheTxFifo[260*16] = {0}; //size must 260, number must be 16 or 32
#endif
/******************** ACL connection LinkLayer TX & RX data FIFO allocation, End ***********************************/

/***************** ACL connection L2CAP layer MTU TX & RX data FIFO allocation, Begin ********************************/
//_attribute_ble_data_retention_	uint08 mtu_m_rx_fifo[TLKMMI_LEMGR_MASTER_MAX_NUM * MTU_M_BUFF_SIZE_MAX]; //no BLE master in this project
_attribute_ble_data_retention_	
static uint08 sTlkmmiLemgrMtuSlvRxFifo[TLKMMI_LEMGR_SLAVE_MAX_NUM * TLKMMI_LEMGR_MTU_S_BUFF_SIZE_MAX];
_attribute_ble_data_retention_	
static uint08 sTlkmmiLemgrMtuSlvTxFifo[TLKMMI_LEMGR_SLAVE_MAX_NUM * TLKMMI_LEMGR_MTU_S_BUFF_SIZE_MAX];
/***************** ACL connection L2CAP layer MTU TX & RX data FIFO allocation, End **********************************/

static uint08 sTlkMmiLemgrAdvDataLen = 24;
static uint08 sTlkMmiLemgrAdvData[31] = {
	 10, DT_COMPLETE_LOCAL_NAME,   'T','L','K','W','A','T','C','H','0',
	 2,	 DT_FLAGS, 								0x05, 					// BLE limited discoverable mode and BR/EDR not supported
	 3,  DT_APPEARANCE, 						0xC1, 0x03, 			// Generic Keyboard Generic category
	 5,  DT_INCOMPLT_LIST_16BIT_SERVICE_UUID,	0x12, 0x18, 0x0F, 0x18,	// incomplete list of service class UUIDs (0x1812, 0x180F)
};
static uint08 sTlkMmiLemgrScanRspLen = 11;
static uint08 sTlkMmiLemgrScanRsp[] = {
	 10, DT_COMPLETE_LOCAL_NAME,   'T','L','K','W','A','T','C','H','0',
};
#if (TLKMMI_LEMGR_OTA_SERVER_ENABLE)
_attribute_ble_data_retention_	int sTlkMmiLemgrOtaIsWorking = 0;
#endif


static bool tlkmmi_lemgr_timer(tlkapi_timer_t *pTimer, uint32 userArg);


static tlkmmi_lemgr_acl_t sTlkMmiLemgrAcl;


#if (!BLE_IOS_ANCS_ENABLE)

void latency_turn_off_onceEx(){
	extern void 		bls_pm_setManualLatency(u16 latency);
	bls_pm_setManualLatency(0);
}
#else

_attribute_data_retention_ u8	bls_needSendSecurityReq = 0;
_attribute_data_retention_ u8	bls_is_ios = 0;
_attribute_data_retention_ u32 	tick_findAncsSrv;
_attribute_data_retention_ u32 	tick_encryptionFinish;
_attribute_data_retention_ u32 	tick_conn_update_parm = 0;

enum{
	NO_NEED_SEND_SEC_REQ = 0,
	NEED_SEND_SEC_REQ = 1,
};

enum{
	SYSTEM_TYPE_ANDROID = 0,
	SYSTEM_TYPE_IOS		= 1,
};


typedef enum{
	BLE_DISCONNECTED	= 0,
	BLE_CONNECTED 		= 1,
}BLE_CONNECTION_STATE;
_attribute_ble_data_retention_ u8 bleConnState = BLE_DISCONNECTED;

void app_setCurBleConnState(u8 state){
	bleConnState = state;
}

u8	app_getCurBleConnState(){
	return bleConnState;
}


void app_setSystemType(u8 isIOS){
	bls_is_ios = isIOS;
}

u8 app_getSystemType(){
	return bls_is_ios;
}

void app_setSendSecReqFlag(u8 en){
	bls_needSendSecurityReq = en;
}

u8 app_getSendSecReqFlag(){
	return bls_needSendSecurityReq;
}

void task_ancs_proc(u16 connHandle, u8 *p)
{
	if(ancsFuncIsEn()){
		ancsStackCallback(p);
	}

	if(app_getSendSecReqFlag() == NO_NEED_SEND_SEC_REQ ){
		rf_packet_l2cap_req_t_special_ancs *req = (rf_packet_l2cap_req_t_special_ancs *)p;
		//req = ancs_attPktTrans(p);


		if((req->chanId == 0x04)
			&& (req->opcode == ATT_OP_FIND_BY_TYPE_VALUE_RSP)){///ATT
			app_setSendSecReqFlag(NEED_SEND_SEC_REQ);
			//blc_smp_sendSecurityRequest();
			blt_smp_sendSecurityRequest(connHandle);
			tick_findAncsSrv = 0x00;
			my_dump_str_data(1, "security request send", 0, 0);
			blc_smp_setSecurityLevel_slave(Unauthenticated_Pairing_with_Encryption);  //LE_Security_Mode_1_Level_2
		}else{
			if((req->opcode == ATT_OP_ERROR_RSP) ///op_code
				&& (req->data[0] == ATT_OP_FIND_BY_TYPE_VALUE_RSP)){///err_op_code
				//tick_findAncsSrv = clock_time() | 1;
				//att not found
				app_setSendSecReqFlag(NO_NEED_SEND_SEC_REQ);
			}
		}
	}

	return;
}

void task_findAncsService(){
	 _attribute_data_retention_ static u8 ancsRetry = 0;

	if((tick_findAncsSrv & 0x01) && (clock_time_exceed(tick_findAncsSrv, 2 * 1000 * 1000))){
		ancsRetry++;
		tick_findAncsSrv = clock_time() | 0x01;
		ancs_findAncsService();
	}else{
		if((ancsRetry == 3)
			|| (ancsRetry && (tick_findAncsSrv == 0))){
			ancsRetry = 0;
			tick_findAncsSrv = 0;
		}
	}
}
extern u8			blt_llms_getCurrentState(void);
void task_ancsServiceEstablish(){
	if(app_getCurBleConnState() != BLE_CONNECTED){
		return;  // no connection
	}

	_attribute_data_retention_ static u8 flag_ancsBuidSrv = 0;
	task_findAncsService();///for check ANCS service enable

	if((tick_encryptionFinish & 0x01) && (clock_time_exceed(tick_encryptionFinish, 2 * 1000 * 1000))){
		ancsFuncSetEnable(1);
		tick_encryptionFinish = 0x00;
		flag_ancsBuidSrv = 1;
		my_dump_str_data(DUMP_ACL_MSG,"ancs enable", 0, 0);
	}else{
		if(flag_ancsBuidSrv){
			if(ancsGetConnState() == ANCS_CONNECTION_ESTABLISHED){///must be enabled after the ANCS established
				#if BLE_IOS_AMS_ENABLE
				amsInit(1);
				amsFuncSetEnable(1);
				#endif
				flag_ancsBuidSrv = 0;
			}
		}
	}
}

void app_curSystemType(){
	if(app_getSendSecReqFlag() == NEED_SEND_SEC_REQ){
		app_setSystemType(SYSTEM_TYPE_IOS);
	}else{
		if(ancsGetConnState() == ANCS_CONNECTION_ESTABLISHED){
			app_setSystemType(SYSTEM_TYPE_IOS);
		}
	}

}

void app_ancsTask(){
	if(app_getCurBleConnState() == BLE_CONNECTED){
			task_ancsServiceEstablish();
			app_curSystemType();
			ancs_mainLoopTask();
	}
}

#endif


#if (BLE_IOS_AMS_ENABLE)

void app_amsTask(){
	if(app_getCurBleConnState() == BLE_CONNECTED){
		ams_mainLoopTask();
	}
}

#endif



int tlkmmi_lemgr_aclInit(void)
{
	/* for 1M Flash, flash_sector_mac_address equals to 0xFF000
	 * for 2M Flash, flash_sector_mac_address equals to 0x1FF000 */
	uint08 mac_public[6];
	uint08 mac_random_static[6];

	tmemset(&sTlkMmiLemgrAcl, 0, sizeof(tlkmmi_lemgr_acl_t));
	
	
	blc_initMacAddress(flash_sector_mac_address, mac_public, mac_random_static);
	tlkmmi_lemgr_setAddr1(mac_public, mac_random_static);

	//////////// LinkLayer Initialization  Begin /////////////////////////
	blc_ll_initBasicMCU();
	blc_ll_initStandby_module(mac_public);		//mandatory
	blc_ll_initLegacyAdvertising_module();		//mandatory for BLE slave
	blc_ll_initAclConnection_module();			//mandatory for BLE slave & master
	blc_ll_initAclSlaveRole_module();			//mandatory for BLE slave
	
	//blc_ll_init2MPhyCodedPhy_feature();		//This feature is not supported by default
	
	#if((TLKMMI_LEMGR_MASTER_MAX_NUM + TLKMMI_LEMGR_SLAVE_MAX_NUM + 1) > MAX_BLE_LINK)
		#error MAX BLE lINK not enough
	#endif
	
	blc_ll_setMaxConnectionNumber(TLKMMI_LEMGR_MASTER_MAX_NUM, TLKMMI_LEMGR_SLAVE_MAX_NUM);
	blc_ll_setAclConnMaxOctetsNumber(TLKMMI_LEMGR_CONN_MAX_RX_OCTETS, TLKMMI_LEMGR_MASTER_MAX_TX_OCTETS, TLKMMI_LEMGR_SLAVE_MAX_TX_OCTETS);
	
	/* all ACL connection share same RX FIFO */
	blc_ll_initAclConnRxFifo(sTlkmmiLemgrAclRxFifo, TLKMMI_LEMGR_ACL_RX_FIFO_SIZE, TLKMMI_LEMGR_ACL_RX_FIFO_NUM);
	/* ACL Slave TX FIFO */
	blc_ll_initAclConnSlaveTxFifo(sTlkmmiLemgrAclSlvTxFifo, TLKMMI_LEMGR_ACL_SLAVE_TX_FIFO_SIZE, TLKMMI_LEMGR_ACL_SLAVE_TX_FIFO_NUM, TLKMMI_LEMGR_SLAVE_MAX_NUM);
	
	
	#if (TLKMMI_LEMGR_WORKAROUND_TX_FIFO_4K_LIMITATION_EN && (TLKMMI_LEMGR_MASTER_MAX_TX_OCTETS > 230 || TLKMMI_LEMGR_SLAVE_MAX_TX_OCTETS > 230))
	/* extend TX FIFO size for MAX_TX_OCTETS > 230 if user want use 16 or 32 FIFO */
	blc_ll_initAclConnCacheTxFifo(sTlkmmiLemgrAclCacheTxFifo, 260, 16);
	#endif
	//////////// LinkLayer Initialization  End /////////////////////////


	//////////// HCI Initialization  Begin /////////////////////////
	blc_hci_registerControllerDataHandler(blt_l2cap_pktHandler);
	blc_hci_registerControllerEventHandler(tlkmmi_lemgr_coreEventCB); //controller hci event to host all processed in this func
	
	/* bluetooth event */
	blc_hci_setEventMask_cmd(HCI_EVT_MASK_DISCONNECTION_COMPLETE);
	/* bluetooth low energy(LE) event */
	blc_hci_le_setEventMask_cmd(HCI_LE_EVT_MASK_CONNECTION_COMPLETE | HCI_LE_EVT_MASK_CONNECTION_UPDATE_COMPLETE);
	//////////// HCI Initialization  End /////////////////////////

	uint08 check_status = blc_controller_check_appBufferInitialization();
	if(check_status != BLE_SUCCESS){
		/* here user should set some log to know which application buffer incorrect*/
		#if (BLMS_DEBUG_EN)
			BLMS_ERR_DEBUG(0x88880000 | check_status);
		#endif
		tlkapi_error(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "application buffer incorrect");
	}
	
	//////////// Host Initialization  Begin /////////////////////////
	/* Host Initialization */
	/* GAP initialization must be done before any other host feature initialization !!! */
	blc_gap_init();
	
	/* L2CAP Initialization */
	bls_l2cap_initMtuBuffer(sTlkmmiLemgrMtuSlvRxFifo, TLKMMI_LEMGR_MTU_S_BUFF_SIZE_MAX, sTlkmmiLemgrMtuSlvTxFifo, TLKMMI_LEMGR_MTU_S_BUFF_SIZE_MAX);
	blc_att_setSlaveRxMTUSize(TLKMMI_LEMGR_ATT_MTU_SLAVE_RX_MAX_SIZE); //Do not call this API, the default MTU_SIZE is 23

	/* GATT Initialization */
	tlkmmi_lemgr_attInit();
	blc_gatt_register_data_handler(tlkmmi_lemgr_gattDataCB);
	
		/* SMP Initialization */
	#if (TLKMMI_LEMGR_MASTER_SMP_ENABLE || TLKMMI_LEMGR_SLAVE_SMP_ENABLE)
	blc_smp_configPairingSecurityInfoStorageAddressAndSize(TLK_CFG_FLASH_LE_SMP_PAIRING_ADDR, TLK_CFG_FLASH_LE_SMP_PAIRING_SIZE);
	#endif
	
	#if (TLKMMI_LEMGR_SLAVE_SMP_ENABLE)
	blc_smp_setSecurityLevel_slave(Unauthenticated_Pairing_with_Encryption);  //LE_Security_Mode_1_Level_2
	#else
	blc_smp_setSecurityLevel(No_Security);  //LE_Security_Mode_1_Level_2
	#endif
	//	blc_smp_setPairingMethods(LE_Secure_Connection); //TLKMMI_LEMGR_ATT_MTU_SLAVE_RX_MAX_SIZE or TLKMMI_LEMGR_ATT_MTU_MASTER_RX_MAX_SIZE >= 64
	blc_smp_smpParamInit();
	// Hid device on android7.0/7.1 or later version
	// New paring: send security_request immediately after connection complete
	// reConnect:  send security_request 1000mS after connection complete. If master start paring or encryption before 1000mS timeout, slave do not send security_request.
	blc_smp_configSecurityRequestSending(SecReq_IMM_SEND, SecReq_PEND_SEND, 1000); //if not set, default is:  send "security request" immediately after link layer connection established(regardless of new connection or reconnection)
	
	/* Host (GAP/SMP/GATT/ATT) event process: register host event callback and set event mask */
	blc_gap_registerHostEventHandler(tlkmmi_lemgr_hostEventCB );
	blc_gap_setEventMask( GAP_EVT_MASK_SMP_PAIRING_BEGIN			|  \
						  GAP_EVT_MASK_SMP_PAIRING_SUCCESS			|  \
						  GAP_EVT_MASK_SMP_PAIRING_FAIL 			|  \
						  GAP_EVT_MASK_SMP_SECURITY_PROCESS_DONE);
	//////////// Host Initialization  End /////////////////////////
	
	//////////////////////////// BLE stack Initialization  End //////////////////////////////////
	
	#if (TLK_CFG_PM_ENABLE)
	blc_ll_initPowerManagement_module();
	#endif
	
	//////////////////////////// User Configuration for BLE application ////////////////////////////
	#if (TLKMMI_LEMGR_OTA_SERVER_ENABLE)
	blc_ota_initOtaServer_module();
	blc_ota_setOtaProcessTimeout(200);
	blc_ota_registerOtaStartCmdCb(tlkmmi_lemgr_enterOTAEvt);
	blc_ota_registerOtaResultIndicationCb(tlkmmi_lemgr_leaveOTAEvt);  //debug
	
	extern unsigned int ota_program_offset;
	if(ota_program_offset){
		tlkapi_error(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "Firmware start from 0");
		tlkapi_error(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "Firmware start from 0");
		tlkapi_error(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "Firmware start from 0");
	}
	else{
		tlkapi_error(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "Firmware start from 512K");
		tlkapi_error(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "Firmware start from 512K");
		tlkapi_error(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "Firmware start from 512K");
	}
	#endif
	
	blc_ll_setAdvData((uint08 *)sTlkMmiLemgrAdvData, sTlkMmiLemgrAdvDataLen);
	blc_ll_setScanRspData((uint08 *)sTlkMmiLemgrScanRsp, sTlkMmiLemgrScanRspLen);
	blc_ll_setAdvParam(ADV_INTERVAL_50MS, ADV_INTERVAL_50MS, ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, 0, NULL, BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
	blc_ll_setAdvEnable(BLC_ADV_DISABLE);
	//	blc_ll_setAdvCustomedChannel(37, 37, 37); //debug

	tlkmmi_lemgr_adaptInitTimer(&sTlkMmiLemgrAcl.timer, tlkmmi_lemgr_timer, (uint32)&sTlkMmiLemgrAcl, TLKMMI_LEMGR_TIMEOUT);

	////////////////////////////////////////////////////////////////////////////////////////////////
	tlkmmi_lemgr_setAclName(tlkmmi_lemgr_getName(), tlkmmi_lemgr_getNameLen());
	tlkmmi_lemgr_setAclAddr(tlkmmi_lemgr_getAddr(), 6);
	//tlkmmi_lemgr_startAdv(0, 0);
	extern void btc_rf_set_le_tx_tp_slice (uint08 slice);
	btc_rf_set_le_tx_tp_slice(RF_POWER_P9p11dBm);

	return TLK_ENONE;
}
_attribute_ble_retention_code_
void tlkmmi_lemgr_aclDeepInit(void)
{
	rf_drv_ble_init();

	blc_ll_initBasicMCU();
	blc_ll_recoverDeepRetention();

	reg_rf_irq_status = 0xFFFF;
	plic_interrupt_claim();//claim,clear eip
	plic_interrupt_complete(IRQ15_ZB_RT);//complete
}


int tlkmmi_lemgr_startAdv(uint32 timeout, uint08 advType)
{
	if(sTlkMmiLemgrAcl.connHandle != 0){
		return -TLK_ESTATUS;
	}
	if(timeout == 0) timeout = TLKMMI_LEMGR_ADV_TIMEOUT_DEF;
	else if(timeout < TLKMMI_LEMGR_ADV_TIMEOUT_MIN) timeout = TLKMMI_LEMGR_ADV_TIMEOUT_MIN;
	else if(timeout > TLKMMI_LEMGR_ADV_TIMEOUT_MAX) timeout = TLKMMI_LEMGR_ADV_TIMEOUT_MAX;
	sTlkMmiLemgrAcl.isStart = true;
	sTlkMmiLemgrAcl.advTime = timeout;
	sTlkMmiLemgrAcl.timeout = timeout/TLKMMI_LEMGR_TIMEOUT_MS;
	blc_ll_setAdvEnable(BLC_ADV_ENABLE);
	tlkmmi_lemgr_sendAdvStartEvt();
	tlkmmi_lemgr_adaptInsertTimer(&sTlkMmiLemgrAcl.timer);
	return TLK_ENONE;
}
int tlkmmi_lemgr_closeAdv(void)
{
	sTlkMmiLemgrAcl.isStart = false;
	blc_ll_setAdvEnable(BLC_ADV_DISABLE);
	tlkmmi_lemgr_sendAdvCompleteEvt();
	return TLK_ENONE;
}

int tlkmmi_lemgr_disconn(uint16 handle, uint08 *pPeerAddr)
{
	if(sTlkMmiLemgrAcl.connHandle == 0) return -TLK_ESTATUS;
	if(blc_ll_disconnect(sTlkMmiLemgrAcl.connHandle, 0x13) == BLE_SUCCESS){
		return TLK_ENONE;
	}
	return -TLK_EFAIL;
}

bool tlkmmi_lemgr_aclVolumeInc(void)
{
	uint16 consumer_key;

	if(sTlkMmiLemgrAcl.connHandle == 0) return false;
		
	consumer_key = 0x00e9; //MKEY_VOL_UP;
	blc_gatt_pushHandleValueNotify(sTlkMmiLemgrAcl.connHandle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08*)&consumer_key, 2);
	consumer_key = 0;
	blc_gatt_pushHandleValueNotify(sTlkMmiLemgrAcl.connHandle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08*)&consumer_key, 2);
	
	return true;
}
bool tlkmmi_lemgr_aclVolumeDec(void)
{
	uint16 consumer_key;

	if(sTlkMmiLemgrAcl.connHandle == 0) return false;
		
	consumer_key = 0x00ea; //MKEY_VOL_UP;
	blc_gatt_pushHandleValueNotify(sTlkMmiLemgrAcl.connHandle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08*)&consumer_key, 2);
	consumer_key = 0;
	blc_gatt_pushHandleValueNotify(sTlkMmiLemgrAcl.connHandle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08*)&consumer_key, 2);
	
	return true;
}


int tlkmmi_lemgr_setAclName(uint08 *pName, uint08 nameLen)
{
	uint08 offset;
	
	if(pName == nullptr || nameLen == 0) return TLK_ENONE;
	if(nameLen > 16) nameLen = 16;

	tlkapi_trace(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "tlkmmi_lemgr_setAclName: nameLen-%d", nameLen);

	offset = 0;
	sTlkMmiLemgrAdvData[offset++] = nameLen+1;
	sTlkMmiLemgrAdvData[offset++] = DT_COMPLETE_LOCAL_NAME;
	tmemcpy(sTlkMmiLemgrAdvData+offset, pName, nameLen);
	offset += nameLen;
	sTlkMmiLemgrAdvData[offset++] = 0x02; //BLE limited discoverable mode and BR/EDR not supported
	sTlkMmiLemgrAdvData[offset++] = DT_FLAGS;
	sTlkMmiLemgrAdvData[offset++] = 0x05;
	sTlkMmiLemgrAdvData[offset++] = 0x03; // 384,  Keyboard Control, Generic category
	sTlkMmiLemgrAdvData[offset++] = DT_APPEARANCE;
	sTlkMmiLemgrAdvData[offset++] = 0xC1;
	sTlkMmiLemgrAdvData[offset++] = 0x03;
	sTlkMmiLemgrAdvData[offset++] = 0x05; // incomplete list of service class UUIDs (0x1812, 0x180F)
	sTlkMmiLemgrAdvData[offset++] = DT_INCOMPLT_LIST_16BIT_SERVICE_UUID;
	sTlkMmiLemgrAdvData[offset++] = 0x12;
	sTlkMmiLemgrAdvData[offset++] = 0x18;
	sTlkMmiLemgrAdvData[offset++] = 0x0F;
	sTlkMmiLemgrAdvData[offset++] = 0x18;
	sTlkMmiLemgrAdvDataLen = offset;
	blc_ll_setAdvData((uint08 *)sTlkMmiLemgrAdvData, sTlkMmiLemgrAdvDataLen);

	offset = 0;
	sTlkMmiLemgrScanRsp[offset++] = nameLen+1;
	sTlkMmiLemgrScanRsp[offset++] = DT_COMPLETE_LOCAL_NAME;
	tmemcpy(sTlkMmiLemgrScanRsp+offset, pName, nameLen);
	offset += nameLen;
	sTlkMmiLemgrScanRspLen = offset;
	blc_ll_setScanRspData((uint08 *)sTlkMmiLemgrScanRsp, sTlkMmiLemgrScanRspLen);
	return TLK_ENONE;
}

int tlkmmi_lemgr_setAclAddr(uint08 *pAddr, uint08 addrLen)
{
	return TLK_ENONE;
}


static bool tlkmmi_lemgr_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	if(!sTlkMmiLemgrAcl.isStart) return false;
	if(sTlkMmiLemgrAcl.timeout != 0) sTlkMmiLemgrAcl.timeout --;
	if(sTlkMmiLemgrAcl.timeout == 0){
		tlkmmi_lemgr_closeAdv();
		return false;
	}
	return true;
}

static int tlkmmi_lemgr_coreEventCB(uint32 evtID, uint08 *pData, int dataLen)
{
	if(evtID &HCI_FLAG_EVENT_BT_STD)		//Controller HCI event
	{
		uint08 evtCode = evtID & 0xff;

		//------------ disconnect -------------------------------------
		if(evtCode == HCI_EVT_DISCONNECTION_COMPLETE)  //connection terminate
		{
			tlkmmi_lemgr_disconnCompleteEvt(pData, dataLen);
		}
		else if(evtCode == HCI_EVT_LE_META)  //LE Event
		{
			uint08 subEvt_code = pData[0];
			//------hci le event: le connection complete event---------------------------------
			if (subEvt_code == HCI_SUB_EVT_LE_CONNECTION_COMPLETE)	// connection complete
			{
				//after controller is set to initiating state by host (blc_ll_createConnection(...) )
				//it will scan the specified device(adr_type & mac), when find this adv packet, send a connection request packet to slave
				//and enter to connection state, send connection complete event. but notice that connection complete not
				//equals to connection establish. connection complete means that master controller set all the ble timing
				//get ready, but has not received any slave packet, if slave rf lost the connection request packet, it will
				//not send any packet to master controller
				tlkmmi_lemgr_connectCompleteEvt(pData, dataLen);
			}
			//--------hci le event: le adv report event ----------------------------------------
			else if (subEvt_code == HCI_SUB_EVT_LE_ADVERTISING_REPORT)	// ADV packet
			{
				//after controller is set to scan state, it will report all the adv packet it received by this event
			}
			//------hci le event: le connection update complete event-------------------------------
			else if (subEvt_code == HCI_SUB_EVT_LE_CONNECTION_UPDATE_COMPLETE)	// connection update
			{
				hci_le_connectionUpdateCompleteEvt_t *pUpt = (hci_le_connectionUpdateCompleteEvt_t *)pData;
				tlkapi_trace(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "ble connect update: connInterval-0X%02X connLatency-0X%02X Timeout-0X%02X", pUpt->connInterval,pUpt->connLatency,pUpt->supervisionTimeout);
				//aa_connLatency_temp = pUpt->connLatency;
			}
		}
	}

	
	return 0;
}

static int tlkmmi_lemgr_hostEventCB(uint32 evtID, uint08 *pData, int dataLen)
{
	uint08 event = evtID & 0xFF;

	switch(event)
	{
		case GAP_EVT_SMP_PAIRING_BEGIN:
		{
//			gap_smp_pairingBeginEvt_t *p = (gap_smp_pairingBeginEvt_t *)pData;
		}
		break;

		case GAP_EVT_SMP_PAIRING_SUCCESS:
		{
//			gap_smp_pairingSuccessEvt_t* p = (gap_smp_pairingSuccessEvt_t*)pData;
		}
		break;

		case GAP_EVT_SMP_PAIRING_FAIL:
		{
//			gap_smp_pairingFailEvt_t* p = (gap_smp_pairingFailEvt_t*)pData;
		}
		break;

		case GAP_EVT_SMP_CONN_ENCRYPTION_DONE:
		{

		}
		break;
		
		case GAP_EVT_SMP_SECURITY_PROCESS_DONE:
		{
#if BLE_IOS_ANCS_ENABLE
			app_setSendSecReqFlag(NEED_SEND_SEC_REQ);
#endif
#if BLE_IOS_ANCS_ENABLE
			tick_findAncsSrv = 0x00;
			tick_encryptionFinish = clock_time() | 1;
#endif
#if (BLE_IOS_AMS_ENABLE && (!BLE_IOS_ANCS_ENABLE))
			amsInit(1);
			amsFuncSetEnable(1);
#endif
		}
		break;
		
		case GAP_EVT_SMP_TK_DISPALY:
		{


		}
		break;

		case GAP_EVT_SMP_TK_REQUEST_PASSKEY:
		{

		}
		break;

		case GAP_EVT_SMP_TK_REQUEST_OOB:
		{

		}
		break;

		case GAP_EVT_SMP_TK_NUMERIC_COMPARE:
		{

		}
		break;

		case GAP_EVT_ATT_EXCHANGE_MTU:
		{

		}
		break;

		case GAP_EVT_GATT_HANDLE_VLAUE_CONFIRM:
		{

		}
		break;

		default:
		break;
	}

	return 0;
}





static int tlkmmi_lemgr_connectCompleteEvt(uint08 *pData, uint16 dataLen)
{

	hci_le_connectionCompleteEvt_t *pConnEvt = (hci_le_connectionCompleteEvt_t *)pData;

	sTlkMmiLemgrAcl.connHandle = 0;
	if(pConnEvt->status == BLE_SUCCESS){
		bls_l2cap_requestConnParamUpdate(pConnEvt->connHandle, CONN_INTERVAL_20MS, CONN_INTERVAL_20MS, 49, CONN_TIMEOUT_4S);

		dev_char_info_insert_by_conn_event(pConnEvt);

		tlkapi_trace(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "BLE: conn complete: 0x%x connInterval-0X%02X connLatency-0X%02X Timeout-0X%02X", pConnEvt->connHandle,pConnEvt->connInterval,pConnEvt->slaveLatency,pConnEvt->supervisionTimeout);


		if(pConnEvt->role == LL_ROLE_SLAVE){
			/* Sending a L2CAP request connection parameters update command to master device here can change slave connection timing.
			 * User can also send this command at some other place after connection completed.
			 * SDK Demo do not enable this, because we do not know if user want use this function or what value do user want.
			 * So we give this operation authority to our user.
             */
//			bls_l2cap_requestConnParamUpdate (pConnEvt->connHandle, CONN_INTERVAL_20MS, CONN_INTERVAL_20MS, 49, CONN_TIMEOUT_4S);
		}
		sTlkMmiLemgrAcl.connHandle = pConnEvt->connHandle;
		tmemcpy(sTlkMmiLemgrAcl.connAddr, pConnEvt->peerAddr, 6);

#if (BLE_IOS_ANCS_ENABLE || BLE_IOS_AMS_ENABLE)
		app_setCurBleConnState(BLE_CONNECTED);
#endif

#if BLE_IOS_ANCS_ENABLE
		//ancs_findAncsService(); //should not call when conn back
		app_setSendSecReqFlag(NO_NEED_SEND_SEC_REQ);
		tick_findAncsSrv = clock_time() | 0x01;
		ancs_initial(ANCS_ATT_TYPE_ALL_INCLUDE);
		app_setSystemType(SYSTEM_TYPE_ANDROID);
		ancs_setNewsReceivedStatus(ANCS_SET_NO_NEW_NOTICE);
		tick_conn_update_parm = clock_time() | 1;
#endif
	}
	
	tlkmmi_lemgr_sendAclConnectEvt(pConnEvt->connHandle, pConnEvt->status, 0x02, pConnEvt->peerAddrType, pConnEvt->peerAddr);
	tlkmmi_lemgr_closeAdv();
	
	return 0;
}

static int tlkmmi_lemgr_disconnCompleteEvt(uint08 *pData, uint16 dataLen)
{
	event_disconnection_t *pDiscEvt = (event_disconnection_t *)pData;

	tlkapi_trace(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "BLE: conn disconnect handle-0x%x, reason-%d", 
		pDiscEvt->connHandle, pDiscEvt->reason);

	//terminate reason
	if(pDiscEvt->reason == HCI_ERR_CONN_TIMEOUT){  	//connection timeout

	}
	else if(pDiscEvt->reason == HCI_ERR_REMOTE_USER_TERM_CONN){  	//peer device send terminate command on link layer

	}
	//master host disconnect( blm_ll_disconnect(current_connHandle, HCI_ERR_REMOTE_USER_TERM_CONN) )
	else if(pDiscEvt->reason == HCI_ERR_CONN_TERM_BY_LOCAL_HOST){

	}
	else{

	}

#if BLE_IOS_ANCS_ENABLE
	blc_smp_configSecurityRequestSending(SecReq_IMM_SEND, SecReq_IMM_SEND, 0);
#endif

#if (BLE_IOS_ANCS_ENABLE || BLE_IOS_AMS_ENABLE)
	app_setCurBleConnState(BLE_DISCONNECTED);
#endif


	dev_char_info_delete_by_connhandle(pDiscEvt->connHandle);

	sTlkMmiLemgrAcl.connHandle = 0;
	tlkmmi_lemgr_sendAclDisconnEvt(pDiscEvt->connHandle, pDiscEvt->reason, sTlkMmiLemgrAcl.connAddr);
	tlkmmi_lemgr_startAdv(sTlkMmiLemgrAcl.advTime, 0x00);
	
	return 0;
}





/**
 * @brief      BLE GATT data handler call-back.
 * @param[in]  connHandle     connection handle.
 * @param[in]  pkt             Pointer point to data packet buffer.
 * @return
 */
static int tlkmmi_lemgr_gattDataCB(uint16 connHandle, uint08 *pkt)
{
#if (TLKMMI_LEMGR_MASTER_MAX_NUM)
//	uint16 cur_handle = connHandle;
//	my_dump_str_data(1, "BLE: gatt data", &cur_handle, 2);

	if(dev_char_get_conn_role_by_connhandle(connHandle) == LL_ROLE_MASTER)
	{
		rf_packet_att_t *pAtt = (rf_packet_att_t*)pkt;

		dev_char_info_t* dev_info = dev_char_info_search_by_connhandle (connHandle);
		if(dev_info)
		{
			//-------	user process ------------------------------------------------
			if(pAtt->opcode == ATT_OP_HANDLE_VALUE_NOTI)  //slave handle notify
			{
				
			}
			else if (pAtt->opcode == ATT_OP_HANDLE_VALUE_IND)
			{
				blc_gatt_pushAttHdlValueCfm(connHandle);
			}
		}

		/* The Master does not support GATT Server by default */
		if(!(pAtt->opcode & 0x01)){
			switch(pAtt->opcode){
				case ATT_OP_FIND_INFO_REQ:
				case ATT_OP_FIND_BY_TYPE_VALUE_REQ:
				case ATT_OP_READ_BY_TYPE_REQ:
				case ATT_OP_READ_BY_GROUP_TYPE_REQ:
					blc_gatt_pushErrResponse(connHandle, pAtt->opcode, pAtt->handle, ATT_ERR_ATTR_NOT_FOUND);
					break;
				case ATT_OP_READ_REQ:
				case ATT_OP_READ_BLOB_REQ:
				case ATT_OP_READ_MULTI_REQ:
				case ATT_OP_WRITE_REQ:
				case ATT_OP_PREPARE_WRITE_REQ:
					blc_gatt_pushErrResponse(connHandle, pAtt->opcode, pAtt->handle, ATT_ERR_INVALID_HANDLE);
					break;
				case ATT_OP_EXECUTE_WRITE_REQ:
				case ATT_OP_HANDLE_VALUE_CFM:
				case ATT_OP_WRITE_CMD:
				case ATT_OP_SIGNED_WRITE_CMD:
					//ignore
					break;
				default://no action
					break;
			}
		}
	}
	else{   //GATT data for Slave


	}
#endif

#if BLE_IOS_ANCS_ENABLE
		task_ancs_proc(connHandle, pkt);
#endif
	
#if BLE_IOS_AMS_ENABLE
		if(amsFuncIsEn()){
			amsStackCallback(pkt);
		}
#endif


	return 0;
}






#if (TLKMMI_LEMGR_OTA_SERVER_ENABLE)

/**
 * @brief      this function is used to register the function for OTA start.
 * @param[in]  none
 * @return     none
 */
static void tlkmmi_lemgr_enterOTAEvt(void)
{
	sTlkMmiLemgrOtaIsWorking = 1;

	tlkapi_trace(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "OTA start: %d", ota_program_offset);
}

/**
 * @brief       no matter whether the OTA result is successful or fail.
 *              code will run here to tell user the OTA result.
 * @param[in]   result    OTA result:success or fail(different reason)
 * @return      none
 */
static void tlkmmi_lemgr_leaveOTAEvt(int result)
{
	
}
#endif


#define APP_BLE_TEST_ENABLE    0
#if (APP_BLE_TEST_ENABLE)
#define APP_BLE_TEST_BUFFLEN     (1024+4)
uint32 gAppBleTestTimer = 0;
uint32 gAppBleTestCount = 0;
uint32 gAppBleTestIndex = 0;
uint32 gAppBleTestIntvl = 0; //us
uint16 gAppBleTestSLens = 0;
uint16 gAppBleTestPress = 0;
uint08 gAppBleTestBuff[APP_BLE_TEST_BUFFLEN] = {0};

uint32 gAppBleTestCalcTimer = 0;
uint32 gAppBleTestCalcIndex = 0;
uint32 gAppBleTestCalcRate  = 0;

uint32 gAppBleTestRecvTimer = 0;
uint32 gAppBleTestRecvCount = 0;
uint32 gAppBleTestRecvIndex = 0;
uint32 gAppBleTestRecvLens = 0;
uint32 gAppBleTestRecvRate = 0;


void app_ble_test_start(uint08 isPress, uint16 sendLens, uint32 interval)
{
	uint16 index;

	if(sendLens == 0){
		app_ble_test_stop();
		return;
	}
	if(gAppBleTestTimer != 0) return;
	
	if(sendLens+4 > APP_BLE_TEST_BUFFLEN){
		sendLens = APP_BLE_TEST_BUFFLEN-4;
	}
	if(interval > 10000000) interval = 10000000;
	gAppBleTestTimer = clock_time()|1;
	gAppBleTestCount = 0;
	gAppBleTestIndex = 0;
	gAppBleTestIntvl = interval;
	gAppBleTestSLens = 4+sendLens;
	gAppBleTestPress = isPress;
	for(index=0; index<sendLens; index++){
		gAppBleTestBuff[4+index] = index;
	}

	gAppBleTestCalcTimer = clock_time()|1;

	tlkapi_trace(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "app_ble_test_start: %d %d %d", gAppBleTestPress, gAppBleTestSLens, gAppBleTestIntvl);
}
void app_ble_test_stop(void)
{
	gAppBleTestTimer = 0;
	gAppBleTestIndex = 0;
	gAppBleTestCount = 0;
	gAppBleTestCalcTimer = 0;
	gAppBleTestCalcIndex = 0;
	tlkapi_trace(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "app_ble_test_stop");
}
void app_ble_test_handler(void)
{
	int ret;
	if(gAppBleTestTimer != 0 && clock_time_exceed(gAppBleTestTimer, gAppBleTestIntvl)){
		if(sTlkMmiLemgrAcl.connHandle == 0){
			gAppBleTestTimer = 0;
			gAppBleTestCalcTimer = 0;
			return;
		}else{
			gAppBleTestTimer = clock_time()|1;
		}
		
		gAppBleTestBuff[0] = (gAppBleTestIndex & 0xFF000000) >> 24;
		gAppBleTestBuff[1] = (gAppBleTestIndex & 0x00FF0000) >> 16;
		gAppBleTestBuff[2] = (gAppBleTestIndex & 0x0000FF00) >> 8;
		gAppBleTestBuff[3] = (gAppBleTestIndex & 0x000000FF);
		ret = blc_gatt_pushHandleValueNotify(sTlkMmiLemgrAcl.connHandle, SPP_SERVER_TO_CLIENT_DP_H, 
			gAppBleTestBuff, gAppBleTestSLens);
		if(ret == 0){ //success
			gAppBleTestTimer = clock_time()|1;
			gAppBleTestIndex ++;
			gAppBleTestCount ++;
		}else if(gAppBleTestPress == 0){
			gAppBleTestTimer = clock_time()|1;
			gAppBleTestCount ++;
		}
	}
	if(gAppBleTestCalcTimer != 0 && clock_time_exceed(gAppBleTestCalcTimer, 1000000)){
		uint32 pktCount;
		uint32 pktBytes;
		pktCount = gAppBleTestIndex - gAppBleTestCalcIndex;
		pktBytes = pktCount*gAppBleTestSLens;
		tlkapi_trace(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "Send[sum-%d,snd-%d,ulen-%d,slen-%d,count-%d]", 
			gAppBleTestIndex, pktCount, gAppBleTestSLens, pktBytes, gAppBleTestCount);
		
		gAppBleTestCalcIndex = gAppBleTestIndex;
		gAppBleTestCalcTimer = clock_time()|1;
		gAppBleTestCalcRate  = pktBytes;
	}

	if(gAppBleTestRecvTimer == 0 || clock_time_exceed(gAppBleTestRecvTimer, 1000000)){
		gAppBleTestRecvTimer = clock_time()|1;
		if(gAppBleTestRecvCount != gAppBleTestRecvIndex){
			uint32 pktCount;
			uint32 pktBytes;
			pktCount = (gAppBleTestRecvCount-gAppBleTestRecvIndex);
			pktBytes = pktCount*gAppBleTestRecvLens;
			tlkapi_trace(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "Recv[sum-%d,snd-%d,ulen-%d,slen-%d]", gAppBleTestRecvCount, pktCount, gAppBleTestRecvLens, pktBytes);
			gAppBleTestRecvIndex = gAppBleTestRecvCount;
			gAppBleTestRecvRate  = pktBytes;
		}
	}
}
#endif




#endif //TLKMMI_LEMGR_ENABLE

