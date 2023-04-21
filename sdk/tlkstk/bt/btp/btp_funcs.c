/********************************************************************************************************
 * @file     btp_funcs.c
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
#if (TLK_STK_BTP_ENABLE && TLK_CFG_TEST_ENABLE)
#include "btp_stdio.h"
#include "btp.h"
#include "btp_funcs.h"
#include "btp_module.h"
#include "sdp/btp_sdp.h"
#include "a2dp/btp_a2dp.h"
#include "avrcp/btp_avrcp.h"
#include "rfcomm/btp_rfcomm.h"
#include "hfp/btp_hfp.h"
#include "spp/btp_spp.h"
#include "a2dp/btp_a2dp.h"
#include "pbap/btp_pbap.h"
#include "hid/btp_hid.h"
#include "att/btp_att.h"
#include "a2dp/btp_a2dpInner.h"
#include "a2dp/btp_a2dpSrc.h"
#include "avrcp/btp_avrcpInner.h"


#define BTP_FUNC_DBG_FLAG       ((TLK_MAJOR_DBGID_BTH << 24) | (TLK_MINOR_DBGID_BTH_FUNC << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define BTP_FUNC_DBG_SIGN       nullptr

extern int btp_hfphf_connect(uint16 aclHandle, uint08 channel);
extern int btp_hfphf_disconn(uint16 aclHandle);
extern int btp_hfpag_connect(uint16 aclHandle, uint08 channel);
extern int btp_hfpag_disconn(uint16 aclHandle);


static uint16 sBtpFuncAclHandle = 0;
static uint16 sBtpFuncSppRfcChn = 0;
static uint16 sBtpFuncHfpAgRfcChn = 0;
static uint16 sBtpFuncHfpHfRfcChn = 0;
static const btp_func_item_t scBtpFunSet[] = {
	//btp_setConnTimeout
	{BTP_FUNCID_SET_CONN_TIMEOUT, btp_func_setConnTimeout},
	//SDP
	{BTP_FUNCID_SDP_SRV_CONNECT, btp_func_sdpSrvConnect},
	{BTP_FUNCID_SDP_SRV_DISCONN, btp_func_sdpSrvDisconn},
	{BTP_FUNCID_SDP_CLT_CONNECT, btp_func_sdpCltConnect},
	{BTP_FUNCID_SDP_CLT_DISCONN, btp_func_sdpCltDisconn},
	//RFC
	{BTP_FUNCID_RFC_CONNECT, btp_func_rfcConnect},
	{BTP_FUNCID_RFC_DISCONN, btp_func_rfcDisconn},
	//SPP
	{BTP_FUNCID_SPP_CONNECT, btp_func_sppConnect},
	{BTP_FUNCID_SPP_DISCONN, btp_func_sppDisconn},
	{BTP_FUNCID_SPP_SEND_DATA, btp_func_sppSendData},
	{BTP_FUNCID_SPP_SEND_RLS, btp_func_sppSendRLS},
	//A2DP
	{BTP_FUNCID_A2DP_SET_MODE, btp_func_a2dpSetMode},
	{BTP_FUNCID_A2DP_SRC_CONNECT, btp_func_a2dpSrcConnect},
	{BTP_FUNCID_A2DP_SRC_DISCONN, btp_func_a2dpSrcDisconn},
	{BTP_FUNCID_A2DP_SNK_CONNECT, btp_func_a2dpSnkConnect},
	{BTP_FUNCID_A2DP_SNK_DISCONN, btp_func_a2dpSnkDisconn},
	{BTP_FUNCID_A2DP_SNK_DELAY_RPTORT, btp_func_a2dpSnkDelayReport},
	{BTP_FUNCID_A2DP_SRC_SEND_A2DP_START, btp_func_a2dpSrcSendStart},
	{BTP_FUNCID_A2DP_SRC_SEND_MEDIA_DATA, btp_func_a2dpSrcSendMediaData},
	{BTP_FUNCID_A2DP_SEND_ABORT, btp_func_a2dpSendAbort},
	//AVRCP
	{BTP_FUNCID_AVRCP_CONNECT, btp_func_avrcpConnect},
	{BTP_FUNCID_AVRCP_DISCONN, btp_func_avrcpDisconn},
	{BTP_FUNCID_AVRCP_SEND_KEY_PRESSED, btp_func_avrcpSendKeyPressed},
	{BTP_FUNCID_AVRCP_SEND_KEY_RELEASED, btp_func_avrcpSendKeyReleased},
	{BTP_FUNCID_AVRCP_SEND_REG_EVT_NOTY_CMD, btp_func_avrcpSendRegEvtNotyCmd},
	{BTP_FUNCID_AVRCP_SEND_EVENT_NOTY, btp_func_avrcpSendEventNoty},
	{BTP_FUNCID_AVRCP_SET_TRACK_VALUE, btp_func_avrcpSetTrackValue},
	{BTP_FUNCID_AVRCP_SET_PLAY_STATUS, btp_func_avrcpSetPlayStatus},
	//HFP
	{BTP_FUNCID_HFP_HF_CONNECT, btp_func_hfpHfConnect},
	{BTP_FUNCID_HFP_HF_DISCONN, btp_func_hfpHfDisconn},
	{BTP_FUNCID_HFP_HF_SET_FEATURE, btp_func_hfpHfSetFeature},
	{BTP_FUNCID_HFP_HF_SET_VOLUME,  btp_func_hfpHfSetVolume},
	{BTP_FUNCID_HFP_HF_SET_BATTERY, btp_func_hfpHfSetBattery},
	{BTP_FUNCID_HFP_HF_SET_SIGNAL, btp_func_hfpHfSetSignal},
	{BTP_FUNCID_HFP_HF_ANSWER, btp_func_hfpHfAnswer},
	{BTP_FUNCID_HFP_HF_REDIAL, btp_func_hfpHfRedial},
	{BTP_FUNCID_HFP_HF_DAIL, btp_func_hfpHfDail},
	{BTP_FUNCID_HFP_HF_HUNGUP, btp_func_hfpHfHUngup},
	{BTP_FUNCID_HFP_HF_REJECT, btp_func_hfpHfReject},
	{BTP_FUNCID_HFP_HF_ACTIVATE_VOICE_RECOG, btp_func_hfpHfActivateVoiceRecog},
	{BTP_FUNCID_HFP_HF_REJECT_WAIT_AND_KEEP_ACTIVE, btp_func_hfpHfRejectWaitAndKeepActive},
	{BTP_FUNCID_HFP_HF_ACCEPT_WAIT_AND_HOLD_ACTIVE, btp_func_hfpHfAcceptWaitAndHoldActive},
	{BTP_FUNCID_HFP_HF_HUNGUP_ACTIVE_AND_RESUME_HOLD, btp_func_hfpHfHungupActiveAndResumeHold},
	{BTP_FUNCID_HFP_AG_CONNECT, btp_func_hfpAgConnect},
	{BTP_FUNCID_HFP_AG_DISCONN, btp_func_hfpAgDisconn},
	{BTP_FUNCID_HFP_AG_SET_FEATURE, btp_func_hfpAgSetFeature},
	{BTP_FUNCID_HFP_AG_PALCE_CALL, btp_func_hfpAgPlaceCall},
	{BTP_FUNCID_HFP_AG_REMOVE_CALL, btp_func_hfpAgRemoveCall},
	{BTP_FUNCID_HFP_AG_ACTIVE_CALL, btp_func_hfpAgActiveCall},
	//PBAP
	{BTP_FUNCID_PBAP_CONNECT, btp_func_pbapConnect},
	{BTP_FUNCID_PBAP_DISCONN, btp_func_pbapDisconn},
	
};



int btp_func_call(uint16 funcID, uint08 *pData, uint16 dataLen)
{
	int index;
	int count;
	
	count = sizeof(scBtpFunSet)/sizeof(scBtpFunSet[0]);
	for(index= 0; index<count; index++){
		if(scBtpFunSet[index].funID == funcID) break;
	}
	if(index == count || scBtpFunSet[index].Func == nullptr){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "Function ID is not in BTP !");
		return -TLK_EFAIL;
	}
	return scBtpFunSet[index].Func(pData,dataLen);
}
void btp_func_setAclHandle(uint16 aclHandle)
{
	sBtpFuncAclHandle = aclHandle;
}
void btp_func_setSppRfcChn(uint08 rfcChannel)
{
	sBtpFuncSppRfcChn = rfcChannel;
}
void btp_func_setSppHfpHfChn(uint08 rfcChannel)
{
	sBtpFuncHfpHfRfcChn = rfcChannel;
}
void btp_func_setSppHfpAgChn(uint08 rfcChannel)
{
	sBtpFuncHfpAgRfcChn = rfcChannel;
}


static int btp_func_setConnTimeout(uint08 *pData, uint16 dataLen)
{
	uint16 timeout;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_setConnTimeout: failure - param error");
		return -TLK_EPARAM;
	}
	timeout = ((uint16)pData[1]<<8 | pData[0]);
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_setConnTimeout: timeout[%dms]", timeout);
	btp_setConnTimeout(timeout);
	return TLK_ENONE;
}
static int btp_func_sdpSrvConnect(uint08 *pData, uint16 dataLen)
{
	uint16 handle;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sdpSrvConnect: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sdpSrvConnect: handle[0x%x]", handle);
	return btp_sdp_connect(handle, BTP_USRID_SERVER);
}
static int btp_func_sdpSrvDisconn(uint08 *pData, uint16 dataLen)
{
	uint16 handle;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sdpSrvDisconn: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sdpSrvDisconn: handle[0x%x]", handle);
	return btp_sdp_connect(handle, BTP_USRID_SERVER);
}
static int btp_func_sdpCltConnect(uint08 *pData, uint16 dataLen)
{
	uint16 handle;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sdpCltConnect: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sdpCltConnect: handle[0x%x]", handle);
	return btp_sdp_connect(handle, BTP_USRID_CLIENT);
}
static int btp_func_sdpCltDisconn(uint08 *pData, uint16 dataLen)
{
	uint16 handle;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sdpCltDisconn: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sdpCltDisconn: handle[0x%x]", handle);
	return btp_sdp_connect(handle, BTP_USRID_CLIENT);
}

static int btp_func_rfcConnect(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	
	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_rfcConnect: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	return btp_rfcomm_connect(handle);
}
static int btp_func_rfcDisconn(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	
	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_rfcDisconn: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	return btp_rfcomm_disconn(handle);
}

static int btp_func_sppConnect(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	uint08 channel;

	if(pData == nullptr || dataLen < 3){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sppConnect: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	if(pData[2] == 0x00) channel = sBtpFuncSppRfcChn;
	else channel = pData[2];
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sppConnect: handle[0x%x] channel[%d]",
		handle, channel);
	return btp_spp_connect(handle, channel);
}
static int btp_func_sppDisconn(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	
	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sppDisconn: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sppDisconn: handle[0x%x]", handle);
	return btp_spp_disconn(handle);
}
static int btp_func_sppSendData(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	
	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sppSendData: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sppSendData: handle[0x%x],Length[%d]",
		handle, dataLen-2);
	return btp_spp_sendData(handle, pData+2, dataLen-2, nullptr, 0);
}
static int btp_func_sppSendRLS(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	
	if(pData == nullptr || dataLen < 3){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sppSendRLS: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sppSendRLS: handle[0x%x],Length[%d]",
		handle, dataLen-2);
	return btp_spp_sendRemoteLineStatus(handle, pData[2]);
}

static int btp_func_a2dpSetMode(uint08 *pData, uint16 dataLen)
{
	uint08 mode;
	uint16 handle;

	if(pData == nullptr || dataLen < 3){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSetMode: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	mode = pData[2];
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSetMode: handle[0x%x], mode[%d]",
		handle, mode);
	return btp_a2dp_setMode(handle, mode);
}
static int btp_func_a2dpSrcConnect(uint08 *pData, uint16 dataLen)
{
	uint16 handle;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcConnect: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcConnect: handle[0x%x]", handle);
	return btp_a2dp_connect(handle, BTP_USRID_SERVER);
}
static int btp_func_a2dpSrcDisconn(uint08 *pData, uint16 dataLen)
{
	uint16 handle;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcDisconn: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcDisconn: handle[0x%x]", handle);
	return btp_a2dp_disconn(handle);
}
static int btp_func_a2dpSnkConnect(uint08 *pData, uint16 dataLen)
{
	uint16 handle;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSnkConnect: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSnkConnect: handle[0x%x]", handle);
	return btp_a2dp_connect(handle, BTP_USRID_CLIENT);
}
static int btp_func_a2dpSnkDisconn(uint08 *pData, uint16 dataLen)
{
	uint16 handle;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSnkDisconn: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSnkDisconn: handle[0x%x]", handle);
	return btp_a2dp_disconn(handle);
}
static int btp_func_a2dpSnkDelayReport(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	uint16 delay;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSnkDelayReport: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	delay = ((uint16)pData[3]<<8 | pData[2]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	if(delay == 0) delay = 1000;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSnkDelayReport: handle[0x%x] delay[0x%x]", handle, delay);
	return btp_a2dpsnk_delayreport(handle, delay);
}
static int btp_func_a2dpSrcSendStart(uint08 *pData, uint16 dataLen)
{
	uint16 handle;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcSendA2DPStart: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcSendA2DPStart: handle[0x%x]", handle);
	return btp_a2dpsrc_start(handle);
}
static int btp_func_a2dpSrcSendMediaData(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	uint08 SBCBuffer[] = {	
		0x63,0x10,0x33,0x01,0x56,0xda,0xa5,0xb5,0xb2,0x30,0x93,0xcb,0xe8,0x62,0xea,0x70,0xd3,0x60,0xda,\
		0x7b,0xe3,0x4e,0xd4,0x95,0xe9,0x69,0x26,0x05,0x91,0x5d,0x91,0x9c,0x91,0x20,0x7d,0xca,0x75,0x22,\
		0x22,0x62,0x15,0x5e,0xa0,0xc4,0x6a,0x45,0x0a,0x9c,0xa9,0x66,0xd5,0x4d,0x94,0x59,0x0b,0x33,0x98,\
		0xba,0x5d,0x91,0x27,0xd9,0x09,0x2c,0x4f,0x39,0xe1,0x33,0x1c,0x4a,0x63,0x9c,0x91,0x20,0xa9,0xca,\
		0x75,0x21,0x22,0x8f,0x75,0x5a,0x96,0x64,0xd0,0x61,0x55,0x59,0x8e,0x26,0xb2,0x8c,0x6f,0x9b,0x44,\
		0x9b,0xd3,0xa8,0xc1,0x1a,0xc7,0x20,0xc5,0x26,0x4f,0xaf,0x35,0xa4,0x19,0xa2,0x46,0x9c,0x91,0x20,\
		0x7d,0xca,0x75,0x22,0x22,0xc3,0xe0,0x52,0xd4,0x83,0x12,0xf0,0xe1,0x89,0xa3,0xc2,0x8a,0x9d,0x65,\
		0xd5,0x4a,0x08,0x8b,0x52,0x1a,0x63,0xdb,0xd6,0x5c,0xe6,0x2b,0x41,0x59,0xc0,0xa3,0xcf,0x23,0x9c,\
		0x91,0x20,0xea,0xca,0x64,0x22,0x22,0x72,0x91,0x78,0x6b,0x65,0x1b,0x82,0x54,0xcd,0xe7,0xc5,0x53,\
		0x64,0x4c,0x19,0x13,0x56,0x22,0xb2,0xa6,0xc7,0x2c,0xb9,0x21,0x98,0x5b,0x33,0x5d,0x77,0x1e,0x6e,\
		0x23,0x9c,0x91,0x20,0x7d,0xca,0x75,0x22,0x22,0x9f,0x5f,0x32,0x10,0xc1,0xe4,0xc1,0x8a,0x47,0x8d,\
		0x39,0x0a,0x26,0x65,0x52,0x93,0x9d,0x32,0x50,0x94,0xd4,0x19,0x42,0x5c,0x93,0xd1,0xf1,0x9b,0x7e,\
		0xa4,0xbe,0x14,0x9c,0x91,0x20,0x5f,0xca,0x73,0x22,0x21,0xaa,0xd1,0x9c,0xe3,0x1f,0xc3,0xdf,0x59,\
		0x55,0x71,0xe1,0x6d,0xda,0xb6,0x60,0x24,0xb8,0xed,0xd6,0xe4,0x78,0x4a,0x2c,0xe9,0x63,0xed,0xce,\
		0x25,0xca,0x73,0x97,0x43,0x9c,0x91,0x20,0x5e,0xca,0x64,0x32,0x22,0x63,0x8d,0x16,0x09,0x53,0xe7,\
		0x9a,0x89,0xd5,0xae,0xc1,0x9c,0x9e,0x15,0x1e,0x9e,0x76,0x9e,0xea,0xa9,0xb3,0x61,0xb0,0xaa,0xe3,\
		0xdf,0xaa,0x62,0x5f,0xcf,0x2c,0xe3};

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcSendMediaData: failure - param error");
		return -TLK_EPARAM;
	}
	
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	return btp_a2dpsrc_sendMediaData(handle, 0, 0, SBCBuffer, sizeof(SBCBuffer));
}
static int btp_func_a2dpSendAbort(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
		
	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSendAbort: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSendAbort: handle[0x%x]", handle);
	return btp_a2dpsrc_abort(handle);
}


static int btp_func_avrcpConnect(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	uint08 userID;
	
	if(pData == nullptr || dataLen < 3){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpConnect: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	userID = pData[2];
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpConnect: handle[0x%x] userID[0x%x]", handle, userID);
	btp_avrcp_connect(handle, userID);
	return TLK_ENONE;
}
static int btp_func_avrcpDisconn(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	uint08 userID;
	
	if(pData == nullptr || dataLen < 3){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpDisconn: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	userID = pData[2];
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpDisconn: handle[0x%x] userID[0x%x]", handle, userID);
	btp_avrcp_disconn(handle, userID);
	return TLK_ENONE;
}
static int btp_func_avrcpSendKeyPressed(uint08 *pData, uint16 dataLen)
{
	uint08 keyID;
	uint16 handle;
		
	if(pData == nullptr || dataLen < 3){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSendKeyPressed: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	keyID = pData[2];
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSendKeyPressed: handle[0x%x] keyID[0x%x]", handle, keyID);
	btp_avrcp_sendKeyPress(handle, keyID);
	return TLK_ENONE;
}
static int btp_func_avrcpSendKeyReleased(uint08 *pData, uint16 dataLen)
{
	uint08 keyID;
	uint16 handle;
		
	if(pData == nullptr || dataLen < 3){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSendKeyReleased: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	keyID = pData[2];
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSendKeyReleased: handle[0x%x] keyID[0x%x]", handle, keyID);
	btp_avrcp_sendKeyRelease(handle, keyID);
	return TLK_ENONE;
}
static int btp_func_avrcpSendRegEvtNotyCmd(uint08 *pData, uint16 dataLen)
{
	uint08 evtID;
	uint16 handle;
		
	if(pData == nullptr || dataLen < 3){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSendRegEvtNotyCmd: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	evtID = pData[2];
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSendRegEvtNotyCmd: handle[0x%x] keyID[0x%x]", handle, evtID);
	btp_avrcp_regEventNotify(handle, evtID);
	return TLK_ENONE;
}
static int btp_func_avrcpSendEventNoty(uint08 *pData, uint16 dataLen)
{
	uint08 evtID;
	uint16 handle;
		
	if(pData == nullptr || dataLen < 3){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSendEventNoty: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	evtID = pData[2];
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSendEventNoty: handle[0x%x] evtID[0x%x]", handle, evtID);
	btp_avrcp_notifyStatusChange(handle, evtID, pData+3, dataLen-3);
	return TLK_ENONE;
}
static int btp_func_avrcpSetTrackValue(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	uint32 valueH;
	uint32 valueL;
		
	if(pData == nullptr || dataLen < 10){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSetTrackValue: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	ARRAY_TO_UINT32L(pData, 2, valueL);
	ARRAY_TO_UINT32L(pData, 6, valueH);
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSetTrackValue: handle[0x%x]", handle);
	btp_avrcp_setTrackValue(handle, valueH, valueL);
	return TLK_ENONE;
}
static int btp_func_avrcpSetPlayStatus(uint08 *pData, uint16 dataLen)
{
	uint08 status;
	uint16 handle;
		
	if(pData == nullptr || dataLen < 3){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSetPlayStatus: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	status = pData[2];
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSetPlayStatus: handle[0x%x] status[0x%x]", handle, status);
	btp_avrcp_setPlayState(handle, status);
	return TLK_ENONE;
}


static int btp_func_hfpHfConnect(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	uint08 channel;
		
	if(pData == nullptr || dataLen < 3){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfConnect: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	channel = pData[2];
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfConnect: handle[0x%x] channel[0x%x]", handle, channel);
	return btp_hfphf_connect(handle, channel);
}
static int btp_func_hfpHfDisconn(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
		
	if(pData == nullptr || dataLen < 3){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfDisconn: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfDisconn: handle[0x%x]", handle);
	return btp_hfphf_disconn(handle);
}
static int btp_func_hfpHfSetFeature(uint08 *pData, uint16 dataLen)
{
	uint32 feature;

	if(pData == nullptr || dataLen < 4){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfSetFeature: failure - param error");
		return -TLK_EPARAM;
	}
	feature = ((uint32)pData[3]<<24 | pData[2]<<16 | pData[1]<<8 | pData[0]);
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfSetFeature: feature[0x%x]", feature);
	btp_hfphf_setFeature(feature);
	return TLK_ENONE;
}
static int btp_func_hfpHfSetVolume(uint08 *pData, uint16 dataLen)
{
	uint08 volume;
	uint08 volumeType;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfSetVolume: failure - param error");
		return -TLK_EPARAM;
	}
	volumeType = pData[0];
	volume = pData[1];
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfSetVolume: spkVolume[0x%x] volumeType[0x%x]", volume, volumeType);

	if(volumeType > 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfSetVolume: failure - volumeType param error");
		return -TLK_EPARAM;
	}
	if(volumeType == BTP_HFP_VOLUME_TYPE_SPK) return btp_hfphf_setSpkVolume(volume);
	else return btp_hfphf_setMicVolume(volume);
}
static int btp_func_hfpHfSetBattery(uint08 *pData, uint16 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static int btp_func_hfpHfSetSignal(uint08 *pData, uint16 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static int btp_func_hfpHfAnswer(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
		
	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfAnswer: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfAnswer: handle[0x%x]", handle);
	return btp_hfphf_answer(handle);
}
static int btp_func_hfpHfRedial(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
		
	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfRedial: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfRedial: handle[0x%x]", handle);
	return btp_hfphf_redial(handle);
}
static int btp_func_hfpHfDail(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	uint08 numbLen;
	uint08 index = 0;

	if(pData == nullptr || dataLen < 4){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfDail: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	numbLen = pData[2];
	char pNumber[numbLen];
	for(int i = 0; i < numbLen; i++){
		pNumber[index++] = pData[3+i];
	}
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfDail: handle[0x%x]", handle);
	tlkapi_array(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfDail: call number: ", pNumber, numbLen);
	return btp_hfphf_dial(handle, pNumber, numbLen);
}
static int btp_func_hfpHfHUngup(uint08 *pData, uint16 dataLen)
{
	uint16 handle;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfHUngup: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfHUngup: handle[0x%x]", handle);
	return btp_hfphf_hungUp(handle);
}
static int btp_func_hfpHfReject(uint08 *pData, uint16 dataLen)
{
	uint16 handle;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfReject: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfReject: handle[0x%x]", handle);
	return btp_hfphf_reject(handle);
}
static int btp_func_hfpHfActivateVoiceRecog(uint08 *pData, uint16 dataLen)
{
	uint16 handle;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfActivateVoiceRecog: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfActivateVoiceRecog: handle[0x%x]", handle);
	return btp_hfphf_siri_ctrl(handle);
}
static int btp_func_hfpHfRejectWaitAndKeepActive(uint08 *pData, uint16 dataLen)
{
	uint16 handle;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfRejectWaitAndKeepActive: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfRejectWaitAndKeepActive: handle[0x%x]", handle);
	return btp_hfphf_rejectWaitAndKeepActive(handle);
}
static int btp_func_hfpHfAcceptWaitAndHoldActive(uint08 *pData, uint16 dataLen)
{
	uint16 handle;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfAcceptWaitAndHoldActive: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfAcceptWaitAndHoldActive: handle[0x%x]", handle);
	return btp_hfphf_acceptWaitAndHoldActive(handle);
}
static int btp_func_hfpHfHungupActiveAndResumeHold(uint08 *pData, uint16 dataLen)
{	
	uint16 handle;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfHungupActiveAndResumeHold: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfHungupActiveAndResumeHold: handle[0x%x]", handle);

	return btp_hfphf_hungUpActiveAndResumeHold(handle);
}
static int btp_func_hfpAgConnect(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	uint08 channel;
		
	if(pData == nullptr || dataLen < 3){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgConnect: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	channel = pData[2];
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgConnect: handle[0x%x] channel[0x%x]", handle, channel);
	return btp_hfpag_connect(handle, channel);
}
static int btp_func_hfpAgDisconn(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
		
	if(pData == nullptr || dataLen < 2){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgDisconn: failure - param error");
		return -TLK_EPARAM;
	}
	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgDisconn: handle[0x%x]", handle);
	return btp_hfpag_disconn(handle);
}
static int btp_func_hfpAgSetFeature(uint08 *pData, uint16 dataLen)
{
	uint32 feature;

	if(pData == nullptr || dataLen < 4){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgSetFeature: failure - param error");
		return -TLK_EPARAM;
	}
	feature = ((uint32)pData[3]<<24 | pData[2]<<16 | pData[1]<<8 | pData[0]);
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgSetFeature: feature[0x%x]", feature);
	btp_hfpag_setFeature(feature);
	return TLK_ENONE;
}
static int btp_func_hfpAgPlaceCall(uint08 *pData, uint16 dataLen)
{
	
	uint08 numbLen;
	uint08 isIncoming;
	uint08 index = 0;

	if(pData == nullptr || dataLen < 9){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgPlaceCall: failure - param error");
		return -TLK_EPARAM;
	}
	numbLen = pData[7];
	char pNumber[numbLen];
	pNumber[index++] = pData[0];
	pNumber[index++] = pData[1];	
	pNumber[index++] = pData[2];
	pNumber[index++] = pData[3];
	pNumber[index++] = pData[4];	
	pNumber[index++] = pData[5];
	pNumber[index++] = pData[6];
	isIncoming = pData[8];
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgPlaceCall: isIncoming[0x%x] ", isIncoming);
	tlkapi_array(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgPlaceCall: call number: ", pNumber, numbLen);
	return btp_hfpag_insertCall(pNumber, numbLen, isIncoming);
}
static int btp_func_hfpAgRemoveCall(uint08 *pData, uint16 dataLen)
{
	uint08 index;
	uint08 numbLen;

	if(pData == nullptr || dataLen < 8){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgRemoveCall: failure - param error");
		return -TLK_EPARAM;
	}
	numbLen = pData[7];
	char pNumber[numbLen];

	index = 0;
	pNumber[index++] = pData[0];
	pNumber[index++] = pData[1];	
	pNumber[index++] = pData[2];
	pNumber[index++] = pData[3];
	pNumber[index++] = pData[4];	
	pNumber[index++] = pData[5];
	pNumber[index++] = pData[6];
	tlkapi_array(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgRemoveCall: call number: ", pNumber, numbLen);
	return btp_hfpag_removeCall(pNumber, numbLen);
}
static int btp_func_hfpAgActiveCall(uint08 *pData, uint16 dataLen)
{
	uint08 index;
	uint08 numbLen;

	if(pData == nullptr || dataLen < 8){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgActiveCall: failure - param error");
		return -TLK_EPARAM;
	}
	numbLen = pData[7];
	char pNumber[numbLen];

	index = 0;
	pNumber[index++] = pData[0];
	pNumber[index++] = pData[1];	
	pNumber[index++] = pData[2];
	pNumber[index++] = pData[3];
	pNumber[index++] = pData[4];	
	pNumber[index++] = pData[5];
	pNumber[index++] = pData[6];
	tlkapi_array(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgActiveCall: call number: ", pNumber, numbLen);
	return btp_hfpag_activeCall(pNumber, numbLen);
}

static int btp_func_pbapConnect(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	uint08 usrID;
	uint16 psmOrChn;
	bool isL2cap;

	if(pData == nullptr || dataLen < 6){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_pbapConnect: failure - param error");
		return -TLK_EPARAM;
	}

	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	usrID = pData[2];
	psmOrChn = ((uint16)pData[4]<<8 | pData[3]);
	if(psmOrChn == 0) psmOrChn = 9;
	isL2cap = pData[5];
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_pbapConnect: handle[0x%x] usrID[0x%x] psmOrChn[0x%x] isL2cap[0x%x]", handle, usrID, psmOrChn, isL2cap);
	return btp_pbap_connect(handle, usrID, psmOrChn, isL2cap);
}
static int btp_func_pbapDisconn(uint08 *pData, uint16 dataLen)
{
	uint16 handle;
	uint08 usrID;

	if(pData == nullptr || dataLen < 3){
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_pbapDisconn: failure - param error");
		return -TLK_EPARAM;
	}

	handle = ((uint16)pData[1]<<8 | pData[0]);
	if(handle == 0) handle = sBtpFuncAclHandle;
	usrID = pData[2];
	tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_pbapDisconn: handle[0x%x] usrID[0x%x]", handle, usrID);
	return btp_pbap_disconn(handle, usrID);
}

#endif //#if (TLK_STK_BTP_ENABLE && TLK_CFG_TEST_ENABLE)

