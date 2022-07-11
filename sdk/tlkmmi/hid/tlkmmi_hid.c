/********************************************************************************************************
 * @file     tlkmmi_hid.c
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
#include "tlkmmi/tlkmmi_stdio.h"

#include "../../tlkmdi/tlkmdi_hid.h"
#include "tlkmmi_hid.h"

#define TLKMMI_HID_DBG_FLAG         (TLKMMI_HID_DBG_ENABLE | TLKAPI_DBG_FLAG_ALL)
#define TLKMMI_HID_DBG_SIGN         "[HID]"


static tlkmmi_hid_callback_t gtlkmmi_hid_callback;

static void tlkmdi_hid_callback(uint16 aclHandle, uint16 report_type, uint16 report_id, uint16* datalen, uint08* pData, tlkmdi_hid_evt_id_t evtID)
{
    gtlkmmi_hid_callback(aclHandle, report_type, report_id, datalen, pData, evtID);
}


int tlkmmi_hid_init()
{
    tlkapi_trace(TLKMMI_HID_DBG_FLAG, TLKMMI_HID_DBG_SIGN, "tlkmmi_hid_init}");
    tlkmdi_hid_init();
    tlkmdi_hid_register_callback(tlkmdi_hid_callback);
    return TLK_ENONE;
}

uint16 tlkmmi_hid_deinit(uint16 aclHandle)
{
	tlkmdi_hid_deinit(aclHandle);
	tlkmdi_hid_unregister_callback();
	return TLK_ENONE;
}

uint08 tlkmmi_hid_connect(uint16 aclHandle)
{
    tlkapi_trace(TLKMMI_HID_DBG_FLAG, TLKMMI_HID_DBG_SIGN, "tlkmmi_hid_connect:{handle-%d}", aclHandle);
    tlkmdi_hid_connect(aclHandle);
    return TLK_ENONE;
}

uint08 tlkmmi_hid_disconnect(uint16 aclHandle)
{
    tlkapi_trace(TLKMMI_HID_DBG_FLAG, TLKMMI_HID_DBG_SIGN, "tlkmmi_hid_disconnect:{handle-%d}", aclHandle);
	tlkmid_hid_disconnect(aclHandle);
	return TLK_ENONE;
}

uint08 tlkmmi_hid_sendData(uint16 aclHandle, uint08* pData, uint16 datalen)
{
	tlkmdi_hid_send_data(pData, datalen, aclHandle);
	return TLK_ENONE;
}

void tlkmmi_hid_register_callback(tlkmmi_hid_callback_t callback)
{
	gtlkmmi_hid_callback = callback;
}

void tlkmmi_hid_unregister_callback()
{
	gtlkmmi_hid_callback = NULL;
}

