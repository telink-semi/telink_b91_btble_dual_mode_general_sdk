/********************************************************************************************************
 * @file     btp_iap.c
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
#if (TLK_STK_BTP_ENABLE)
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/btp_adapt.h"
#include "tlkstk/bt/btp/rfcomm/btp_rfcomm.h"
#include "tlkstk/bt/btp/iap/btp_iap.h"



static void btp_iap_resetItem(btp_iap_item_t *pItem);

static int  btp_iap_rfcommEventCB(uint08 evtID, uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen);
static void btp_iap_rfcommRdataCB(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen);
static void btp_iap_rfcommCreditCB(uint08 aclHandle, uint16 rfcHandle, uint08 flowNumb);


static btp_iap_ctrl_t sBtpIapCtrl;
static BtpIapRecvDataCB sBtpIapRecvDataCB = nullptr;

/******************************************************************************
 * Function: IAP Init interface
 * Descript: This interface be used by User to initial iap resource.
 * Params:
 * Return:
*******************************************************************************/
int btp_iap_init(void)
{
	int ret;

	tmemset(&sBtpIapCtrl, 0, sizeof(btp_iap_ctrl_t));
	
	btp_rfcomm_appendChnDict(0, TLKBT_CFG_IAP_RFC_CHANNEL, BTP_RFCOMM_SRVID_IAP); //Add an element to the dictionary
	ret = btp_rfcomm_appendService(BTP_RFCOMM_SRVID_IAP, btp_iap_rfcommEventCB, btp_iap_rfcommRdataCB, btp_iap_rfcommCreditCB);
	if(ret != TLK_ENONE){
		tlkapi_error(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "btp_iap_init: failure");
	}
	
	return ret;
}

/******************************************************************************
 * Function: IAP Register Callback interface
 * Descript:
 * Params:
 * Return:
*******************************************************************************/
void btp_iap_regDataCB(BtpIapRecvDataCB dataCB)
{
	sBtpIapRecvDataCB = dataCB;
}


/******************************************************************************
 * Function: IAP send Connect interface
 * Descript:
 * Params:
 * Return:
*******************************************************************************/
int  btp_iap_connect(uint16 aclHandle, uint08 channel)
{
	btp_iap_item_t *pItem;
	
	if(aclHandle == 0 || channel == 0) return -TLK_EPARAM;
	
	pItem = btp_iap_getUsedItem(aclHandle);
	if(pItem != nullptr) return -TLK_EBUSY;

	btp_rfcomm_appendChnDict(aclHandle, channel, BTP_RFCOMM_SRVID_IAP);
	return btp_rfcomm_connectChannel(aclHandle, channel, BTP_RFCOMM_SRVID_IAP);
}

/******************************************************************************
 * Function: IAP send DisConnect interface
 * Descript:
 * Params:
 * Return:
*******************************************************************************/
int  btp_iap_disconn(uint16 aclHandle)
{
	int ret;
	uint08 state;
	btp_iap_item_t *pItem;
	
	if(aclHandle == 0) return -TLK_EPARAM;
	
	pItem = btp_iap_getUsedItem(aclHandle);
	if(pItem != nullptr) return -TLK_EBUSY;
	
	ret = btp_rfcomm_disconnChannel(pItem->rfcHandle);
	if(ret == TLK_ENONE || ret == TLK_EBUSY) return ret;

	state = pItem->state;
	btp_iap_resetItem(pItem);
	if(state == TLK_STATE_CONNING){
		btp_send_connectEvt(TLK_ETIMEOUT, BTP_PTYPE_IAP, 0, aclHandle, 0);
	}else{
		btp_send_disconnEvt(BTP_PTYPE_IAP, 0, aclHandle);
	}
	return TLK_ENONE;
}

/******************************************************************************
 * Function: IAP send Destroy interface
 * Descript:
 * Params:
 * Return:
*******************************************************************************/
void btp_iap_destroy(uint16 aclHandle)
{
	btp_rfcomm_destroyChannel(aclHandle);
}

/******************************************************************************
 * Function: IAP send Data interface
 * Descript:
 * Params:
 * Return:
*******************************************************************************/
int btp_iap_sendData(uint16 aclHandle, uint08 *pHead, uint08 headLen, uint08 *pData, uint16 dataLen)
{
	btp_iap_item_t *pItem;

	if(pHead == nullptr && headLen != 0) headLen = 0;
	if(pData == nullptr && dataLen != 0) dataLen = 0;
	if(aclHandle == 0){
		tlkapi_error(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "btp_iap_sendData: failure - invalid handle");
		return -TLK_EHANDLE;
	}
	if((headLen+dataLen) == 0){
		tlkapi_error(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "btp_iap_sendData: failure - invalid param");
		return -TLK_EPARAM;
	}
	
	pItem = btp_iap_getConnItem(aclHandle);
	if(pItem == nullptr){
		tlkapi_error(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "btp_iap_sendData: failure - invalid handle");
		return -TLK_EPARAM;
	}
	
	if(dataLen > pItem->mtuSize){
		tlkapi_error(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "btp_iap_sendData: failure - overflow");
		return -TLK_EOVERFLOW;
	}
	
	return btp_rfcomm_sendData(pItem->rfcHandle, pHead, headLen, pData, dataLen);
}

/******************************************************************************
 * Function: IAP get Idle count interface
 * Descript:
 * Params:
 * Return:
*******************************************************************************/
uint08 btp_iap_getIdleCount(void)
{
	uint08 index;
	uint08 count = 0;
	for(index=0; index<TLK_BT_IAP_MAX_NUMB; index++){
		if(sBtpIapCtrl.item[index].state == TLK_STATE_CLOSED) count ++;
	}
	return count;
}

/******************************************************************************
 * Function: IAP get Used count interface
 * Descript:
 * Params:
 * Return:
*******************************************************************************/
uint08 btp_iap_getUsedCount(void)
{
	uint08 index;
	uint08 count = 0;
	for(index=0; index<TLK_BT_IAP_MAX_NUMB; index++){
		if(sBtpIapCtrl.item[index].state != TLK_STATE_CLOSED) count ++;
	}
	return count;
}

/******************************************************************************
 * Function: IAP get Connect count interface
 * Descript:
 * Params:
 * Return:
*******************************************************************************/
uint08 btp_iap_getConnCount(void)
{
	uint08 index;
	uint08 count = 0;
	for(index=0; index<TLK_BT_IAP_MAX_NUMB; index++){
		if(sBtpIapCtrl.item[index].state == TLK_STATE_CONNECT) count ++;
	}
	return count;
}

/******************************************************************************
 * Function: IAP get Idle interface
 * Descript:
 * Params:
 * Return:The iap control block.
*******************************************************************************/
btp_iap_item_t *btp_iap_getIdleItem(void)
{
	uint08 index;
	for(index=0; index<TLK_BT_IAP_MAX_NUMB; index++){
		if(sBtpIapCtrl.item[index].state == TLK_STATE_CLOSED) break;
	}
	if(index == TLK_BT_IAP_MAX_NUMB) return nullptr;
	return &sBtpIapCtrl.item[index];
}

/******************************************************************************
 * Function: IAP get used interface
 * Descript:
 * Params:
 * Return:The iap control block.
*******************************************************************************/
btp_iap_item_t *btp_iap_getUsedItem(uint16 aclHandle)
{
	uint08 index;
	for(index=0; index<TLK_BT_IAP_MAX_NUMB; index++){
		if(sBtpIapCtrl.item[index].state != TLK_STATE_CLOSED && sBtpIapCtrl.item[index].aclHandle == aclHandle) break;
	}
	if(index == TLK_BT_IAP_MAX_NUMB) return nullptr;
	return &sBtpIapCtrl.item[index];
}

/******************************************************************************
 * Function: IAP get Connected interface
 * Descript:
 * Params:
 * Return:The iap control block.
*******************************************************************************/
btp_iap_item_t *btp_iap_getConnItem(uint16 aclHandle)
{
	uint08 index;
	for(index=0; index<TLK_BT_IAP_MAX_NUMB; index++){
		if(sBtpIapCtrl.item[index].state == TLK_STATE_CONNECT && sBtpIapCtrl.item[index].aclHandle == aclHandle) break;
	}
	if(index == TLK_BT_IAP_MAX_NUMB) return nullptr;
	return &sBtpIapCtrl.item[index];
}


static void btp_iap_resetItem(btp_iap_item_t *pItem)
{
	if(pItem == nullptr) return;
	pItem->state = TLK_STATE_CLOSED;
	pItem->rfcHandle = 0;
	pItem->aclHandle = 0;
	pItem->mtuSize = 0;
	pItem->credit = 0;
}

static int btp_iap_rfcommEventCB(uint08 evtID, uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen)
{
	btp_iap_item_t *pIap;
	
	tlkapi_info(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "btp_iap_rfcommEventCB: evtId %d aclHandle %d rfcHandle %d datalen %d", 
		evtID, aclHandle, rfcHandle, dataLen);
	
	if(evtID == BTP_RFCOMM_EVTID_REQUEST){
		pIap = btp_iap_getUsedItem(aclHandle);
		if(pIap != nullptr){
			tlkapi_error(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "IapEvent[Request]: The request was rejected - Device Running");
			return -TLK_EBUSY;
		}else if(btp_iap_getIdleCount() == 0){
			tlkapi_error(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "IapEvent[Request]: The request was rejected - No Idle Device");
			return -TLK_EQUOTA;
		}else{
			tlkapi_trace(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "IapEvent[Request]: The request was accepted");
			return TLK_ENONE;
		}
	}
	else if(evtID == BTP_RFCOMM_EVTID_CONNECT){
		uint16 mtuSize = 0;
		btp_rfcomm_connectEvt_t *pEvt;
		pEvt = (btp_rfcomm_connectEvt_t*)pData;
		pIap = btp_iap_getUsedItem(aclHandle);
		if(pIap == nullptr) pIap = btp_iap_getIdleItem();
		if(pIap == nullptr){
			tlkapi_error(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "IapEvent[Connect]: Connection was disconnected - no resource");
			btp_rfcomm_disconnChannel(rfcHandle);
			return -TLK_EFAIL;
		}
		if(pEvt->status != 0 || btp_rfcomm_getMtuSize(rfcHandle, &mtuSize) != TLK_ENONE){
			uint08 state = pIap->state;
			tlkapi_info(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "IapEvent[Connect]: Get MtuSize Failure");
			btp_iap_resetItem(pIap);
			btp_rfcomm_disconnChannel(rfcHandle);
			if(state == TLK_STATE_CONNING){
				btp_send_connectEvt(TLK_EFAIL, BTP_PTYPE_IAP, 0, aclHandle, 0);
			}
			return -TLK_EFAIL;
		}
		tlkapi_info(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "IapEvent[Connect]: Connection is success");
		pIap->state = TLK_STATE_CONNECT;
		pIap->rfcHandle = rfcHandle;
		pIap->aclHandle = aclHandle;
		pIap->mtuSize = mtuSize;
		btp_send_connectEvt(TLK_ENONE, BTP_PTYPE_IAP, 0, aclHandle, 0);
	}
	else if(evtID == BTP_RFCOMM_EVTID_DISCONN){
		pIap = btp_iap_getUsedItem(aclHandle);
		if(pIap == nullptr){
			tlkapi_error(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "IapEvent[Disconn]: fault - no device");
		}else{
			tlkapi_info(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "IapEvent[Disconn]: Device Disconnect");
			btp_iap_resetItem(pIap);
			btp_send_disconnEvt(BTP_PTYPE_IAP, 0, aclHandle);
		}
	}
	return TLK_ENONE;
}
static void btp_iap_rfcommRdataCB(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen)
{
    tlkapi_info(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "IapRdataCB: aclHandle %d rfcHandle %d datalen %d", aclHandle, rfcHandle, dataLen);
	if(sBtpIapRecvDataCB != nullptr){
		sBtpIapRecvDataCB(aclHandle, rfcHandle, pData, dataLen);
	}
}
static void btp_iap_rfcommCreditCB(uint08 aclHandle, uint16 rfcHandle, uint08 flowNumb)
{
	btp_iap_item_t *pIap;
	
	pIap = btp_iap_getUsedItem(aclHandle);
	if(pIap == nullptr) return;

	tlkapi_info(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "IapCreditCB: aclHandle %d rfcHandle %d flowNum %d", aclHandle, rfcHandle, flowNumb);
	pIap->credit = flowNumb;
}


#endif //#if (TLK_STK_BTP_ENABLE)

