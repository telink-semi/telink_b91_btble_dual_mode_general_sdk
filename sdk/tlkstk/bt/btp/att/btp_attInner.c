/********************************************************************************************************
 * @file     btp_attInner.c
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
#if (TLKBTP_CFG_ATT_ENABLE)
#include "tlkstk/bt/btp/btp_adapt.h"
#include "tlkstk/bt/btp/att/btp_att.h"
#include "tlkstk/bt/btp/att/btp_attInner.h"
#include "tlkstk/bt/btp/att/btp_attsrv.h"



static int  btp_att_eventCB(uint08 evtID, uint16 psmID, uint08 *pData, uint16 dataLen);
static void btp_att_rdataCB(uint16 psmID, uint16 chnID, uint16 handle, uint08 *pData, uint16 dataLen);

static int btp_att_closedEventDeal(uint08 evtID, uint16 psmID, uint08 *pData, uint16 dataLen);
static int btp_att_requestEventDeal(uint08 evtID, uint16 psmID, uint08 *pData, uint16 dataLen);
static int btp_att_connectEventDeal(uint08 evtID, uint16 psmID, uint08 *pData, uint16 dataLen);
static int btp_att_disconnEventDeal(uint08 evtID, uint16 psmID, uint08 *pData, uint16 dataLen);



static btp_att_ctrl_t sBtpAttCtrl;

volatile uint32 AAAA_att_test001 = 0;

int btp_att_innerInit(void)
{
	tmemset(&sBtpAttCtrl, 0, sizeof(btp_att_ctrl_t));
	AAAA_att_test001 = bth_l2cap_regServiceCB(BTP_PSMID_ATT, btp_att_eventCB, btp_att_rdataCB);
	return TLK_ENONE;
}


void btp_att_destroy(uint16 aclHandle)
{
	uint08 state;
	uint08 usrID;
	uint16 chnID;
	btp_att_node_t *pAtt;

	state = TLK_STATE_CLOSED;
	usrID = BTP_USRID_CLIENT;
	chnID = 0;
	
	pAtt = btp_att_getAnyUsedNode(aclHandle);
	if(pAtt != nullptr){
		state = pAtt->state;
		usrID = pAtt->usrID;
		chnID = pAtt->chnID;
		btp_att_resetNode(pAtt);
	}

	bth_signal_destoryConnect(aclHandle, BTP_PSMID_ATT);
	
	if(state == TLK_STATE_CONNING){
		btp_send_connectEvt(TLK_ETIMEOUT, BTP_PTYPE_ATT, usrID, aclHandle, chnID);
	}else if(state != TLK_STATE_CLOSED){
		btp_send_disconnEvt(BTP_PTYPE_ATT, usrID, aclHandle);
	}
}

/******************************************************************************
 * Function: ATT reset current control block interface.
 * Descript: Defines the format of the att trigger reset the att control block.
 * Params:
 * 	    @pSnk[IN]--The Current att control block.
 * Return: null.
 *******************************************************************************/
void btp_att_resetNode(btp_att_node_t *pItem)
{
	if(pItem == nullptr) return;
	btp_adapt_removeTimer(&pItem->timer);
	tmemset(pItem, 0, sizeof(btp_att_node_t));
}

/******************************************************************************
 * Function: ATT check the att idle control block num interface.
 * Descript: Defines check how many att idle control block exist.
 * Params:
 * Return: Returning The total number of att control block.
 *******************************************************************************/
uint08 btp_att_getIdleCount(void)
{
	uint08 index;
	uint08 count = 0;
	for(index=0; index<TLK_BT_ATT_MAX_NUMB; index++){
		if(sBtpAttCtrl.item[index].state == TLK_STATE_CLOSED) count++;
	}
	return count;
}

/******************************************************************************
 * Function: ATT check the att used control block num interface.
 * Descript: Defines check how many att used control block exist.
 * Params:
 * Return: Returning The total number of att control block.
 *******************************************************************************/
uint08 btp_att_getUsedCount(void)
{
	uint08 index;
	uint08 count = 0;
	for(index=0; index<TLK_BT_ATT_MAX_NUMB; index++){
		if(sBtpAttCtrl.item[index].state != TLK_STATE_CLOSED) count++;
	}
	return count;
}

/******************************************************************************
 * Function: ATT check the att connected control block num interface.
 * Descript: Defines check how many att connected control block exist.
 * Params:
 * Return: Returning The total number of att control block.
 *******************************************************************************/
uint08 btp_att_getConnCount(void)
{
	uint08 index;
	uint08 count = 0;
	for(index=0; index<TLK_BT_ATT_MAX_NUMB; index++){
		if(sBtpAttCtrl.item[index].state == TLK_STATE_CONNECT) count++;
	}
	return count;
}

/******************************************************************************
 * Function: ATT Get the att idle control block.
 * Descript: Defines get a new att control block in resource.
 * Params:
 * Return: Returning The new att control block.
 *******************************************************************************/
btp_att_node_t *btp_att_getIdleNode(void)
{
	uint08 index;
	for(index=0; index<TLK_BT_ATT_MAX_NUMB; index++){
		if(sBtpAttCtrl.item[index].state == TLK_STATE_CLOSED) break;
	}
	if(index == TLK_BT_ATT_MAX_NUMB) return nullptr;
	return &sBtpAttCtrl.item[index];
}

btp_att_node_t *btp_att_getBusyNode(uint16 aclHandle)
{
	uint08 index;
	for(index=0; index<TLK_BT_ATT_MAX_NUMB; index++){
		if(sBtpAttCtrl.item[index].state == TLK_STATE_CONNING
			&& sBtpAttCtrl.item[index].handle == aclHandle) break;
	}
	if(index == TLK_BT_ATT_MAX_NUMB) return nullptr;
	return &sBtpAttCtrl.item[index];
}
/******************************************************************************
 * Function: ATT Get the att Used control block.
 * Descript: Defines get the special Used att control block in resource.
 *           via aclhandle and usrid.
 * Params:
 *       @aclHandle[IN]--The aclHandle to  search the att control block.
 *       @usrID[IN]--The usrID to search the att control block
 * Return: Returning The used att control block.
 *******************************************************************************/
btp_att_node_t *btp_att_getUsedNode(uint16 aclHandle, uint08 usrID)
{
	uint08 index;
	for(index=0; index<TLK_BT_ATT_MAX_NUMB; index++){
		if(sBtpAttCtrl.item[index].state != TLK_STATE_CLOSED
			&& sBtpAttCtrl.item[index].usrID == usrID
			&& sBtpAttCtrl.item[index].handle == aclHandle) break;
	}
	if(index == TLK_BT_ATT_MAX_NUMB) return nullptr;
	return &sBtpAttCtrl.item[index];
}

/******************************************************************************
 * Function: ATT Get the att Connected control block.
 * Descript: Defines get the special Connected att control block in resource.
 *           via aclhandle and usrid.
 * Params:
 *       @aclHandle[IN]--The aclHandle to  search the att control block.
 *       @usrID[IN]--The usrID to search the att control block
 * Return: Returning The Connected att control block.
 *******************************************************************************/
btp_att_node_t *btp_att_getConnNode(uint16 aclHandle, uint08 usrID)
{
	uint08 index;
	for(index=0; index<TLK_BT_ATT_MAX_NUMB; index++){
		if(sBtpAttCtrl.item[index].state == TLK_STATE_CONNECT
			&& sBtpAttCtrl.item[index].usrID == usrID
			&& sBtpAttCtrl.item[index].handle == aclHandle) break;
	}
	if(index == TLK_BT_ATT_MAX_NUMB) return nullptr;
	return &sBtpAttCtrl.item[index];
}

/******************************************************************************
 * Function: ATT Get the att Used control block.
 * Descript: Defines get the client or server Used att control block 
 *           in resource via aclhandle and usrid. 
 * Params:
 *       @aclHandle[IN]--The aclHandle to search the att control block.
 * Return: Returning The used att control block.
 *******************************************************************************/
btp_att_node_t *btp_att_getUsedNodeByChnID(uint16 aclHandle, uint16 chnID)
{
	uint08 index;
	for(index=0; index<TLK_BT_ATT_MAX_NUMB; index++){
		if(sBtpAttCtrl.item[index].state != TLK_STATE_CLOSED
			&& sBtpAttCtrl.item[index].handle == aclHandle
			&& sBtpAttCtrl.item[index].chnID == chnID) break;
	}
	if(index == TLK_BT_ATT_MAX_NUMB) return nullptr;
	return &sBtpAttCtrl.item[index];
}

/******************************************************************************
 * Function: ATT Get the att Used control block.
 * Descript: Defines get the client or server Used att control block 
 *           in resource via aclhandle and usrid. 
 * Params:
 *       @aclHandle[IN]--The aclHandle to search the att control block.
 * Return: Returning The used att control block.
 *******************************************************************************/
btp_att_node_t *btp_att_getAnyUsedNode(uint16 aclHandle)
{
	uint08 index;
	for(index=0; index<TLK_BT_ATT_MAX_NUMB; index++){
		if(sBtpAttCtrl.item[index].state != TLK_STATE_CLOSED
			&& sBtpAttCtrl.item[index].handle == aclHandle) break;
	}
	if(index == TLK_BT_ATT_MAX_NUMB) return nullptr;
	return &sBtpAttCtrl.item[index];
}

/******************************************************************************
 * Function: ATT Get the att Connected control block.
 * Descript: Defines get the client or server Connected att control block 
 *           in resource via aclhandle and usrid. 
 * Params:
 *       @aclHandle[IN]--The aclHandle to  search the att control block.
 * Return: Returning The connected att control block.
 *******************************************************************************/
btp_att_node_t *btp_att_getAnyConnNode(uint16 aclHandle)
{
	uint08 index;
	for(index=0; index<TLK_BT_ATT_MAX_NUMB; index++){
		if(sBtpAttCtrl.item[index].state == TLK_STATE_CONNECT
			&& sBtpAttCtrl.item[index].handle == aclHandle) break;
	}
	if(index == TLK_BT_ATT_MAX_NUMB) return nullptr;
	return &sBtpAttCtrl.item[index];
}






static int btp_att_eventCB(uint08 evtID, uint16 psmID, uint08 *pData, uint16 dataLen)
{
	if(evtID == BTH_L2CAP_EVTID_CLOSED){
		tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_eventCB[CLOSED]: {evtID-%d,psmID-%d,dataLen-%d}", evtID, psmID, dataLen);
		return btp_att_closedEventDeal(evtID, psmID, pData, dataLen);
	}else if(evtID == BTH_L2CAP_EVTID_REQUEST){
		tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_eventCB[REQUEST]: {evtID-%d,psmID-%d,dataLen-%d}", evtID, psmID, dataLen);
		return btp_att_requestEventDeal(evtID, psmID, pData, dataLen);
	}else if(evtID == BTH_L2CAP_EVTID_CONNECT){
		tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_eventCB[CONNECT]: {evtID-%d,psmID-%d,dataLen-%d}", evtID, psmID, dataLen);
		return btp_att_connectEventDeal(evtID, psmID, pData, dataLen);
	}else if(evtID == BTH_L2CAP_EVTID_DISCONN){
		tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_eventCB[DISCONN]: {evtID-%d,psmID-%d,dataLen-%d}", evtID, psmID, dataLen);
		return btp_att_disconnEventDeal(evtID, psmID, pData, dataLen);
	}else{
		return -TLK_ENOSUPPORT;
	}
	
	return TLK_ENONE;
}
static void btp_att_rdataCB(uint16 psmID, uint16 chnID, uint16 handle, uint08 *pData, uint16 dataLen)
{
	btp_att_node_t *pAtt;

	pAtt = btp_att_getUsedNodeByChnID(handle, chnID);
	if(pAtt == nullptr){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_rdataCB: no node - chnID[%d]", chnID);
		return;
	}

//	tlkapi_debug_delayForPrint(20000);

	tlkapi_array(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_rdataCB: ", pData, dataLen);
	if(pAtt->usrID == BTP_USRID_SERVER){
		btp_attsrv_recvHandler(pAtt, pData, dataLen);
	}else{
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_rdataCB: no supp - usrID[%d]", pAtt->usrID);
	}
}


static int btp_att_closedEventDeal(uint08 evtID, uint16 psmID, uint08 *pData, uint16 dataLen)
{
	uint08 usrID;
	btp_att_node_t *pAtt;
	bth_l2cap_closedEvt_t *pEvt;

	pEvt = (bth_l2cap_closedEvt_t*)pData;
	pAtt = btp_att_getAnyUsedNode(pEvt->handle);
	if(pAtt == nullptr) return -TLK_EFAIL;
		
	tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_closedEventDeal:");
	
	usrID = pAtt->usrID;
	btp_att_resetNode(pAtt);
	btp_send_disconnEvt(BTP_PTYPE_ATT, usrID, pEvt->handle);

	return TLK_ENONE;
}
static int btp_att_requestEventDeal(uint08 evtID, uint16 psmID, uint08 *pData, uint16 dataLen)
{
	#if (TLKBTP_CFG_ATTSRV_ENABLE)
		btp_att_node_t *pAtt;
		bth_l2cap_requestEvt_t *pEvt;
		pEvt = (bth_l2cap_requestEvt_t*)pData;
		pAtt = btp_att_getAnyUsedNode(pEvt->handle);
		if(pAtt != nullptr) return -TLK_EREPEAT;
		if(btp_att_getIdleCount() == 0) return -TLK_EQUOTA;
		else return TLK_ENONE;
	#else
		return -TLK_ENOSUPPORT;
	#endif
}
static int btp_att_connectEventDeal(uint08 evtID, uint16 psmID, uint08 *pData, uint16 dataLen)
{
	bth_handle_t *pHandle;
	btp_att_node_t *pAtt;
	bth_l2cap_connectEvt_t *pEvt;

	pEvt = (bth_l2cap_connectEvt_t*)pData;
	if(pEvt->scid == 0){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_connectEventDeal: error param");
		return -TLK_EPARAM;
	}
	
	pHandle = bth_handle_getConnAcl(pEvt->handle);
	if(pHandle == nullptr){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_connectEventDeal: invalid handle");
		bth_signal_disconnChannel(pEvt->handle, pEvt->scid);
		return -TLK_EFAIL;
	}
	
	pAtt = btp_att_getAnyUsedNode(pEvt->handle);
	if(pAtt != nullptr && (pAtt->state == TLK_STATE_CONNECT || pAtt->state == TLK_STATE_DISCING)){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_connectEventDeal: error event");
		bth_signal_disconnChannel(pEvt->handle, pEvt->scid);
		return -TLK_EFAIL;
	}
	if(pAtt == nullptr) pAtt = btp_att_getIdleNode();
	if(pAtt == nullptr){
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_connectEventDeal: no idle node");
		bth_signal_disconnChannel(pEvt->handle, pEvt->scid);
		return -TLK_EFAIL;
	}
	
	pAtt->state = TLK_STATE_CONNING;
	if(pAtt->usrID == BTP_USRID_NONE){
		if(!pEvt->isActive) pAtt->usrID = BTP_USRID_SERVER;
		else pAtt->usrID = BTP_USRID_CLIENT;
	}
	pAtt->chnID = pEvt->scid;
	pAtt->handle = pEvt->handle;
	pAtt->mtuSize = pEvt->mtuSize;
	
	tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_connectEventDeal:{chnID-0x%x,active-%d,SCID--0x%x,usrID-%d}",
		pAtt->chnID, pEvt->isActive, pEvt->scid, pAtt->usrID);
	if(pAtt->usrID == BTP_USRID_SERVER){
		btp_attsrv_connectEvt(pAtt);
	}else if(pAtt->usrID == BTP_USRID_CLIENT){ //not supported
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_connectEventDeal: not support client");
		btp_att_resetNode(pAtt);
		bth_signal_disconnChannel(pEvt->handle, pEvt->scid);
	}else{
		tlkapi_error(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_connectEventDeal: invalid usrid");
		btp_att_resetNode(pAtt);
		bth_signal_disconnChannel(pEvt->handle, pEvt->scid);
	}
	return TLK_ENONE;
}
static int btp_att_disconnEventDeal(uint08 evtID, uint16 psmID, uint08 *pData, uint16 dataLen)
{
	btp_att_node_t *pAtt;
	bth_l2cap_disconnEvt_t *pEvt;

	pEvt = (bth_l2cap_disconnEvt_t*)pData;
	pAtt = btp_att_getAnyUsedNode(pEvt->handle);
	if(pAtt == nullptr) return -TLK_EFAIL;

	tlkapi_trace(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_disconnEventDeal:{chnID-0x%x,SCID-0x%x}",
		pAtt->chnID, pEvt->scid);
	
	pAtt->state = TLK_STATE_DISCING;
	if(pAtt->usrID == BTP_USRID_SERVER){
		btp_attsrv_disconnEvt(pAtt);
	}else if(pAtt->usrID == BTP_USRID_CLIENT){ //not supported
		
	}
	btp_att_resetNode(pAtt);
	return TLK_ENONE;
}




#endif //#if (TLKBTP_CFG_ATT_ENABLE)


