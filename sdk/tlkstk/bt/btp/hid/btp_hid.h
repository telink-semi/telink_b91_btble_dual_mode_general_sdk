/********************************************************************************************************
 * @file     btp_hid.h
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

#ifndef BTP_HID_H
#define BTP_HID_H

#if 0



#include "tlkapi/tlkapi_stdio.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_l2cap.h"


/* HIDP header masks */
#define HIDP_HEADER_TRANS_MASK			0xf0
#define HIDP_HEADER_PARAM_MASK			0x0f

/* HIDP transaction types */
#define HIDP_TRANS_HANDSHAKE			0x00
#define HIDP_TRANS_HID_CONTROL			0x10
#define HIDP_TRANS_GET_REPORT			0x40
#define HIDP_TRANS_SET_REPORT			0x50
#define HIDP_TRANS_GET_PROTOCOL			0x60
#define HIDP_TRANS_SET_PROTOCOL			0x70
#define HIDP_TRANS_GET_IDLE			    0x80
#define HIDP_TRANS_SET_IDLE			    0x90
#define HIDP_TRANS_DATA				    0xa0
#define HIDP_TRANS_DATC				    0xb0

/* HIDP handshake results */
#define HIDP_HSHK_SUCCESSFUL			0x00
#define HIDP_HSHK_NOT_READY			    0x01
#define HIDP_HSHK_ERR_INVALID_REPORT_ID		0x02
#define HIDP_HSHK_ERR_UNSUPPORTED_REQUEST	0x03
#define HIDP_HSHK_ERR_INVALID_PARAMETER		0x04
#define HIDP_HSHK_ERR_UNKNOWN			0x0e
#define HIDP_HSHK_ERR_FATAL			    0x0f

/* HIDP control operation parameters */
#define HIDP_CTRL_NOP				    0x00
#define HIDP_CTRL_HARD_RESET			0x01
#define HIDP_CTRL_SOFT_RESET			0x02
#define HIDP_CTRL_SUSPEND			    0x03
#define HIDP_CTRL_EXIT_SUSPEND			0x04
#define HIDP_CTRL_VIRTUAL_CABLE_UNPLUG  0x05

/* HIDP data transaction headers */
#define HIDP_DATA_RTYPE_MASK			0x03
#define HIDP_DATA_RSRVD_MASK			0x0c
#define HIDP_DATA_RTYPE_OTHER			0x00
#define HIDP_DATA_RTYPE_INPUT			0x01
#define HIDP_DATA_RTYPE_OUPUT			0x02
#define HIDP_DATA_RTYPE_FEATURE			0x03

/* HIDP protocol header parameters */
#define HIDP_PROTO_BOOT				    0x00
#define HIDP_PROTO_REPORT			    0x01

#define HIDP_VIRTUAL_CABLE_UNPLUG		0
#define HIDP_BOOT_PROTOCOL_MODE			1
#define HIDP_BLUETOOTH_VENDOR_ID		9
#define HIDP_WAITING_FOR_RETURN			10
#define HIDP_WAITING_FOR_SEND_ACK		11


typedef struct
{
    uint16_t len;
    uint16_t channel;
    uint8_t  dat[1];
}l2cap_data_packet_header_t;


typedef void (*hid_output_callback_t)(uint8_t* p, uint16_t len);
typedef void (*hid_set_report_callback_t)(unsigned char report_id, unsigned char *p_dat, int len);
typedef unsigned char* (*hid_get_report_callback_t)(unsigned char report_id, int *len);
typedef void (*hid_connection_callback_t)(int connected);

typedef struct
{
    hid_output_callback_t       output;
    hid_set_report_callback_t   set_report;
    hid_get_report_callback_t   get_report;
    hid_connection_callback_t   connection;
}hid_callback_t;

typedef enum
{
    HIDP_CLOSED,
    HIDP_CLOSING,
    HIDP_CONNECTING,
    HIDP_CONNECTED,
}hid_profile_state_t;

typedef struct
{
    hid_profile_state_t state;
	
	uint16 ctlScid;
	uint16 intScid;
	uint16 aclHandle;
	
    hid_callback_t   hid_callback;

    /* hid flags */
    uint32_t initiator: 1;
    uint32_t rsv:      31;
}hid_env_t;


typedef enum
{
    HID_ERR_SUCCESS = 0,
    HID_ERR_INVALID_STATE,
    HID_ERR_INVALID_PARAMETER,
}hid_err_t;

typedef enum
{
    HID_EVENT_CONNECTED,
    HID_EVENT_DISCONNECTED,
}hid_device_event_t;

typedef struct
{
    uint8_t l2cap_handle_interupt;
    uint8_t l2cap_handle_control;
}connected_para_t;


/*
 * Data type for HID device
 */

typedef union
{
    connected_para_t conn_para;
}hid_device_event_data_t;


/*
 * Functions for HID device
 */
hid_err_t hid_device_init(hid_callback_t *callback);
hid_err_t hid_device_open(struct bd_addr *remote);
hid_err_t hid_device_close(struct bd_addr *remote);
hid_err_t hid_device_report(uint8_t *p_dat, int len);
hid_err_t hid_device_destory(struct bd_addr *remote);


#endif

#endif //BTP_HID_H
