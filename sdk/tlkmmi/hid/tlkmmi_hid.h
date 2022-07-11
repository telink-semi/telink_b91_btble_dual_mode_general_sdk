/********************************************************************************************************
 * @file     tlkmmi_hid.h
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

#ifndef TLKMMI_HID_H
#define TLKMMI_HID_H

typedef enum{
    TLKMMI_HID_GETREPORT_ID = 0,
	TLKMMI_HID_SETREPORT_ID,
	TLKMMI_HID_CONTROL_ID,
	TLKMMI_HID_OUTPUT_ID,
}tlkmmi_hid_evt_id_t;


typedef void(*tlkmmi_hid_callback_t)(uint16 aclHandle, uint16 report_type, uint16 report_id, uint16* datalen, uint08* pData, tlkmmi_hid_evt_id_t evtID);

int tlkmmi_hid_init();
uint16 tlkmmi_hid_deinit(uint16 aclHandle);

uint08 tlkmmi_hid_connect(uint16 aclHandle);
uint08 tlkmmi_hid_disconnect(uint16 aclHandle);

uint08 tlkmmi_hid_sendData(uint16 aclHandle, uint08* pData, uint16 datalen);

void tlkmmi_hid_register_callback(tlkmmi_hid_callback_t callback);
void tlkmmi_hid_unregister_callback();


#endif

