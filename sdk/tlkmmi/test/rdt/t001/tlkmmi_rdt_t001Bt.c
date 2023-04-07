/********************************************************************************************************
 * @file     tlkmmi_rdt_t001Bt.c
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
#if (TLKMMI_TEST_ENABLE)
#include "../../tlkmmi_testStdio.h"
#if (TLK_TEST_RDT_ENABLE)
#include "../tlkmmi_rdtStd.h"
#if (TLKMMI_RDT_CASE_T001_ENABLE)
#include "tlkmmi_rdt_t001.h"
#include "tlkmmi_rdt_t001Bt.h"
#include "../bt/tlkmmi_rdt_bt.h"


static void tlkmmi_rdt_t001BtAclConnectCB(uint16 handle, uint08 status);
static void tlkmmi_rdt_t001BtAclEncryptCB(uint16 handle, uint08 status);
static void tlkmmi_rdt_t001BtAclDisconnCB(uint16 handle, uint08 reason);
static void tlkmmi_rdt_t001BtScoConnectCB(uint16 handle, uint08 status);
static void tlkmmi_rdt_t001BtScoDisconnCB(uint16 handle, uint08 reason);
static void tlkmmi_rdt_t001BtPrfConnectCB(uint16 handle, uint08 ptype, uint08 status);
static void tlkmmi_rdt_t001BtPrfDisconnCB(uint16 handle, uint08 ptype, uint08 reason);

static void tlkmmi_rdt_t001BtSetPeerInfoDeal(uint08 *pData, uint16 dataLen);


static tlkmmi_rdt_t001Bt_t sTlkMmiRdtT001Bt = {0};


void tlkmmi_rdt_t001BtStart(void)
{
	sTlkMmiRdtT001Bt.busys = RDT_CASE_T001_BTBUSY_NONE;
	tlkmmi_rdt_btRegAclConnectCB(tlkmmi_rdt_t001BtAclConnectCB);
	tlkmmi_rdt_btRegAclEncryptCB(tlkmmi_rdt_t001BtAclEncryptCB);
	tlkmmi_rdt_btRegAclDisconnCB(tlkmmi_rdt_t001BtAclDisconnCB);
	tlkmmi_rdt_btRegScoConnectCB(tlkmmi_rdt_t001BtScoConnectCB);
	tlkmmi_rdt_btRegScoDisconnCB(tlkmmi_rdt_t001BtScoDisconnCB);
	tlkmmi_rdt_btRegPrfConnectCB(tlkmmi_rdt_t001BtPrfConnectCB);
	tlkmmi_rdt_btRegPrfDisconnCB(tlkmmi_rdt_t001BtPrfDisconnCB);
	if(tlkmmi_rdt_t001GetRole() == TLKMMI_RDT_ROLE_DUT){
		if(tlkmmi_rdt_btStartScan(true, true) != TLK_ENONE){
			sTlkMmiRdtT001Bt.busys |= RDT_CASE_T001_BTBUSY_START_SCAN;
		}
	}else{
		if(sTlkMmiRdtT001Bt.devClass == 0){
			sTlkMmiRdtT001Bt.busys |= RDT_CASE_T001_BTBUSY_WAIT_PEER_ATTR;
		}else{
			tlkmmi_rdt_btConnect(sTlkMmiRdtT001Bt.btaddr, sTlkMmiRdtT001Bt.devClass, sTlkMmiRdtT001Bt.initRole);
		}
	}
}
void tlkmmi_rdt_t001BtClose(void)
{
	if(tlkmmi_rdt_t001GetRole() == TLKMMI_RDT_ROLE_DUT){
		tlkmmi_rdt_btCloseScan();
	}else{
		tlkmmi_rdt_btDisconn(0);
	}
	tlkmmi_rdt_btRegAclConnectCB(nullptr);
	tlkmmi_rdt_btRegAclDisconnCB(nullptr);
	tlkmmi_rdt_btRegScoConnectCB(nullptr);
	tlkmmi_rdt_btRegScoDisconnCB(nullptr);
	tlkmmi_rdt_btRegPrfConnectCB(nullptr);
	tlkmmi_rdt_btRegPrfDisconnCB(nullptr);
}

bool tlkmmi_rdt_t001BtTimer(void)
{
	if((sTlkMmiRdtT001Bt.busys & RDT_CASE_T001_BTBUSY_START_SCAN) != 0){
		if(tlkmmi_rdt_btStartScan(true, true) != TLK_ENONE){
			sTlkMmiRdtT001Bt.busys |= RDT_CASE_T001_BTBUSY_START_SCAN;
		}else{
			sTlkMmiRdtT001Bt.busys &= ~RDT_CASE_T001_BTBUSY_START_SCAN;
		}
	}
	if((sTlkMmiRdtT001Bt.busys & RDT_CASE_T001_BTBUSY_CLOSE_SCAN) != 0){
		if(tlkmmi_rdt_btCloseScan() != TLK_ENONE){
			sTlkMmiRdtT001Bt.busys |= RDT_CASE_T001_BTBUSY_CLOSE_SCAN;
		}else{
			sTlkMmiRdtT001Bt.busys &= ~RDT_CASE_T001_BTBUSY_CLOSE_SCAN;
		}
	}
	if((sTlkMmiRdtT001Bt.busys & RDT_CASE_T001_BTBUSY_WAIT_PEER_ATTR) != 0){
		if(sTlkMmiRdtT001Bt.devClass == 0){
			sTlkMmiRdtT001Bt.busys |= RDT_CASE_T001_BTBUSY_WAIT_PEER_ATTR;
		}else{
			sTlkMmiRdtT001Bt.busys &= ~RDT_CASE_T001_BTBUSY_WAIT_PEER_ATTR;
			tlkmmi_rdt_btConnect(sTlkMmiRdtT001Bt.btaddr, sTlkMmiRdtT001Bt.devClass, sTlkMmiRdtT001Bt.initRole);
		}
	}
	
	return true;
}

void tlkmmi_rdt_t001BtInput(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	if(msgID == TLKMMI_RDT_T001_MSGID_SET_PEER_INFO){
		
		tlkmmi_rdt_t001BtSetPeerInfoDeal(pData, dataLen);
	}
}

static void tlkmmi_rdt_t001BtSetPeerInfoDeal(uint08 *pData, uint16 dataLen)
{
	if(dataLen < 10){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t001BtInput: error length");
		return;
	}
	tmemcpy(sTlkMmiRdtT001Bt.btaddr, pData, 6);
	tmemcpy(&sTlkMmiRdtT001Bt.devClass, pData+6, 3);
	sTlkMmiRdtT001Bt.initRole = pData[9];
}


static void tlkmmi_rdt_t001BtAclConnectCB(uint16 handle, uint08 status)
{
	
}
static void tlkmmi_rdt_t001BtAclEncryptCB(uint16 handle, uint08 status)
{
//	tlkmmi_rdt_btDisconn(0);
}
static void tlkmmi_rdt_t001BtAclDisconnCB(uint16 handle, uint08 reason)
{
	if(tlkmmi_rdt_t001GetState() != TLK_STATE_OPENED) return;
	tlkmmi_rdt_btClsPeerInfo();
	if(tlkmmi_rdt_t001GetRole() == TLKMMI_RDT_ROLE_DUT){
		if(tlkmmi_rdt_btStartScan(true, true) != TLK_ENONE){
			sTlkMmiRdtT001Bt.busys |= RDT_CASE_T001_BTBUSY_START_SCAN;
		}
	}else{
		if(sTlkMmiRdtT001Bt.devClass == 0){
			sTlkMmiRdtT001Bt.busys |= RDT_CASE_T001_BTBUSY_WAIT_PEER_ATTR;
		}else{
			tlkmmi_rdt_btConnect(sTlkMmiRdtT001Bt.btaddr, sTlkMmiRdtT001Bt.devClass, 0);
		}
	}
}
static void tlkmmi_rdt_t001BtScoConnectCB(uint16 handle, uint08 status)
{
	
}
static void tlkmmi_rdt_t001BtScoDisconnCB(uint16 handle, uint08 reason)
{
	
}
static void tlkmmi_rdt_t001BtPrfConnectCB(uint16 handle, uint08 ptype, uint08 status)
{
	
}
static void tlkmmi_rdt_t001BtPrfDisconnCB(uint16 handle, uint08 ptype, uint08 reason)
{
	
}



#endif //#if (TLKMMI_RDT_CASE_T001_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)

