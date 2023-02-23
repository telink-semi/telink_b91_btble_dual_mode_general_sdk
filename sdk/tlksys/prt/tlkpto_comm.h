/********************************************************************************************************
 * @file     tlkpto_comm.h
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

#ifndef TLKPTO_COMM_H
#define TLKPTO_COMM_H



/******************************************************************************
 *
 * Frame Struct:
 *-----------------------------------------------------------------------------
 * HeadSign(WORD)+Attr(WORD<Lens[0~11],PType[12~15]>)+Data(0~n)+CRC16(WORD)+TailSign(WORD)
 * FrmLen = length(Attr)+length(Data)+length(CRC16)
 *=============================================================================
 * 
 * CMD/EVT Packet Struct
 *-----------------------------------------------------------------------------
 * Mtype(Byte)+MsgID(Byte)+Numb(Byte)+Lens(Byte)+Body(0~255)
 *-----------------------------------------------------------------------------
 * RSP Body: Status(Byte)+Reason(Byte)+Body(0~255)
 *=============================================================================
 * 
 * Data Packet Struct
 *-----------------------------------------------------------------------------
 * Num(WORD)+DID(Byte)+Lens(Byte)+Body(1~256)
 * Lens = length(Body)-1
 *=============================================================================
 * 
 * 
 ******************************************************************************/


#define TLKPRT_COMM_FRM_EXTLEN         8
#define TLKPRT_COMM_CMDHEAD_LENS       4
#define TLKPRT_COMM_DATHEAD_LENS       4
#define TLKPRT_COMM_CMDBODY_MAXLEN     120
#define TLKPRT_COMM_DATBODY_MAXLEN     256

#define TLKPRT_COMM_FRM_DATA_MAXLEN    (TLKPRT_COMM_DATHEAD_LENS+TLKPRT_COMM_DATBODY_MAXLEN)
#define TLKPRT_COMM_FRM_MAXLEN         (TLKPRT_COMM_FRM_EXTLEN+TLKPRT_COMM_FRM_DATA_MAXLEN)


#define TLKPRT_COMM_HEAD_SIGN0         0x7F
#define TLKPRT_COMM_HEAD_SIGN1         0x7F
#define TLKPRT_COMM_TAIL_SIGN0         0x80
#define TLKPRT_COMM_TAIL_SIGN1         0x80


typedef enum{
	TLKPRT_COMM_STATE_NONE = 0,
	TLKPRT_COMM_STATE_START,
	TLKPRT_COMM_STATE_CLOSE,
	TLKPRT_COMM_STATE_PAUSE,
}TLKPRT_COMM_STATE_ENUM;

typedef enum{
	TLKPRT_COMM_RSP_STATUE_SUCCESS = 0,
	TLKPRT_COMM_RSP_STATUE_FAILURE,
	TLKPRT_COMM_RSP_STATUE_COMPLETE, //Completed, but no results yet.
}TLKPRT_COMM_RSP_STATUE_ENUM;
typedef enum{
	TLKPRT_COMM_RSP_REASON_NONE = 0,
	TLKPRT_COMM_RSP_REASON_PARAM, //Paramete Error
	TLKPRT_COMM_RSP_REASON_STATUS, //Status Error or Not Allowed
	TLKPRT_COMM_RSP_REASON_ACCESS, //Access Error or Access Denied
	TLKPRT_COMM_RSP_REASON_TIMEOUT,
}TLKPRT_COMM_RSP_REASON_ENUM;

typedef enum{
	TLKPRT_COMM_CALL_ROLE_CLIENT = 0,
	TLKPRT_COMM_CALL_ROLE_SERVER,
}TLKPRT_COMM_HFP_ROLE_ENUM;

#define TLKPRT_COMM_SYS_DAT_PORT     0x00
typedef enum{
	TLKPRT_COMM_SYS_DAT_NONE = 0x00,
	TLKPRT_COMM_SYS_DAT_LOG  = 0x01,
	TLKPRT_COMM_SYS_DAT_VCD  = 0x02,
}TLKPRT_COMM_SYS_DAT_ENUM;

typedef enum{
	TLKPRT_COMM_BT_CHN_ACL = 0x00,
	TLKPRT_COMM_BT_CHN_SCO = 0x01,
	TLKPRT_COMM_BT_CHN_SPP = 0x10,
	TLKPRT_COMM_BT_CHN_HFP_HF = 0x11,
	TLKPRT_COMM_BT_CHN_HFP_AG = 0x12,
	TLKPRT_COMM_BT_CHN_A2DP_SNK = 0x13,
	TLKPRT_COMM_BT_CHN_A2DP_SRC = 0x14,
	TLKPRT_COMM_BT_CHN_AVRCP = 0x15,
	TLKPRT_COMM_BT_CHN_MAP = 0x16,
	TLKPRT_COMM_BT_CHN_PBAP = 0x17,
	TLKPRT_COMM_BT_CHN_ATT = 0x21,
	TLKPRT_COMM_BT_CHN_HID = 0x22,
	TLKPRT_COMM_BT_CHN_IAP = 0x23,
}TLKPRT_COMM_BT_CHN_ENUM;
typedef enum{
	TLKPRT_COMM_AUDIO_CHN_NONE = 0x00,
	TLKPRT_COMM_AUDIO_CHN_PLAY = 0x01, //--Play;
	TLKPRT_COMM_AUDIO_CHN_TONE = 0x02, //--Tone;
	TLKPRT_COMM_AUDIO_CHN_SCO  = 0x06, //--SCO;
	TLKPRT_COMM_AUDIO_CHN_HFP_HF = 0x07, //--HFP(HF);
	TLKPRT_COMM_AUDIO_CHN_HFP_AG = 0x08, //--HFP(AG);
	TLKPRT_COMM_AUDIO_CHN_A2DP_SRC = 0x09, //--A2DP(SRC);
	TLKPRT_COMM_AUDIO_CHN_A2DP_SNK = 0x0A, //--A2DP(SNK);
	TLKPRT_COMM_AUDIO_CHN_UAC = 0x0B, //--A2DP(UAC);
}TLKPRT_COMM_AUDIO_CHN_ENUM;
typedef enum{
	TLKPRT_COMM_VOLUME_TYPE_AUTO  = 0x00,
	TLKPRT_COMM_VOLUME_TYPE_MUSIC = 0x01,
	TLKPRT_COMM_VOLUME_TYPE_VOICE = 0x02,
	TLKPRT_COMM_VOLUME_TYPE_HEADSET = 0x03,
	TLKPRT_COMM_VOLUME_TYPE_TONE  = 0x04,
}TLKPRT_COMM_VOLUME_TYPE_ENUM;


//Packet Type
typedef enum{
	TLKPRT_COMM_PTYPE_CMD = 0,
	TLKPRT_COMM_PTYPE_RSP,
	TLKPRT_COMM_PTYPE_EVT,
	TLKPRT_COMM_PTYPE_DAT,
}TLKPRT_COMM_PTYPE_ENUM;
//Moudle Type
typedef enum{
	TLKPRT_COMM_MTYPE_NONE = 0,
	TLKPRT_COMM_MTYPE_SYS,
	TLKPRT_COMM_MTYPE_BT,
	TLKPRT_COMM_MTYPE_LE,
	TLKPRT_COMM_MTYPE_DBG,
	TLKPRT_COMM_MTYPE_FILE,
	TLKPRT_COMM_MTYPE_CALL,
	TLKPRT_COMM_MTYPE_AUDIO,
	TLKPRT_COMM_MTYPE_TEST,
	TLKPRT_COMM_MTYPE_VIEW,
	TLKPRT_COMM_MTYPE_MAX,
}TLKPRT_COMM_MTYPE_ENUM;
//MsgID
typedef enum{
	//TLKPRT_COMM_MTYPE_SYS
	TLKPRT_COMM_CMDID_SYS_NONE = 0,
	TLKPRT_COMM_CMDID_SYS_VERSION = 0x01,
	TLKPRT_COMM_CMDID_SYS_REBOOT,
	TLKPRT_COMM_CMDID_SYS_POWER_OFF,
	TLKPRT_COMM_CMDID_SYS_SET_HEART = 0x05,
	TLKPRT_COMM_CMDID_SYS_DBG_LOAD,
	TLKPRT_COMM_CMDID_SYS_DBG_MAJOR_UPDATE,
    TLKPRT_COMM_CMDID_SYS_DBG_MINOR_UPDATE,
	//TLKPRT_COMM_MTYPE_BT
	TLKPRT_COMM_CMDID_BT_CONNECT = 0x01, //ACL,SCO
	TLKPRT_COMM_CMDID_BT_DISCONN = 0x02,
	TLKPRT_COMM_CMDID_BT_CONN_PROF = 0x03, //SPP,HFP(HF/AG),MAP,PBAP,A2DP
	TLKPRT_COMM_CMDID_BT_DISC_PROF = 0x04,
	TLKPRT_COMM_CMDID_BT_INQUIRY = 0x05,
	TLKPRT_COMM_CMDID_BT_INQ_CANCEL = 0x06,
	TLKPRT_COMM_CMDID_BT_GET_NAME = 0x11,
	TLKPRT_COMM_CMDID_BT_GET_ADDR,
	TLKPRT_COMM_CMDID_BT_SET_NAME,
	TLKPRT_COMM_CMDID_BT_SET_ADDR,
	TLKPRT_COMM_CMDID_BT_VOL_INC,
	TLKPRT_COMM_CMDID_BT_VOL_DEC,
	TLKPRT_COMM_CMDID_BT_GET_PDL = 0x21, //PDL -- Peer Device List
	TLKPRT_COMM_CMDID_BT_DEL_PDL,
	TLKPRT_COMM_CMDID_BT_CLS_PDL,
	TLKPRT_COMM_CMDID_BT_GET_CDL, //CDL -- Connect Device List
	TLKPRT_COMM_CMDID_BT_START_PAIR = 0x31,
	TLKPRT_COMM_CMDID_BT_CLOSE_PAIR,
	//TLKPRT_COMM_MTYPE_LE
	TLKPRT_COMM_CMDID_LE_CONNECT = 0x01,
	TLKPRT_COMM_CMDID_LE_DISCONN = 0x02,
	TLKPRT_COMM_CMDID_LE_START_ADV = 0x06,
	TLKPRT_COMM_CMDID_LE_CLOSE_ADV = 0x07,
	TLKPRT_COMM_CMDID_LE_START_SCAN = 0x08,
	TLKPRT_COMM_CMDID_LE_CLOSE_SCAN = 0x09,
	TLKPRT_COMM_CMDID_LE_GET_NAME = 0x11,
	TLKPRT_COMM_CMDID_LE_GET_ADDR,
	TLKPRT_COMM_CMDID_LE_SET_NAME,
	TLKPRT_COMM_CMDID_LE_SET_ADDR,
	TLKPRT_COMM_CMDID_LE_VOL_INC,
	TLKPRT_COMM_CMDID_LE_VOL_DEC, 
	TLKPRT_COMM_CMDID_LE_GET_PDL = 0x21, //PDL -- Peer Device List
	TLKPRT_COMM_CMDID_LE_DEL_PDL,
	TLKPRT_COMM_CMDID_LE_CLS_PDL,
	TLKPRT_COMM_CMDID_LE_GET_CDL, //CDL -- Connect Device List
	

	//TLKPRT_COMM_MTYPE_DBG
	TLKPRT_COMM_CMDID_DBG_NONE = 0,
	TLKPRT_COMM_CMDID_DBG_START_TONE,
	TLKPRT_COMM_CMDID_DBG_GET_PHONE_BOOK,
	TLKPRT_COMM_CMDID_DBG_SET_USB_MODE,
	TLKPRT_COMM_CMDID_DBG_SIMULATE_KEY,
	TLKPRT_COMM_CMDID_DBG_SEND_SHELL,
    TLKPRT_COMM_CMDID_DBG_SEND_HEX,

	//TLKPRT_COMM_MTYPE_FILE
	TLKPRT_COMM_CMDID_FILE_NONE = 0,
	TLKPRT_COMM_CMDID_FILE_START_TRAN    = 0x01,
	TLKPRT_COMM_CMDID_FILE_START_AUTH    = 0x02,
	TLKPRT_COMM_CMDID_FILE_CRYPT_SHAKE   = 0x03,
	TLKPRT_COMM_CMDID_FILE_SET_DATA_SCH  = 0x06,
	TLKPRT_COMM_CMDID_FILE_SET_FILE_NAME = 0x07,
	TLKPRT_COMM_CMDID_FILE_FAST_START    = 0x10,
	TLKPRT_COMM_CMDID_FILE_START_DATA    = 0x11,
	TLKPRT_COMM_CMDID_FILE_CLOSE_TRAN    = 0x12,
	TLKPRT_COMM_CMDID_FILE_GET_VERSION   = 0x20,
	
	//TLKPRT_COMM_MTYPE_CALL
	TLKPRT_COMM_CMDID_CALL_NONE = 0,
	TLKPRT_COMM_CMDID_CALL_DIAL, //
	TLKPRT_COMM_CMDID_CALL_HOLD,
	TLKPRT_COMM_CMDID_CALL_REDIAL,
	TLKPRT_COMM_CMDID_CALL_ACCEPT,
	TLKPRT_COMM_CMDID_CALL_REJECT,
	TLKPRT_COMM_CMDID_CALL_HUNGUP,
	//TLKPRT_COMM_MTYPE_AUDIO
	TLKPRT_COMM_CMDID_AUDIO_NONE = 0x00,
	TLKPRT_COMM_CMDID_AUDIO_PLAY_START = 0x01,
	TLKPRT_COMM_CMDID_AUDIO_PLAY_CLOSE = 0x02,
	TLKPRT_COMM_CMDID_AUDIO_PLAY_NEXT = 0x05,
	TLKPRT_COMM_CMDID_AUDIO_PLAY_PREV = 0x06,
	TLKPRT_COMM_CMDID_AUDIO_GET_STATE = 0x11,
	TLKPRT_COMM_CMDID_AUDIO_GET_PROGRESS,
	TLKPRT_COMM_CMDID_AUDIO_GET_DURATION,
	TLKPRT_COMM_CMDID_AUDIO_GET_FILENAME,
	TLKPRT_COMM_CMDID_AUDIO_GET_SINGER,
	TLKPRT_COMM_CMDID_AUDIO_GET_PLAY_MODE = 0x21,
	TLKPRT_COMM_CMDID_AUDIO_SET_PLAY_MODE,
	TLKPRT_COMM_CMDID_AUDIO_GET_VOLUME,
	TLKPRT_COMM_CMDID_AUDIO_SET_VOLUME,
	TLKPRT_COMM_CMDID_AUDIO_INC_VOLUME,
	TLKPRT_COMM_CMDID_AUDIO_DEC_VOLUME,
	TLKPRT_COMM_CMDID_AUDIO_START_REPORT,
	TLKPRT_COMM_CMDID_AUDIO_CLOSE_REPORT,	

}TLKPRT_COMM_CMDID_ENUM;
typedef enum{
	//TLKPRT_COMM_MTYPE_SYS
	TLKPRT_COMM_EVTID_SYS_NONE       = 0,
	TLKPRT_COMM_EVTID_SYS_READY      = 0x01,
	TLKPRT_COMM_EVTID_SYS_HEART_BEAT = 0x02,
	TLKPRT_COMM_EVTID_SYS_BATTERY    = 0x03,
	TLKPRT_COMM_EVTID_SYS_DBG_LOAD,

	//TLKPRT_COMM_MTYPE_BT
	TLKPRT_COMM_EVTID_BT_CONNECT = 0x01, //ACL,SCO
	TLKPRT_COMM_EVTID_BT_DISCONN = 0x02,
	TLKPRT_COMM_EVTID_BT_PROF_CONN = 0x03, //SPP,HFP(HF/AG),MAP,PBAP,A2DP(TG/AG)
	TLKPRT_COMM_EVTID_BT_PROF_DISC = 0x04,
	TLKPRT_COMM_EVTID_BT_INQUIRY = 0x05,
	TLKPRT_COMM_EVTID_BT_INQUIRY_COMPLETE = 0x06,

	//TLKPRT_COMM_MTYPE_LE
	TLKPRT_COMM_EVTID_LE_CONNECT = 0x01,
	TLKPRT_COMM_EVTID_LE_DISCONN = 0x02,
	TLKPRT_COMM_EVTID_LE_ADV_START = 0x05,
	TLKPRT_COMM_EVTID_LE_ADV_COMPLETE = 0x06,
	TLKPRT_COMM_EVTID_LE_SCAN_START = 0x07,
	TLKPRT_COMM_EVTID_LE_SCAN_COMPLETE = 0x08,
	
	//TLKPRT_COMM_MTYPE_AUDIO
	TLKPRT_COMM_EVTID_AUDIO_STATUS_CHANGE = 0x01,
	TLKPRT_COMM_EVTID_AUDIO_SONG_CHANGE,
	TLKPRT_COMM_EVTID_AUDIO_VOLUME_CHANGE,
	TLKPRT_COMM_EVTID_AUDIO_PROGRESS_REPORT,
	
	//TLKPRT_COMM_MTYPE_FILE
	TLKPRT_COMM_EVTID_FILE_CLOSE = 0x01,
	TLKPRT_COMM_EVTID_FILE_SHAKE = 0x02,
	TLKPRT_COMM_EVTID_FILE_SYNC  = 0x03,
	
	//TLKPRT_COMM_MTYPE_CALL
	TLKPRT_COMM_EVTID_CALL_NONE = 0,
	TLKPRT_COMM_EVTID_CALL_CLOSE,
	TLKPRT_COMM_EVTID_CALL_START,
	TLKPRT_COMM_EVTID_CALL_ALART,
	TLKPRT_COMM_EVTID_CALL_ACTIVE,
	TLKPRT_COMM_EVTID_CALL_HELD,
	TLKPRT_COMM_EVTID_CALL_WAIT,
	TLKPRT_COMM_EVTID_CALL_RESUME,
}TLKPRT_COMM_EVTID_ENUM;


typedef struct{
	uint08 mtype; //Refer: TLKPRT_COMM_MTYPE_ENUM
	uint08 msgID;
	uint08 numb;
	uint08 lens;
	uint08 *pBody;
}tlkprot_comm_cmd_t;
typedef struct{
	uint08 did;
	uint08 lens;
	uint16 numb;
	uint08 *pBody;
}tlkprot_comm_dat_t;





#endif //TLKPTO_COMM_H

