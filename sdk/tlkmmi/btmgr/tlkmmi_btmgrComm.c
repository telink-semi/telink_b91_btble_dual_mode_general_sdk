/********************************************************************************************************
 * @file     tlkmmi_btmgrComm.c
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

#include "string.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi/tlkmmi_stdio.h"
#if (TLKMMI_BTMGR_ENABLE)
#include "tlkprt/tlkprt_comm.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkmdi/tlkmdi_btacl.h"
#include "tlkmmi/btmgr/tlkmmi_btmgr.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrComm.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrCtrl.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrAcl.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrInq.h"



static void tlkmmi_btmgr_cmdHandler(uint08 msgID, uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_getNameCmdDeal(void);
static void tlkmmi_btmgr_getAddrCmdDeal(void);
static void tlkmmi_btmgr_getPDLCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_delPDLCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_clsPDLCmdDeal(void);
static void tlkmmi_btmgr_getCDLCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_btInquiryCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_btInqCancelCmdDeal(void);
static void tlkmmi_btmgr_btConnectCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_btDisconnCmdDeal(uint08 *pData, uint08 dataLen);



/******************************************************************************
 * Function: tlkmmi_btmgr_commInit
 * Descript: register the bt manager cmd handler callback. 
 * Params:
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_commInit(void)
{
	tlkmdi_comm_regBtCB(tlkmmi_btmgr_cmdHandler);
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_btmgr_sendAclConnectEvt
 * Descript: Send ACL connect Evt to app. 
 * Params:
 *        @handle[IN]--The acl handle.
 *        @status[IN]--The current status.
 *        @pBtAddr[IN]--The BT Address.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_btmgr_sendAclConnectEvt(uint16 handle, uint08 status, uint08 *pBtAddr)
{
	uint08 buffLen;
	uint08 buffer[32];
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "SendAclConnectEvt[handle-0x%x,status-%d,btaddr-0x%04x]", handle, status, *(uint32*)pBtAddr);
	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_BT_CHN_ACL; //Object
	buffer[buffLen++] = status; //Status
	buffer[buffLen++] = (handle & 0x00FF); //Handle
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	tmemcpy(buffer+buffLen, pBtAddr, 6); //MAC
	buffLen += 6;
	tlkmdi_comm_sendBtEvt(TLKPRT_COMM_EVTID_BT_CONNECT, buffer, buffLen);
}

/******************************************************************************
 * Function: tlkmmi_btmgr_sendAclDisconnEvt
 * Descript: Send ACL connect Evt to app. 
 * Params:
 *        @handle[IN]--The acl handle.
 *        @reason[IN]--The disconnect reason.
 *        @pBtAddr[IN]--The BT Address.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_btmgr_sendAclDisconnEvt(uint16 handle, uint08 reason, uint08 *pBtAddr)
{
	uint08 buffLen;
	uint08 buffer[32];
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "SendAclDisconnEvt[handle-0x%x,status-%d,btaddr-0x%04x]", handle, reason, *(uint32*)pBtAddr);
	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_BT_CHN_ACL; //Object
	buffer[buffLen++] = reason; //reason
	buffer[buffLen++] = (handle & 0x00FF); //Handle
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	tmemcpy(buffer+buffLen, pBtAddr, 6); //MAC
	buffLen += 6;
	tlkmdi_comm_sendBtEvt(TLKPRT_COMM_EVTID_BT_DISCONN, buffer, buffLen);
}

/******************************************************************************
 * Function: tlkmmi_btmgr_sendProfConnectEvt
 * Descript: Send profile connect Evt to app. 
 * Params:
 *        @handle[IN]--The acl handle.
 *        @status[IN]--The current status.
 *        @ptype[IN]--The profile type.
 *        @usrID[IN]--The user id(Client/Server).
 *        @pBtAddr[IN]--The Bt address.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_btmgr_sendProfConnectEvt(uint16 handle, uint08 status, uint08 ptype, uint08 usrID, uint08 *pBtAddr)
{
	uint08 buffLen;
	uint08 buffer[32];
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "SendProfConnectEvt[handle-0x%x,ptype-%d,status-%d,usrID-%d]", handle, ptype, status, usrID);
	buffLen = 0;
	buffer[buffLen++] = tlkmmi_btmgr_ptypeToCtype(ptype, usrID); //Object
	buffer[buffLen++] = status; //Status
	buffer[buffLen++] = (handle & 0x00FF); //Handle
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	tmemcpy(buffer+buffLen, pBtAddr, 6); //MAC
	buffLen += 6;
	tlkmdi_comm_sendBtEvt(TLKPRT_COMM_EVTID_BT_CONNECT, buffer, buffLen);
}

/******************************************************************************
 * Function: tlkmmi_btmgr_sendProfDisconnEvt
 * Descript: Send profle disconnect Evt to app. 
 * Params:
 *        @handle[IN]--The acl handle.
 *        @reason[IN]--The disconnect reason.
 *        @ptype[IN]--The profile type.
 *        @usrID[IN]--The user id(Client/Server).
 *        @pBtAddr[IN]--The Bt address.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_btmgr_sendProfDisconnEvt(uint16 handle, uint08 reason, uint08 ptype, uint08 usrID, uint08 *pBtAddr)
{
	uint08 buffLen;
	uint08 buffer[32];
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "SendProfDisconnEvt[handle-0x%x,ptype-%d,reason-%d,usrID-%d]", handle, ptype, reason, usrID);
	buffLen = 0;
	buffer[buffLen++] = tlkmmi_btmgr_ptypeToCtype(ptype, usrID); //Object
	buffer[buffLen++] = reason; //reason
	buffer[buffLen++] = (handle & 0x00FF); //Handle
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	tmemcpy(buffer+buffLen, pBtAddr, 6);  //MAC
	buffLen += 6;
	tlkmdi_comm_sendBtEvt(TLKPRT_COMM_EVTID_BT_DISCONN, buffer, buffLen);
}

/******************************************************************************
 * Function: tlkmmi_btmgr_ptypeToCtype
 * Descript: convert the ptype to ctype. 
 * Params:
 *        @ptype[IN]--The profile type
 *        @usrID[IN]--The user id(Client or Server).
 * Return: The communication type(AG/HFP/A2DPSNK/A2DPSRC..).
 * Others: None.
*******************************************************************************/
uint08 tlkmmi_btmgr_ptypeToCtype(uint08 ptype, uint08 usrID)
{
	uint08 ctype = 0xFF;
	switch(ptype){
		case BTP_PTYPE_HFP:
			if(usrID == BTP_USRID_SERVER) ctype = TLKPRT_COMM_BT_CHN_HFP_AG;
			else ctype = TLKPRT_COMM_BT_CHN_HFP_HF;
			break;
		case BTP_PTYPE_IAP:
			break;
		case BTP_PTYPE_SPP:
			ctype = TLKPRT_COMM_BT_CHN_SPP;
			break;
		case BTP_PTYPE_HID:
			break;
		case BTP_PTYPE_ATT:
			break;
		case BTP_PTYPE_A2DP:
			if(usrID == BTP_USRID_CLIENT) ctype = TLKPRT_COMM_BT_CHN_A2DP_SNK;
			else ctype = TLKPRT_COMM_BT_CHN_A2DP_SRC;
			break;
		case BTP_PTYPE_AVRCP:
			ctype = TLKPRT_COMM_BT_CHN_AVRCP;
			break;
		case BTP_PTYPE_PBAP:
			ctype = TLKPRT_COMM_BT_CHN_PBAP;
			break;
	}
	return ctype;
}

static void tlkmmi_btmgr_cmdHandler(uint08 msgID, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_cmdHandler: %d", msgID);
	
	if(msgID == TLKPRT_COMM_CMDID_BT_CONNECT){
		tlkmmi_btmgr_btConnectCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_DISCONN){
		tlkmmi_btmgr_btDisconnCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_INQUIRY){
		tlkmmi_btmgr_btInquiryCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_INQ_CANCEL){
		tlkmmi_btmgr_btInqCancelCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_BT_GET_NAME){
		tlkmmi_btmgr_getNameCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_BT_GET_ADDR){
		tlkmmi_btmgr_getAddrCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_BT_GET_PDL){
		tlkmmi_btmgr_getPDLCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_DEL_PDL){
		tlkmmi_btmgr_delPDLCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_CLS_PDL){
		tlkmmi_btmgr_clsPDLCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_BT_GET_CDL){
		tlkmmi_btmgr_getCDLCmdDeal(pData, dataLen);
	}else{
		tlkmdi_comm_sendBtRsp(msgID, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}
}
static void tlkmmi_btmgr_getNameCmdDeal(void)
{
	uint08 buffLen;
	uint08 nameLen;
	uint08 *pBtName;
	uint08 buffer[64+4];

	pBtName = tlkmmi_btmgr_getBtName();
	if(pBtName == nullptr) nameLen = 0;
	else nameLen = strlen((char*)pBtName);
	if(nameLen == 0 || nameLen > 60){
		nameLen = 60;
	}
		
	buffLen = 0;
	buffer[buffLen++] = nameLen;
	tmemcpy(buffer+buffLen, pBtName, nameLen);
	buffLen += nameLen;
	tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_GET_NAME, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_btmgr_getAddrCmdDeal(void)
{
	uint08 buffLen;
	uint08 buffer[6];
	uint08 *pBtAddr;
	
	pBtAddr = tlkmmi_btmgr_getBtAddr();
	
	buffLen = 0;
	tmemcpy(buffer+buffLen, pBtAddr, 6);
	buffLen += 6;
	tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_GET_ADDR, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_btmgr_getPDLCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 index;
	uint08 start;
	uint08 count;
	uint08 devNum;
	uint08 buffLen;
	uint08 buffer[86];
	bth_device_item_t *pItem;

	start = pData[0];
	count = pData[1];
	if(dataLen < 2){
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_GET_PDL, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	devNum = bth_device_count();
	if(start >= devNum) count = 0;
	else if(start+count > devNum) count = devNum-start;
	if(count > 10) count = 10;
	
	buffLen = 0;
	buffer[buffLen++] = devNum;
	buffer[buffLen++] = start;
	buffer[buffLen++] = count;
	for(index=0; index<count; index++){
		pItem = bth_device_getInfo(start+index);
		if(pItem == nullptr) break;
		buffer[buffLen++] = bth_devClassToDevType(pItem->devClass);
		buffer[buffLen++] = 0;
		tmemcpy(buffer+buffLen, pItem->devAddr, 6);
		buffLen += 6;
	}
	buffer[2] = index;
	tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_GET_PDL, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_btmgr_delPDLCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 index;
	uint08 count;
	uint08 devNum;
	uint08 bdaddr[6];
	uint08 buffLen;
	uint08 buffer[20];
	bth_device_item_t *pPeerItem;
	tlkmdi_btacl_item_t  *pConnItem;

	count = pData[0];
	if(dataLen < 2){
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_DEL_PDL, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFORMAT, nullptr, 0);
		return;
	}

	devNum = bth_device_count();
	if(count > devNum) count = devNum;
	if(count > 10) count = 10;

	pData   += 1;
	dataLen -= 1;

	buffLen = 0;
	buffer[buffLen++] = count;
	for(index=0; index<count; index++){
		if(dataLen < 6){
			buffer[buffLen++] = TLK_EPARAM;
			continue;
		}
		tmemcpy(bdaddr, pData, 6);
		pPeerItem = bth_device_getItem(bdaddr, nullptr);
		#if (TLK_MDI_BTACL_ENABLE)
		pConnItem = tlkmdi_btacl_searchUsedItem(bdaddr);
		#else
		pConnItem = nullptr;
		#endif
		if(pPeerItem == nullptr){
			buffer[buffLen++] = TLK_EPARAM;
		}else if(pConnItem != nullptr){
			buffer[buffLen++] = TLK_EBUSY;
		}else{
			buffer[buffLen++] = TLK_ENONE;
			bth_device_delItem(bdaddr);
		}
		pData   += 6;
		dataLen -= 6;
	}
	tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_DEL_PDL, TLKPRT_COMM_RSP_STATUE_SUCCESS,
			TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_btmgr_clsPDLCmdDeal(void)
{
	#if (TLK_MDI_BTACL_ENABLE)
	if(tlkmdi_btacl_getUsedCount() != 0){
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_CLS_PDL, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EBUSY, nullptr, 0);
	}else
	#endif //#if (TLK_MDI_BTACL_ENABLE)
	if(bth_device_count() == 0){
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_CLS_PDL, TLKPRT_COMM_RSP_STATUE_SUCCESS,
			TLK_ENONE, nullptr, 0);
	}else{
		bth_device_clsItem();
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_CLS_PDL, TLKPRT_COMM_RSP_STATUE_SUCCESS,
			TLK_ENONE, nullptr, 0);
	}
}
static void tlkmmi_btmgr_getCDLCmdDeal(uint08 *pData, uint08 dataLen)
{
	#if !(TLK_MDI_BTACL_ENABLE)
	tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_GET_CDL, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_ENOSUPPORT, nullptr, 0);
	#else
	uint08 index;
	uint08 start;
	uint08 count;
	uint08 devNum;
	uint08 buffLen;
	uint08 buffer[86];
	tlkmdi_btacl_item_t *pItem;

	start = pData[0];
	count = pData[1];
	if(dataLen < 2){
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_GET_CDL, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	devNum = tlkmdi_btacl_getConnCount();
	if(start >= devNum) count = 0;
	else if(start+count > devNum) count = devNum-start;
	if(count > 10) count = 10;
	
	buffLen = 0;
	buffer[buffLen++] = devNum;
	buffer[buffLen++] = start;
	buffer[buffLen++] = count;
	for(index=0; index<count; index++){
		pItem = tlkmdi_btacl_getConnItemByIndex(start+index);
		if(pItem == nullptr) break;
		buffer[buffLen++] = pItem->handle & 0xFF;
		buffer[buffLen++] = (pItem->handle & 0xFF00) >> 8;
		tmemcpy(buffer+buffLen, pItem->btaddr, 6);
		buffLen += 6;
	}
	buffer[2] = index;
	tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_GET_CDL, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, buffer, buffLen);
	#endif
}
static void tlkmmi_btmgr_btInquiryCmdDeal(uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 type;
	uint08 numb;
	uint08 rssi;
	uint08 reason;
	uint08 inqWind;
	uint08 inqNumb;
	
	type = pData[0];
	numb = pData[1];
	rssi = pData[2];
	inqWind = pData[3];
	inqNumb = pData[4];
	if(dataLen < 5 || (type != 1 && type != 2 && type != 4) || inqWind > 100 || inqWind < 3){
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_INQUIRY, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}

	ret = tlkmmi_btmgr_startInquiry(type, rssi, numb, inqWind, inqNumb, true);
	if(ret == TLK_ENONE) reason = TLK_ENONE;
	else if(ret == -TLK_EBUSY) reason = TLK_EBUSY;
	else reason = TLK_EPARAM;

	if(reason == TLK_ENONE){
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_INQUIRY, TLKPRT_COMM_RSP_STATUE_SUCCESS, reason, nullptr, 0);
	}else{
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_INQUIRY, TLKPRT_COMM_RSP_STATUE_FAILURE, reason, nullptr, 0);
	}
}
static void tlkmmi_btmgr_btInqCancelCmdDeal(void)
{
	int ret;
	uint08 reason;
	
	ret = tlkmmi_btmgr_closeInquiry();
	if(ret == TLK_ENONE) reason = TLK_ENONE;
	else if(ret == -TLK_EBUSY) reason = TLK_EBUSY;
	else reason = TLK_EPARAM;
	if(reason == TLK_ENONE){
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_INQ_CANCEL, TLKPRT_COMM_RSP_STATUE_SUCCESS, reason, nullptr, 0);
	}else{
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_INQ_CANCEL, TLKPRT_COMM_RSP_STATUE_FAILURE, reason, nullptr, 0);
	}
}
static void tlkmmi_btmgr_btConnectCmdDeal(uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 object;
	uint16 handle;
	uint08 btAddr[6];
	uint16 timeout;
	
	if(dataLen < 6 || (pData[0] != 0 && pData[0] != 1)){
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_btConnectCmdDeal: Error Param");
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_CONNECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}
	if(pData[0] == 0 && dataLen < 10){
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_btConnectCmdDeal: Error Length");
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_CONNECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}

	object = pData[1];
	timeout = ((uint16)pData[3] << 8) | pData[2];
	if(pData[0] == 0) tmemcpy(btAddr, pData+4, 6);
	else handle = ((uint16)pData[5] << 8) | pData[4];
	
	(void)handle;
	if(object == TLKPRT_COMM_BT_CHN_ACL){
		if(pData[0] != 0){
			tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_btConnectCmdDeal: Only Support Method - MAC");
			tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_CONNECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
			return;
		}
		ret = tlkmmi_btmgr_connect(btAddr, timeout);
		if(ret == TLK_ENONE){
			tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_btConnectCmdDeal: Start Connect - none");
			tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_CONNECT, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
		}else if(ret == -TLK_EBUSY){
			tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_btConnectCmdDeal: Start Connect - busy");
			tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_CONNECT, TLKPRT_COMM_RSP_STATUE_COMPLETE, TLK_ENONE, nullptr, 0);
		}else{
			tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_btConnectCmdDeal: Start Connect - fail");
			tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_CONNECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
	}else{
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_btConnectCmdDeal: Only Support Object-ACL");
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_CONNECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}
}
static void tlkmmi_btmgr_btDisconnCmdDeal(uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 object;
	uint16 handle = 0;
	uint08 btAddr[6] = {0};

	if(dataLen < 4 || (pData[0] != 0 && pData[0] != 1)){
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_DISCONN, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}
	if(pData[0] == 0 && dataLen < 8){
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_DISCONN, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}

	object = pData[1];
	if(pData[0] == 0) tmemcpy(btAddr, pData+2, 6);
	else handle = ((uint16)pData[3] << 8) | pData[2];
	if(object == TLKPRT_COMM_BT_CHN_ACL){
		tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_btDisconnCmdDeal:object-%d,handle-0x%x", object, handle);
		if(pData[0] == 0) ret = tlkmmi_btmgr_disconnByAddr(btAddr);
		else ret = tlkmmi_btmgr_disconn(handle);
		if(ret == TLK_ENONE){
			tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_DISCONN, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
		}else if(ret == -TLK_EBUSY){
			tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_DISCONN, TLKPRT_COMM_RSP_STATUE_COMPLETE, TLK_ENONE, nullptr, 0);
		}else{
			tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_DISCONN, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
	}else{
		tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_btDisconnCmdDeal: Only Support Object-ACL");
		tlkmdi_comm_sendBtRsp(TLKPRT_COMM_CMDID_BT_DISCONN, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}
}





#endif //#if (TLKMMI_BTMGR_ENABLE)

