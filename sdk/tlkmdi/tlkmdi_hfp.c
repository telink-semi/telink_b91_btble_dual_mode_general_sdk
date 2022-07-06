/********************************************************************************************************
 * @file     tlkmdi_hfp.c
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
#if (TLK_MDI_HFP_ENABLE)
#include "tlkmdi/tlkmdi_adapt.h"
#include "tlkmdi/tlkmdi_event.h"
#include "tlkmdi/tlkmdi_hfp.h"


#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkprt/tlkprt_stdio.h"


#define TLKMDI_HFPHF_DBG_FLAG         (TLKMDI_HFP_DBG_ENABLE | TLKMDI_DBG_FLAG) 
#define TLKMDI_HFPHF_DBG_SIGN         TLKMDI_DBG_SIGN


static void tlkmdi_hfphf_reset(void);
static bool tlkmdi_hfphf_timer(tlkapi_timer_t *pTimer, void *pUsrArg);
static int tlkmdi_hfphf_codecChangedEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_hfphf_volumeChangedEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_hfphf_statusChangedEvt(uint08 *pData, uint16 dataLen);

static void tlkmdi_hfphf_sendStatusEvt(uint08 evtID, uint16 handle, uint08 callDir, uint08 numbLen);

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
	tlkmdi_adapt_initTimer(&sTlkMdiHfpCtrl.timer, tlkmdi_hfphf_timer, &sTlkMdiHfpCtrl, TLKMDI_HFPHF_TIMEOUT);
	
	btp_event_regCB(BTP_EVTID_HFPHF_CODEC_CHANGED,  tlkmdi_hfphf_codecChangedEvt);
	btp_event_regCB(BTP_EVTID_HFPHF_VOLUME_CHANGED, tlkmdi_hfphf_volumeChangedEvt);
	btp_event_regCB(BTP_EVTID_HFPHF_STATUS_CHANGED, tlkmdi_hfphf_statusChangedEvt);
	
	
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
	if(sTlkMdiHfpCtrl.callHandle == aclHandle){
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
uint08 *tlkmdi_hfphf_getCallNumber(void)
{
	return sTlkMdiHfpCtrl.number;
}

static bool tlkmdi_hfphf_timer(tlkapi_timer_t *pTimer, void *pUsrArg)
{
	if((sTlkMdiHfpCtrl.callBusy & TLKMDI_HFPHF_CALL_BUSY_WAIT_NUMBER) != 0){
		if(sTlkMdiHfpCtrl.timeout != 0) sTlkMdiHfpCtrl.timeout--;
		if(sTlkMdiHfpCtrl.timeout == 0){
			if((sTlkMdiHfpCtrl.callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_START) == 0){
				sTlkMdiHfpCtrl.callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_START;
				tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_START, sTlkMdiHfpCtrl.callHandle,
					sTlkMdiHfpCtrl.callDir, sTlkMdiHfpCtrl.numbLen);
			}
			if((sTlkMdiHfpCtrl.callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE) == 0){
				sTlkMdiHfpCtrl.callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE;
				tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_ACTIVE, sTlkMdiHfpCtrl.callHandle,
					sTlkMdiHfpCtrl.callDir, sTlkMdiHfpCtrl.numbLen);
			}
		}
	}
	if(sTlkMdiHfpCtrl.timeout != 0) return true;
	return false;
}


static void tlkmdi_hfphf_reset(void)
{
	uint16 handle  = sTlkMdiHfpCtrl.callHandle;
	uint08 callDir = sTlkMdiHfpCtrl.callDir;
	uint08 numbLen = sTlkMdiHfpCtrl.numbLen;
	uint08 callFlag = sTlkMdiHfpCtrl.callFlag;
	sTlkMdiHfpCtrl.callDir = 0;
	sTlkMdiHfpCtrl.numbLen = 0;
	sTlkMdiHfpCtrl.callBusy = TLKMDI_HFPHF_CALL_BUSY_NONE;
	sTlkMdiHfpCtrl.callFlag = TLKMDI_HFPHF_CALL_FLAG_NONE;
	sTlkMdiHfpCtrl.callHandle  = 0;
	tlkmdi_adapt_removeTimer(&sTlkMdiHfpCtrl.timer);
	if((callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_MASK) != 0
		&& (callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_CLOSE) == 0){
		tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_CLOSE, handle, callDir, numbLen);
	}
}
static void tlkmdi_hfphf_sendStatusEvt(uint08 evtID, uint16 handle, uint08 callDir, uint08 numbLen)
{
	tlkmdi_hfphf_statusEvt_t evt;
	evt.handle  = handle;
	evt.callDir = callDir;
	evt.numbLen = numbLen;
	tlkmdi_sendPhoneEvent(evtID, &evt, sizeof(tlkmdi_hfphf_statusEvt_t));
}
static int tlkmdi_hfphf_codecChangedEvt(uint08 *pData, uint16 dataLen)
{
	btp_hfpCodecChangedEvt_t *pEvt;
	
	pEvt = (btp_hfpCodecChangedEvt_t*)pData;
	if(sTlkMdiHfpCtrl.handle != pEvt->handle) return -TLK_EFAIL;

	
	return TLK_ENONE;
}
static int tlkmdi_hfphf_volumeChangedEvt(uint08 *pData, uint16 dataLen)
{
	btp_hfpVolumeChangedEvt_t *pEvt;

	pEvt = (btp_hfpVolumeChangedEvt_t*)pData;
	if(sTlkMdiHfpCtrl.handle != pEvt->handle) return -TLK_EFAIL;
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
	if(pEvt->handle != sTlkMdiHfpCtrl.callHandle && ((sTlkMdiHfpCtrl.callFlag & TLKMDI_HFPHF_CALL_FLAG_START) != 0
		|| (sTlkMdiHfpCtrl.callFlag & TLKMDI_HFPHF_CALL_FLAG_ACTIVE) != 0)){
		tlkmdi_hfphf_reset();
	}
	
	tlkapi_trace(TLKMDI_HFPHF_DBG_FLAG, TLKMDI_HFPHF_DBG_SIGN, "tlkmdi_hfphf_statusChangedEvt: handle-%d,status-%d,callDir-%d,numbLen-%d", 
		pEvt->handle, pEvt->status, pEvt->callDir, pEvt->numbLen);

	sTlkMdiHfpCtrl.handle = pEvt->handle;
	sTlkMdiHfpCtrl.callHandle = pEvt->handle;
	if(pEvt->numbLen != 0 && sTlkMdiHfpCtrl.numbLen == 0){
		if(pEvt->numbLen <= TLKMDI_HFPHF_NUMBER_MAX_LEN){
			sTlkMdiHfpCtrl.numbLen = pEvt->numbLen;
		}else{
			sTlkMdiHfpCtrl.numbLen = TLKMDI_HFPHF_NUMBER_MAX_LEN;
		}
		tmemcpy(sTlkMdiHfpCtrl.number, pEvt->pNumber, sTlkMdiHfpCtrl.numbLen);
		sTlkMdiHfpCtrl.callFlag |= TLKMDI_HFPHF_CALL_FLAG_NUMBER;
	}
	if(pEvt->callDir != BTP_HFP_CALL_DIR_NONE){
		sTlkMdiHfpCtrl.callDir = pEvt->callDir;
	}
	if(pEvt->status == BTP_HFP_CALL_STATUS_NORING){
		if((sTlkMdiHfpCtrl.callFlag & TLKMDI_HFPHF_CALL_FLAG_ACTIVE) == 0){
			tlkmdi_hfphf_reset();
		}
	}else if(pEvt->status == BTP_HFP_CALL_STATUS_START){
		sTlkMdiHfpCtrl.callFlag |= TLKMDI_HFPHF_CALL_FLAG_START;
	}else if(pEvt->status == BTP_HFP_CALL_STATUS_ALART){
		sTlkMdiHfpCtrl.callFlag |= TLKMDI_HFPHF_CALL_FLAG_ALART;
	}else if(pEvt->status == BTP_HFP_CALL_STATUS_ACTIVE){
		sTlkMdiHfpCtrl.callFlag |= TLKMDI_HFPHF_CALL_FLAG_ACTIVE;
	}else if(pEvt->status == BTP_HFP_CALL_STATUS_CLOSE){
		sTlkMdiHfpCtrl.callFlag |= TLKMDI_HFPHF_CALL_FLAG_CLOSE;
	}else{
		return -TLK_EFAIL;
	}

	if((sTlkMdiHfpCtrl.callFlag & TLKMDI_HFPHF_CALL_FLAG_STATUS_MASK) == 0) return TLK_ENONE;
	if(sTlkMdiHfpCtrl.numbLen == 0 && (sTlkMdiHfpCtrl.callFlag & TLKMDI_HFPHF_CALL_FLAG_CLOSE) == 0){
		if((sTlkMdiHfpCtrl.callBusy & TLKMDI_HFPHF_CALL_BUSY_WAIT_NUMBER) == 0
			&& (sTlkMdiHfpCtrl.callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_START) == 0){
			sTlkMdiHfpCtrl.callBusy |= TLKMDI_HFPHF_CALL_BUSY_WAIT_NUMBER;
			sTlkMdiHfpCtrl.timeout = TLKMDI_HFPHF_CALL_WAIT_TIMEOUT;
			tlkmdi_adapt_insertTimer(&sTlkMdiHfpCtrl.timer);
		}
		return TLK_ENONE;
	}

	if((sTlkMdiHfpCtrl.callFlag & TLKMDI_HFPHF_CALL_FLAG_START) != 0
		&& (sTlkMdiHfpCtrl.callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_START) == 0){
		sTlkMdiHfpCtrl.callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_START;
		tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_START, sTlkMdiHfpCtrl.callHandle,
			sTlkMdiHfpCtrl.callDir, sTlkMdiHfpCtrl.numbLen);
	}
	if((sTlkMdiHfpCtrl.callFlag & TLKMDI_HFPHF_CALL_FLAG_ACTIVE) != 0 
		&& (sTlkMdiHfpCtrl.callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE) == 0){
		sTlkMdiHfpCtrl.callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE;
		tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_ACTIVE, sTlkMdiHfpCtrl.callHandle,
			sTlkMdiHfpCtrl.callDir, sTlkMdiHfpCtrl.numbLen);
	}
	if((sTlkMdiHfpCtrl.callFlag & TLKMDI_HFPHF_CALL_FLAG_CLOSE) != 0 
		&& (sTlkMdiHfpCtrl.callFlag & TLKMDI_HFPHF_CALL_FLAG_REPORT_CLOSE) == 0){
		sTlkMdiHfpCtrl.callFlag |= TLKMDI_HFPHF_CALL_FLAG_REPORT_CLOSE;
		tlkmdi_hfphf_sendStatusEvt(TLKMDI_HFPHF_EVTID_CALL_CLOSE, sTlkMdiHfpCtrl.callHandle,
			sTlkMdiHfpCtrl.callDir, sTlkMdiHfpCtrl.numbLen);
		tlkmdi_hfphf_reset();
	}
	
	return TLK_ENONE;
}


#endif //#if (TLK_MDI_HFP_ENABLE)

