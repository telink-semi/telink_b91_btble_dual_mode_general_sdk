/********************************************************************************************************
 * @file     tlkmmi_rdt_t002Bt.c
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
#if (TLKMMI_RDT_CASE_T002_ENABLE)
#include "tlkmmi_rdt_t002.h"
#include "tlkmmi_rdt_t002Bt.h"
#include "../bt/tlkmmi_rdt_bt.h"


static void tlkmmi_rdt_t002BtAclConnectCB(uint16 handle, uint08 status);
static void tlkmmi_rdt_t002BtAclEncryptCB(uint16 handle, uint08 status);
static void tlkmmi_rdt_t002BtAclDisconnCB(uint16 handle, uint08 reason);
static void tlkmmi_rdt_t002BtScoConnectCB(uint16 handle, uint08 status);
static void tlkmmi_rdt_t002BtScoDisconnCB(uint16 handle, uint08 reason);
static void tlkmmi_rdt_t002BtPrfConnectCB(uint16 handle, uint08 ptype, uint08 status);
static void tlkmmi_rdt_t002BtPrfDisconnCB(uint16 handle, uint08 ptype, uint08 reason);

static void tlkmmi_rdt_t002BtSetPeerInfoDeal(uint08 *pData, uint16 dataLen);


static tlkmmi_rdt_t002Bt_t sTlkMmiRdtT002Bt = {0};


void tlkmmi_rdt_t002BtStart(void)
{
	sTlkMmiRdtT002Bt.busys = RDT_CASE_T002_BTBUSY_NONE;
	tlkmmi_rdt_btRegAclConnectCB(tlkmmi_rdt_t002BtAclConnectCB);
	tlkmmi_rdt_btRegAclEncryptCB(tlkmmi_rdt_t002BtAclEncryptCB);
	tlkmmi_rdt_btRegAclDisconnCB(tlkmmi_rdt_t002BtAclDisconnCB);
	tlkmmi_rdt_btRegScoConnectCB(tlkmmi_rdt_t002BtScoConnectCB);
	tlkmmi_rdt_btRegScoDisconnCB(tlkmmi_rdt_t002BtScoDisconnCB);
	tlkmmi_rdt_btRegPrfConnectCB(tlkmmi_rdt_t002BtPrfConnectCB);
	tlkmmi_rdt_btRegPrfDisconnCB(tlkmmi_rdt_t002BtPrfDisconnCB);
	tlkmmi_rdt_btRegPrfDisconnCB(tlkmmi_rdt_t002BtPrfDisconnCB);
	if(tlkmmi_rdt_t002GetRole() == TLKMMI_RDT_ROLE_DUT){


		if(tlkmmi_rdt_btStartScan(true, true) != TLK_ENONE){

			sTlkMmiRdtT002Bt.busys |= RDT_CASE_T002_BTBUSY_START_SCAN;
		}
	}else{
		if(sTlkMmiRdtT002Bt.devClass == 0){
			sTlkMmiRdtT002Bt.busys |= RDT_CASE_T002_BTBUSY_WAIT_PEER_ATTR;
		}else{
			tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002BtStart:peeraddr--[%x-%x-%x-%x-%x-%x]",\

					sTlkMmiRdtT002Bt.btaddr[0], sTlkMmiRdtT002Bt.btaddr[1], \
					sTlkMmiRdtT002Bt.btaddr[2], sTlkMmiRdtT002Bt.btaddr[3], \
					sTlkMmiRdtT002Bt.btaddr[4], sTlkMmiRdtT002Bt.btaddr[5]);


			tlkmmi_rdt_btConnect(sTlkMmiRdtT002Bt.btaddr, sTlkMmiRdtT002Bt.devClass, sTlkMmiRdtT002Bt.initRole);
		}
	}
}
void tlkmmi_rdt_t002BtClose(void)
{
	if(tlkmmi_rdt_t002GetRole() == TLKMMI_RDT_ROLE_DUT){
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

bool tlkmmi_rdt_t002BtTimer(void)
{
	if((sTlkMmiRdtT002Bt.busys & RDT_CASE_T002_BTBUSY_START_SCAN) != 0){
		if(tlkmmi_rdt_btStartScan(true, true) != TLK_ENONE){
			sTlkMmiRdtT002Bt.busys |= RDT_CASE_T002_BTBUSY_START_SCAN;
		}else{
			sTlkMmiRdtT002Bt.busys &= ~RDT_CASE_T002_BTBUSY_START_SCAN;
		}
	}
	if((sTlkMmiRdtT002Bt.busys & RDT_CASE_T002_BTBUSY_CLOSE_SCAN) != 0){
		if(tlkmmi_rdt_btCloseScan() != TLK_ENONE){
			sTlkMmiRdtT002Bt.busys |= RDT_CASE_T002_BTBUSY_CLOSE_SCAN;
		}else{
			sTlkMmiRdtT002Bt.busys &= ~RDT_CASE_T002_BTBUSY_CLOSE_SCAN;
		}
	}
	if((sTlkMmiRdtT002Bt.busys & RDT_CASE_T002_BTBUSY_WAIT_PEER_ATTR) != 0){
		if(sTlkMmiRdtT002Bt.devClass == 0){
			sTlkMmiRdtT002Bt.busys |= RDT_CASE_T002_BTBUSY_WAIT_PEER_ATTR;
		}else{
			sTlkMmiRdtT002Bt.busys &= ~RDT_CASE_T002_BTBUSY_WAIT_PEER_ATTR;
			tlkmmi_rdt_btConnect(sTlkMmiRdtT002Bt.btaddr, sTlkMmiRdtT002Bt.devClass, sTlkMmiRdtT002Bt.initRole);
		}
	}
	
	return true;
}

void tlkmmi_rdt_t002BtInput(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	if(msgID == TLKMMI_RDT_T002_MSGID_SET_PEER_INFO){
		tlkmmi_rdt_t002BtSetPeerInfoDeal(pData, dataLen);
	}
}

static void tlkmmi_rdt_t002BtSetPeerInfoDeal(uint08 *pData, uint16 dataLen)
{
	if(dataLen < 10){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002BtInput: error length");
		return;
	}
	tmemcpy(sTlkMmiRdtT002Bt.btaddr, pData, 6);
	tmemcpy(&sTlkMmiRdtT002Bt.devClass, pData+6, 3);
	sTlkMmiRdtT002Bt.initRole = pData[9];
}


static void tlkmmi_rdt_t002BtAclConnectCB(uint16 handle, uint08 status)
{
	
}
static void tlkmmi_rdt_t002BtAclEncryptCB(uint16 handle, uint08 status)
{
	if(tlkmmi_rdt_t002GetRole() == TLKMMI_RDT_ROLE_AUT){
//		btp_sdpclt_connect(handle);
	}

}
static void tlkmmi_rdt_t002BtAclDisconnCB(uint16 handle, uint08 reason)
{
	if(tlkmmi_rdt_t002GetState() != TLK_STATE_OPENED) return;
	tlkmmi_rdt_btClsPeerInfo();
	if(tlkmmi_rdt_t002GetRole() == TLKMMI_RDT_ROLE_DUT){
		if(tlkmmi_rdt_btStartScan(true, true) != TLK_ENONE){
			sTlkMmiRdtT002Bt.busys |= RDT_CASE_T002_BTBUSY_START_SCAN;
		}
	}else{
		if(sTlkMmiRdtT002Bt.devClass == 0){
			sTlkMmiRdtT002Bt.busys |= RDT_CASE_T002_BTBUSY_WAIT_PEER_ATTR;
		}else{
			tlkmmi_rdt_btConnect(sTlkMmiRdtT002Bt.btaddr, sTlkMmiRdtT002Bt.devClass, 0);
		}
	}
}
static void tlkmmi_rdt_t002BtScoConnectCB(uint16 handle, uint08 status)
{
	
}
static void tlkmmi_rdt_t002BtScoDisconnCB(uint16 handle, uint08 reason)
{
	
}
static void tlkmmi_rdt_t002BtPrfConnectCB(uint16 handle, uint08 ptype, uint08 status)
{
	if(tlkmmi_rdt_t002GetRole() == TLKMMI_RDT_ROLE_AUT){
//		btp_rfcomm_connect(handle);
	}
}
static void tlkmmi_rdt_t002BtPrfDisconnCB(uint16 handle, uint08 ptype, uint08 reason)
{
	
}



#endif //#if (TLKMMI_RDT_CASE_T002_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)

