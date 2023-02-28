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

#define BTP_FUNC_DBG_FLAG       ((TLK_MAJOR_DBGID_BTH << 24) | (TLK_MINOR_DBGID_BTH_FUNC << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define BTP_FUNC_DBG_SIGN       nullptr


static uint16 sBtpFuncAclHandle = 0;
static const btp_func_item_t scBtpFunSet[] = {
	//SDP
	{BTP_FUNCID_SDP_SRV_CONNECT, btp_func_sdpSrvConnect},
	{BTP_FUNCID_SDP_SRV_DISCONN, btp_func_sdpSrvDisconn},
	{BTP_FUNCID_SDP_CLT_CONNECT, btp_func_sdpCltConnect},
	{BTP_FUNCID_SDP_CLT_DISCONN, btp_func_sdpCltDisconn},
	//SPP
	{BTP_FUNCID_SPP_CONNECT, btp_func_sppConnect},
	{BTP_FUNCID_SPP_DISCONN, btp_func_sppDisconn},
	//A2DP
	{BTP_FUNCID_A2DP_SET_MODE, btp_func_a2dpSetMode},
	{BTP_FUNCID_A2DP_SRC_CONNECT, btp_func_a2dpSrcConnect},
	{BTP_FUNCID_A2DP_SRC_DISCONN, btp_func_a2dpSrcDisconn},
	{BTP_FUNCID_A2DP_SNK_CONNECT, btp_func_a2dpSnkConnect},
	{BTP_FUNCID_A2DP_SNK_DISCONN, btp_func_a2dpSnkDisconn},
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
		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "Function ID is not in BTH !");
		return -TLK_EFAIL;
	}
	return scBtpFunSet[index].Func(pData,dataLen);
}
void btp_func_setAclHandle(uint16 aclHandle)
{
	sBtpFuncAclHandle = aclHandle;
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
	channel = pData[2];
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







#endif //#if (TLK_STK_BTP_ENABLE && TLK_CFG_TEST_ENABLE)

