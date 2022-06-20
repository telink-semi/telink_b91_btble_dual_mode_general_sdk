/********************************************************************************************************
 * @file     btp_sdpConst.h
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

#ifndef BTP_SDP_CONST_H
#define BTP_SDP_CONST_H





#define BTP_SDP_AIRPODS_ENABLE  0


#define BTP_SDP_SPP_HANDLE                0x00100001
#define BTP_SDP_OPP_HANDLE                0x00100002
#define BTP_SDP_FTP_HANDLE                0x00100003
#define BTP_SDP_IAP_HANDLE                0x00100014

#define BTP_SDP_HFP_HF_HANDLE             0x00100010
#define BTP_SDP_HFP_AG_HANDLE             0x00100011
#define BTP_SDP_PNP_INFO_HANDLE           0x00100012
#define BTP_SDP_AVRCP_CT_HANDLE           0x00100015
#define BTP_SDP_AVRCP_TG_HANDLE           0x00100016
#define BTP_SDP_A2DP_SRC_HANDLE           0x00100017
#define BTP_SDP_A2DP_SNK_HANDLE           0x00100018

/*
  * macro used in sdp database
  */
#define BTP_SDP_HEADSET_VERSION         0x0100
#define BTP_SDP_HFP_VERSION             0x0105
#define BTP_SDP_OPP_VERSION             0x0100
#define BTP_SDP_FTP_VERSION             0x0100
#define BTP_SDP_DUN_VERSION             0x0100
#define BTP_SDP_A2DP_SNK_VERSION        0x0102
#define BTP_SDP_A2DP_SRC_VERSION        0x0102
#define BTP_SDP_RC_CONTROL_VERSION      0x0103
#define BTP_SDP_AVRCP_TG_VERSION        0x0103
#define BTP_SDP_PNP_VERSION             0x0100


/*
  **************************************************************
  *	Constant and Macro defination
  **************************************************************
  */

#define SDP_RSP_ARR_LEN	750
#define PTS_TEST_FOR_CONTINUE      0

#if PTS_TEST_FOR_CONTINUE
#define SDP_MAX_ATTR_BYTES_CNT  100
#define SDP_MAX_RES_SERVICE_RECODS_NUM  2
#define SDP_CLIENT_MAX_ATT_DATA_RETURN  100
#else
#define SDP_MAX_ATTR_BYTES_CNT  650
#define SDP_MAX_RES_SERVICE_RECODS_NUM  48
#define SDP_CLIENT_MAX_ATT_DATA_RETURN  256
#endif

#define SDP_CLIENT_MAX_RX_CNT  700




/*
 * The Data representation in SDP PDUs (pps 339, 340 of BT SDP Spec)
 * These are the exact data type+size descriptor values
 * that go into the PDU buffer.
 *
 * The datatype (leading 5bits) + size descriptor (last 3 bits)
 * is 8 bits. The size descriptor is critical to extract the
 * right number of bytes for the data value from the PDU.
 *
 * For most basic types, the datatype+size descriptor is
 * straightforward. However for constructed types and strings,
 * the size of the data is in the next "n" bytes following the
 * 8 bits (datatype+size) descriptor. Exactly what the "n" is
 * specified in the 3 bits of the data size descriptor.
 *
 * TextString and URLString can be of size 2^{8, 16, 32} bytes
 * DataSequence and DataSequenceAlternates can be of size 2^{8, 16, 32}
 * The size are computed post-facto in the API and are not known apriori
 */
#define SDP_DATA_NIL 	0x00
#define SDP_UINT8  		0x08
#define SDP_UINT16		0x09
#define SDP_UINT32		0x0A
#define SDP_UINT64		0x0B
#define SDP_UINT128		0x0C
#define SDP_INT8		0x10
#define SDP_INT16		0x11
#define SDP_INT32		0x12
#define SDP_INT64		0x13
#define SDP_INT128		0x14
#define SDP_UUID_UNSPEC		0x18
#define SDP_UUID16		0x19
#define SDP_UUID32		0x1A
#define SDP_UUID128		0x1C
#define SDP_TEXT_STR_UNSPEC	0x20
#define SDP_TEXT_STR8		0x25
#define SDP_TEXT_STR16		0x26
#define SDP_TEXT_STR32		0x27
#define SDP_BOOL		0x28
#define SDP_SEQ_UNSPEC		0x30
#define SDP_SEQ8		0x35
#define SDP_SEQ16		0x36
#define SDP_SEQ32		0x37
#define SDP_ALT_UNSPEC		0x38
#define SDP_ALT8		0x3D
#define SDP_ALT16		0x3E
#define SDP_ALT32		0x3F
#define SDP_URL_STR_UNSPEC	0x40
#define SDP_URL_STR8		0x45
#define SDP_URL_STR16		0x46
#define SDP_URL_STR32		0x47






typedef struct{
	uint32 flag;
	uint16 uuid;
	uint08 dtype; //Refer to BTP_SDP_DTYPE_ENUM
	uint08 dsize; //Refer to BTP_SDP_DSIZE_ENUM
	uint32 value;
	uint08 *pValue;
}btp_sdp_serviceItem_t;

typedef struct{
	uint32 handle;
	uint32 count;
	btp_sdp_serviceItem_t *pItem;
}btp_sdp_serviceList_t;


extern const btp_sdp_serviceList_t gcBthSdpServiceList[];


#endif //BTP_SDP_CONST_H

