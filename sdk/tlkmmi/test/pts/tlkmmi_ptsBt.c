/********************************************************************************************************
 * @file     tlkmmi_ptsBt.c
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
#if (TLKMMI_TEST_ENABLE)
#include "../tlkmmi_testStdio.h"
#if (TLK_TEST_PTS_ENABLE)
#include "tlkmmi_pts.h"
#include "tlkmmi_ptsBt.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/sdp/btp_sdp.h"


extern int bt_ll_set_bd_addr(uint8_t *bdAddr);
extern void bth_func_setAclHandle(uint16 aclHandle);
extern void bth_func_setScoHandle(uint16 scoHandle);
extern void btp_func_setAclHandle(uint16 aclHandle);

static int tlkmmi_pts_btAclRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btAclConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btAclEncryptEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btAclDisconnEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btScoRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btScoConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btScoDisconnEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btProfileServiceEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btProfileChannelEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btProfileRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btProfileConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btProfileDisconnEvt(uint08 *pData, uint16 dataLen);


uint08 gTlkMmiTestPtsHfpChannel = 0;
uint08 gTlkMmiTestPtsSppChannel = 0;
uint16 gTlkMmiTestPtsBtAclHandle = 0;
uint16 gTlkMmiTestPtsBtScoHandle = 0;
uint08 gTlkMmiTestPtsDongleAddr[6] = {0};


int tlkmmi_pts_btInit(void)
{
	uint08 bdaddr[6] = TLKMMI_BTPTS_BDADDR_DEF;
	
	bth_hci_sendWriteClassOfDeviceCmd(TLKMMI_BTPTS_DEVICE_CLASS);
	bth_hci_sendWriteSimplePairingModeCmd(1);// enable simple pairing mode

	bt_ll_set_bd_addr(bdaddr);
	bth_hci_sendWriteLocalNameCmd(TLKMMI_BTPTS_BTNAME_DEF);
	
	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  tlkmmi_pts_btAclRequestEvt);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, tlkmmi_pts_btAclConnectEvt);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, tlkmmi_pts_btAclEncryptEvt);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, tlkmmi_pts_btAclDisconnEvt);

	bth_event_regCB(BTH_EVTID_SCOCONN_REQUEST,  tlkmmi_pts_btScoRequestEvt);
	bth_event_regCB(BTH_EVTID_SCOCONN_COMPLETE, tlkmmi_pts_btScoConnectEvt);
	bth_event_regCB(BTH_EVTID_SCOCONN_COMPLETE, tlkmmi_pts_btScoDisconnEvt);

	btp_event_regCB(BTP_EVTID_PROFILE_SERVICE, tlkmmi_pts_btProfileServiceEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_CHANNEL, tlkmmi_pts_btProfileChannelEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_REQUEST, tlkmmi_pts_btProfileRequestEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_CONNECT, tlkmmi_pts_btProfileConnectEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_DISCONN, tlkmmi_pts_btProfileDisconnEvt);

	return TLK_ENONE;
}

void tlkmmi_pts_btConnDevice(uint08 *pAddr)
{
	bth_acl_connect(gTlkMmiTestPtsDongleAddr, 0x240404, 1, 5000);
}
void tlkmmi_pts_btDiscDevice(void)
{
	bth_acl_disconn(gTlkMmiTestPtsBtAclHandle);
}

void tlkmmi_pts_btConnProfile(uint08 ptype, uint08 usrID)
{

}
void tlkmmi_pts_btDiscProfile(uint08 ptype, uint08 usrID)
{

}

static int tlkmmi_pts_btAclRequestEvt(uint08 *pData, uint16 dataLen)
{
	uint08 btRole;
	bth_aclConnRequestEvt_t *pEvt;

	pEvt = (bth_aclConnRequestEvt_t*)pData;
	
	tlkapi_trace(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_btAclRequestEvt: {devClass - 0x%x}", pEvt->devClass);

	uint08 devType = bth_devClassToDevType(pEvt->devClass);
	if(devType == BTH_REMOTE_DTYPE_HEADSET){
		btRole = BTH_ROLE_MASTER;
	}else if((devType == BTH_REMOTE_DTYPE_COMPUTER)||( devType == BTH_REMOTE_DTYPE_PHONE)){
		btRole = BTH_ROLE_SLAVE;
	}else{
		btRole = BTH_ROLE_NOT_SET;
	}
	btRole = BTH_ROLE_NOT_SET;
	bth_acl_setInitRole(pEvt->peerMac, btRole);
	
	return TLK_ENONE;
}
static int tlkmmi_pts_btAclConnectEvt(uint08 *pData, uint16 dataLen)
{
	bth_aclConnComplateEvt_t *pEvt;
	
	pEvt = (bth_aclConnComplateEvt_t*)pData;
	if(pEvt->status != TLK_ENONE){
		tlkapi_error(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_btAclConnectEvt: failure -- %d", pEvt->status);
		return TLK_ENONE;
	}
	
	tlkapi_array(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_btAclConnectEvt", pData, dataLen);
	
	gTlkMmiTestPtsBtAclHandle = pEvt->handle;
	bth_func_setAclHandle(gTlkMmiTestPtsBtAclHandle);
	btp_func_setAclHandle(gTlkMmiTestPtsBtAclHandle);
	
	return TLK_ENONE;
}
static int tlkmmi_pts_btAclEncryptEvt(uint08 *pData, uint16 dataLen)
{
	return TLK_ENONE;
}
static int tlkmmi_pts_btAclDisconnEvt(uint08 *pData, uint16 dataLen)
{
	bth_aclDiscComplateEvt_t *pEvt;

	pEvt = (bth_aclDiscComplateEvt_t*)pData;

	btp_destroy(pEvt->handle);
	bth_destroy(pEvt->handle);

	tlkapi_trace(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_btAclDisconnEvt: success - 0x%x", pEvt->handle);

	gTlkMmiTestPtsBtScoHandle = 0;
	bth_func_setAclHandle(gTlkMmiTestPtsBtAclHandle);
	btp_func_setAclHandle(gTlkMmiTestPtsBtAclHandle);

	return TLK_ENONE;
}


static int tlkmmi_pts_btScoRequestEvt(uint08 *pData, uint16 dataLen)
{
	return TLK_ENONE;
}
static int tlkmmi_pts_btScoConnectEvt(uint08 *pData, uint16 dataLen)
{
	bth_scoConnComplateEvt_t *pEvt;
	
	pEvt = (bth_scoConnComplateEvt_t*)pData;
	if(pEvt->status != TLK_ENONE){
		tlkapi_error(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_btScoConnectEvt: failure -- %d", pEvt->status);
		return TLK_ENONE;
	}
	
	tlkapi_array(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_btScoConnectEvt", pData, dataLen);
	
	gTlkMmiTestPtsBtScoHandle = pEvt->scoHandle;
	bth_func_setScoHandle(gTlkMmiTestPtsBtScoHandle);
	
	return TLK_ENONE;
}
static int tlkmmi_pts_btScoDisconnEvt(uint08 *pData, uint16 dataLen)
{
	bth_scoDiscComplateEvt_t *pEvt;

	pEvt = (bth_scoDiscComplateEvt_t*)pData;

	tlkapi_trace(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_btScoDisconnEvt: success - 0x%x", pEvt->scoHandle);

	gTlkMmiTestPtsBtScoHandle = 0;
	bth_func_setScoHandle(gTlkMmiTestPtsBtScoHandle);
	
	return TLK_ENONE;
}



static int tlkmmi_pts_btProfileServiceEvt(uint08 *pData, uint16 dataLen)
{
	//Not Support.
	return TLK_ENONE;
}
static int tlkmmi_pts_btProfileChannelEvt(uint08 *pData, uint16 dataLen)
{
	btp_channelEvt_t *pEvt;

	pEvt = (btp_channelEvt_t*)pData;
	
	tlkapi_trace(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_btProfileChannelEvt:{handle-0x%x,service-%d,channel-%d}",
		pEvt->handle, pEvt->service, pEvt->channel);
	
	if(pEvt->service == BTP_SDP_SRVCLASS_ID_HANDSFREE){
		
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_HANDSFREE_AGW){
		
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_SERIAL_PORT){
		
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_PBAP_PSE){
		
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_IAP2_TEMP){
		
	}
	
	return TLK_ENONE;
}
static int tlkmmi_pts_btProfileRequestEvt(uint08 *pData, uint16 dataLen)
{
//	btp_requestEvt_t *pEvt;
//	pEvt = (btp_requestEvt_t*)pData;
	return TLK_ENONE;
}
static int tlkmmi_pts_btProfileConnectEvt(uint08 *pData, uint16 dataLen)
{
//	btp_connectEvt_t *pEvt;
//	pEvt = (btp_connectEvt_t*)pData;
	return TLK_ENONE;
}
static int tlkmmi_pts_btProfileDisconnEvt(uint08 *pData, uint16 dataLen)
{
//	btp_disconnEvt_t *pEvt;
//	pEvt = (btp_disconnEvt_t*)pData;

	return TLK_ENONE;
}



#endif //#if (TLK_TEST_PTS_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)
