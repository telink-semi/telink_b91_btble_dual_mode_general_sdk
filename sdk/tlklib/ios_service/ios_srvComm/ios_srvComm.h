/********************************************************************************************************
 * @file     ios_srvComm.h
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     16.04.2021
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 ********************************************************************************************************/

#ifndef _IOS_SRVCOMM_H_
#define _IOS_SRVCOMM_H_
#include "tlkstk/ble/ble.h"
#include "tlkmdi/le/tlkmdi_le_common.h"

#define TLKMMI_LEMGR_IOS_FLAG       ((TLK_MAJOR_DBGID_MMI_LEMGR << 24) | (TLK_MINOR_DBGID_MMI_LEMGR << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMMI_LEMGR_IOS_SIGN       "[IOS]"



#define GET_START_HANDLE_TIMEOUT	1000 * 1000///1000ms���������޸�
#define BUILD_SERVICE_TIMEOUT		1000 * 1000///1000ms���������޸�

#ifndef ATT_ERR_ATTR_NOT_FOUND
#define ATT_ERR_ATTR_NOT_FOUND							0x0a
#endif

#ifndef ATT_DB_UUID128_NUM
#define	ATT_DB_UUID128_NUM				4
#endif
#define	ATT_DB_HDLIST_MAX_NUM			ATT_DB_UUID128_NUM

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 	handle[2];
	u8 	data[1];
}rf_pkt_notifyRsp_t;///�ײ�̶��ذ���ʽ,��Ҫ�޸�

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust
							//with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u16 startHandle;
	u16 endingHandle;
}rf_pkt_custom_find_info_req;

typedef struct{
	u32 dma_len;

	u8	type;
	u8	rf_len;
	u16 l2cap_len;

	u16 chan_id;
	u8	opcode;

	u16 start_handle;
	u16 ending_handle;

	u16 service;
	u8	uuid[16];
}rf_pkt_find_by_type_t;

typedef struct{
	u32 dma_len;
	u8	type;
	u8  rf_len;

	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8	startingHandle;
	u8	startingHandle1;
	u8	endingHandle;
	u8	endingHandle1;
	u8	attType[2];				//
}ios_srv_rf_packet_att_readByType_t;

typedef struct{
	u8	type;
	u8  rf_len;

	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 	errOpcode;
	u16 errHandle;
	u8  errReason;
}ios_srv_rf_packet_att_errRsp_t;

typedef struct{
	u8	type;
	u8  rf_len;

	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8  datalen;
	u8  data[1];			// character_handle / property / value_handle / value
}ios_srv_rf_packet_att_readByTypeRsp_t;
#endif
