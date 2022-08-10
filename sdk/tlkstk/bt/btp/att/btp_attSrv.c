/********************************************************************************************************
 * @file     btp_attSrv.c
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
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_l2cap.h"
#include "tlkstk/bt/bth/bth_signal.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#if (TLKBTP_CFG_ATTSRV_ENABLE)
#include "tlkstk/bt/btp/btp_adapt.h"
#include "tlkstk/bt/btp/att/btp_att.h"
#include "tlkstk/bt/btp/att/btp_attInner.h"
#include "tlkstk/bt/btp/att/btp_attSrv.h"


static const btp_gattItem_t *btp_attsrv_getItem(uint16 attHandle);
static const btp_gattItem_t *btp_attsrv_searchItem(uint16 sHandle, uint16 eHandle, 
	uint08 *pUUID, uint08 uuidLen, uint16 *pHandle);

static bool btp_attsrv_timer(tlkapi_timer_t *pTimer, void *pUsrArg);
static void btp_attsrv_procs(btp_att_node_t *pAtt);

static void btp_attsrv_sendReadRspProcs(btp_att_node_t *pAtt);
static void btp_attsrv_sendReadBlobRspProcs(btp_att_node_t *pAtt);
static void btp_attsrv_sendWriteRspProcs(btp_att_node_t *pAtt);
static void btp_attsrv_sendErrorRspProcs(btp_att_node_t *pAtt);
static void btp_attsrv_sendReadGroupRspProcs(btp_att_node_t *pAtt);
static void btp_attsrv_sendReadTypeRspProcs(btp_att_node_t *pAtt);
static void btp_attsrv_sendFindInfoRspProcs(btp_att_node_t *pAtt);
static void btp_attsrv_sendFindTypeRspProcs(btp_att_node_t *pAtt);


static void btp_attsrv_recvReadGroupReqProcs(btp_att_node_t *pAtt, uint08 *pData, uint16 dataLen);
static void btp_attsrv_recvReadTypeReqProcs(btp_att_node_t *pAtt, uint08 *pData, uint16 dataLen);
static void btp_attsrv_recvFindInfoReqProcs(btp_att_node_t *pAtt, uint08 *pData, uint16 dataLen);
static void btp_attsrv_recvReadReqProcs(btp_att_node_t *pAtt, uint08 *pData, uint16 dataLen);
static void btp_attsrv_recvWriteReqProcs(btp_att_node_t *pAtt, uint08 *pData, uint16 dataLen);
static void btp_attsrv_recvReadBlobReqProcs(btp_att_node_t *pAtt, uint08 *pData, uint16 dataLen);


static uint16 sBtpAttItemCount;
static const btp_attItem_t *sBtpAttItemTable;


int btp_attsrv_init(void)
{
	sBtpAttItemCount = 0;
	sBtpAttItemTable = nullptr;
	
	return TLK_ENONE;
}


int btp_attsrv_setTable(const btp_attItem_t *pTable, uint16 count)
{
	sBtpAttItemCount = count;
	sBtpAttItemTable = pTable;
	
	return TLK_ENONE;
}

uint btp_attsrv_getChnID(uint16 aclHandle)
{
	btp_att_node_t *pAtt;
	pAtt = btp_att_getAnyUsedNode(aclHandle);
	if(pAtt == nullptr) return 0;
	else return pAtt->chnID;
}

void btp_attsrv_connectEvt(btp_att_node_t *pAtt)
{
	pAtt->state = TLK_STATE_CONNECT;
	pAtt->param.server.mtuSize = 23;
	
	btp_adapt_initTimer(&pAtt->timer, btp_attsrv_timer, pAtt, BTP_ATT_TIMEOUT);
	btp_adapt_insertTimer(&pAtt->timer);
}
void btp_attsrv_disconnEvt(btp_att_node_t *pAtt)
{
	
}

void btp_attsrv_recvHandler(btp_att_node_t *pAtt, uint08 *pData, uint16 dataLen)
{
	uint08 opcode;
	
	opcode = pData[0];
	tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvHandler: opcode - %d", opcode);
	switch(opcode){
		case BTP_ATT_OPCODE_READ_GROUP_REQ:
			btp_attsrv_recvReadGroupReqProcs(pAtt, pData+1, dataLen-1);
			break;
		case BTP_ATT_OPCODE_READ_TYPE_REQ:
			btp_attsrv_recvReadTypeReqProcs(pAtt, pData+1, dataLen-1);
			break;
		case BTP_ATT_OPCODE_FIND_INFO_REQ:
			btp_attsrv_recvFindInfoReqProcs(pAtt, pData+1, dataLen-1);
			break;
		case BTP_ATT_OPCODE_FIND_TYPE_REQ:
			btp_attsrv_recvFindInfoReqProcs(pAtt, pData+1, dataLen-1);
			break;
		case BTP_ATT_OPCODE_READ_REQ:
			btp_attsrv_recvReadReqProcs(pAtt, pData+1, dataLen-1);
			break;
		case BTP_ATT_OPCODE_READ_BLOB_REQ:
			btp_attsrv_recvReadBlobReqProcs(pAtt, pData+1, dataLen-1);
			break;
		case BTP_ATT_OPCODE_WRITE_REQ:
		case BTP_ATT_OPCODE_WRITE_CMD:
			btp_attsrv_recvWriteReqProcs(pAtt, pData+1, dataLen-1);
			break;
	}
	if(pAtt->busys != BTP_ATTSRV_BUSY_NONE){
		btp_adapt_insertTimer(&pAtt->timer);
	}
}

static bool btp_attsrv_timer(tlkapi_timer_t *pTimer, void *pUsrArg)
{
	btp_att_node_t *pAtt = (btp_att_node_t*)pUsrArg;
	if(pAtt->state == TLK_STATE_CLOSED || pAtt->busys == BTP_ATTSRV_BUSY_NONE){
		return false; //Not Repeat
	}else{
		btp_attsrv_procs(pAtt);
		return true; //Repeat
	}
}
static void btp_attsrv_procs(btp_att_node_t *pAtt)
{
	if((pAtt->busys & BTP_ATTSRV_BUSY_SEND_READ_RSP) != 0){
		pAtt->busys &= ~BTP_ATTSRV_BUSY_SEND_READ_RSP;
		btp_attsrv_sendReadRspProcs(pAtt);
	}else if((pAtt->busys & BTP_ATTSRV_BUSY_SEND_READ_BLOB_RSP) != 0){
		pAtt->busys &= ~BTP_ATTSRV_BUSY_SEND_READ_BLOB_RSP;
		btp_attsrv_sendReadBlobRspProcs(pAtt);
	}else if((pAtt->busys & BTP_ATTSRV_BUSY_SEND_WRITE_RSP) != 0){
		pAtt->busys &= ~BTP_ATTSRV_BUSY_SEND_WRITE_RSP;
		btp_attsrv_sendWriteRspProcs(pAtt);
	}else if((pAtt->busys & BTP_ATTSRV_BUSY_SEND_ERROR_RSP) != 0){
		pAtt->busys &= ~BTP_ATTSRV_BUSY_SEND_ERROR_RSP;
		btp_attsrv_sendErrorRspProcs(pAtt);
	}else if((pAtt->busys & BTP_ATTSRV_BUSY_SEND_READ_GROUP) != 0){
		pAtt->busys &= ~BTP_ATTSRV_BUSY_SEND_READ_GROUP;
		btp_attsrv_sendReadGroupRspProcs(pAtt);
	}else if((pAtt->busys & BTP_ATTSRV_BUSY_SEND_READ_TYPE) != 0){
		pAtt->busys &= ~BTP_ATTSRV_BUSY_SEND_READ_TYPE;
		btp_attsrv_sendReadTypeRspProcs(pAtt);
	}else if((pAtt->busys & BTP_ATTSRV_BUSY_SEND_FIND_INFO) != 0){
		pAtt->busys &= ~BTP_ATTSRV_BUSY_SEND_FIND_INFO;
		btp_attsrv_sendFindInfoRspProcs(pAtt);
	}else if((pAtt->busys & BTP_ATTSRV_BUSY_SEND_FIND_TYPE) != 0){
		pAtt->busys &= ~BTP_ATTSRV_BUSY_SEND_FIND_TYPE;
		btp_attsrv_sendFindTypeRspProcs(pAtt);
	}else{
		pAtt->busys = BTP_ATTSRV_BUSY_NONE;
	}
}


static void btp_attsrv_sendReadRspProcs(btp_att_node_t *pAtt)
{
	uint16 sendLen;
	const btp_attItem_t *pItem;

	pItem = btp_attsrv_getItem(pAtt->param.server.ahandle);
	if(pItem == nullptr){
		pAtt->param.server.opcode = BTP_ATT_OPCODE_READ_REQ;
		pAtt->param.server.reason = BTP_ATT_ECODE_INVALID_HANDLE;
		btp_attsrv_sendErrorRspProcs(pAtt);
		return;
	}
	
	if(pItem->attrLen+1 <= pAtt->param.server.mtuSize) sendLen = pItem->attrLen;
	else sendLen = pAtt->param.server.mtuSize-1;
	if(btp_att_sendReadRsp(pAtt->handle, pAtt->chnID, pItem->pAttr, sendLen) == TLK_ENONE){
		tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_sendReadRspProcs");
	}else{
		pAtt->busys |= BTP_ATTSRV_BUSY_SEND_READ_RSP;
		btp_adapt_insertTimer(&pAtt->timer);
	}
}
static void btp_attsrv_sendReadBlobRspProcs(btp_att_node_t *pAtt)
{
	uint16 roffset;
	uint16 sendLen;
	const btp_attItem_t *pItem;

	pItem = btp_attsrv_getItem(pAtt->param.server.ahandle);
	if(pItem == nullptr){
		pAtt->param.server.opcode = BTP_ATT_OPCODE_READ_BLOB_REQ;
		pAtt->param.server.reason = BTP_ATT_ECODE_INVALID_HANDLE;
		btp_attsrv_sendErrorRspProcs(pAtt);
		return;
	}

	roffset = pAtt->param.server.roffset;
	if(roffset >= pItem->attrLen){
		sendLen = 0;
	}else{
		sendLen = pItem->attrLen-roffset;
		if(sendLen+1 > pAtt->param.server.mtuSize) sendLen = pAtt->param.server.mtuSize-1;
	}
	
	if(btp_att_sendReadBlobRsp(pAtt->handle, pAtt->chnID, pItem->pAttr+roffset, sendLen) == TLK_ENONE){
		tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_sendReadBlobRspProcs");
	}else{
		pAtt->busys |= BTP_ATTSRV_BUSY_SEND_READ_BLOB_RSP;
		btp_adapt_insertTimer(&pAtt->timer);
	}
}
static void btp_attsrv_sendWriteRspProcs(btp_att_node_t *pAtt)
{
	if(btp_att_sendWriteRsp(pAtt->handle, pAtt->chnID) == TLK_ENONE){
		tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_sendWriteRspProcs");
	}else{
		pAtt->busys |= BTP_ATTSRV_BUSY_SEND_WRITE_RSP;
		btp_adapt_insertTimer(&pAtt->timer);
	}
}
static void btp_attsrv_sendErrorRspProcs(btp_att_node_t *pAtt)
{
	if(btp_att_sendErrorRsp(pAtt->handle, pAtt->chnID, pAtt->param.server.opcode,
		pAtt->param.server.shandle, pAtt->param.server.reason) != TLK_ENONE){
		pAtt->busys |= BTP_ATTSRV_BUSY_SEND_ERROR_RSP;
		btp_adapt_insertTimer(&pAtt->timer);
	}else{
		tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_sendErrorRspProcs");
	}
}
static void btp_attsrv_sendReadGroupRspProcs(btp_att_node_t *pAtt)
{
	uint16 cHandle;
	uint16 sHandle;
	uint16 buffLen;
	uint16 dataLen;
	uint08 fillLen;
	uint08 buffer[BTP_ATTSRV_BUFFER_SIZE];
	uint08 *pBuffer;
	const btp_gattItem_t *pItem;
	
	pBuffer = buffer;
	if(pAtt->mtuSize <= BTP_ATTSRV_BUFFER_SIZE) buffLen = pAtt->mtuSize;
	else buffLen = BTP_ATTSRV_BUFFER_SIZE;

	tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_sendReadGroupRspProcs: shandle-0x%x ehandle-0x%x",
		pAtt->param.server.shandle, pAtt->param.server.ehandle);

	cHandle = 0;
	fillLen = 0;
	dataLen = 0;
	sHandle = pAtt->param.server.shandle;
	while(true){
		pItem = btp_attsrv_searchItem(sHandle, pAtt->param.server.ehandle, pAtt->param.server.attUUID,
			pAtt->param.server.uuidLen, &cHandle);
		if(pItem == nullptr) break;
		
		if(fillLen == 0) fillLen = pItem->attrLen;
		if(pItem->attrLen != fillLen || 4+fillLen > buffLen) break;
		pBuffer[dataLen++] = (cHandle & 0x00FF);
		pBuffer[dataLen++] = (cHandle & 0xFF00) >> 8;
		pBuffer[dataLen++] = ((cHandle+pItem->attNum-1) & 0x00FF);
		pBuffer[dataLen++] = ((cHandle+pItem->attNum-1) & 0xFF00) >> 8;
		if(pItem->attrLen != 0){
			tmemcpy(&pBuffer[dataLen], pItem->pAttr, pItem->attrLen);
			dataLen += pItem->attrLen;
		}
		if(dataLen+4+fillLen > buffLen) break;

		sHandle = cHandle+1;
	}
	
	if(dataLen == 0){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_sendReadGroupRspProcs: not found");
		pAtt->param.server.opcode = BTP_ATT_OPCODE_READ_GROUP_REQ;
		pAtt->param.server.reason = BTP_ATT_ECODE_ATTR_NOT_FOUND;
		btp_attsrv_sendErrorRspProcs(pAtt);
		return;
	}
	
	if(btp_att_sendReadGroupRsp(pAtt->handle, pAtt->chnID, 4+fillLen, pBuffer, dataLen) != TLK_ENONE){
		pAtt->busys |= BTP_ATTSRV_BUSY_SEND_READ_GROUP;
		btp_adapt_insertTimer(&pAtt->timer);
	}else{
		tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_sendReadGroupRspProcs");
	}
}
static void btp_attsrv_sendReadTypeRspProcs(btp_att_node_t *pAtt)
{
	uint16 buffLen;
	uint16 dataLen;
	uint16 sHandle;
	uint16 eHandle;
	uint16 cHandle;
	uint08 fillLen;
	uint08 buffer[BTP_ATTSRV_BUFFER_SIZE];
	uint08 *pBuffer;
	const btp_gattItem_t *pItem;
	
	pBuffer = buffer;
	if(pAtt->mtuSize <= BTP_ATTSRV_BUFFER_SIZE) buffLen = pAtt->mtuSize;
	else buffLen = BTP_ATTSRV_BUFFER_SIZE;
	
	fillLen = 0;
	cHandle = 0;
	dataLen = 0;
	sHandle = pAtt->param.server.shandle;
	eHandle = pAtt->param.server.ehandle;
	while(true){
		pItem = btp_attsrv_searchItem(sHandle, eHandle, pAtt->param.server.attUUID, pAtt->param.server.uuidLen, &cHandle);
		if(pItem == nullptr || pItem->pAttr == nullptr) break;
		if(dataLen+2+pItem->attrLen > buffLen) break;
		if(fillLen == 0) fillLen = pItem->attrLen;
		if(pItem->attrLen != fillLen) break;
		pBuffer[dataLen++] = (cHandle & 0x00FF);
		pBuffer[dataLen++] = (cHandle & 0xFF00) >> 8;
		if(pItem->attrLen != 0){
			tmemcpy(&pBuffer[dataLen], (char*)pItem->pAttr, pItem->attrLen);
			dataLen += pItem->attrLen;
		}
		if(dataLen+2+pItem->attrLen > buffLen) break;
		sHandle = cHandle+1;
	}
	
	if(dataLen == 0){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_sendReadTypeRspProcs: not found");
		pAtt->param.server.opcode = BTP_ATT_OPCODE_READ_TYPE_REQ;
		pAtt->param.server.reason = BTP_ATT_ECODE_ATTR_NOT_FOUND;
		btp_attsrv_sendErrorRspProcs(pAtt);
		return;
	}
	
	if(btp_att_sendReadTypeRsp(pAtt->handle, pAtt->chnID, 2+fillLen, pBuffer, dataLen) != TLK_ENONE){
		pAtt->busys |= BTP_ATTSRV_BUSY_SEND_READ_TYPE;
		btp_adapt_insertTimer(&pAtt->timer);
	}else{
		tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_sendReadTypeRspProcs");
	}
}
static void btp_attsrv_sendFindInfoRspProcs(btp_att_node_t *pAtt)
{
	uint08 format;
	uint16 buffLen;
	uint16 dataLen;
	uint16 sHandle;
	uint16 eHandle;
	uint08 uuidLen;
	uint08 buffer[BTP_ATTSRV_BUFFER_SIZE];
	uint08 *pBuffer;
	const btp_gattItem_t *pItem;
	
	pBuffer = buffer;
	if(pAtt->mtuSize <= BTP_ATTSRV_BUFFER_SIZE) buffLen = pAtt->mtuSize;
	else buffLen = BTP_ATTSRV_BUFFER_SIZE;

	uuidLen = 0;
	dataLen = 0;
	sHandle = pAtt->param.server.shandle;
	eHandle = pAtt->param.server.ehandle;
	if(sHandle != 0 && sHandle <= eHandle){
		while(sHandle <= eHandle){
			pItem = btp_attsrv_getItem(sHandle);
			if(pItem == nullptr || dataLen+2+pItem->uuidLen > buffLen) break;
			if(uuidLen == 0) uuidLen = pItem->uuidLen;
			if(uuidLen != pItem->uuidLen) break;
			pBuffer[dataLen++] = (sHandle & 0x00FF);
			pBuffer[dataLen++] = (sHandle & 0xFF00) >> 8;
			tmemcpy((char*)&pBuffer[dataLen], (char*)pItem->pUUID, pItem->uuidLen);
			dataLen += pItem->uuidLen;
			sHandle ++;
		}
	}
	
	if(dataLen == 0){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_sendFindInfoRspProcs: not found");
		pAtt->param.server.opcode = BTP_ATT_OPCODE_FIND_INFO_REQ;
		pAtt->param.server.reason = BTP_ATT_ECODE_ATTR_NOT_FOUND;
		btp_attsrv_sendErrorRspProcs(pAtt);
		return;
	}
	
	if(uuidLen == 2) format = 1;
	else format = 2;
	if(btp_att_sendFindInfoRsp(pAtt->handle, pAtt->chnID, format, pBuffer, dataLen) != TLK_ENONE){
		pAtt->busys |= BTP_ATTSRV_BUSY_SEND_FIND_INFO;
		btp_adapt_insertTimer(&pAtt->timer);
	}else{
		tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_sendFindInfoRspProcs");
	}
}
static void btp_attsrv_sendFindTypeRspProcs(btp_att_node_t *pAtt)
{
	
}


/////////////////////////////////////////////////////////////////////////////////////////////
static void btp_attsrv_recvReadGroupReqProcs(btp_att_node_t *pAtt, uint08 *pData, uint16 dataLen)
{
	if(dataLen != 6 && dataLen != 20){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvReadGroupReqProcs: error length");
		pAtt->param.server.shandle = 0x0000;
		pAtt->param.server.opcode = BTP_ATT_OPCODE_READ_GROUP_REQ;
		pAtt->param.server.reason = BTP_ATT_ECODE_INVALID_PDU;
		btp_attsrv_sendErrorRspProcs(pAtt);
		return;
	}
	
	pAtt->param.server.shandle = (((uint16)pData[1])<<8) | pData[0];
	pAtt->param.server.ehandle = (((uint16)pData[3])<<8) | pData[2];
	if(pAtt->param.server.shandle > pAtt->param.server.ehandle){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvReadGroupReqProcs: error param");
		pAtt->param.server.opcode = BTP_ATT_OPCODE_READ_GROUP_REQ;
		pAtt->param.server.reason = BTP_ATT_ECODE_VALUE_NOT_ALLOWED;
		btp_attsrv_sendErrorRspProcs(pAtt);
		return;
	}
	if(dataLen != 6 || (tmemcmp(cBtpUUID16Primary, pData+4, 2) != 0
		&& tmemcmp(cBtpUUID16SecondS, pData+4, 2) != 0)){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvReadGroupReqProcs: not found");
		pAtt->param.server.opcode = BTP_ATT_OPCODE_READ_GROUP_REQ;
		pAtt->param.server.reason = BTP_ATT_ECODE_ATTR_NOT_FOUND;
		btp_attsrv_sendErrorRspProcs(pAtt);
		return;
	}

	tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvReadGroupReqProcs:");
	
	pAtt->param.server.uuidLen = 2;
	tmemcpy(pAtt->param.server.attUUID, (void*)cBtpUUID16Primary, 2);
	
	btp_attsrv_sendReadGroupRspProcs(pAtt);
}
static void btp_attsrv_recvReadTypeReqProcs(btp_att_node_t *pAtt, uint08 *pData, uint16 dataLen)
{
	if(dataLen != 6 && dataLen != 20){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvReadTypeReqProcs: error length");
		pAtt->param.server.shandle = 0x0000;
		pAtt->param.server.opcode = BTP_ATT_OPCODE_READ_TYPE_REQ;
		pAtt->param.server.reason = BTP_ATT_ECODE_INVALID_PDU;
		btp_attsrv_sendErrorRspProcs(pAtt);
		return;
	}

	tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvReadTypeReqProcs:");
	pAtt->param.server.shandle = (((uint16)pData[1])<<8) | pData[0];
	pAtt->param.server.ehandle = (((uint16)pData[3])<<8) | pData[2];
	if(dataLen == 20) pAtt->param.server.uuidLen = 16;
	else pAtt->param.server.uuidLen = 2;
	tmemcpy(pAtt->param.server.attUUID, pData+4, pAtt->param.server.uuidLen);
	btp_attsrv_sendReadTypeRspProcs(pAtt);
}

static void btp_attsrv_recvFindInfoReqProcs(btp_att_node_t *pAtt, uint08 *pData, uint16 dataLen)
{
	if(dataLen != 4){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvFindInfoReqProcs: error length");
		pAtt->param.server.shandle = 0x0000;
		pAtt->param.server.opcode = BTP_ATT_OPCODE_FIND_INFO_REQ;
		pAtt->param.server.reason = BTP_ATT_ECODE_INVALID_PDU;
		btp_attsrv_sendErrorRspProcs(pAtt);
		return;
	}

	tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvFindInfoReqProcs:");
	pAtt->param.server.shandle = (((uint16)pData[1])<<8) | pData[0];
	pAtt->param.server.ehandle = (((uint16)pData[3])<<8) | pData[2];
	btp_attsrv_sendFindInfoRspProcs(pAtt);
}

static void btp_attsrv_recvReadReqProcs(btp_att_node_t *pAtt, uint08 *pData, uint16 dataLen)
{
	uint16 ahandle;
	const btp_attItem_t *pItem;

	ahandle = (((uint16)pData[1])<<8) | pData[0];
	pItem = btp_attsrv_getItem(ahandle);
	if(pItem == nullptr || dataLen < 2){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvWriteReqProcs: no atthandle or error length");
		pAtt->param.server.opcode = BTP_ATT_OPCODE_READ_REQ;
		pAtt->param.server.reason = BTP_ATT_ECODE_INVALID_HANDLE;
		btp_attsrv_sendErrorRspProcs(pAtt);
		return;
	}
	if((pItem->permit & BTP_ATT_PERMIT_READ) == 0){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvWriteReqProcs: write is not supported!");
		pAtt->param.server.opcode = BTP_ATT_OPCODE_READ_REQ;
		pAtt->param.server.reason = BTP_ATT_ECODE_READ_NOT_PERMITTED;
		btp_attsrv_sendErrorRspProcs(pAtt);
		return;
	}

	tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvReadReqProcs:");

	pAtt->param.server.ahandle = ahandle;
	btp_attsrv_sendReadRspProcs(pAtt);
}
static void btp_attsrv_recvReadBlobReqProcs(btp_att_node_t *pAtt, uint08 *pData, uint16 dataLen)
{
	uint16 roffset;
	uint16 ahandle;
	const btp_attItem_t *pItem;

	ahandle = (((uint16)pData[1])<<8) | pData[0];
	roffset = (((uint16)pData[3])<<8) | pData[2];
	pItem = btp_attsrv_getItem(ahandle);
	if(pItem == nullptr || dataLen < 4){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvWriteReqProcs: no atthandle or error length");
		pAtt->param.server.opcode = BTP_ATT_OPCODE_READ_BLOB_REQ;
		pAtt->param.server.reason = BTP_ATT_ECODE_INVALID_HANDLE;
		btp_attsrv_sendErrorRspProcs(pAtt);
		return;
	}
	if((pItem->permit & BTP_ATT_PERMIT_READ) == 0){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvWriteReqProcs: write is not supported!");
		pAtt->param.server.opcode = BTP_ATT_OPCODE_READ_BLOB_REQ;
		pAtt->param.server.reason = BTP_ATT_ECODE_READ_NOT_PERMITTED;
		btp_attsrv_sendErrorRspProcs(pAtt);
		return;
	}

	tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvReadReqProcs:");

	pAtt->param.server.ahandle = ahandle;
	pAtt->param.server.roffset = roffset;
	btp_attsrv_sendReadBlobRspProcs(pAtt);
}

static void btp_attsrv_recvWriteReqProcs(btp_att_node_t *pAtt, uint08 *pData, uint16 dataLen)
{
	uint08 reason;
	uint16 ahandle;
	const btp_attItem_t *pItem;
	
	ahandle = (((uint16)pData[1])<<8) | pData[0];
	pItem = btp_attsrv_getItem(ahandle);
	if(pItem == nullptr || dataLen < 2){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvWriteReqProcs: no atthandle or error length");
		pAtt->param.server.opcode = BTP_ATT_OPCODE_WRITE_REQ;
		pAtt->param.server.reason = BTP_ATT_ECODE_INVALID_HANDLE;
		btp_attsrv_sendErrorRspProcs(pAtt);
		return;
	}
	if((pItem->permit & BTP_ATT_PERMIT_WRITE) == 0 && (pItem->permit & BTP_ATT_PERMIT_WRITE_WITHOUT_RSP) == 0){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvWriteReqProcs: write is not supported!");
		pAtt->param.server.opcode = BTP_ATT_OPCODE_WRITE_REQ;
		pAtt->param.server.reason = BTP_ATT_ECODE_WRITE_NOT_PERMITTED;
		btp_attsrv_sendErrorRspProcs(pAtt);
		return;
	}

	if(pItem->Write != nullptr){
		reason = pItem->Write(pAtt->handle, pAtt->chnID, pData+2, dataLen-2);
	}else if(dataLen-2 > pItem->attrLen){
		reason = BTP_ATT_ECODE_INVALID_VALUE_LENGTH;
	}else{
		reason = BTP_ATT_ECODE_NONE;
		tmemcpy(pItem->pAttr, pData+2, dataLen-2);
	}
	
	if(reason != BTP_ATT_ECODE_NONE){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvWriteReqProcs: reason-%d, attrLen-%d, dataLen-%d",
			reason, pItem->attrLen, dataLen-2);
		pAtt->param.server.opcode = BTP_ATT_OPCODE_WRITE_REQ;
		pAtt->param.server.reason = reason;
		btp_attsrv_sendErrorRspProcs(pAtt);
	}else if((pItem->permit & BTP_ATT_PERMIT_WRITE) != 0){
		btp_attsrv_sendWriteRspProcs(pAtt);
		tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_attsrv_recvWriteReqProcs: success");
	}
}



/******************************************************************************
 * Function: btp_attsrv_searchItemByHandle
 * Descript: 
 * Params:
 *     @aHanle[IN]--
 *     @pAtt[IN]--
 * Return: None.
 * Others: None.
*******************************************************************************/
static const btp_gattItem_t *btp_attsrv_getItem(uint16 attHandle)
{
	if(attHandle == 0 || attHandle >= sBtpAttItemCount) return nullptr;
	return &sBtpAttItemTable[attHandle];
}
/******************************************************************************
 * Function: btp_attsrv_searchItem
 * Descript: 
 * Params:
 *     @eHandle[IN]--
 *     @pHandle[IN]--
 *     @pAtt[IN]--
 *     @pUUID[IN]--
 *     @sHandle[IN]--
 *     @uuidLen[IN]--
 * Return: None.
 * Others: None.
*******************************************************************************/
static const btp_gattItem_t *btp_attsrv_searchItem(uint16 sHandle, uint16 eHandle, 
	uint08 *pUUID, uint08 uuidLen, uint16 *pHandle)
{
	uint16 index;
	const btp_gattItem_t *pItem;

	pItem = nullptr;
	if(sHandle == 0) sHandle = 1;
	if(eHandle > sBtpAttItemCount) eHandle = sBtpAttItemCount-1;
	if(sHandle > eHandle || sHandle > sBtpAttItemCount) return nullptr;
	
	if(sHandle > eHandle) return nullptr;
	for(index=sHandle; index<=eHandle; index++){
		pItem = &sBtpAttItemTable[index];
		if(pItem->uuidLen == uuidLen && tmemcmp(pItem->pUUID, pUUID, uuidLen) == 0) break;
	}
	if(index > eHandle) return nullptr;
	if(pHandle != nullptr) *pHandle = index;
	return pItem;
}


#endif //#if (TLKBTP_CFG_ATTSRV_ENABLE)

