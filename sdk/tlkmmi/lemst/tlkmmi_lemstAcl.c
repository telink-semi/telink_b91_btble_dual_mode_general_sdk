/********************************************************************************************************
 * @file	tlkmmi_lemstAcl.c
 *
 * @brief	This is the source file for BTBLE SDK
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
#include "tlkapi/tlkapi_stdio.h"
#if (TLKMMI_LEMST_ENABLE)
#include "tlkstk/ble/ble.h"
#include "tlkmdi/le/tlkmdi_le_common.h"
#include "tlkmdi/le/tlkmdi_le_device_manage.h"
#include "tlkmdi/le/tlkmdi_le_simple_sdp.h"
#include "tlkmdi/le/tlkmdi_le_custom_pair.h"
#include "tlkmmi/lemst/tlkmmi_lemstAdapt.h"
#include "tlkmmi/lemst/tlkmmi_lemst.h"
#include "tlkmmi/lemst/tlkmmi_lemstMsgOuter.h"
#include "tlkmmi/lemst/tlkmmi_lemstCtrl.h"
#include "tlkmmi/lemst/tlkmmi_lemstAcl.h"



static int tlkmmi_lemst_coreEventCB(uint32 evtID, uint08 *pData, int dataLen);
static int tlkmmi_lemst_hostEventCB(uint32 evtID, uint08 *pData, int dataLen);
static int tlkmmi_lemst_advReportEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_lemst_connectCompleteEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_lemst_disconnCompleteEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_lemst_gattDataCB(uint16 connHandle, uint08 *pkt);

extern uint32 flash_sector_mac_address;

/***************** ACL connection L2CAP layer MTU TX & RX data FIFO allocation, End **********************************/

/**
 * @brief	ACL RX buffer. size & number defined in app_buffer.h
 * ACL RX FIFO is shared by all connections to hold LinkLayer RF RX data, user should define this buffer
 * if either ACl connection master role or ACl connection slave role is used.
 */
//_attribute_ble_data_retention_
_attribute_data_no_init_  
static uint08 sTlkmmiLemstAclRxFifo[TLKMMI_LEMST_ACL_RX_FIFO_SIZE * TLKMMI_LEMST_ACL_RX_FIFO_NUM] = {0};

/**
 * @brief	ACL TX buffer. size & number defined in app_buffer.h
 *  ACL MASTER TX buffer should be defined only when ACl connection master role is used.
 */
_attribute_ble_data_retention_ uint08	tlkmmiLemstAclMstTxFifo[TLKMMI_LEMST_ACL_MASTER_TX_FIFO_SIZE * TLKMMI_LEMST_ACL_MASTER_TX_FIFO_NUM * TLKMMI_LEMST_MASTER_MAX_NUM] = {0}; //no BLE master in this project

/**
 * @brief	ACL TX cache FIFO, used to workaround problem caused by
 *          "(ACL_xxx_TX_FIFO_SIZE * (ACL_xxx_TX_FIFO_NUM - 1)) must be less than 4096 (4K)"
 *          CAL_LL_ACL_TX_FIFO_SIZE(230) = 240, CAL_LL_ACL_TX_FIFO_SIZE(231) = 256
 *          when ACL_xxx_MAX_TX_OCTETS > 230, ACL_xxx_TX_FIFO_SIZE >= 256, ACL_xxx_TX_FIFO_NUM can only be 9
 *          if you want TX FIFO 16 or 32, can use Cache TX FIFO to extend this function.
 *
 */
#if (TLKMMI_LEMST_WORKAROUND_TX_FIFO_4K_LIMITATION_EN && (TLKMMI_LEMST_MASTER_MAX_TX_OCTETS > 230 || TLKMMI_LEMST_SLAVE_MAX_TX_OCTETS > 230))
	//_attribute_ble_data_retention_  //iRam not enough now, add later
static uint08 sTlkmmiLemstAclCacheTxFifo[260*16] = {0}; //size must 260, number must be 16 or 32
#endif
/******************** ACL connection LinkLayer TX & RX data FIFO allocation, End ***********************************/

/***************** ACL connection L2CAP layer MTU TX & RX data FIFO allocation, Begin ********************************/
_attribute_ble_data_retention_	uint08 mtu_m_rx_fifo[TLKMMI_LEMST_MASTER_MAX_NUM * TLKMMI_LEMST_MTU_M_BUFF_SIZE_MAX]; //no BLE master in this project
/***************** ACL connection L2CAP layer MTU TX & RX data FIFO allocation, End **********************************/

static tlkmmi_lemst_acl_t sTlkMmiLemstAcl;


int tlkmmi_lemst_aclInit(void)
{
	/* for 1M Flash, flash_sector_mac_address equals to 0xFF000
	 * for 2M Flash, flash_sector_mac_address equals to 0x1FF000 */
	uint08 mac_public[6];
	uint08 mac_random_static[6];

	tmemset(&sTlkMmiLemstAcl, 0, sizeof(tlkmmi_lemst_acl_t));
	
	
	blc_initMacAddress(flash_sector_mac_address, mac_public, mac_random_static);
	tlkmmi_lemst_setAddr(mac_public);

	//////////// LinkLayer Initialization  Begin /////////////////////////
	blc_ll_initBasicMCU();
	blc_ll_initStandby_module(mac_public);		//mandatory
	blc_ll_initLegacyScanning_module();
	blc_ll_initLegacyInitiating_module();
	blc_ll_initAclConnection_module();			//mandatory for BLE slave & master
	blc_ll_initAclMasterRole_module();
	
	//blc_ll_init2MPhyCodedPhy_feature();		//This feature is not supported by default
	
	#if((TLKMMI_LEMST_MASTER_MAX_NUM + TLKMMI_LEMST_SLAVE_MAX_NUM + 1) > MAX_BLE_LINK)
		#error MAX BLE lINK not enough
	#endif
	
	blc_ll_setMaxConnectionNumber(TLKMMI_LEMST_MASTER_MAX_NUM, 0);
	blc_ll_setAclConnMaxOctetsNumber(TLKMMI_LEMST_CONN_MAX_RX_OCTETS, TLKMMI_LEMST_MASTER_MAX_TX_OCTETS, TLKMMI_LEMST_SLAVE_MAX_TX_OCTETS);
	
	/* all ACL connection share same RX FIFO */
	blc_ll_initAclConnRxFifo(sTlkmmiLemstAclRxFifo, TLKMMI_LEMST_ACL_RX_FIFO_SIZE, TLKMMI_LEMST_ACL_RX_FIFO_NUM);
	/* ACL Master TX FIFO */
	blc_ll_initAclConnMasterTxFifo(tlkmmiLemstAclMstTxFifo, TLKMMI_LEMST_ACL_MASTER_TX_FIFO_SIZE, TLKMMI_LEMST_ACL_MASTER_TX_FIFO_NUM, TLKMMI_LEMST_MASTER_MAX_NUM);
	
	#if (TLKMMI_LEMST_WORKAROUND_TX_FIFO_4K_LIMITATION_EN && (TLKMMI_LEMST_MASTER_MAX_TX_OCTETS > 230 || TLKMMI_LEMST_SLAVE_MAX_TX_OCTETS > 230))
	/* extend TX FIFO size for MAX_TX_OCTETS > 230 if user want use 16 or 32 FIFO */
	blc_ll_initAclConnCacheTxFifo(sTlkmmiLemstAclCacheTxFifo, 260, 16);
	#endif
	//////////// LinkLayer Initialization  End /////////////////////////


	//////////// HCI Initialization  Begin /////////////////////////
	blc_hci_registerControllerDataHandler(blt_l2cap_pktHandler);
	blc_hci_registerControllerEventHandler(tlkmmi_lemst_coreEventCB); //controller hci event to host all processed in this func
	
	/* bluetooth event */
	blc_hci_setEventMask_cmd(HCI_EVT_MASK_DISCONNECTION_COMPLETE);
	/* bluetooth low energy(LE) event */
	blc_hci_le_setEventMask_cmd(	HCI_LE_EVT_MASK_CONNECTION_COMPLETE 	\
								| 	HCI_LE_EVT_MASK_ADVERTISING_REPORT		\
								|	HCI_LE_EVT_MASK_CONNECTION_UPDATE_COMPLETE);
	//////////// HCI Initialization  End /////////////////////////

	uint08 check_status = blc_controller_check_appBufferInitialization();
	if(check_status != BLE_SUCCESS){
		/* here user should set some log to know which application buffer incorrect*/
		BLMS_ERR_DEBUG(0x88880000 | check_status);
		tlkapi_error(TLKMMI_LEMST_DBG_FLAG, TLKMMI_LEMST_DBG_SIGN, "application buffer incorrect: 0x%X", check_status);
	}
	
	//////////// Host Initialization  Begin /////////////////////////
	/* Host Initialization */
	/* GAP initialization must be done before any other host feature initialization !!! */
	blc_gap_init();
	
	/* L2CAP Initialization */
	blm_l2cap_initMtuBuffer(mtu_m_rx_fifo, TLKMMI_LEMST_MTU_M_BUFF_SIZE_MAX, NULL,	0);

	/* GATT Initialization */
	blc_gatt_register_data_handler(tlkmmi_lemst_gattDataCB);
	
		/* SMP Initialization */
	#if (TLKMMI_LEMST_MASTER_SMP_ENABLE)
	blc_smp_configPairingSecurityInfoStorageAddressAndSize(TLK_CFG_FLASH_LE_SMP_PAIRING_ADDR, TLK_CFG_FLASH_LE_SMP_PAIRING_SIZE);
	#else
	blc_smp_setSecurityLevel(No_Security);
	#endif
	blc_smp_smpParamInit();
	
	/* Host (GAP/SMP/GATT/ATT) event process: register host event callback and set event mask */
	blc_gap_registerHostEventHandler(tlkmmi_lemst_hostEventCB );
	blc_gap_setEventMask( GAP_EVT_MASK_SMP_PAIRING_BEGIN			|  \
						  GAP_EVT_MASK_SMP_PAIRING_SUCCESS			|  \
						  GAP_EVT_MASK_SMP_PAIRING_FAIL 			|  \
						  GAP_EVT_MASK_SMP_SECURITY_PROCESS_DONE);
	//////////// Host Initialization  End /////////////////////////
	
	//////////////////////////// BLE stack Initialization  End //////////////////////////////////

	//////////////////////////// User Configuration for BLE application ////////////////////////////
	blc_ll_setScanParameter(SCAN_TYPE_ACTIVE, SCAN_INTERVAL_100MS, SCAN_WINDOW_50MS, OWN_ADDRESS_PUBLIC, SCAN_FP_ALLOW_ADV_ANY);
	blc_ll_setScanEnable (BLC_SCAN_ENABLE, DUP_FILTER_DISABLE);

	////////////////////////////////////////////////////////////////////////////////////////////////
	extern void rf_set_le_tx_tp_slice (uint08 slice);
	rf_set_le_tx_tp_slice(RF_POWER_P9p11dBm);

	return TLK_ENONE;
}

int tlkmmi_lemst_startScan(uint32 timeout, uint08 advType)
{
	if(sTlkMmiLemstAcl.connHandle != 0){
		return -TLK_ESTATUS;
	}
	sTlkMmiLemstAcl.isStart = true;
	blc_ll_setScanEnable (BLC_SCAN_ENABLE, DUP_FILTER_DISABLE);
	tlkmmi_lemst_sendScanStartEvt();
	return TLK_ENONE;
}
int tlkmmi_lemst_closeScan(void)
{
	sTlkMmiLemstAcl.isStart = false;
	blc_ll_setScanEnable (BLC_SCAN_DISABLE, DUP_FILTER_DISABLE);
	tlkmmi_lemst_sendScanCompleteEvt();
	return TLK_ENONE;
}

int tlkmmi_lemst_disconn(uint16 handle, uint08 *pPeerAddr)
{
	if(sTlkMmiLemstAcl.connHandle == 0) return -TLK_ESTATUS;
	if(blc_ll_disconnect(sTlkMmiLemstAcl.connHandle, 0x13) == BLE_SUCCESS){
		return TLK_ENONE;
	}
	return -TLK_EFAIL;
}

bool tlkmmi_lemst_aclVolumeInc(void)
{
	uint16 consumer_key;

	if(sTlkMmiLemstAcl.connHandle == 0) return false;
		
//	consumer_key = 0x00e9; //MKEY_VOL_UP;
//	blc_gatt_pushHandleValueNotify(sTlkMmiLemgrAcl.connHandle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08*)&consumer_key, 2);
//	consumer_key = 0;
//	blc_gatt_pushHandleValueNotify(sTlkMmiLemgrAcl.connHandle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08*)&consumer_key, 2);
	
	return true;
}
bool tlkmmi_lemst_aclVolumeDec(void)
{
	uint16 consumer_key;

	if(sTlkMmiLemstAcl.connHandle == 0) return false;
		
//	consumer_key = 0x00ea; //MKEY_VOL_UP;
//	blc_gatt_pushHandleValueNotify(sTlkMmiLemgrAcl.connHandle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08*)&consumer_key, 2);
//	consumer_key = 0;
//	blc_gatt_pushHandleValueNotify(sTlkMmiLemgrAcl.connHandle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08*)&consumer_key, 2);
	
	return true;
}

int tlkmmi_lemst_setAclAddr(uint08 *pAddr, uint08 addrLen)
{
	return TLK_ENONE;
}

static int tlkmmi_lemst_coreEventCB(uint32 evtID, uint08 *pData, int dataLen)
{
	if(evtID &HCI_FLAG_EVENT_BT_STD)		//Controller HCI event
	{
		uint08 evtCode = evtID & 0xff;

		//------------ disconnect -------------------------------------
		if(evtCode == HCI_EVT_DISCONNECTION_COMPLETE)  //connection terminate
		{
			tlkmmi_lemst_disconnCompleteEvt(pData, dataLen);
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
				tlkmmi_lemst_connectCompleteEvt(pData, dataLen);
			}
			//--------hci le event: le adv report event ----------------------------------------
			else if (subEvt_code == HCI_SUB_EVT_LE_ADVERTISING_REPORT)	// ADV packet
			{
				//after controller is set to scan state, it will report all the adv packet it received by this event
				tlkmmi_lemst_advReportEvt(pData, dataLen);
			}
			//------hci le event: le connection update complete event-------------------------------
			else if (subEvt_code == HCI_SUB_EVT_LE_CONNECTION_UPDATE_COMPLETE)	// connection update
			{
				//hci_le_connectionUpdateCompleteEvt_t *pUpt = (hci_le_connectionUpdateCompleteEvt_t *)p;
				//aa_interval_temp = pUpt->connInterval;
				//aa_connLatency_temp = pUpt->connLatency;
			}
		}
	}

	
	return 0;
}

static int tlkmmi_lemst_hostEventCB(uint32 evtID, uint08 *pData, int dataLen)
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

u32	tick_adv_rpt = 0;
static int tlkmmi_lemst_advReportEvt(uint08 *pData, uint16 dataLen)
{
	event_adv_report_t *pa = (event_adv_report_t *)pData;

#if 1   //CodeBlock  just for debug, mac_address filter
	u8 filter_mac[3] = {0x77, 0x66, 0x55};
	if (memcmp(pa->mac +3, filter_mac, 3)) {  //not equal
		return 1;  //no connect
	}
#endif

	#if 1  //debug, print ADV report number every 5 seconds
		if(clock_time_exceed(tick_adv_rpt, 5000000)){
			my_dump_str_data(TLKMMI_LEMST_DBG_FLAG, "Adv report", pa->mac, 6+1+pa->len);
			tick_adv_rpt = clock_time();
		}
	#endif

	u8 status = blc_ll_createConnection( SCAN_INTERVAL_100MS, SCAN_WINDOW_100MS, INITIATE_FP_ADV_SPECIFY,  \
							 pa->adr_type, pa->mac, OWN_ADDRESS_PUBLIC, \
							 CONN_INTERVAL_31P25MS, CONN_INTERVAL_48P75MS, 0, CONN_TIMEOUT_4S, \
							 0, 0xFFFF);

	my_dump_str_data(TLKMMI_LEMST_DBG_FLAG, "Create Conn", &status, 1);

	return 0;
}

static int tlkmmi_lemst_connectCompleteEvt(uint08 *pData, uint16 dataLen)
{

	hci_le_connectionCompleteEvt_t *pConnEvt = (hci_le_connectionCompleteEvt_t *)pData;

	sTlkMmiLemstAcl.connHandle = 0;
	if(pConnEvt->status == BLE_SUCCESS){
		dev_char_info_insert_by_conn_event(pConnEvt);
		tlkapi_trace(TLKMMI_LEMST_DBG_FLAG, TLKMMI_LEMST_DBG_SIGN, "BLE: conn complete: 0x%x", pConnEvt->connHandle);


		if(pConnEvt->role == LL_ROLE_MASTER){
			my_dump_str_data(TLKMMI_LEMST_DBG_FLAG, "Master", pConnEvt->peerAddr, 6);
		}
		sTlkMmiLemstAcl.connHandle = pConnEvt->connHandle;
		tmemcpy(sTlkMmiLemstAcl.connAddr, pConnEvt->peerAddr, 6);
	}
	
	tlkmmi_lemst_sendAclConnectEvt(pConnEvt->connHandle, pConnEvt->status, 0x02, pConnEvt->peerAddrType, pConnEvt->peerAddr);
	
	return 0;
}

static int tlkmmi_lemst_disconnCompleteEvt(uint08 *pData, uint16 dataLen)
{
	event_disconnection_t *pDiscEvt = (event_disconnection_t *)pData;

	tlkapi_trace(TLKMMI_LEMST_DBG_FLAG, TLKMMI_LEMST_DBG_SIGN, "BLE: conn disconnect handle-0x%x, reason-%d", 
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


	dev_char_info_delete_by_connhandle(pDiscEvt->connHandle);

	sTlkMmiLemstAcl.connHandle = 0;
	tlkmmi_lemst_sendAclDisconnEvt(pDiscEvt->connHandle, pDiscEvt->reason, sTlkMmiLemstAcl.connAddr);
	
	return 0;
}





/**
 * @brief      BLE GATT data handler call-back.
 * @param[in]  connHandle     connection handle.
 * @param[in]  pkt             Pointer point to data packet buffer.
 * @return
 */
static int tlkmmi_lemst_gattDataCB(uint16 connHandle, uint08 *pkt)
{
#if (TLKMMI_LEMST_MASTER_MAX_NUM)

	if(dev_char_get_conn_role_by_connhandle(connHandle) == LL_ROLE_MASTER)
	{
		rf_packet_att_t *pAtt = (rf_packet_att_t*)pkt;

		dev_char_info_t* dev_info = dev_char_info_search_by_connhandle (connHandle);
		if(dev_info)
		{
			//-------	user process ------------------------------------------------
			if(pAtt->opcode == ATT_OP_HANDLE_VALUE_NOTI)
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

	return 0;
}


#define APP_BLE_TEST_ENABLE   0
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

	tlkapi_trace(TLKMMI_LEMST_DBG_FLAG, TLKMMI_LEMST_DBG_SIGN, "app_ble_test_start: %d %d %d", gAppBleTestPress, gAppBleTestSLens, gAppBleTestIntvl);
}
void app_ble_test_stop(void)
{
	gAppBleTestTimer = 0;
	gAppBleTestIndex = 0;
	gAppBleTestCount = 0;
	gAppBleTestCalcTimer = 0;
	gAppBleTestCalcIndex = 0;
	tlkapi_trace(TLKMMI_LEMST_DBG_FLAG, TLKMMI_LEMST_DBG_SIGN, "app_ble_test_stop");
}
void app_ble_test_handler(void)
{
	int ret;
	if(gAppBleTestTimer != 0 && clock_time_exceed(gAppBleTestTimer, gAppBleTestIntvl)){
		if(sTlkMmiLemstAcl.connHandle == 0){
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
		ret = blc_gatt_pushHandleValueNotify(sTlkMmiLemstAcl.connHandle, SPP_SERVER_TO_CLIENT_DP_H, 
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
		tlkapi_trace(TLKMMI_LEMST_DBG_FLAG, TLKMMI_LEMST_DBG_SIGN, "Send[sum-%d,snd-%d,ulen-%d,slen-%d,count-%d]", 
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
			tlkapi_trace(TLKMMI_LEMST_DBG_FLAG, TLKMMI_LEMST_DBG_SIGN, "Recv[sum-%d,snd-%d,ulen-%d,slen-%d]", gAppBleTestRecvCount, pktCount, gAppBleTestRecvLens, pktBytes);
			gAppBleTestRecvIndex = gAppBleTestRecvCount;
			gAppBleTestRecvRate  = pktBytes;
		}
	}
}
#endif




#endif //TLKMMI_LEMST_ENABLE

