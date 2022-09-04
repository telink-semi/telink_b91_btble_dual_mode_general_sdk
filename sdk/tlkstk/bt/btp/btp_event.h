/********************************************************************************************************
 * @file     btp_event.h
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

#ifndef BTP_EVENT_H
#define BTP_EVENT_H


typedef int(*btp_event_func)(uint08 *pData, uint16 dataLen);



typedef enum{
	BTP_EVTID_NONE = 0,
	
	//Profile Event
	BTP_EVTID_PROFILE_REQUEST,
	BTP_EVTID_PROFILE_CONNECT,
	BTP_EVTID_PROFILE_DISCONN,

	BTP_EVTID_PROFILE_SERVICE, //L2cap Service
	BTP_EVTID_PROFILE_CHANNEL, //Rfcomm Channel

	//SDP
	BTP_EVTID_SDP_COMPLETE,
	//HFP
	BTP_EVTID_HFPHF_CODEC_CHANGED,
	BTP_EVTID_HFPAG_CODEC_CHANGED,
	BTP_EVTID_HFPHF_STATUS_CHANGED,
	BTP_EVTID_HFPHF_NUMBER_INQUIRY,
	BTP_EVTID_HFPHF_VOLUME_CHANGED,
	BTP_EVTID_HFPAG_STATUS_CHANGED,
	BTP_EVTID_HFPAG_VOLUME_CHANGED,
	//A2DP
	BTP_EVTID_A2DPSRC_CODEC_CHANGED,
	BTP_EVTID_A2DPSNK_CODEC_CHANGED,
	BTP_EVTID_A2DPSRC_STATUS_CHANGED,
	BTP_EVTID_A2DPSNK_STATUS_CHANGED,	
	
	BTP_EVTID_MAX,
}BTP_EVTID_ENUM;


typedef struct{
	uint08 ptype; //Profile Type, Refer to BTP_PTYPE_ENUM.
	uint16 chnID;
	uint16 handle;
}btp_requestEvt_t;
typedef struct{
	uint08 status;
	uint08 ptype; //Profile Type, Refer to BTP_PTYPE_ENUM.
	uint08 usrID;
	uint16 chnID;
	uint16 handle;
}btp_connectEvt_t;
typedef struct{
	uint08 ptype; //Profile Type, Refer to BTP_PTYPE_ENUM.
	uint08 usrID;
	uint16 handle;
}btp_disconnEvt_t;
typedef struct{
	uint16 handle;
	uint16 service;
	uint16 psmID;
}btp_serviceEvt_t;
typedef struct{
	uint16 handle;
	uint16 service; //Refer to .
	uint08 channel;
}btp_channelEvt_t;
typedef struct{
	uint16 handle;
}btp_sdpCompleteEvt_t;
typedef struct{
	uint16 handle;
	uint08 codec;
}btp_hfpCodecChangedEvt_t;
typedef struct{
	uint16 handle;
	uint08 volume;
	uint08 volType; //0-SPK, 1-MIC
}btp_hfpVolumeChangedEvt_t;
typedef struct{
	uint16 handle;
	uint08 status;
	uint08 callDir;
	uint08 numbLen;
	uint08 *pNumber;
}btp_hfpStatusChangedEvt_t;
typedef struct{
	uint08 state; //Inquiry State: 0-Start, 1-continue, 2-Stop
	uint16 handle;
	uint08 status;
	uint08 callDir;
	uint08 numbLen;
	uint08 *pNumber;
}btp_hfpNumberInquiryEvt_t;
typedef struct{
	uint16 handle;
	uint08 status; // 0-Closed, 1-Opened, 2-Paused, 3-Stream, Refer to BTP_A2DP_STATUS_ENUM.
}btp_a2dpStatusChangeEvt_t;
typedef struct{
	uint16 handle;
    uint08 chnMode; //Refer to BTP_A2DP_CHN_MODE_ENUM.
	uint08 codecType; //Refer to BTP_A2DP_CODEC_ENUM.
	uint16 frequence;
	uint08 aacObjType; //Refer to .
	uint32 aacBitRate; //Refer to BTP_A2DP_FREQUENCY_ENUM.
}btp_a2dpCodecChangeEvt_t;
typedef struct{
	uint16 handle;
	uint08 status; //0-success, others-failure
}btp_a2dpReconfigCompleteEvt_t;


int  btp_event_init(void);
void btp_event_regCB(uint16 evtID, btp_event_func func);
int  btp_send_event(uint16 evtID, uint08 *pData, uint16 dataLen);

int btp_send_requestEvt(uint08 ptype, uint16 aclHandle, uint16 chnID);
int btp_send_connectEvt(uint08 status, uint08 ptype, uint08 usrID, uint16 aclHandle, uint16 chnID);
int btp_send_disconnEvt(uint08 ptype, uint08 usrID, uint16 aclHandle);

int btp_send_serviceEvt(uint16 aclHandle, uint16 service, uint16 psmID);
int btp_send_channelEvt(uint16 aclHandle, uint16 service, uint08 channel);
int btp_send_sdpCompleteEvt(uint16 aclHandle);


int btp_send_hfphfCodecChangedEvt(uint16 aclHandle, uint08 codec);
int btp_send_hfpagCodecChangedEvt(uint16 aclHandle, uint08 codec);
int btp_send_hfphfVolumeChangedEvt(uint16 aclHandle, uint08 type, uint08 volume);
int btp_send_hfpagVolumeChangedEvt(uint16 aclHandle, uint08 type, uint08 volume);
int btp_send_hfphfStatusChangedEvt(uint16 aclHandle, uint08 status, uint08 callDir, uint08 *pNumber, uint08 numbLen);
int btp_send_hfpagStatusChangedEvt(uint16 aclHandle, uint08 status, uint08 callDir, uint08 *pNumber, uint08 numbLen);
int btp_send_hfphfNumberInquiryEvt(uint16 aclHandle, uint08 state, uint08 status, uint08 callDir, uint08 *pNumber, uint08 numbLen);


int btp_send_a2dpSrcCodecChangedEvt(uint16 aclHandle, uint08 chnMode, uint08 codecType, uint32 frequence, uint32 bitRate, uint08 objType);
int btp_send_a2dpSnkCodecChangedEvt(uint16 aclHandle, uint08 chnMode, uint08 codecType, uint32 frequence, uint32 bitRate, uint08 objType);
int btp_send_a2dpSrcStatusChangedEvt(uint16 aclHandle, uint08 status);
int btp_send_a2dpSnkStatusChangedEvt(uint16 aclHandle, uint08 status);




#endif //BTP_EVENT_H

