/********************************************************************************************************
 * @file     btp_attStdio.c
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
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_l2cap.h"
#include "tlkstk/bt/bth/bth_signal.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#if (TLKBTP_CFG_ATT_ENABLE)
#include "tlkstk/bt/btp/btp_adapt.h"
#include "tlkstk/bt/btp/att/btp_att.h"
#include "tlkstk/bt/btp/att/btp_attInner.h"
#include "tlkstk/bt/btp/att/btp_attSrv.h"
#include "tlkstk/bt/btp/att/btp_attClt.h"
#include "tlkstk/bt/btp/att/btp_attStdio.h"



//3.4.1 Error handling
int btp_att_sendErrorRsp(uint16 handle, uint16 chnID, uint08 errOpcode, uint16 errHandle, uint08 errReason)
{
	uint08 buffLen;
	uint08 buffer[5];

	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_ERROR_RSP;
	buffer[buffLen++] = errOpcode;
	buffer[buffLen++] = errHandle & 0xFF;
	buffer[buffLen++] = (errHandle & 0xFF00) >> 8;
	buffer[buffLen++] = errReason;
	return bth_l2cap_sendChannelData(handle, chnID, nullptr, 0, buffer, buffLen);
}

//3.4.2 MTU exchange
int btp_att_sendExchangeMtuReq(uint16 handle, uint16 chnID, uint16 mtuSize)
{
	uint08 buffLen;
	uint08 buffer[5];

	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_EXCHG_MTU_REQ;
	buffer[buffLen++] = mtuSize & 0xFF;
	buffer[buffLen++] = (mtuSize & 0xFF00) >> 8;
	return bth_l2cap_sendChannelData(handle, chnID, nullptr, 0, buffer, buffLen);
}
int btp_att_sendExchangeMtuRsp(uint16 handle, uint16 chnID, uint16 mtuSize)
{
	uint08 buffLen;
	uint08 buffer[5];
	
	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_EXCHG_MTU_RSP;
	buffer[buffLen++] = mtuSize & 0xFF;
	buffer[buffLen++] = (mtuSize & 0xFF00) >> 8;
	return bth_l2cap_sendChannelData(handle, chnID, nullptr, 0, buffer, buffLen);
}

//3.4.3 Find information
int btp_att_sendFindInfoReq(uint16 handle, uint16 chnID, uint16 startAttHandle, uint16 endAttHandle)
{
	uint08 buffLen;
	uint08 buffer[20];
	
	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_FIND_INFO_REQ;
	buffer[buffLen++] = startAttHandle & 0xFF;
	buffer[buffLen++] = (startAttHandle & 0xFF00) >> 8;
	buffer[buffLen++] = endAttHandle & 0xFF;
	buffer[buffLen++] = (endAttHandle & 0xFF00) >> 8;
	return bth_l2cap_sendChannelData(handle, chnID, nullptr, 0, buffer, buffLen);
}
int btp_att_sendFindInfoRsp(uint16 handle, uint16 chnID, uint08 format, uint08 *pData, uint16 dataLen)
{
	uint08 buffLen;
	uint08 buffer[2];

	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_FIND_INFO_RSP;
	buffer[buffLen++] = format;
	tlkapi_array(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_sendFindInfoRsp 0:", buffer, buffLen);
	tlkapi_array(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_sendFindInfoRsp 1:", pData, dataLen);
	return bth_l2cap_sendChannelData(handle, chnID, buffer, buffLen, pData, dataLen);
}
int btp_att_sendFindTypeReq(uint16 handle, uint16 chnID, uint16 startAttHandle, uint16 endAttHandle, 
	uint08 *pUuid, uint08* pData, int valueLen)
{
	uint08 buffLen;
	uint08 buffer[20];
		
	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_FIND_TYPE_REQ;
	buffer[buffLen++] = startAttHandle & 0xFF;
	buffer[buffLen++] = (startAttHandle & 0xFF00) >> 8;
	buffer[buffLen++] = endAttHandle & 0xFF;
	buffer[buffLen++] = (endAttHandle & 0xFF00) >> 8;
	tmemcpy(&buffer[buffLen], pUuid, 2);
	buffLen += 2;
	return bth_l2cap_sendChannelData(handle, chnID, buffer, buffLen, pData, valueLen);
}
int btp_att_sendFindTypeRsp(uint16 handle, uint16 chnID, uint08 *pData, uint16 dataLen)
{
	uint08 buffLen;
	uint08 buffer[2];
	
	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_FIND_TYPE_RSP;
	tlkapi_array(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_sendFindTypeRsp 0:", buffer, buffLen);
	tlkapi_array(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_sendFindTypeRsp 1:", pData, dataLen);
	return bth_l2cap_sendChannelData(handle, chnID, buffer, buffLen, pData, dataLen);
}
int btp_att_sendReadTypeReq(uint16 handle, uint16 chnID, uint16 startAttHandle, uint16 endAttHandle, uint08 *uuid, int uuidLen)
{
	uint08 buffLen;
	uint08 buffer[32];
	
	if(uuidLen != 0 && uuidLen > 16) return -TLK_EPARAM;
	
	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_READ_TYPE_REQ;
	buffer[buffLen++] = startAttHandle & 0xFF;
	buffer[buffLen++] = (startAttHandle & 0xFF00) >> 8;
	buffer[buffLen++] = endAttHandle & 0xFF;
	buffer[buffLen++] = (endAttHandle & 0xFF00) >> 8;
	tmemcpy(&buffer[buffLen], uuid, uuidLen);
	buffLen += uuidLen;
	return bth_l2cap_sendChannelData(handle, chnID, nullptr, 0, buffer, buffLen);
}
int btp_att_sendReadTypeRsp(uint16 handle, uint16 chnID, uint08 typeLen, uint08 *pData, uint08 dataLen)
{
	uint08 buffLen;
	uint08 buffer[3];
	
	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_READ_TYPE_RSP;
	buffer[buffLen++] = typeLen;
	tlkapi_array(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_sendReadTypeRsp 0:", buffer, buffLen);
	tlkapi_array(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_sendReadTypeRsp 1:", pData, dataLen);
	return bth_l2cap_sendChannelData(handle, chnID, buffer, buffLen, pData, dataLen);
}
int btp_att_sendReadGroupReq(uint16 handle, uint16 chnID, uint16 startAttHandle, uint16 endAttHandle, uint08 *uuid, int uuidLen)
{
	uint08 buffLen;
	uint08 buffer[20];
	
	if(uuidLen != 0 && uuidLen < 16) return -TLK_EPARAM;
	
	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_READ_GROUP_REQ;
	buffer[buffLen++] = startAttHandle & 0xFF;
	buffer[buffLen++] = (startAttHandle & 0xFF00) >> 8;
	buffer[buffLen++] = endAttHandle & 0xFF;
	buffer[buffLen++] = (endAttHandle & 0xFF00) >> 8;
	tmemcpy(&buffer[buffLen], uuid, uuidLen);
	buffLen += uuidLen;
	return bth_l2cap_sendChannelData(handle, chnID, nullptr, 0, buffer, buffLen);
}
int btp_att_sendReadGroupRsp(uint16 handle, uint16 chnID, uint08 typeLen, uint08 *pData, uint16 dataLen)
{
	uint08 buffLen;
	uint08 buffer[3];
	
	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_READ_GROUP_RSP;
	buffer[buffLen++] = typeLen;
	tlkapi_array(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_sendReadGroupRsp 0:", buffer, buffLen);
	tlkapi_array(BTP_ATT_DBG_FLAG, BTP_ATT_DBG_SIGN, "btp_att_sendReadGroupRsp 1:", pData, dataLen);
	return bth_l2cap_sendChannelData(handle, chnID, buffer, buffLen, pData, dataLen);
}

int btp_att_sendReadReq(uint16 handle, uint16 chnID, uint16 attHandle)
{
	uint08 buffLen;
	uint08 buffer[5];
		
	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_READ_REQ;
	buffer[buffLen++] = attHandle & 0xFF;
	buffer[buffLen++] = (attHandle & 0xFF00) >> 8;
	return bth_l2cap_sendChannelData(handle, chnID, nullptr, 0, buffer, buffLen);
}
int btp_att_sendReadRsp(uint16 handle, uint16 chnID, uint08 *pData, uint08 dataLen)
{
	uint08 buffLen;
	uint08 buffer[2];

	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_READ_RSP;
	return bth_l2cap_sendChannelData(handle, chnID, buffer, buffLen, pData, dataLen);
}
int btp_att_sendReadBlobReq(uint16 handle, uint16 chnID, uint16 attHandle, uint16 offset)
{
	uint08 buffLen;
	uint08 buffer[5];

	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_READ_BLOB_RSP;
	buffer[buffLen++] = attHandle & 0xFF;
	buffer[buffLen++] = (attHandle & 0xFF00) >> 8;
	buffer[buffLen++] = offset & 0xFF;
	buffer[buffLen++] = (offset & 0xFF00) >> 8;
	return bth_l2cap_sendChannelData(handle, chnID, nullptr, 0, buffer, buffLen);
}
int btp_att_sendReadBlobRsp(uint16 handle, uint16 chnID, uint08 *pData, uint08 dataLen)
{
	uint08 buffLen;
	uint08 buffer[2];

	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_READ_BLOB_RSP;
	return bth_l2cap_sendChannelData(handle, chnID, buffer, buffLen, pData, dataLen);
}

//3.4.5 Writing attributes
int btp_att_sendWriteReq(uint16 handle, uint16 chnID, uint16 attHandle, uint08 *pData, uint16 dataLen)
{
	uint08 buffLen;
	uint08 buffer[5];

	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_WRITE_REQ;
	buffer[buffLen++] = attHandle & 0xFF;
	buffer[buffLen++] = (attHandle & 0xFF00) >> 8;
	return bth_l2cap_sendChannelData(handle, chnID, buffer, buffLen, pData, dataLen);
}
int btp_att_sendWriteRsp(uint16 handle, uint16 chnID)
{
	uint08 buffLen;
	uint08 buffer[5];

	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_WRITE_RSP;
	return bth_l2cap_sendChannelData(handle, chnID, NULL, 0, buffer, buffLen);
}
int btp_att_sendWriteCmd(uint16 handle, uint16 chnID, uint16 attHandle, uint08 *pData, uint16 dataLen)
{
	uint08 buffLen;
	uint08 buffer[5];

	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_WRITE_CMD;
	buffer[buffLen++] = attHandle & 0xFF;
	buffer[buffLen++] = (attHandle & 0xFF00) >> 8;
	return bth_l2cap_sendChannelData(handle, chnID, buffer, buffLen, pData, dataLen);
}

//3.4.7 Server initiated
int btp_att_sendHandleValueNtf(uint16 handle, uint16 chnID, uint16 attHandle, uint08 *pData, uint16 dataLen)
{
	uint08 buffLen;
	uint08 buffer[5];

	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_HANDLE_VALUE_NTF;
	buffer[buffLen++] = attHandle & 0xFF;
	buffer[buffLen++] = (attHandle & 0xFF00) >> 8;
	return bth_l2cap_sendChannelData(handle, chnID, buffer, buffLen, pData, dataLen);
}
int btp_att_sendHandleValueNtf1(uint16 handle, uint16 chnID, uint16 attHandle, uint08 *pHead, uint08 headLen, uint08 *pData, uint16 dataLen)
{
	uint08 buffLen;
	uint08 buffer[70];
	
	if(headLen > 64) return -TLK_EOVERFLOW;
	
	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_HANDLE_VALUE_NTF;
	buffer[buffLen++] = attHandle & 0xFF;
	buffer[buffLen++] = (attHandle & 0xFF00) >> 8;
	if(headLen != 0){
		tmemcpy(buffer+buffLen, pHead, headLen);
		buffLen += headLen;
	} 
	return bth_l2cap_sendChannelData(handle, chnID, buffer, buffLen, pData, dataLen);
}
int btp_att_sendHandleValueInd(uint16 handle, uint16 chnID, uint16 attHandle, uint08 *pData, uint16 dataLen)
{
	uint08 buffLen;
	uint08 buffer[5];

	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_HANDLE_VALUE_IND;
	buffer[buffLen++] = attHandle & 0xFF;
	buffer[buffLen++] = (attHandle & 0xFF00) >> 8;
	return bth_l2cap_sendChannelData(handle, chnID, nullptr, 0, buffer, buffLen);
}
int btp_att_sendHandleValueCfm(uint16 handle, uint16 chnID)
{
	uint08 buffLen;
	uint08 buffer[5];

	buffLen = 0;
	buffer[buffLen++] = BTP_ATT_OPCODE_HANDLE_VALUE_CFM;
	return bth_l2cap_sendChannelData(handle, chnID, nullptr, 0, buffer, buffLen);
}



#define BTP_GATT_UUID_SPLIT(uuid)    {(uuid)&0xFF, ((uuid)&0xFF00)>>8}
//Major UUID
const uint08 cBtpUUID16Primary[] = BTP_GATT_UUID_SPLIT(BTP_GATT_UUID_PRIMARY_SERVICE);
const uint08 cBtpUUID16SecondS[] = BTP_GATT_UUID_SPLIT(BTP_GATT_UUID_SECONDARY_SERVICE);
const uint08 cBtpUUID16Charact[] = BTP_GATT_UUID_SPLIT(BTP_GATT_UUID_CHARACT);
const uint08 cBtpUUID16Include[] = BTP_GATT_UUID_SPLIT(BTP_GATT_UUID_INCLUDE);
const uint08 cBtpUUID16ExtProp[] = BTP_GATT_UUID_SPLIT(BTP_GATT_UUID_CHAR_EXT_PROPS);
//Minor UUID
const uint08 cBtpUUID16CharUserDesc[] = BTP_GATT_UUID_SPLIT(BTP_GATT_UUID_CHAR_USER_DESC);
const uint08 cBtpUUID16ClientCharCfg[] = BTP_GATT_UUID_SPLIT(BTP_GATT_UUID_CLIENT_CHAR_CFG);
const uint08 cBtpUUID16ServerCharCfg[] = BTP_GATT_UUID_SPLIT(BTP_GATT_UUID_SERVER_CHAR_CFG);
const uint08 cBtpUUID16CharFormat[] = BTP_GATT_UUID_SPLIT(BTP_GATT_UUID_CHAR_PRESENT_FORMAT);
const uint08 cBtpUUID16CharAggFormat[] = BTP_GATT_UUID_SPLIT(BTP_GATT_UUID_CHAR_AGG_FORMAT);
const uint08 cBtpUUID16ValidRange[] = BTP_GATT_UUID_SPLIT(BTP_GATT_UUID_VALID_RANGE);
const uint08 cBtpUUID16ExtReportRef[] = BTP_GATT_UUID_SPLIT(BTP_GATT_UUID_EXT_REPORT_REF);
const uint08 cBtpUUID16ReportRef[] = BTP_GATT_UUID_SPLIT(BTP_GATT_UUID_REPORT_REF);

//Service
const uint08 cBtpUUID16GapService[] = BTP_GATT_UUID_SPLIT(BTP_SERVICE_UUID_GENERIC_ACCESS);
const uint08 cBtpUUID16DevService[] = BTP_GATT_UUID_SPLIT(BTP_SERVICE_UUID_DEVICE_INFORMATION);
const uint08 cBtpUUID16HidService[] = BTP_GATT_UUID_SPLIT(BTP_SERVICE_UUID_HUMAN_INTERFACE_DEVICE);
const uint08 cBtpUUID16GattService[] = BTP_GATT_UUID_SPLIT(BTP_SERVICE_UUID_GENERIC_ATTRIBUTE);

//GAP
const uint08 cBtpUUID16DeviceName[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_DEVICE_NAME);
const uint08 cBtpUUID16Appearance[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_APPEARANCE);
const uint08 cBtpUUID16PrivacyFlag[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_PERIPHERAL_PRIVACY);
const uint08 cBtpUUID16ReconnectAddr[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_RECONNRCT_ADDRESS);
const uint08 cBtpUUID16PeriConnParam[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_PERI_CONN_PARAM);

//Gatt
const uint08 cBtpUUID16ServiceChanged[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_SERVICE_CHANGE);

//Battery
const uint08 cBtpUUID16BatteryLevel[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_BATTERY_LEVEL);

//Device 
const uint08 cBtpUUID16PnpID[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_PNP_ID);
const uint08 cBtpUUID16SystemID[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_SYSTEM_ID);
const uint08 cBtpUUID16ManuName[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_MANU_NAME_STRING);
const uint08 cBtpUUID16ModelNum[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_MODEL_NUM_STRING);
const uint08 cBtpUUID16SerialNum[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_SERIAL_NUM_STRING);
const uint08 cBtpUUID16HWVersion[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_HW_REVISION_STRING);
const uint08 cBtpUUID16FWVersion[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_FW_REVISION_STRING);
const uint08 cBtpUUID16SWVersion[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_SW_REVISION_STRING);

//HID
const uint08 cBtpUUID16HidBootKeyInput[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_HID_BOOT_KEY_INPUT);
const uint08 cBtpUUID16HidBootKeyOutput[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_HID_BOOT_KEY_OUTPUT);
const uint08 cBtpUUID16HidBootMouseInput[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_HID_BOOT_MOUSE_INPUT);
const uint08 cBtpUUID16HidInformation[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_HID_INFORMATION);
const uint08 cBtpUUID16HidReportMap[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_HID_REPORT_MAP);
const uint08 cBtpUUID16HidControlPoint[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_HID_CONTROL_POINT);
const uint08 cBtpUUID16HidReport[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_HID_REPORT);
const uint08 cBtpUUID16HidProtocolMode[] = BTP_GATT_UUID_SPLIT(BTP_CHARACT_UUID_HID_PROTOCOL_MODE);




#endif //#if (TLKBTP_CFG_ATT_ENABLE)

