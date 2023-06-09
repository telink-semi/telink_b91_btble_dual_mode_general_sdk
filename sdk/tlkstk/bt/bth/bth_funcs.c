/********************************************************************************************************
 * @file     bth_funcs.c
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
#if (TLK_STK_BTH_ENABLE && TLK_CFG_TEST_ENABLE)
#include "tlksys/prt/tlkpto_comm.h"
#include "types.h"
#include "bth_stdio.h"
#include "bth_l2cap.h"
#include "bth_signal.h"
#include "bth_funcs.h"
#include "bth_acl.h"
#include "bth_sco.h"
#include "bth_hcicmd.h"


#define BTH_FUNC_DBG_FLAG       ((TLK_MAJOR_DBGID_BTH << 24) | (TLK_MINOR_DBGID_BTH_FUNC << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define BTH_FUNC_DBG_SIGN       nullptr


static uint16 sBthFuncAclHandle;
static uint16 sBthFuncScoHandle;
static uint08 sBthFuncPeerAddr[6];
static const bth_func_item_t scBthFunSet[] = {
	//hci
	{BTH_FUNCID_COMM_START_SCAN, bth_func_commStartScan},
	{BTH_FUNCID_COMM_CLOSE_SCAN, bth_func_commCloseScan},
	{BTH_FUNCID_COMM_CLS_PEER_INFO, bth_func_commClsPeerInfo},
	//acl
	{BTH_FUNCID_ACL_CONNECT, bth_func_aclConnect},
	{BTH_FUNCID_ACL_DISCONN, bth_func_aclDisconn},
	{BTH_FUNCID_ACL_DISCONN_BY_ADDR, bth_func_aclDisconnByAddr},
	{BTH_FUNCID_ACL_CANCEL_CONNECT,  bth_func_aclConnectCancel},
	//sco
	{BTH_FUNCID_SCO_CONNECT, bth_func_scoConnect},
	{BTH_FUNCID_SCO_DISCONN, bth_func_scoDisconn},
	{BTH_FUNCID_SCO_DISCONN_BY_ADDR, bth_func_scoDisconnByAddr},
};



int bth_func_call(uint16 funcID, uint08 *pData, uint16 dataLen)
{
	int index;
	int count;
	
	count = sizeof(scBthFunSet)/sizeof(scBthFunSet[0]);
	for(index= 0; index<count; index++){
		if(scBthFunSet[index].funID == funcID) break;
	}
	if(index == count || scBthFunSet[index].Func == nullptr){
		tlkapi_error(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "Function ID is not in BTH !");
		return -TLK_EFAIL;
	}
	return scBthFunSet[index].Func(pData,dataLen);
}
void bth_func_setAclHandle(uint16 aclHandle)
{
	sBthFuncAclHandle = aclHandle;
}
void bth_func_setScoHandle(uint16 scoHandle)
{
	sBthFuncScoHandle = scoHandle;
}
void bth_func_setPeerAddr(uint08 peerAddr[6])
{
	tmemcpy(sBthFuncPeerAddr, peerAddr, 6);
}



static int bth_func_commStartScan(uint08 *pData, uint16 dataLen)
{
	uint08 scan = 0;

//	INQUIRY_SCAN_ENABLE  = 0x01,
//	PAGE_SCAN_ENABLE	 = 0x02,
	if(dataLen != 0) scan = pData[0];
	if(scan == 0) scan = 0x03;
	
	tlkapi_trace(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_commStartScan: scan[%d]", scan);
	return bth_hci_sendWriteScanEnableCmd(scan);
}
static int bth_func_commCloseScan(uint08 *pData, uint16 dataLen)
{
	tlkapi_trace(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_commCloseScan");
	return bth_hci_sendWriteScanEnableCmd(0);
}
static int bth_func_commClsPeerInfo(uint08 *pData, uint16 dataLen)
{
	tlkapi_trace(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_commClsPeerInfo");
	bth_device_clsItem();
	return TLK_ENONE;
}


static int bth_func_aclConnect(uint08 *pData, uint16 dataLen)
{
	uint08 btAddr[6] = {0};
	uint32 devClass = 0;
	uint08 initRole;
	uint16 timeout;
	
	if(pData == nullptr || dataLen < 10){
		tlkapi_error(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_aclConnect: failure - param error");
		return -TLK_EPARAM;
	}

	tmemcpy(btAddr, pData, 6);
	if(btAddr[0] == 0 && btAddr[1] == 0 && btAddr[2] == 0 && btAddr[3] == 0 && btAddr[4] == 0 && btAddr[5] == 0){
		tmemcpy(btAddr, sBthFuncPeerAddr, 6);
	}
	
	devClass |= (pData[8] << 16) & 0xFF0000;
	devClass |= (pData[7] <<  8) & 0xFF00;
	devClass |= pData[6] & 0xFF;
	initRole = pData[9];
	timeout  = (((uint16)pData[12] << 8) | pData[11])*100;
	tlkapi_trace(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_aclConnect: initRole[%d], timeout[%d], devClass[0x%x], btAddr[%x-%x-%x-%x-%x-%x]",
		initRole, timeout, devClass, btAddr[0], btAddr[1], btAddr[2], btAddr[3], btAddr[4], btAddr[5]);
	return bth_acl_connect(btAddr, devClass, initRole,timeout);
}
static int bth_func_aclDisconn(uint08 *pData, uint16 dataLen)
{
	uint16 handle;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_aclDisconn: failure - param error");
		return -TLK_EPARAM;
	}

	handle = ((uint16)pData[1]<<8) | pData[0];
	if(handle == 0) handle = sBthFuncAclHandle;
	tlkapi_trace(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_aclDisconn: handle[0x%x]", handle);

	return bth_acl_disconn(handle);
}
static int bth_func_aclConnectCancel(uint08 *pData, uint16 dataLen)
{
	uint08 btAddr[6];

	if(pData == nullptr || dataLen < 6){
		tlkapi_error(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_aclConnectCancel: failure - param error");
		return -TLK_EPARAM;
	}
	
	tmemcpy(btAddr,pData,6);
	tlkapi_trace(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_aclConnectCancel: btAddr[%x-%x-%x-%x-%x-%x]",
		btAddr[0], btAddr[1], btAddr[2], btAddr[3], btAddr[4], btAddr[5]);
	
	return bth_acl_connectCancel(pData);
}
static int bth_func_aclDisconnByAddr(uint08 *pData, uint16 dataLen)
{
	uint08 btAddr[6];
	
	if(pData == nullptr || dataLen < 6){
		tlkapi_error(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_aclDisconnByAddr: failure - param error");
		return -TLK_EPARAM;
	}
	
	tmemcpy(btAddr,pData,6);
	tlkapi_trace(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_aclDisconnByAddr: btAddr[%x-%x-%x-%x-%x-%x]",
		btAddr[0], btAddr[1], btAddr[2], btAddr[3], btAddr[4], btAddr[5]);
	
	return bth_acl_disconnByAddr(btAddr);
}


static int bth_func_scoConnect(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	uint16 linkType;
	uint08 airMode;

	if(pData == nullptr || dataLen < 5){
		tlkapi_error(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_scoConnect: failure - param error");
		return -TLK_EPARAM;
	}
	
	handle = ((uint16)pData[1]<<8 | pData[0]);
	linkType = ((uint16)pData[3]<<8 | pData[2]);
	airMode = pData[4];
	if(handle == 0) handle = sBthFuncAclHandle;
	tlkapi_trace(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_scoConnect: handle[0x%x], linkType[%d], airMode[0x%x]",
		handle, linkType, airMode);
	
	return bth_sco_connect(handle,linkType, airMode);
}
static int bth_func_scoDisconn(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	uint08 reason;
	
	if(pData == nullptr || dataLen < 3){
		tlkapi_error(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_scoDisconn: failure - param error");
		return -TLK_EPARAM;
	}
	
	handle = ((uint16)pData[1]<<8 | pData[0]);
	reason = pData[2];
	if(handle == 0) handle = sBthFuncScoHandle;
	tlkapi_trace(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_scoDisconn: handle[0x%x], reason[0x%x], airMode[0x%x]",
		handle, reason);
	
	return bth_sco_disconn(handle, reason);
}
static int bth_func_scoDisconnByAddr(uint08 *pData, uint16 dataLen)
{
	uint08 reason;
	uint08 btAddr[6];
		
	if(pData == nullptr || dataLen < 7){
		tlkapi_error(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_scoDisconnByAddr: failure - param error");
		return -TLK_EPARAM;
	}
	
	tmemcpy(btAddr,pData,6);
	if(btAddr[0] == 0 && btAddr[1] == 0 && btAddr[2] == 0 && btAddr[3] == 0 && btAddr[4] == 0 && btAddr[5] == 0){
		tmemcpy(btAddr, sBthFuncPeerAddr, 6);
	}
	
	reason = pData[6];
	tlkapi_trace(BTH_FUNC_DBG_FLAG, BTH_FUNC_DBG_SIGN, "bth_func_scoDisconnByAddr: reason[0x%x] btAddr[%x-%x-%x-%x-%x-%x]",
		reason, btAddr[0], btAddr[1], btAddr[2], btAddr[3], btAddr[4], btAddr[5]);
	
	return bth_sco_disconnByAddr(btAddr, reason);
}





#endif //#if (TLK_STK_BTH_ENABLE && TLK_CFG_TEST_ENABLE)

