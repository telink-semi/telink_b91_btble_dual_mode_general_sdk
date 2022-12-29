/********************************************************************************************************
 * @file     tlk_debug.h
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
#ifndef TLK_DEBUG_H
#define TLK_DEBUG_H



#define TLK_DEBUG_DBG_FLAG_WARN          0x02
#define TLK_DEBUG_DBG_FLAG_INFO          0x04
#define TLK_DEBUG_DBG_FLAG_TRACE         0x08
#define TLK_DEBUG_DBG_FLAG_ERROR         0x10
#define TLK_DEBUG_DBG_FLAG_FATAL         0x20
#define TLK_DEBUG_DBG_FLAG_ARRAY         0x40
#define TLK_DEBUG_DBG_FLAG_ASSERT        0x80
#define TLK_DEBUG_DBG_FLAG_ALL           0xFE


enum TLK_DEBUG_MAJOR_ID_ENUM{
	TLK_MAJOR_DBGID_NONE = 0,
	TLK_MAJOR_DBGID_SYS,
	TLK_MAJOR_DBGID_BTC,
	TLK_MAJOR_DBGID_BTH,
	TLK_MAJOR_DBGID_BTP,
	TLK_MAJOR_DBGID_LEC,
	TLK_MAJOR_DBGID_LEH,
	TLK_MAJOR_DBGID_LEP,
	TLK_MAJOR_DBGID_DRV,
	TLK_MAJOR_DBGID_DEV,
	TLK_MAJOR_DBGID_APP,
	TLK_MAJOR_DBGID_MDI_MISC,
	TLK_MAJOR_DBGID_MDI_AUDIO,
	TLK_MAJOR_DBGID_MDI_BT,
	TLK_MAJOR_DBGID_MDI_LE,
	TLK_MAJOR_DBGID_MMI_AUDIO,
	TLK_MAJOR_DBGID_MMI_BTMGR,
	TLK_MAJOR_DBGID_MMI_LEMGR,
	TLK_MAJOR_DBGID_MMI_PHONE,
	TLK_MAJOR_DBGID_MMI_FILE,
	//User Region Start
	//User Region End
	TLK_MAJOR_DBGID_MAX,
};
enum TLK_DEBUG_MINOR_ID_ENUM{
	TLK_MINOR_DBGID_NONE = 0,
	//TLK_MAJOR_DBGID_SYS
	TLK_MINOR_DBGID_SYS = 0,
	TLK_MINOR_DBGID_SYS_API,
	TLK_MINOR_DBGID_SYS_ALG,
	TLK_MINOR_DBGID_SYS_SCH,
	TLK_MINOR_DBGID_SYS_MAX,
	//TLK_MAJOR_DBGID_BTC
	TLK_MINOR_DBGID_BTC = 0,
	TLK_MINOR_DBGID_BTC_LMP,
	TLK_MINOR_DBGID_BTC_ACL,
	TLK_MINOR_DBGID_BTC_SCO,
	TLK_MINOR_DBGID_BTC_HCI,
	TLK_MINOR_DBGID_BTC_EVT,
	TLK_MINOR_DBGID_BTC_ENC,
	TLK_MINOR_DBGID_BTC_RSW,
	TLK_MINOR_DBGID_BTC_QOS,
	TLK_MINOR_DBGID_BTC_AFH,
	TLK_MINOR_DBGID_BTC_CHN,
	TLK_MINOR_DBGID_BTC_AUTH,
	TLK_MINOR_DBGID_BTC_PAIR,
	TLK_MINOR_DBGID_BTC_TASK,
	TLK_MINOR_DBGID_BTC_NAME,
	TLK_MINOR_DBGID_BTC_PAGE,
	TLK_MINOR_DBGID_BTC_SNIFF,
	TLK_MINOR_DBGID_BTC_TIMER,
	TLK_MINOR_DBGID_BTC_RESET,
	TLK_MINOR_DBGID_BTC_DETACH,
	TLK_MINOR_DBGID_BTC_MAX,
	//TLK_MAJOR_DBGID_BTH
	TLK_MINOR_DBGID_BTH = 0,
	TLK_MINOR_DBGID_BTH_ACL,
	TLK_MINOR_DBGID_BTH_SCO,
	TLK_MINOR_DBGID_BTH_DEV,
	TLK_MINOR_DBGID_BTH_CMD, //HCI-CMD
	TLK_MINOR_DBGID_BTH_EVT, //HCI-EVT
	TLK_MINOR_DBGID_BTH_L2C,
	TLK_MINOR_DBGID_BTH_SIG,
	TLK_MINOR_DBGID_BTH_MAX,
	//TLK_MAJOR_DBGID_BTP
	TLK_MINOR_DBGID_BTP = 0,
	TLK_MINOR_DBGID_BTP_SDP,
	TLK_MINOR_DBGID_BTP_SDPC,
	TLK_MINOR_DBGID_BTP_SDPS,
	TLK_MINOR_DBGID_BTP_RFCOMM,
	TLK_MINOR_DBGID_BTP_IAP,
	TLK_MINOR_DBGID_BTP_SPP,
	TLK_MINOR_DBGID_BTP_ATT,
	TLK_MINOR_DBGID_BTP_A2DP,
	TLK_MINOR_DBGID_BTP_A2DPS,
	TLK_MINOR_DBGID_BTP_A2DPC,
	TLK_MINOR_DBGID_BTP_HID,
	TLK_MINOR_DBGID_BTP_HIDS,
	TLK_MINOR_DBGID_BTP_HIDC,
	TLK_MINOR_DBGID_BTP_HFP,
	TLK_MINOR_DBGID_BTP_HFPAG,
	TLK_MINOR_DBGID_BTP_HFPHF,
	TLK_MINOR_DBGID_BTP_AVRCP,
	TLK_MINOR_DBGID_BTP_OBEX,
	TLK_MINOR_DBGID_BTP_PBAP,
	TLK_MINOR_DBGID_BTP_MAX,
	//TLK_MAJOR_DBGID_LEC
	//TLK_MAJOR_DBGID_LEH
	//TLK_MAJOR_DBGID_LEP
	//TLK_MAJOR_DBGID_DRV
	TLK_MINOR_DBGID_DRV = 0,
	TLK_MINOR_DBGID_DRV_SYS,
	TLK_MINOR_DBGID_DRV_EXT,
	TLK_MINOR_DBGID_DRV_USR,
	TLK_MINOR_DBGID_DRV_MAX,
	//TLK_MAJOR_DBGID_DEV
	TLK_MINOR_DBGID_DEV = 0,
	TLK_MINOR_DBGID_DEV_SYS,
	TLK_MINOR_DBGID_DEV_EXT,
	TLK_MINOR_DBGID_DEV_USR,
	TLK_MINOR_DBGID_DEV_MAX,
	//TLK_MAJOR_DBGID_APP
	TLK_MINOR_DBGID_APP = 0,
	TLK_MINOR_DBGID_APP_MAX,
	//TLK_MAJOR_DBGID_MDI_MISC
	TLK_MINOR_DBGID_MDI_MISC = 0,
	TLK_MINOR_DBGID_MDI_COMM,
	TLK_MINOR_DBGID_MDI_FILE,
	TLK_MINOR_DBGID_MDI_FS,
	TLK_MINOR_DBGID_MDI_EVT,
	TLK_MINOR_DBGID_MDI_KEY,
	TLK_MINOR_DBGID_MDI_LED,
	TLK_MINOR_DBGID_MDI_USB,
	TLK_MINOR_DBGID_MDI_MAX,
	//TLK_MAJOR_DBGID_MDI_AUDIO
	TLK_MINOR_DBGID_MDI_AUD = 0,
	TLK_MINOR_DBGID_MDI_AUD_PLAY,
	TLK_MINOR_DBGID_MDI_AUD_TONE,
	TLK_MINOR_DBGID_MDI_AUD_MP3,
	TLK_MINOR_DBGID_MDI_AUD_HFP,
	TLK_MINOR_DBGID_MDI_AUD_SCO,
	TLK_MINOR_DBGID_MDI_AUD_SNK,
	TLK_MINOR_DBGID_MDI_AUD_SRC,
	TLK_MINOR_DBGID_MDI_AUD_UAC,
	TLK_MINOR_DBGID_MDI_AUD_MAX,
	//TLK_MAJOR_DBGID_MDI_BT
	TLK_MINOR_DBGID_MDI_BT = 0,
	TLK_MINOR_DBGID_MDI_BT_ACL,
	TLK_MINOR_DBGID_MDI_BT_ATT,
	TLK_MINOR_DBGID_MDI_BT_HFP,
	TLK_MINOR_DBGID_MDI_BT_HID,
	TLK_MINOR_DBGID_MDI_BT_INQ,
	TLK_MINOR_DBGID_MDI_BT_REC,
	TLK_MINOR_DBGID_MDI_BT_MAX,
	//TLK_MAJOR_DBGID_MDI_LE
	//TLK_MAJOR_DBGID_MMI_AUDIO
	TLK_MINOR_DBGID_MMI_AUDIO = 0,
	TLK_MINOR_DBGID_MMI_AUDIO_MAX,
	//TLK_MAJOR_DBGID_MMI_BTMGR
	TLK_MINOR_DBGID_MMI_BTMGR = 0,
	TLK_MINOR_DBGID_MMI_BTMGR_MAX,
	//TLK_MAJOR_DBGID_MMI_LEMGR
	TLK_MINOR_DBGID_MMI_LEMGR = 0,
	TLK_MINOR_DBGID_MMI_LEMGR_MAX,
	//TLK_MAJOR_DBGID_MMI_PHONE
	TLK_MINOR_DBGID_MMI_PHONE = 0,
	TLK_MINOR_DBGID_MMI_PHONE_MAX,
	//TLK_MAJOR_DBGID_MMI_FILE
	TLK_MINOR_DBGID_MMI_FILE = 0,
	TLK_MINOR_DBGID_MMI_FILE_MAX,
};




#endif //TLK_DEBUG_H
