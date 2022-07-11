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
#if (TLK_MDI_HID_ENABLE)
#include "tlkmdi/tlkmdi_adapt.h"
#include "tlkmdi/tlkmdi_event.h"
#include "tlkmdi/tlkmdi_hid.h"
#include "../tlkstk/bt/btp/hid/btp_hiddevice.h"


#define TLKMDI_HID_DBG_FLAG         (TLKMDI_HID_DBG_ENABLE | TLKAPI_DBG_FLAG_ALL)
#define TLKMDI_HID_DBG_SIGN         "[HIDMDI]"


static tlkmdi_hid_callback_t gtlkmdi_hid_callback;

static void tlkmdi_hid_output_callback(uint16 aclHandle, uint16 report_type, uint16 report_id, uint16 datalen, uint08* pData);
static void btp_hid_control_callback(uint16 aclHandle, uint16 evtID);
static void tlkmdi_hid_set_report_callback(uint16 aclHandle, uint08 report_id, uint08 *pData, uint16 datalen);
static uint08 tlkmdi_hid_get_report_callback(uint16 aclHandle, uint08 report_type, uint08 report_id, uint16* report_len, uint08* pOutData);
static void tlkmdi_hid_connection_callback(int connected);

static btp_hid_callback_t callback;

static void tlkmdi_hid_output_callback(uint16 aclHandle, uint16 report_type, uint16 report_id, uint16 datalen, uint08* pData)
{
    gtlkmdi_hid_callback(aclHandle, report_type, report_id, &datalen, pData, TLKMDI_HID_OUTPUT_ID);
    return;
}
static void tlkmdi_hid_set_report_callback(uint16 aclHandle, uint08 report_id, uint08 *pData, uint16 datalen)
{
    gtlkmdi_hid_callback(aclHandle, 0, report_id, &datalen, pData, TLKMDI_HID_SETREPORT_ID);
    return;
}
static uint08 tlkmdi_hid_get_report_callback(uint16 aclHandle, uint08 report_type, uint08 report_id, uint16* report_len, uint08* pOutData)
{
    gtlkmdi_hid_callback(aclHandle, 0, report_id, report_len, pOutData, TLKMDI_HID_SETREPORT_ID);
    return nullptr;
}
static void tlkmdi_hid_connection_callback(int connected)
{
    return;
}

static void btp_hid_control_callback(uint16 aclHandle, uint16 evtID)
{
    gtlkmdi_hid_callback(aclHandle, 0, evtID, NULL, NULL, TLKMDI_HID_CONTROL_ID);
    return;
}


void tlkmdi_hid_init()
{
    callback.connection = tlkmdi_hid_connection_callback;//we reserved for hot unplug/plug
	callback.get_report = tlkmdi_hid_get_report_callback;
	callback.control = btp_hid_control_callback;
	callback.output = tlkmdi_hid_output_callback;
	callback.set_report = tlkmdi_hid_set_report_callback;
    btp_hid_device_init(&callback, BTP_HIDP_PROTO_BOOT);
}

void tlkmdi_hid_connect(uint16 aclHandle)
{
    int ret = TLK_ENONE;

	ret = btp_hid_device_open(aclHandle);
    if (ret != TLK_ENONE) {
        return;
	}
}

void tlkmid_hid_disconnect(uint16 aclHandle)
{
    int ret = TLK_ENONE;

	ret = btp_hid_device_close(aclHandle);
    if (ret != TLK_ENONE) {
        return;
	}
}

void tlkmdi_hid_deinit(uint16 aclHandle)
{
    int ret = TLK_ENONE;

	ret = btp_hid_device_destory(aclHandle);
    if (ret != TLK_ENONE) {
        return;
	}
}

void tlkmdi_hid_send_data(uint08 *pData, uint16 dataLen, uint16 aclHandle)
{
    int ret = TLK_ENONE;
	tlkapi_trace(TLKMDI_HID_DBG_FLAG, TLKMDI_HID_DBG_SIGN, "tlkmdi_hid_send_data enter");
    ret = btp_hid_device_report(pData, dataLen, aclHandle);
	if (ret != TLK_ENONE) {
		tlkapi_error(TLKMDI_HID_DBG_FLAG, TLKMDI_HID_DBG_SIGN, "tlkmdi_hid_send_data error");
		return;
	}
}

void tlkmdi_hid_register_callback(tlkmdi_hid_callback_t callback)
{
    gtlkmdi_hid_callback = callback;
}
void tlkmdi_hid_unregister_callback()
{
	gtlkmdi_hid_callback = NULL;
}


#endif //#if (TLK_MDI_DFU_ENABLE)

