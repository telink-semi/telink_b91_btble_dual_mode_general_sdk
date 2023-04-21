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
#if (TLK_MDI_BTHFP_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmdi/bt/tlkmdi_bta2dp.h"

#if (TLK_STK_BTP_ENABLE)
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkstk/bt/btp/a2dp/btp_a2dp.h"
#include "tlkstk/bt/btp/avrcp/btp_avrcp.h"
#endif


#define TLKMDI_BTA2DP_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_BT << 24) | (TLK_MINOR_DBGID_MDI_BT_A2DP << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_BTA2DP_DBG_SIGN       "[MHFP]"

#if (TLK_STK_BTP_ENABLE)
static int tlkmdi_btavrcp_volumeChangeEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_btavrcp_peerEvtMaskEvt(uint08 *pData, uint16 dataLen);
#endif


/******************************************************************************
 * Function: tlkmdi_hfphf_init.
 * Descript: Trigger to Initial the HF control block and register the callback.
 * Params: None.
 * Return: Return TLK_ENONE is success, other's value is false.
 * Others: None.
*******************************************************************************/
int tlkmdi_bta2dp_init(void)
{	
	#if (TLK_STK_BTP_ENABLE)
	btp_event_regCB(BTP_EVTID_AVRCP_VOLUME_CHANGED, tlkmdi_btavrcp_volumeChangeEvt);
	btp_event_regCB(BTP_EVTID_AVRCP_PEER_EVT_MASK,  tlkmdi_btavrcp_peerEvtMaskEvt);
	#endif
	
	return TLK_ENONE;
}

void tlkmdi_bta2dp_connectEvt(uint16 aclHandle, uint08 usrID)
{
#if (TLK_STK_BTP_ENABLE)
	if(usrID == BTP_USRID_SERVER){
		btp_avrcp_regEventNotify(aclHandle, BTP_AVRCP_EVTID_VOLUME_CHANGED);
	}else if(usrID == BTP_USRID_CLIENT){
		btp_avrcp_regEventNotify(aclHandle, BTP_AVRCP_EVTID_PLAYBACK_STATUS_CHANGED);
	}
#endif
}

#if (TLK_STK_BTP_ENABLE)
static int tlkmdi_btavrcp_volumeChangeEvt(uint08 *pData, uint16 dataLen)
{
	uint08 buffer[6];
	uint08 buffLen = 0;
	btp_avrcpVolumeChangeEvt_t *pEvt;
	
	pEvt = (btp_avrcpVolumeChangeEvt_t*)pData;
	buffer[buffLen++] = (pEvt->handle & 0xFF);
	buffer[buffLen++] = (pEvt->handle & 0xFF00) >> 8;
	buffer[buffLen++] = btp_hfphf_isIosDev(pEvt->handle);
	if(btp_a2dp_isSrc(pEvt->handle)) buffer[buffLen++] = 0x01;
	else buffer[buffLen++] = 0x00;
	buffer[buffLen++] = pEvt->volume;
	tlksys_sendInnerMsg(TLKSYS_TASKID_AUDIO, TLKPTI_AUD_MSGID_AVRCP_CHG_VOLUME_EVT, buffer, buffLen);

	return TLK_ENONE;
}
static int tlkmdi_btavrcp_peerEvtMaskEvt(uint08 *pData, uint16 dataLen)
{
	btp_avrcpPeerEvtMaskEvt_t *pEvt = (btp_avrcpPeerEvtMaskEvt_t*)pData;
	if(btp_a2dp_isSrc(pEvt->handle)){
		btp_avrcp_regEventNotify(pEvt->handle, BTP_AVRCP_EVTID_VOLUME_CHANGED);
	}else if(btp_a2dp_isSnk(pEvt->handle)){
		btp_avrcp_regEventNotify(pEvt->handle, BTP_AVRCP_EVTID_PLAYBACK_STATUS_CHANGED);
	}

	return TLK_ENONE;
}
#endif



#endif //#if (TLK_MDI_BTHFP_ENABLE)

