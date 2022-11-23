/********************************************************************************************************
 * @file     tlkmdi_hid.c
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

#include "tlkapi/tlkapi_stdio.h"
#include "tlkdev/tlkdev_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmdi/tlkmdi_config.h"
#if (TLK_MDI_BTHID_ENABLE)
#include "tlkmdi/tlkmdi_adapt.h"
#include "tlkmdi/tlkmdi_event.h"
#include "tlkmdi/tlkmdi_bthid.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/sdp/btp_sdp.h"
#include "tlkstk/bt/btp/hid/btp_hid.h"


#define TLKMDI_BTHID_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_BT << 24) | (TLK_MINOR_DBGID_MDI_BT_HID << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_BTHID_DBG_SIGN       "[MDI]"


static uint tlkmdi_bthid_setProtocolCB(uint16 aclHandle, uint08 protoMode);
static uint tlkmdi_bthid_getProtocolCB(uint16 aclHandle, uint08 *pProtoMode);

static uint tlkmdi_bthid_setReportCB(uint16 aclHandle, uint08 reportType, uint08 reportID, uint08 *pData, uint16 dataLen);
static uint tlkmdi_bthid_getReportCB(uint16 aclHandle, uint08 reportType, uint08 reportID, uint08 *pBuff, uint16 *pBuffLen);

static int tlkmdi_bthid_irqRecvDataCB(uint16 handle, uint08 *pData, uint16 dataLen);

tlkmdi_bthid_report_t *tlkmdi_bthid_getReportCtrl(uint08 rtype, uint08 rptID);
tlkmdi_bthid_report_t *tlkmdi_bthid_getReportCtrlByRptID(uint08 rptID);


static uint08 sTlkMdiBtHidProtocolMode;


int tlkmdi_bthid_init(void)
{
	sTlkMdiBtHidProtocolMode = BTP_HID_PROTO_BOOT;
    btp_hidd_regCB(tlkmdi_bthid_setReportCB, tlkmdi_bthid_getReportCB,
    	tlkmdi_bthid_setProtocolCB, tlkmdi_bthid_getProtocolCB,
		nullptr, tlkmdi_bthid_irqRecvDataCB);
	
	return TLK_ENONE;
}

int tlkmdi_bthid_connect(uint16 aclHandle)
{
	return btp_hidd_connect(aclHandle);
}

int tlkmid_bthid_disconn(uint16 aclHandle)
{
	return btp_hidd_disconn(aclHandle);
}


int tlkmdi_bthid_sendData(uint16 aclHandle, uint08 reportID, uint08 *pData, uint16 dataLen)
{
    int ret;
	tlkmdi_bthid_report_t *pReport;
	
	pReport = tlkmdi_bthid_getReportCtrlByRptID(reportID);
	if(pReport == nullptr){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_sendData: invalid report ID");
		return -TLK_EPARAM;
	}
	
    ret = btp_hidd_sendData(aclHandle, reportID, pReport->rtype, pData, dataLen);
	if(ret != TLK_ENONE) {
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_sendData error");
		return ret;
	}
	tlkapi_trace(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_sendData");
	return TLK_ENONE;
}
int tlkmdi_bthid_sendDataWithoutReportID(uint16 aclHandle, uint08 reportType, uint08 *pData, uint16 dataLen)
{
    int ret;
	
    ret = btp_hidd_sendDataWithoutReportID(aclHandle, reportType, pData, dataLen);
	if(ret != TLK_ENONE){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_sendData error");
		return ret;
	}
	tlkapi_trace(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_sendDataWithoutReportID");
	return TLK_ENONE;
}

static uint tlkmdi_bthid_setProtocolCB(uint16 aclHandle, uint08 protoMode)
{
	if(protoMode > 2){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_setProtocolCB: invalid param");
		return BTP_HID_HSHK_ERR_INVALID_PARAMETER;
	}
	tlkapi_trace(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_setProtocolCB");
	sTlkMdiBtHidProtocolMode = protoMode;
	return BTP_HID_HSHK_SUCCESS;
}
static uint tlkmdi_bthid_getProtocolCB(uint16 aclHandle, uint08 *pProtoMode)
{
	if(pProtoMode == nullptr){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_getProtocolCB: invalid param");
		return BTP_HID_HSHK_ERR_INVALID_PARAMETER;
	}
	tlkapi_trace(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_getProtocolCB");
	*pProtoMode = sTlkMdiBtHidProtocolMode;
	return BTP_HID_HSHK_SUCCESS;
}

static uint tlkmdi_bthid_setReportCB(uint16 aclHandle, uint08 reportType, uint08 reportID, uint08 *pData, uint16 dataLen)
{
	tlkmdi_bthid_report_t *pReport;

	if(pData == NULL || dataLen < 2){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_setReportCB: invalid param");
		return BTP_HID_HSHK_ERR_INVALID_PARAMETER;
	}
	
	pReport = tlkmdi_bthid_getReportCtrlByRptID(reportID);
	if(pReport == nullptr){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_setReportCB: invalid report id");
		return BTP_HID_HSHK_ERR_INVALID_REPORT_ID;
	}
	if(reportType != BTP_HID_DATA_RTYPE_OTHER && reportType != pReport->rtype){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_setReportCB: invalid report type");
		return BTP_HID_HSHK_ERR_INVALID_PARAMETER;
	}
	if(!pReport->enChg){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_setReportCB: unsupport request");
		return BTP_HID_HSHK_ERR_UNSUPPORTED_REQUEST;
	}
	if(dataLen > pReport->bsize){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_setReportCB: overflow");
		return BTP_HID_HSHK_ERR_FATAL;
	}

	tlkapi_trace(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_setReportCB");
	tmemcpy(pReport->pData, pData, dataLen);
	pReport->dlens = dataLen;
	
	return BTP_HID_HSHK_SUCCESS;
}
static uint tlkmdi_bthid_getReportCB(uint16 aclHandle, uint08 reportType, uint08 reportID, uint08 *pBuff, uint16 *pBuffLen)
{
	uint16 buffLen;
	tlkmdi_bthid_report_t *pReport;

	if(pBuff == NULL || pBuffLen == nullptr || (*pBuffLen) == 0){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_getReportCB: invalid param");
		return BTP_HID_HSHK_ERR_INVALID_PARAMETER;
	}

	buffLen = *pBuffLen;
	pReport = tlkmdi_bthid_getReportCtrlByRptID(reportID);
	if(pReport == nullptr){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_getReportCB: invalid report id");
		return BTP_HID_HSHK_ERR_INVALID_REPORT_ID;
	}
	if(reportType != BTP_HID_DATA_RTYPE_OTHER && reportType != pReport->rtype){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_getReportCB: invalid report type");
		return BTP_HID_HSHK_ERR_INVALID_PARAMETER;
	}

	tlkapi_trace(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_getReportCB");
	
	if(buffLen > pReport->dlens) buffLen = pReport->dlens;
	tmemcpy(pBuff, pReport->pData, buffLen);
	*pBuffLen = buffLen;
		
	return BTP_HID_HSHK_SUCCESS;
}
static int tlkmdi_bthid_irqRecvDataCB(uint16 handle, uint08 *pData, uint16 dataLen)
{
	return TLK_ENONE;
}


static const uint08 scTlkMdiBtHidKeyboardReportData[] = {
	//keyboard report in
	0x05, 0x01,     // Usage Pg (Generic Desktop)
	0x09, 0x06,     // Usage (Keyboard)
	0xA1, 0x01,     // Collection: (Application)
	0x85, TLKMDI_BTHID_REPORT_ID_KEYBOARD_INPUT, // Report Id (keyboard)
	0x05, 0x07,     // Usage Pg (Key Codes)
	0x19, 0xE0,     // Usage Min (224)  VK_CTRL:0xe0
	0x29, 0xE7,     // Usage Max (231)  VK_RWIN:0xe7
	0x15, 0x00,     // Log Min (0)
	0x25, 0x01,     // Log Max (1)
	//Modifier byte
	0x75, 0x01,     // Report Size (1)   1 bit * 8
	0x95, 0x08,     // Report Count (8)
	0x81, 0x02,     // Input: (Data, Variable, Absolute)
	//Reserved byte
	0x95, 0x01,     // Report Count (1)
	0x75, 0x08,     // Report Size (8)
	0x81, 0x01,     // Input: (static constant)
	//keyboard output
	//5 bit led ctrl: NumLock CapsLock ScrollLock Compose kana
	0x95, 0x05,    //Report Count (5)
	0x75, 0x01,    //Report Size (1)
	0x05, 0x08,    //Usage Pg (LEDs )
	0x19, 0x01,    //Usage Min
	0x29, 0x05,    //Usage Max
	0x91, 0x02,    //Output (Data, Variable, Absolute)
	//3 bit reserved
	0x95, 0x01,    //Report Count (1)
	0x75, 0x03,    //Report Size (3)
	0x91, 0x01,    //Output (static constant)
	//Key arrays (6 bytes)
	0x95, 0x06,     // Report Count (6)
	0x75, 0x08,     // Report Size (8)
	0x15, 0x00,     // Log Min (0)
	0x25, 0xF1,     // Log Max (241)
	0x05, 0x07,     // Usage Pg (Key Codes)
	0x19, 0x00,     // Usage Min (0)
	0x29, 0xf1,     // Usage Max (241)
	0x81, 0x00,     // Input: (Data, Array)
	0xC0,            // End Collection
};
static tlkmdi_bthid_report_t sTlkMdiBtHidKeyboardReportCtrl = {
	false, //.enChg
	sizeof(scTlkMdiBtHidKeyboardReportData), //.bsize
	BTP_HID_DATA_RTYPE_INPUT, //rtype
	TLKMDI_BTHID_REPORT_ID_KEYBOARD_INPUT, //rptID
	sizeof(scTlkMdiBtHidKeyboardReportData), //.dlens
	(uint08*)scTlkMdiBtHidKeyboardReportData,
};

static const uint08 scTlkMdiBtHidConsumerReportData[] = {
	//consumer report in
	0x05, 0x0C,   // Usage Page (Consumer)
	0x09, 0x01,   // Usage (Consumer Control)
	0xA1, 0x01,   // Collection (Application)
	0x85, TLKMDI_BTHID_REPORT_ID_CONSUMER_INPUT,   //     Report Id
	0x75, 0x10,     //global, report size 16 bits
	0x95, 0x01,     //global, report count 1
	0x15, 0x01,     //global, min  0x01
	0x26, 0x8c,0x02,  //global, max  0x28c
	0x19, 0x01,     //local, min   0x01
	0x2a, 0x8c,0x02,  //local, max    0x28c
	0x81, 0x00,     //main,  input data varible, absolute
	0xc0,        //main, end collection
};
static tlkmdi_bthid_report_t sTlkMdiBtHidConsumerReportCtrl = {
	false, //.enChg
	sizeof(scTlkMdiBtHidConsumerReportData), //.bsize
	BTP_HID_DATA_RTYPE_INPUT, //rtype
	TLKMDI_BTHID_REPORT_ID_CONSUMER_INPUT, //rptID
	sizeof(scTlkMdiBtHidConsumerReportData), //.dlens
	(uint08*)scTlkMdiBtHidConsumerReportData,
};


static tlkmdi_bthid_report_t *spTlkMdiBtHidReportCtrl[] = {
	&sTlkMdiBtHidKeyboardReportCtrl,
	&sTlkMdiBtHidConsumerReportCtrl,
};
static uint08 sTlkMdiBtHidReportCount = sizeof(spTlkMdiBtHidReportCtrl)/sizeof(tlkmdi_bthid_report_t*);


tlkmdi_bthid_report_t *tlkmdi_bthid_getReportCtrl(uint08 rtype, uint08 rptID)
{
	uint08 index;
	tlkmdi_bthid_report_t *pReport;
	for(index=0; index<sTlkMdiBtHidReportCount; index++){
		pReport = spTlkMdiBtHidReportCtrl[index];
		if(pReport != nullptr && pReport->rtype == rtype && pReport->rptID == rptID) break;
	}
	if(index == sTlkMdiBtHidReportCount) return nullptr;
	return spTlkMdiBtHidReportCtrl[index];
}
tlkmdi_bthid_report_t *tlkmdi_bthid_getReportCtrlByRptID(uint08 rptID)
{
	uint08 index;
	tlkmdi_bthid_report_t *pReport;
	for(index=0; index<sTlkMdiBtHidReportCount; index++){
		pReport = spTlkMdiBtHidReportCtrl[index];
		if(pReport != nullptr && pReport->rptID == rptID) break;
	}
	if(index == sTlkMdiBtHidReportCount) return nullptr;
	return spTlkMdiBtHidReportCtrl[index];
}



#endif //#if (TLK_MDI_DFU_ENABLE)

