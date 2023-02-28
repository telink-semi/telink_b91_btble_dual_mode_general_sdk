/********************************************************************************************************
 * @file     btp_funcs.h
 *
 * @brief    This is the header file for BTBLE SDK
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
#ifndef BTP_FUNCS_H
#define BTP_FUNCS_H

#if (TLK_STK_BTP_ENABLE)


enum BTP_FUNCID_SET_ENUM{
	BTP_FUNCID_NONE = 0x0000,
	//SDP Start
	BTP_FUNCID_SDP_START = 0x0100,
	BTP_FUNCID_SDP_SRV_CONNECT = 0x01+BTP_FUNCID_SDP_START,
	BTP_FUNCID_SDP_SRV_DISCONN = 0x02+BTP_FUNCID_SDP_START,
	BTP_FUNCID_SDP_CLT_CONNECT = 0x03+BTP_FUNCID_SDP_START,
	BTP_FUNCID_SDP_CLT_DISCONN = 0x04+BTP_FUNCID_SDP_START,
	//RFC Start
	BTP_FUNCID_RFC_START = 0x0200,
 	//IAP Start
 	BTP_FUNCID_IAP_START = 0x0300,
	//SPP Start
	BTP_FUNCID_SPP_START = 0x0400,
	BTP_FUNCID_SPP_CONNECT = 0x01+BTP_FUNCID_SPP_START,
	BTP_FUNCID_SPP_DISCONN = 0x02+BTP_FUNCID_SPP_START,
	//ATT Start
	BTP_FUNCID_ATT_START = 0x0500,
	//A2DP Start
	BTP_FUNCID_A2DP_START = 0x0600,
	BTP_FUNCID_A2DP_SET_MODE    = 0x01+BTP_FUNCID_A2DP_START,
	BTP_FUNCID_A2DP_SRC_CONNECT = 0x11+BTP_FUNCID_A2DP_START,
	BTP_FUNCID_A2DP_SRC_DISCONN = 0x12+BTP_FUNCID_A2DP_START,
	BTP_FUNCID_A2DP_SNK_CONNECT = 0x13+BTP_FUNCID_A2DP_START,
	BTP_FUNCID_A2DP_SNK_DISCONN = 0x14+BTP_FUNCID_A2DP_START,
	//HID Start
	BTP_FUNCID_HID_START = 0x0700,
	//HFP Start
	BTP_FUNCID_HFP_START = 0x0800,
	//AVRCP Start
	BTP_FUNCID_AVRCP_START = 0x0900,
	//OBEX Start
	BTP_FUNCID_OBEX_START = 0x0A00,
	//PBAP Start
	BTP_FUNCID_PBAP_START = 0x0B00,
};


typedef struct{
	uint16 funID;
	int(*Func)(uint08 *pData, uint16 dataLen);
}btp_func_item_t;


int btp_func_call(uint16 funcID, uint08 *pData, uint16 dataLen);

void btp_func_setAclHandle(uint16 aclHandle);


static int btp_func_sdpSrvConnect(uint08 *pData, uint16 dataLen);
static int btp_func_sdpSrvDisconn(uint08 *pData, uint16 dataLen);
static int btp_func_sdpCltConnect(uint08 *pData, uint16 dataLen);
static int btp_func_sdpCltDisconn(uint08 *pData, uint16 dataLen);

static int btp_func_sppConnect(uint08 *pData, uint16 dataLen);
static int btp_func_sppDisconn(uint08 *pData, uint16 dataLen);

static int btp_func_a2dpSetMode(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSrcConnect(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSrcDisconn(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSnkConnect(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSnkDisconn(uint08 *pData, uint16 dataLen);




#endif //#if (TLK_STK_BTP_ENABLE)

#endif //BTP_FUNCS_H

