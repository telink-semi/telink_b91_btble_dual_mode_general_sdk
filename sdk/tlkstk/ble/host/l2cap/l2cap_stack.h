/********************************************************************************************************
 * @file     l2cap_stack.h
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

#ifndef STACK_BLE_L2CAP_STACK_H_
#define STACK_BLE_L2CAP_STACK_H_

#include "tlkstk/ble/host/host_stack.h"

typedef struct
{
	u8 *rx_p;
	u8 *tx_p;

	u16 max_rx_size;
	u16 max_tx_size;

	u16 init_MTU;
	u16 rsvd;

	u32 mtuReqSendTimeUs;

}l2cap_buff_t;

extern l2cap_buff_t l2cap_buff_m;
extern l2cap_buff_t l2cap_buff_s;
extern l2cap_buff_t *pl2cap_buff;

void 	blt_l2cap_para_pre_init(void);

u8   	blt_UpdateParameter_request (u16 connHandle);


u8*  	blt_l2cap_get_s_tx_buff(u16 connHandle);
u8*  	blt_l2cap_get_m_tx_buff(u16 connHandle);
ble_sts_t  blt_l2cap_pushData_2_controller (u16 connHandle, u16 cid, u8 *format, int format_len, u8 *pDate, int data_len);



//////////////////////////////////////////////////////////////////////////////////////////////

#if (L2CAP_CREDIT_BASED_FLOW_CONTROL_MODE_EN)

int blc_l2cap_sendCommandRejectRsp(u16 aclHandle, u8 identifier, u16 reason, u8 dataLen, u8 *pData);
int blc_l2cap_sendDisconnectReq(u16 aclHandle, u8 identifier, u16 dcid, u16 scid);
int blc_l2cap_sendDisconnectRsp(u16 aclHandle, u8 identifier, u16 dcid, u16 scid);
int blc_l2cap_sendLECreditBasedConnectReq(u16 aclHandle, u8 identifier, u16 spsm, 
	u16 scid, u16 mtu, u16 mps, u16 credits);
int blc_l2cap_sendLECreditBasedConnectRsp(u16 aclHandle, u8 identifier, u16 dcid, 
	u16 mtu, u16 mps, u16 credits, u16 result);
int blc_l2cap_sendFlowControlCreditInd(u16 aclHandle, u8 identifier, u8 cid, u8 credits);
int blc_l2cap_sendCreditBasedConnectReq(u16 aclHandle, u8 identifier, u16 spsm, u16 mtu,
	u16 mps, u16 credits, u8 numCids, u16 *pSCidList);
int blc_l2cap_sendCreditBasedConnectRsp(u16 aclHandle, u8 identifier, u16 mtu, u16 mps, 
	u16 credits, u16 result, u8 numCids, u16 *pDCidList);
int blc_l2cap_sendCreditBasedReconfigReq(u16 aclHandle, u8 identifier, u16 mtu, u16 mps,
	u8 numCids, u16 *pDCidList);
int blc_l2cap_sendCreditBasedReconfigRsp(u16 aclHandle, u8 identifier, u16 result);


l2cap_coc_handle_t *blt_l2cap_cocGetHandle(u16 aclHandle);
l2cap_coc_handle_t *blt_l2cap_cocGetIdleHandle(void);

void blt_l2cap_cocDataHandler(u16 aclHandle, rf_packet_l2cap_t *pL2cap);

bool blt_l2cap_channel_is_busy(u16 scid);
void blt_l2cap_set_channel_busy(u16 scid);
void blt_l2cap_clr_channel_busy(u16 scid);

u8 blt_l2cap_getIdleIdentifier(void);
u8 blt_l2cap_cocGetSrvNum(l2cap_coc_service_t *pServer);
l2cap_coc_service_t *blt_l2cap_cocGetService(u8 srvNum);
l2cap_coc_service_t *blt_l2cap_cocGetIdleService(u16 spsm);
l2cap_coc_service_t *blt_l2cap_cocSearchService(u16 aclHandle, u8 identifier);
l2cap_coc_service_t *blt_l2cap_cocSearchPendingService(u16 aclHandle, u8 identifier);
l2cap_coc_service_t *blt_l2cap_cocSearchDisconnService(u16 aclHandle, u8 identifier);
l2cap_coc_service_t *blt_l2cap_cocSearchServiceBySpsm(u16 aclHandle, u16 spsm);
l2cap_coc_service_t *blt_l2cap_cocSearchServiceByScid(u16 aclHandle, u16 scid, u16 *pDcid);
l2cap_coc_service_t *blt_l2cap_cocSearchServiceByDcid(u16 aclHandle, u16 dcid, u16 *pScid);
int  blt_l2cap_cocMallocService(u16 spsm, u8 chCount, blc_l2cap_coc_evtcb evtCb, 
	 blc_l2cap_coc_datacb dataCb);
void blt_l2cap_cocFreeService(u16 servNum);
void blt_l2cap_cocFreeService1(l2cap_coc_service_t *pServer);

l2cap_coc_channel_t *blt_l2cap_cocGetChannel(u16 scid);
l2cap_coc_channel_t *blt_l2cap_cocSearchChByDcid(u16 aclHandle, u16 dcid, u16 *pScid);
u8 blt_l2cap_cocMallocChannel(u16 chCount, u16 *pChList);
void blt_l2cap_cocFreeChannel(u16 chCount, u16 *pCidList);

u8 *blt_signal_l2capCmdHandler(u16 connHandle, u8 * p);

#endif

#endif /* L2CAP_STACK_H_ */
