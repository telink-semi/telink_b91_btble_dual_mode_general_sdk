/********************************************************************************************************
 * @file     btp_hid.h
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
#ifndef BTP_HID_H
#define BTP_HID_H

#if (TLK_STK_BTP_ENABLE)

/* HIDP handshake results */
#define BTP_HID_HSHK_SUCCESS                    0x00
#define BTP_HID_HSHK_NOT_READY                  0x01
#define BTP_HID_HSHK_ERR_INVALID_REPORT_ID      0x02
#define BTP_HID_HSHK_ERR_UNSUPPORTED_REQUEST    0x03
#define BTP_HID_HSHK_ERR_INVALID_PARAMETER      0x04
#define BTP_HID_HSHK_ERR_UNKNOWN                0x0e
#define BTP_HID_HSHK_ERR_FATAL                  0x0f

/* HIDP data transaction headers */
#define BTP_HID_DATA_RSRVD_MASK             0x0c
#define BTP_HID_DATA_RTYPE_OTHER            0x00
#define BTP_HID_DATA_RTYPE_INPUT            0x01
#define BTP_HID_DATA_RTYPE_OUPUT            0x02
#define BTP_HID_DATA_RTYPE_FEATURE          0x03

/* HIDP protocol header parameters */
#define BTP_HID_PROTO_BOOT                  0x00
#define BTP_HID_PROTO_REPORT                0x01
#define BTP_HID_PROTO_MODE_REPORT_WITH_FALLBACK_TO_BOOT   0x02



typedef uint (*BtpHidSetProtocolCallback)(uint16 aclHandle, uint08 protoMode);
typedef uint (*BtpHidGetProtocolCallback)(uint16 aclHandle, uint08 *pProtoMode);

typedef uint (*BtpHidSetReportCallback)(uint16 aclHandle, uint08 reportType, uint08 reportID, uint08 *pData, uint16 datalen);
typedef uint (*BtpHidGetReportCallback)(uint16 aclHandle, uint08 reportType, uint08 reportID, uint08 *pBuff, uint16 *pBuffLen);

typedef int(*BtpHiddRecvDataCallback)(uint16 handle, uint08 *pData, uint16 dataLen);


int btp_hid_init(void);

int btp_hid_connect(uint16 handle, uint08 usrID);
int btp_hid_disconn(uint16 handle, uint08 usrID);




/******************************************************************************
 * Function: HID destroy interface.
 * Descript: Defines destory the hid control block.
 * Params: [IN]handle--ACL handle
 * Return: Returning TLK_ENONE is success.
 *******************************************************************************/
void btp_hid_destroy(uint16 handle);

/******************************************************************************
 * Function: HID device initial interface.
 * Descript: Defines the interface for initial hid connected control block exist.
 * Params: [IN]callback--callback function
 * Return: Returning TLK_ENONE is success.
 *******************************************************************************/
extern void btp_hidd_regCB(BtpHidSetReportCallback setReportCB, BtpHidGetReportCallback getReportCB,
	BtpHidSetProtocolCallback setProtocolCB, BtpHidGetProtocolCallback getProtocolCB,
	BtpHiddRecvDataCallback ctrDataCB, BtpHiddRecvDataCallback irqDataCB);

/******************************************************************************
 * Function: HID connect interface.
 * Descript: Defines hid connect interface.
 * Params: [IN]handle--ACL handle
 * Return: Returning TLK_ENONE is success.
 *******************************************************************************/
extern int btp_hidd_connect(uint16 handle);

/******************************************************************************
 * Function: HID disconnect interface.
 * Descript: Defines hid disconnect interface.
 * Params: [IN]handle--ACL handle
 * Return: Returning TLK_ENONE is success.
 *******************************************************************************/
extern int btp_hidd_disconn(uint16 handle);

extern uint16 btp_hidd_getAnyConnHandle(void);

extern int btp_hidd_sendData(uint16 aclHandle, uint08 reportID, uint08 reportType, uint08 *pData, uint16 dataLen);
extern int btp_hidd_sendDataWithoutReportID(uint16 aclHandle, uint08 reportType, uint08 *pData, uint16 dataLen);



#endif //#if (TLK_STK_BTP_ENABLE)

#endif //#ifndef BTP_HID_H

