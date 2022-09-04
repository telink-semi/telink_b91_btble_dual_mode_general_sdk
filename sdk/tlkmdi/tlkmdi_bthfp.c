/********************************************************************************************************
 * @file     tlkmdi_bthfp.c
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
#include "tlkdev/tlkdev_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#if (TLK_MDI_BTHFP_ENABLE)
#include "tlkmdi/tlkmdi_adapt.h"
#include "tlkmdi/tlkmdi_event.h"
#include "tlkmdi/tlkmdi_bthfp.h"


#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkprt/tlkprt_stdio.h"


#define TLKMDI_HFPHF_DBG_FLAG         (TLKMDI_HFP_DBG_ENABLE | TLKMDI_DBG_FLAG) 
#define TLKMDI_HFPHF_DBG_SIGN         TLKMDI_DBG_SIGN


static void tlkmdi_hfphf_reset(void);
static int tlkmdi_hfphf_codecChangedEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_hfphf_volumeChangedEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_hfphf_statusChangedEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_hfphf_numberInquiryEvt(uint08 *pData, uint16 dataLen);

static void tlkmdi_hfphf_sendStatusEvt(uint08 evtID, uint16 handle, uint08 callNum, uint08 callDir, uint08 numbLen);

static void tlkmdi_hfphf_inquiryStopDeal(void);
static void tlkmdi_hfphf_inquiryBusyDeal(void);
static void tlkmdi_hfphf_inquirySaveDeal(btp_hfpNumberInquiryEvt_t *pEvt);
static tlkmdi_hfphf_unit_t *tlkmdi_hfphf_getIdleCallUnit(void);
static tlkmdi_hfphf_unit_t *tlkmdi_hfphf_getUsedCallUnit(uint08 numbLen, uint08 *pNumber);



static tlkmdi_hfphf_ctrl_t sTlkMdiHfpCtrl;


/******************************************************************************
 * Function: tlkmdi_hfphf_init.
 * Descript: Trigger to Initial the HF control block and register the callback.
 * Params: None.
 * Return: Return TLK_ENONE is success, other's value is false.
 * Others: None.
*******************************************************************************/
int tlkmdi_hfp_init(void)
{	
	btp_event_regCB(BTP_EVTID_HFPHF_CODEC_CHANGED,  tlkmdi_hfphf_codecChangedEvt);
	btp_event_regCB(BTP_EVTID_HFPHF_VOLUME_CHANGED, tlkmdi_hfphf_volumeChangedEvt);
	btp_event_regCB(BTP_EVTID_HFPHF_STATUS_CHANGED, tlkmdi_hfphf_statusChangedEvt);
	btp_event_regCB(BTP_EVTID_HFPHF_NUMBER_INQUIRY, tlkmdi_hfphf_numberInquiryEvt);
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_hfphf_destroy.
 * Descript: Reset the HF control block adn release resource.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_hfp_destroy(uint16 aclHandle)
{
	if(sTlkMdiHfpCtrl.handle == aclHandle){
		tlkmdi_hfphf_reset();
	}
}

/******************************************************************************
 * Function: tlkmdi_hfphf_getCallNumber.
 * Descript: Get the call number.
 * Params: None.
 * Return: Return call number.
 * Others: None.
*******************************************************************************/
uint08 *tlkmdi_hfphf_getCallNumber(uint08 callNum)
{
	if(callNum >= 2) return nullptr;
	return sTlkMdiHfpCtrl.unit[callNum].number;
}


int tlkmdi_hfphf_rejectWaitAndKeepActive(void)
{
	if(sTlkMdiHfpCtrl.handle == 0) return -TLK_EFAIL;
	return btp_hfphf_rejectWaitAndKeepActive(sTlkMdiHfpCtrl.handle);
}

int tlkmdi_hfphf_acceptWaitAndHoldActive(void)
{
	if(sTlkMdiHfpCtrl.handle == 0) return -TLK_EFAIL;
	return btp_hfphf_acceptWaitAndHoldActive(sTlkMdiHfpCtrl.handle);
}

int tlkmdi_hfphf_hungUpActiveAndResumeHold(void)
{
	if(sTlkMdiHfpCtrl.handle == 0) return -TLK_EFAIL;
	return btp_hfphf_hungUpActiveAndResumeHold(sTlkMdiHfpCtrl.handle);
}



static void tlkmdi_hfphf_reset(void)
{
	uint16 handle;
	uint08 callDir;
	uint08 numbLen;
	uint08 callFlag;
	tlkmdi_hfphf_unit_t *pUnit;
	
	handle = sTlkMdiHfpCtrl.handle;
	
	sTlkMdiHfpCtrl.handle = 0;
	sTlkMdiHfpCtrl.flags  = TLKMDI_HFPHF_FLAG_NONE;

	pUnit = &sTlkMdiHfpCtrl.unit[0];
	callDir = pUnit->callDir;
	numbLen = pUnit->numbLen;
	callFlag = pUnit->callFlag;
	pUnit->numbLen = 0;
	pUnit->callDir = 0;
	pUnit->callStat = 0;
	pUnit->callFlag = TLKMDI_HFPHF_CALL_FLAG_NONE;
	pUnit->callBusy = TLKMDI_HFPHF_CALL_BUSY_NONE;
	if((callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_MASK) != 0 && (callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_CLOSE) == 0){
		tlkapi_warn(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "TLKMDI_HFPHF_EVTID_CALL_CLOSE: 04");
		tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_CLOSE, handle, 0, callDir, numbLen);
	}
	
	pUnit = &sTlkMdiHfpCtrl.unit[1];
	callDir = pUnit->callDir;
	numbLen = pUnit->numbLen;
	callFlag = pUnit->callFlag;
	pUnit->numbLen = 0;
	pUnit->callDir = 0;
	pUnit->callStat = 0;
	pUnit->callFlag = TLKMDI_HFPHF_CALL_FLAG_NONE;
	pUnit->callBusy = TLKMDI_HFPHF_CALL_BUSY_NONE;
	if((callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_MASK) != 0 && (callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_CLOSE) == 0){
		tlkapi_warn(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "TLKMDI_HFPHF_EVTID_CALL_CLOSE: 05");
		tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_CLOSE, handle, 1, callDir, numbLen);
	}
}
static void tlkmdi_hfphf_sendStatusEvt(uint08 evtID, uint16 handle, uint08 callNum, uint08 callDir, uint08 numbLen)
{
	tlkmdi_hfphf_statusEvt_t evt;

	tlkapi_trace(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "tlkmdi_hfphf_sendStatusEvt: evtID-%d,callNum-%d,callDir-%d,numbLen-%d", 
		evtID, callNum, callDir, numbLen);
	
	evt.handle  = handle;
	evt.callNum = callNum;
	evt.callDir = callDir;
	evt.numbLen = numbLen;
	tlkmdi_sendPhoneEvent(evtID, &evt, sizeof(tlkmdi_hfphf_statusEvt_t), true);
}
static int tlkmdi_hfphf_codecChangedEvt(uint08 *pData, uint16 dataLen)
{
//	btp_hfpCodecChangedEvt_t *pEvt;
//	pEvt = (btp_hfpCodecChangedEvt_t*)pData;	
	return TLK_ENONE;
}
static int tlkmdi_hfphf_volumeChangedEvt(uint08 *pData, uint16 dataLen)
{
	btp_hfpVolumeChangedEvt_t *pEvt;

	pEvt = (btp_hfpVolumeChangedEvt_t*)pData;
	if(pEvt->volType == BTP_HFP_VOLUME_TYPE_SPK){
		tlkapi_trace(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "tlkmdi_hfphf_volumeChangedEvt: %d", pEvt->volume);
		tlkmdi_audio_setVoiceVolume(pEvt->volume);
	}
	return TLK_ENONE;
}

static int tlkmdi_hfphf_statusChangedEvt(uint08 *pData, uint16 dataLen)
{
	btp_hfpStatusChangedEvt_t *pEvt;

	pEvt = (btp_hfpStatusChangedEvt_t*)pData;
	if(sTlkMdiHfpCtrl.handle != 0 && pEvt->handle != sTlkMdiHfpCtrl.handle){
		tlkapi_error(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "tlkmdi_hfphf_statusChangedEvt: busys ");
		return -TLK_EBUSY;
	}
	
	tlkapi_trace(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "tlkmdi_hfphf_statusChangedEvt: handle-%d,status-%d,callDir-%d,numbLen-%d", 
		pEvt->handle, pEvt->status, pEvt->callDir, pEvt->numbLen);
	sTlkMdiHfpCtrl.handle = pEvt->handle;
	if(pEvt->status == BTP_HFP_CALL_STATUS_START){
		sTlkMdiHfpCtrl.flags |= TLKMDI_HFPHF_FLAG_START;
	}else if(pEvt->status == BTP_HFP_CALL_STATUS_ALART){
		sTlkMdiHfpCtrl.flags |= TLKMDI_HFPHF_FLAG_ALART;
	}else if(pEvt->status == BTP_HFP_CALL_STATUS_ACTIVE){
		sTlkMdiHfpCtrl.flags |= TLKMDI_HFPHF_FLAG_ACTIVE;
	}
	
	if(pEvt->status == BTP_HFP_CALL_STATUS_CLOSE){
		tlkmdi_hfphf_reset();
		return TLK_ENONE;
	}
	if(pEvt->status == BTP_HFP_CALL_STATUS_NORING){
		if((sTlkMdiHfpCtrl.flags & TLKMDI_HFPHF_FLAG_ACTIVE) == 0){
			tlkapi_error(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "tlkmdi_hfphf_reset");
			tlkmdi_hfphf_reset();
		}else if((sTlkMdiHfpCtrl.unit[0].callFlag & TLKMDI_HFPHF_CALL_FLAG_READY) != 0
			&& (sTlkMdiHfpCtrl.unit[1].callFlag & TLKMDI_HFPHF_CALL_FLAG_READY) != 0){
			btp_hfphf_queryCallList(pEvt->handle);
		}
		return TLK_ENONE;
	}
		
	return TLK_ENONE;
}
static int tlkmdi_hfphf_numberInquiryEvt(uint08 *pData, uint16 dataLen)
{
	btp_hfpNumberInquiryEvt_t *pEvt;

	pEvt = (btp_hfpNumberInquiryEvt_t*)pData;
	if(sTlkMdiHfpCtrl.handle != pEvt->handle){
		tlkapi_warn(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "tlkmdi_hfphf_numberInquiryEvt: unexpected status 1");
		return -TLK_EFAIL;
	}
	
	if(pEvt->state == 0x00){ //Start
		sTlkMdiHfpCtrl.unit[0].callFlag &= ~TLKMDI_HFPHF_CALL_FLAG_READY;
		sTlkMdiHfpCtrl.unit[1].callFlag &= ~TLKMDI_HFPHF_CALL_FLAG_READY;
	}
	tlkmdi_hfphf_inquirySaveDeal(pEvt);
	tlkmdi_hfphf_inquiryBusyDeal();
	if(pEvt->state == 0x02){ //Stop
		tlkmdi_hfphf_inquiryStopDeal();
	}
	return TLK_ENONE;
}

static void tlkmdi_hfphf_inquirySaveDeal(btp_hfpNumberInquiryEvt_t *pEvt)
{
	tlkmdi_hfphf_unit_t *pUnit;
		
	if(pEvt->numbLen == 0) return;
	if(pEvt->numbLen > TLKMDI_HFPHF_NUMBER_MAX_LEN) pEvt->numbLen = TLKMDI_HFPHF_NUMBER_MAX_LEN;

	tlkapi_trace(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "tlkmdi_hfphf_inquirySaveDeal: handle-%d,status-%d,callDir-%d,numbLen-%d", 
		pEvt->handle, pEvt->status, pEvt->callDir, pEvt->numbLen);
	pUnit = tlkmdi_hfphf_getUsedCallUnit(pEvt->numbLen, pEvt->pNumber);
	if(pUnit == nullptr){
		pUnit = tlkmdi_hfphf_getIdleCallUnit();
		if(pUnit == nullptr){
			if(sTlkMdiHfpCtrl.unit[0].callStat == BTP_HFP_CALL_STATUS_WAITING){
				pUnit = &sTlkMdiHfpCtrl.unit[0];
			}else{
				pUnit = &sTlkMdiHfpCtrl.unit[1];
			}
		}
		if(pUnit->numbLen != 0 && (pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_MASK) != 0
			&& (pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_CLOSE) == 0){
			uint08 numb;
			if(pUnit == &sTlkMdiHfpCtrl.unit[0]) numb = 0;
			else numb = 1;
			tlkapi_warn(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "TLKMDI_HFPHF_EVTID_CALL_CLOSE: 01");
			tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_CLOSE, sTlkMdiHfpCtrl.handle, numb, pUnit->callDir, pUnit->numbLen);
		}
		pUnit->callStat = pEvt->status;
		pUnit->numbLen = pEvt->numbLen;
		pUnit->callDir = pEvt->callDir;
		tmemcpy(pUnit->number, pEvt->pNumber, pEvt->numbLen);
		pUnit->callFlag = TLKMDI_HFPHF_CALL_FLAG_NONE;
		pUnit->callBusy = TLKMDI_HFPHF_CALL_BUSY_NONE;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_READY;
		if(pEvt->status == BTP_HFP_CALL_STATUS_WAITING){
			pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_WAIT;
		}else if(pEvt->status == BTP_HFP_CALL_STATUS_ACTIVE){
			pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE;
		}else if(pEvt->status == BTP_HFP_CALL_STATUS_PAUSED){
			pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED;
		}else{
			pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_START;
		}
	}
	else{
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_READY;
		if(pUnit->callStat == pEvt->status) return;
		pUnit->callStat = pEvt->status;
		if(pEvt->status == BTP_HFP_CALL_STATUS_WAITING){
			pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_WAIT;
			pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED;
			pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE;
			pUnit->callFlag &= ~TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE;
		}else if(pEvt->status == BTP_HFP_CALL_STATUS_ACTIVE){
			pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED;
			if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_PAUSED) != 0){
				pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_RESUME;
			}else if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE) == 0){
				pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE;
			}
			pUnit->callFlag &= ~TLKMDI_HFPHF_CALL_FLAG_REPORT_PAUSED;
		}else if(pEvt->status == BTP_HFP_CALL_STATUS_PAUSED){
			pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE;
			if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_PAUSED) == 0){
				pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED;
			}
			pUnit->callFlag &= ~TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE;
		}else{
			if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_START) == 0){
				pUnit->callBusy |= TLKMDI_HFPHF_CALL_BUSY_REPORT_START;
			}
		}
	}

	if(pUnit == &sTlkMdiHfpCtrl.unit[0]){
		tlkapi_warn(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "save node 0");
	}else{
		tlkapi_warn(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "save node 1");
	}
}
static void tlkmdi_hfphf_inquiryStopDeal(void)
{
	tlkmdi_hfphf_unit_t *pUnit;

	tlkapi_warn(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "tlkmdi_hfphf_inquiryStopDeal:");
	pUnit = &sTlkMdiHfpCtrl.unit[0];
	if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_READY) == 0){
		tlkapi_warn(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "tlkmdi_hfphf_inquiryStopDeal: 01");
		if(pUnit->numbLen != 0 && (pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_MASK) != 0
			&& (pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_CLOSE) == 0){
			tlkapi_warn(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "TLKMDI_HFPHF_EVTID_CALL_CLOSE: 02");
			tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_CLOSE, sTlkMdiHfpCtrl.handle, 0, pUnit->callDir, pUnit->numbLen);
		}
		pUnit->numbLen = 0;
		pUnit->callDir = 0;
		pUnit->callStat = 0;
		pUnit->callFlag = TLKMDI_HFPHF_CALL_FLAG_NONE;
		pUnit->callBusy = TLKMDI_HFPHF_CALL_BUSY_NONE;
	}
	pUnit = &sTlkMdiHfpCtrl.unit[1];
	if((pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_READY) == 0){
		tlkapi_warn(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "tlkmdi_hfphf_inquiryStopDeal: 02");
		if(pUnit->numbLen != 0 && (pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_MASK) != 0
			&& (pUnit->callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_CLOSE) == 0){
			tlkapi_warn(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "TLKMDI_HFPHF_EVTID_CALL_CLOSE: 03");
			tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_CLOSE, sTlkMdiHfpCtrl.handle, 1, pUnit->callDir, pUnit->numbLen);
		}
		pUnit->numbLen = 0;
		pUnit->callDir = 0;
		pUnit->callStat = 0;
		pUnit->callFlag = TLKMDI_HFPHF_CALL_FLAG_NONE;
		pUnit->callBusy = TLKMDI_HFPHF_CALL_BUSY_NONE;
	}
}
static void tlkmdi_hfphf_inquiryBusyDeal(void)
{
	tlkmdi_hfphf_unit_t *pUnit;

	tlkapi_warn(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "tlkmdi_hfphf_inquiryBusyDeal:");
	
	pUnit = &sTlkMdiHfpCtrl.unit[0];
	if((pUnit->callBusy & TLKMDI_HFPHF_CALL_BUSY_REPORT_WAIT) != 0){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_WAIT;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_WAIT;
		tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_WAIT, sTlkMdiHfpCtrl.handle, 0, pUnit->callDir, pUnit->numbLen);
	}
	pUnit = &sTlkMdiHfpCtrl.unit[1];
	if((pUnit->callBusy & TLKMDI_HFPHF_CALL_BUSY_REPORT_WAIT) != 0){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_WAIT;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_WAIT;
		tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_WAIT, sTlkMdiHfpCtrl.handle, 1, pUnit->callDir, pUnit->numbLen);
	}
	
	pUnit = &sTlkMdiHfpCtrl.unit[0];
	if((pUnit->callBusy & TLKMDI_HFPHF_CALL_BUSY_REPORT_START) != 0){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_START;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_START;
		tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_START, sTlkMdiHfpCtrl.handle, 0, pUnit->callDir, pUnit->numbLen);
	}
	if((pUnit->callBusy & TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE) != 0){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE;
		tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_ACTIVE, sTlkMdiHfpCtrl.handle, 0, pUnit->callDir, pUnit->numbLen);
	}
	if((pUnit->callBusy & TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED) != 0){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_PAUSED;
		tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_HOLD, sTlkMdiHfpCtrl.handle, 0, pUnit->callDir, pUnit->numbLen);
	}
	if((pUnit->callBusy & TLKMDI_HFPHF_CALL_BUSY_REPORT_RESUME) != 0){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_RESUME;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE;
		tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_RESUME, sTlkMdiHfpCtrl.handle, 0, pUnit->callDir, pUnit->numbLen);
	}
	pUnit = &sTlkMdiHfpCtrl.unit[1];
	if((pUnit->callBusy & TLKMDI_HFPHF_CALL_BUSY_REPORT_START) != 0){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_START;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_START;
		tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_START, sTlkMdiHfpCtrl.handle, 1, pUnit->callDir, pUnit->numbLen);
	}
	if((pUnit->callBusy & TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE) != 0){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE;
		tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_ACTIVE, sTlkMdiHfpCtrl.handle, 1, pUnit->callDir, pUnit->numbLen);
	}
	if((pUnit->callBusy & TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED) != 0){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_PAUSED;
		tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_HOLD, sTlkMdiHfpCtrl.handle, 1, pUnit->callDir, pUnit->numbLen);
	}
	if((pUnit->callBusy & TLKMDI_HFPHF_CALL_BUSY_REPORT_RESUME) != 0){
		pUnit->callBusy &= ~TLKMDI_HFPHF_CALL_BUSY_REPORT_RESUME;
		pUnit->callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE;
		tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_RESUME, sTlkMdiHfpCtrl.handle, 1, pUnit->callDir, pUnit->numbLen);
	}
}
static tlkmdi_hfphf_unit_t *tlkmdi_hfphf_getIdleCallUnit(void)
{
	if(sTlkMdiHfpCtrl.unit[0].numbLen == 0) return &sTlkMdiHfpCtrl.unit[0];
	if(sTlkMdiHfpCtrl.unit[1].numbLen == 0) return &sTlkMdiHfpCtrl.unit[1];
	return nullptr;
}
static tlkmdi_hfphf_unit_t *tlkmdi_hfphf_getUsedCallUnit(uint08 numbLen, uint08 *pNumber)
{
	if(numbLen == 0 || pNumber == nullptr) return nullptr;
	if(sTlkMdiHfpCtrl.unit[0].numbLen == numbLen && tmemcmp(sTlkMdiHfpCtrl.unit[0].number, pNumber, numbLen) == 0){
		return &sTlkMdiHfpCtrl.unit[0];
	}
	if(sTlkMdiHfpCtrl.unit[1].numbLen == numbLen && tmemcmp(sTlkMdiHfpCtrl.unit[1].number, pNumber, numbLen) == 0){
		return &sTlkMdiHfpCtrl.unit[1];
	}
	return nullptr;
}



#endif //#if (TLK_MDI_BTHFP_ENABLE)

