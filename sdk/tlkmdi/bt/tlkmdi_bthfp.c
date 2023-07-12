/********************************************************************************************************
 * @file	tlkmdi_bthfp.c
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
#if (TLK_MDI_BTHFP_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmdi/bt/tlkmdi_bthfp.h"


#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlksys/prt/tlkpto_comm.h"


#define TLKMDI_BTHFP_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_BT << 24) | (TLK_MINOR_DBGID_MDI_BT_HFP << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_BTHFP_DBG_SIGN       "[MHFP]"


static void tlkmdi_bthfp_hfReset(uint16 aclHandle);
static int tlkmdi_bthfp_hfCodecChangedEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_bthfp_hfVolumeChangedEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_bthfp_hfStatusChangedEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_bthfp_hfStatusInquiryEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_bthfp_hfNumberInquiryEvt(uint08 *pData, uint16 dataLen);

static void tlkmdi_bthfp_hfSendStatusEvt(uint08 evtID, uint16 handle, uint08 callNum, uint08 callDir, uint08 numbLen);

static void tlkmdi_bthfp_hfInquiryStopDeal(uint16 handle);
static void tlkmdi_bthfp_hfInquiryBusyDeal(uint16 handle,tlkmdi_hfphf_unit_t *pUnit);
static void tlkmdi_bthfp_hfInquirySaveDeal(btp_hfpNumberInquiryEvt_t *pEvt);

static void tlkmdi_bthfp_hfInquiryCheckDeal(uint16 handle, tlkmdi_hfphf_unit_t *pUnit);
static void tlkmdi_bthfp_hfInquiryPushDeal(uint16 handle, tlkmdi_hfphf_unit_t *pTemp);

static tlkmdi_hfphf_unit_t *tlkmdi_bthfp_hfGetIdleCallTemp(uint16 handle);
static tlkmdi_hfphf_unit_t *tlkmdi_bthfp_hfGetUsedCallTemp(uint16 handle, uint08 numbLen, uint08 *pNumber);
static tlkmdi_hfphf_unit_t *tlkmdi_bthfp_hfGetIdleCallUnit(uint16 handle);
//static tlkmdi_hfphf_unit_t *tlkmdi_bthfp_hfGetUsedCallUnit(uint08 numbLen, uint08 *pNumber);


static tlkmdi_hfphf_ctrl_t sTlkMdiBtHfpCtrl[TLKMDI_HFPHF_MAX_NUMBER];


/******************************************************************************
 * Function: tlkmdi_hfphf_init.
 * Descript: Trigger to Initial the HF control block and register the callback.
 * Params: None.
 * Return: Return TLK_ENONE is success, other's value is false.
 * Others: None.
*******************************************************************************/
int tlkmdi_bthfp_init(void)
{	
	btp_event_regCB(BTP_EVTID_HFPHF_CODEC_CHANGED,  tlkmdi_bthfp_hfCodecChangedEvt);
	btp_event_regCB(BTP_EVTID_HFPHF_VOLUME_CHANGED, tlkmdi_bthfp_hfVolumeChangedEvt);
	btp_event_regCB(BTP_EVTID_HFPHF_STATUS_CHANGED, tlkmdi_bthfp_hfStatusChangedEvt);
	btp_event_regCB(BTP_EVTID_HFPHF_STATUS_INQUIRY, tlkmdi_bthfp_hfStatusInquiryEvt);
	btp_event_regCB(BTP_EVTID_HFPHF_NUMBER_INQUIRY, tlkmdi_bthfp_hfNumberInquiryEvt);
		
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_hfphf_destroy.
 * Descript: Reset the HF control block adn release resource.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_bthfp_destroy(uint16 aclHandle)
{
	tlkmdi_bthfp_hfReset(aclHandle);
}

/******************************************************************************
 * Function: tlkmdi_bthfp_getHfCallNumber.
 * Descript: Get the call number.
 * Params: None.
 * Return: Return call number.
 * Others: None.
*******************************************************************************/
uint08 *tlkmdi_bthfp_getHfCallNumber(uint16 aclHandle, uint08 callNum)
{
	tlkmdi_hfphf_ctrl_t *pItem;
	pItem = tlkmdi_bthfp_getUsedItem(aclHandle);
	
	if(pItem == nullptr || callNum >= 2) return nullptr;
	return pItem->unit[callNum].number;
}


int tlkmdi_bthfp_rejectHfWaitAndKeepActive(void)
{
	if(sTlkMdiBtHfpCtrl[0].handle == 0) return -TLK_EFAIL;
	return btp_hfphf_rejectWaitAndKeepActive(sTlkMdiBtHfpCtrl[0].handle);
	return 0;
}

int tlkmdi_bthfp_acceptHfWaitAndHoldActive(void)
{
	if(sTlkMdiBtHfpCtrl[0].handle == 0) return -TLK_EFAIL;
	return btp_hfphf_acceptWaitAndHoldActive(sTlkMdiBtHfpCtrl[0].handle);
	return 0;
}

int tlkmdi_bthfp_hungupHfActiveAndResumeHold(void)
{
	if(sTlkMdiBtHfpCtrl[0].handle == 0) return -TLK_EFAIL;
	return btp_hfphf_hungUpActiveAndResumeHold(sTlkMdiBtHfpCtrl[0].handle);
	return 0;
}



static void tlkmdi_bthfp_hfReset(uint16 aclHandle)
{
	uint16 handle;
	uint08 callDir;
	uint08 numbLen;
	uint08 callFlag;
	tlkmdi_hfphf_unit_t *pUnit;
	tlkmdi_hfphf_ctrl_t *pItem;

	pItem = tlkmdi_bthfp_getUsedItem(aclHandle);
	if(pItem == nullptr){
		tlkapi_error(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN,"tlkmdi_bthfp_hfReset - failure: not exit handle: 0x%x",aclHandle);
		return;
	}

	handle = pItem->handle;
	pItem->handle = 0;
	pItem->flags = TLKMDI_HFPHF_FLAG_NONE;

	pUnit = &pItem->unit[0];
	if(pUnit->numbLen != 0 && (pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_READY) != 0){
		callDir  = pUnit->callDir;
		numbLen  = pUnit->numbLen;
		callFlag = pUnit->callFlag;
		pUnit->numbLen  = 0;
		pUnit->callDir  = 0;
		pUnit->callStat = 0;
		pUnit->callFlag = TLKMDI_HFPHF_CALL_FLAG_NONE;
		pUnit->callBusy = TLKMDI_HFPHF_CALL_BUSY_NONE;
		if((callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_MASK) != 0 && (callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_CLOSE) == 0){
			tlkapi_warn(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "TLKPTI_PHONE_MSGID_CALL_CLOSE_EVT: 04");
			tlkmdi_bthfp_hfSendStatusEvt(TLKPTI_PHONE_MSGID_CALL_CLOSE_EVT, handle, 0, callDir, numbLen);
		}
	}

	pUnit = &pItem->unit[1];
	if(pUnit->numbLen != 0 && (pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_READY) != 0){
		callDir  = pUnit->callDir;
		numbLen  = pUnit->numbLen;
		callFlag = pUnit->callFlag;
		pUnit->numbLen  = 0;
		pUnit->callDir  = 0;
		pUnit->callStat = 0;
		pUnit->callFlag = TLKMDI_HFPHF_CALL_FLAG_NONE;
		pUnit->callBusy = TLKMDI_HFPHF_CALL_BUSY_NONE;
		if((callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_MASK) != 0 && (callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_CLOSE) == 0){
			tlkapi_warn(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "TLKPTI_PHONE_MSGID_CALL_CLOSE_EVT: 05");
			tlkmdi_bthfp_hfSendStatusEvt(TLKPTI_PHONE_MSGID_CALL_CLOSE_EVT, handle, 1, callDir, numbLen);
		}
	}
}
static void tlkmdi_bthfp_hfSendStatusEvt(uint08 evtID, uint16 handle, uint08 callNum, uint08 callDir, uint08 numbLen)
{
	tlkmdi_hfphf_statusEvt_t evt;

	tlkapi_trace(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfSendStatusEvt: evtID-%d,callNum-%d,callDir-%d,numbLen-%d", 
		evtID, callNum, callDir, numbLen);
	
	evt.handle  = handle;
	evt.callNum = callNum;
	evt.callDir = callDir;
	evt.numbLen = numbLen;
	tlksys_sendInnerMsg(TLKSYS_TASKID_PHONE, evtID, (uint08*)&evt, sizeof(tlkmdi_hfphf_statusEvt_t));
}
static int tlkmdi_bthfp_hfCodecChangedEvt(uint08 *pData, uint16 dataLen)
{
//	btp_hfpCodecChangedEvt_t *pEvt;
//	pEvt = (btp_hfpCodecChangedEvt_t*)pData;	
	return TLK_ENONE;
}
static int tlkmdi_bthfp_hfVolumeChangedEvt(uint08 *pData, uint16 dataLen)
{
	btp_hfpVolumeChangedEvt_t *pEvt;

	pEvt = (btp_hfpVolumeChangedEvt_t*)pData;
	if(pEvt->volType == BTP_HFP_VOLUME_TYPE_SPK){
		tlkapi_trace(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfVolumeChangedEvt: %d", pEvt->volume);
		tlksys_sendInnerMsg(TLKSYS_TASKID_AUDIO, TLKPTI_AUD_MSGID_HFP_CHG_VOLUME_EVT, &pEvt->volume, 1);
	}
	return TLK_ENONE;
}

static int tlkmdi_bthfp_hfStatusChangedEvt(uint08 *pData, uint16 dataLen)
{
	tlkmdi_hfphf_ctrl_t *pItem;
	btp_hfpStatusChangedEvt_t *pEvt;

	pEvt = (btp_hfpStatusChangedEvt_t*)pData;

	pItem = tlkmdi_bthfp_getUsedItem(pEvt->handle);
	if(pItem == nullptr){
		tlkapi_error(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfStatusChangedEvt: busys ");
		return -TLK_EBUSY;
	}

	tlkapi_trace(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfStatusChangedEvt: handle-%d,status-%d,callDir-%d,numbLen-%d", 
		pEvt->handle, pEvt->status, pEvt->callDir, pEvt->numbLen);

	pItem->handle = pEvt->handle;
	if(pEvt->status == BTP_HFP_CALL_STATUS_START){
		pItem->flags |= TLKMDI_HFPHF_FLAG_START;
	}else if(pEvt->status == BTP_HFP_CALL_STATUS_ALART){
		pItem->flags |= TLKMDI_HFPHF_FLAG_ALART;
	}else if(pEvt->status == BTP_HFP_CALL_STATUS_ACTIVE){
		pItem->flags |= TLKMDI_HFPHF_FLAG_ACTIVE;
	}

	if(pEvt->status == BTP_HFP_CALL_STATUS_CLOSE){
		if(pItem->unit[0].numbLen != 0 && pItem->unit[1].numbLen != 0){
			tlkapi_trace(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfStatusChangedEvt: reset wait");
		}else{
			tlkmdi_bthfp_hfReset(pEvt->handle);
		}
		return TLK_ENONE;
	}
	if(pEvt->status == BTP_HFP_CALL_STATUS_NORING){
		btp_hfphf_queryCallState(pEvt->handle);
		tlkapi_trace(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfStatusChangedEvt 03");
		return TLK_ENONE;
	}
	
	return TLK_ENONE;
}
static int tlkmdi_bthfp_hfStatusInquiryEvt(uint08 *pData, uint16 dataLen)
{
	tlkmdi_hfphf_ctrl_t *pItem;
	btp_hfpStatusInquiryEvt_t *pEvt;	

	pEvt = (btp_hfpStatusInquiryEvt_t*)pData;

	pItem = tlkmdi_bthfp_getUsedItem(pEvt->handle);
	if(pItem == nullptr){
		pItem = tlkmdi_bthfp_getIdleItem();
	}
	if(pItem == nullptr){
		return -TLK_EBUSY;
	}
	
	pItem->handle = pEvt->handle;
	
	if(pEvt->callState == 0 && pEvt->callSetup == 0){
		tlkmdi_bthfp_hfReset(pEvt->handle);
	}
	
	return TLK_ENONE;
}
static int tlkmdi_bthfp_hfNumberInquiryEvt(uint08 *pData, uint16 dataLen)
{
	tlkmdi_hfphf_ctrl_t *pItem;
	btp_hfpNumberInquiryEvt_t *pEvt;

	pEvt = (btp_hfpNumberInquiryEvt_t*)pData;

	pItem = tlkmdi_bthfp_getUsedItem(pEvt->handle);
	if(pItem == nullptr && pEvt->numbLen == 0){
		tlkapi_warn(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfNumberInquiryEvt: unexpected status 2");
		return -TLK_EFAIL;
	}else if(pItem == nullptr){
		pItem = tlkmdi_bthfp_getIdleItem();
	}

	if(pItem == nullptr){
		tlkapi_warn(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfNumberInquiryEvt: unexpected status 3");
		return -TLK_EFAIL;
	}else if(pItem->handle == 0){
		pItem->handle = pEvt->handle;
	}

	if(pEvt->state == 0x00){ //Start
		pItem->temp[0].numbLen = 0;
		pItem->temp[1].numbLen = 0;
		pItem->temp[0].callFlag = 0;
		pItem->temp[1].callFlag = 0;
	}

	tlkmdi_bthfp_hfInquirySaveDeal(pEvt);
	if(pEvt->state == 0x02){ //Stop
		tlkmdi_bthfp_hfInquiryStopDeal(pEvt->handle);
	}
	return TLK_ENONE;
}
static void tlkmdi_bthfp_hfInquirySaveDeal(btp_hfpNumberInquiryEvt_t *pEvt)
{
	tlkmdi_hfphf_unit_t *pTemp;
	
	if(pEvt->numbLen == 0) return;
	if(pEvt->numbLen > TLKMDI_HFPHF_NUMBER_MAX_LEN) pEvt->numbLen = TLKMDI_HFPHF_NUMBER_MAX_LEN;

	tlkapi_trace(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, 
		"tlkmdi_bthfp_hfInquirySaveDeal: handle-%d,status-%d,callDir-%d,numbLen-%d, number-%s", 
		pEvt->handle, pEvt->status, pEvt->callDir, pEvt->numbLen, pEvt->pNumber);
	
	pTemp = tlkmdi_bthfp_hfGetUsedCallTemp(pEvt->handle,pEvt->numbLen, pEvt->pNumber);
	if(pTemp == nullptr){
		pTemp = tlkmdi_bthfp_hfGetIdleCallTemp(pEvt->handle);
	}
	if(pTemp == nullptr){
		tlkapi_fatal(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfInquirySaveDeal: no node is valid !");
		return;
	}
	
	pTemp->callFlag = TLKMDI_HFPHF_CALL_FLAG_READY;
	pTemp->callStat = pEvt->status;
	pTemp->numbLen = pEvt->numbLen;
	pTemp->callDir = pEvt->callDir;
	tmemcpy(pTemp->number, pEvt->pNumber, pEvt->numbLen);	
}
static void tlkmdi_bthfp_hfInquiryStopDeal(uint16 handle)
{
	tlkmdi_hfphf_unit_t *pUnit;
	tlkmdi_hfphf_unit_t *pTemp;
	tlkmdi_hfphf_ctrl_t *pItem;

	
	tlkapi_warn(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfInquiryStopDeal: handle - 0x%x",handle);

	pItem = tlkmdi_bthfp_getUsedItem(handle);
	if(pItem == nullptr){
		tlkapi_error(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfInquiryStopDeal: failure - invalid handle: 0x%x",handle);
		return;
	}

	//Checks the existing list
	pUnit = &pItem->unit[0];
	if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_READY) != 0){
		tlkmdi_bthfp_hfInquiryCheckDeal(handle, pUnit);
	}
	pUnit = &pItem->unit[1];
	if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_READY) != 0){
		tlkmdi_bthfp_hfInquiryCheckDeal(handle, pUnit);
	}

	//Check to see if there are any calls left
	pTemp = &pItem->temp[0];
	if((pTemp->callFlag & TLKMDI_HFPHF_CALL_FLAG_READY) != 0){
		tlkmdi_bthfp_hfInquiryPushDeal(handle, pTemp);	
	}
	pTemp = &pItem->temp[1];
	if((pTemp->callFlag & TLKMDI_HFPHF_CALL_FLAG_READY) != 0){
		tlkmdi_bthfp_hfInquiryPushDeal(handle, pTemp);	
	}
}

static void tlkmdi_bthfp_hfInquiryCheckDeal(uint16 handle, tlkmdi_hfphf_unit_t *pUnit)
{
	uint08 index = 0;
	tlkmdi_hfphf_ctrl_t *pItem;
	tlkmdi_hfphf_unit_t *pTemp;

	pItem = tlkmdi_bthfp_getUsedItem(handle);
	if(pItem == nullptr){
		tlkapi_error(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfInquiryCheckDeal: failure - invalid handle: 0x%x",handle);
		return;
	}

	pTemp = tlkmdi_bthfp_hfGetUsedCallTemp(pItem->handle,pUnit->numbLen, pUnit->number);
	if(pTemp == nullptr){ //The existing call has been hung up
		tlkapi_trace(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfInquiryCheckDeal: 01");
		if(pUnit->numbLen != 0 && (pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_MASK) != 0
			&& (pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_CLOSE) == 0){
				if(pUnit == &pItem->unit[0]) index = 0;
				else index = 1;
			
			tlkapi_trace(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "TLKPTI_PHONE_MSGID_CALL_CLOSE_EVT: 02");
			tlkmdi_bthfp_hfSendStatusEvt(TLKPTI_PHONE_MSGID_CALL_CLOSE_EVT, pItem->handle, index, pUnit->callDir, pUnit->numbLen);
		}
		pUnit->numbLen = 0;
		pUnit->callDir = 0;
		pUnit->callStat = 0;
		pUnit->callFlag = TLKMDI_HFPHF_CALL_FLAG_NONE;
		pUnit->callBusy = TLKMDI_HFPHF_CALL_BUSY_NONE;
		return;
	}
	if(pTemp->callStat == pUnit->callStat){
		pTemp->numbLen = 0;
		pTemp->callFlag = TLKMDI_HFPHF_CALL_FLAG_NONE;
		tlkapi_trace(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfInquiryCheckDeal: status is same");
		return;
	}
	pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_READY;
	tlkapi_trace(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfInquiryCheckDeal: 02");
	if(pTemp->callStat == BTP_HFP_CALL_STATUS_WAITING){
		pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_WAIT;
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED;
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE;
		pUnit->callFlag &= ~TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_START;
	}else if(pTemp->callStat == BTP_HFP_CALL_STATUS_ACTIVE){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED;
		if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_START) == 0){
			pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_START;
		}
		if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_PAUSED) != 0){
			pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_RESUME;
		}else if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE) == 0){
			pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE;
		}
		pUnit->callFlag &= ~TLKMDI_HFPHF_CALL_FLAG_REPORT_PAUSED;
	}else if(pTemp->callStat == BTP_HFP_CALL_STATUS_PAUSED){ //Hold
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE;
		if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_PAUSED) == 0){
			pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED;
			pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_START;
		}
		pUnit->callFlag &= ~TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE;
	}else{
		if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_START) == 0){
			pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_START;
		}
	}
	pTemp->numbLen = 0;
	pTemp->callFlag = TLKMDI_HFPHF_CALL_FLAG_NONE;
	tlkmdi_bthfp_hfInquiryBusyDeal(pItem->handle, pUnit);	
}
static void tlkmdi_bthfp_hfInquiryPushDeal(uint16 handle, tlkmdi_hfphf_unit_t *pTemp)
{
	tlkmdi_hfphf_unit_t *pUnit;
	tlkmdi_hfphf_ctrl_t *pItem;

	pItem = tlkmdi_bthfp_getUsedItem(handle);
	if(pItem == nullptr){
		tlkapi_error(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfInquiryPushDeal - failure: invalid handle");
		return;
	}
	if(pTemp->callStat == BTP_HFP_CALL_STATUS_CLOSE){
		tlkapi_warn(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfInquiryPushDeal: new call is close !");
		return;
	}
	
	pUnit = tlkmdi_bthfp_hfGetIdleCallUnit(handle);
	if(pUnit == nullptr){
		tlkapi_fatal(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfInquiryPushDeal: no node is valid !");
		return;
	}
	pUnit->callStat = pTemp->callStat;
	pUnit->numbLen = pTemp->numbLen;
	pUnit->callDir = pTemp->callDir;
	tmemcpy(pUnit->number, pTemp->number, pTemp->numbLen);
	pUnit->callFlag = TLKMDI_HFPHF_CALL_FLAG_NONE;
	pUnit->callBusy = TLKMDI_HFPHF_CALL_BUSY_NONE;
	pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_READY;
	if(pTemp->callStat == BTP_HFP_CALL_STATUS_WAITING){
		pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_WAIT;
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED;
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE;
		pUnit->callFlag &= ~TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_START;
	}else if(pTemp->callStat == BTP_HFP_CALL_STATUS_ACTIVE){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED;
		if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_START) == 0){
			pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_START;
		}
		if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_PAUSED) != 0){
			pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_RESUME;
		}else if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE) == 0){
			pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE;
		}
		pUnit->callFlag &= ~TLKMDI_HFPHF_CALL_FLAG_REPORT_PAUSED;
	}else if(pTemp->callStat == BTP_HFP_CALL_STATUS_PAUSED){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE;
		if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_PAUSED) == 0){
			pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED;
			pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_START;
		}
		pUnit->callFlag &= ~TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE;
	}else{
		if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_START) == 0){
			pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_START;
		}
	}
	pTemp->numbLen = 0;
	pTemp->callFlag = TLKMDI_HFPHF_CALL_FLAG_NONE;
	tlkmdi_bthfp_hfInquiryBusyDeal(pItem->handle,pUnit);
}
static void tlkmdi_bthfp_hfInquiryBusyDeal(uint16 handle,tlkmdi_hfphf_unit_t *pUnit)
{
	uint08 index = 0;
	tlkmdi_hfphf_ctrl_t *pItem;

	pItem = tlkmdi_bthfp_getUsedItem(handle);
	if(pItem == nullptr){
		tlkapi_error(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfInquiryBusyDeal: failure - invalid handle: 0x%x", handle);
		return;
	}

	if(pUnit == &pItem->unit[0]) index = 0;
	else index = 1;
	tlkapi_trace(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfInquiryBusyDeal: %d", index);
	
	if((pUnit->callBusy & TLKMDI_HFPHF_CALL_BUSY_REPORT_WAIT) != 0){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_WAIT;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_WAIT;
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_START;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_START;
		tlkmdi_bthfp_hfSendStatusEvt(TLKPTI_PHONE_MSGID_CALL_WAIT_EVT, pItem->handle, index, pUnit->callDir, pUnit->numbLen);
	}
	if((pUnit->callBusy & TLKMDI_HFPHF_CALL_BUSY_REPORT_START) != 0){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_START;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_START;
		tlkmdi_bthfp_hfSendStatusEvt(TLKPTI_PHONE_MSGID_CALL_START_EVT, pItem->handle, index, pUnit->callDir, pUnit->numbLen);
	}
	if((pUnit->callBusy & TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE) != 0){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE;
		tlkmdi_bthfp_hfSendStatusEvt(TLKPTI_PHONE_MSGID_CALL_ACTIVE_EVT, pItem->handle, index, pUnit->callDir, pUnit->numbLen);
	}
	if((pUnit->callBusy & TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED) != 0){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_PAUSED;
		tlkmdi_bthfp_hfSendStatusEvt(TLKPTI_PHONE_MSGID_CALL_HOLD_EVT, pItem->handle, index, pUnit->callDir, pUnit->numbLen);
	}
	if((pUnit->callBusy & TLKMDI_HFPHF_CALL_BUSY_REPORT_RESUME) != 0){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_RESUME;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE;
		tlkmdi_bthfp_hfSendStatusEvt(TLKPTI_PHONE_MSGID_CALL_RESUME_EVT, pItem->handle, index, pUnit->callDir, pUnit->numbLen);
	}
}


static tlkmdi_hfphf_unit_t *tlkmdi_bthfp_hfGetIdleCallTemp(uint16 handle)
{
	tlkmdi_hfphf_ctrl_t *pItem = tlkmdi_bthfp_getUsedItem(handle);
	if(pItem != nullptr){
		if(pItem->temp[0].numbLen == 0) return &pItem->temp[0];
		if(pItem->temp[1].numbLen == 0) return &pItem->temp[1];
	}
	return nullptr;
}
static tlkmdi_hfphf_unit_t *tlkmdi_bthfp_hfGetUsedCallTemp(uint16 handle, uint08 numbLen, uint08 *pNumber)
{
	tlkmdi_hfphf_ctrl_t *pItem;
	pItem = tlkmdi_bthfp_getUsedItem(handle);
	
	if(pItem == nullptr || numbLen == 0 || pNumber == nullptr) return nullptr;

	if(pItem->temp[0].numbLen == numbLen && tmemcmp(pItem->temp[0].number, pNumber, numbLen) == 0){
		return &pItem->temp[0];
	}
	if(pItem->temp[1].numbLen == numbLen && tmemcmp(pItem->temp[1].number, pNumber, numbLen) == 0){
		return &pItem->temp[1];
	}

	return nullptr;
}

static tlkmdi_hfphf_unit_t *tlkmdi_bthfp_hfGetIdleCallUnit(uint16 handle)
{
	tlkmdi_hfphf_ctrl_t *pItem;

	pItem = tlkmdi_bthfp_getUsedItem(handle);
	if(pItem == nullptr){
		tlkapi_error(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_hfGetIdleCallUnit - failure invalid handle: 0x%x", handle);
		return nullptr;
	}

	if(pItem->unit[0].numbLen == 0) return &pItem->unit[0];
	if(pItem->unit[1].numbLen == 0) return &pItem->unit[1];
	return nullptr;
}

tlkmdi_hfphf_ctrl_t *tlkmdi_bthfp_getIdleItem(void)
{
	uint08 index;
	for(index=0; index<TLKMDI_HFPHF_MAX_NUMBER; index++){
		if(sTlkMdiBtHfpCtrl[index].handle == 0) break;
	}
	if(index == TLKMDI_HFPHF_MAX_NUMBER) return nullptr;
	return &sTlkMdiBtHfpCtrl[index];
}

tlkmdi_hfphf_ctrl_t *tlkmdi_bthfp_getUsedItem(uint16 aclHandle)
{
	uint08 index;
	for(index=0;index<TLKMDI_HFPHF_MAX_NUMBER;index++)
	{
		if(sTlkMdiBtHfpCtrl[index].handle != 0 && sTlkMdiBtHfpCtrl[index].handle == aclHandle) break;
	}
	if(index == TLKMDI_HFPHF_MAX_NUMBER) return nullptr;
	return &sTlkMdiBtHfpCtrl[index];
}
uint16 tlkmdi_bthfp_getHandle(int index)
{
	if(index >= TLKMDI_HFPHF_MAX_NUMBER){
		tlkapi_error(TLKMDI_BTHFP_DBG_FLAG, TLKMDI_BTHFP_DBG_SIGN, "tlkmdi_bthfp_getHandle - failure invalid index: %d", index);
		return -TLK_EPARAM;
	}
	return sTlkMdiBtHfpCtrl[index].handle;
}


#endif //#if (TLK_MDI_BTHFP_ENABLE)

