/********************************************************************************************************
 * @file     btp_hiddevice.h
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



#include "tlkapi/tlkapi_stdio.h"
#include "tlkstk/bt/btp/btp_config.h"
#include "tlkstk/bt/bth/bth_stdio.h"


#define BTP_HIDP_TIMEOUT              400000 //100ms
#define BTP_HIDP_CONN_TIMEOUT         (7000000/BTP_HIDP_TIMEOUT)
#define BTP_HIDP_DISC_TIMEOUT         (5000000/BTP_HIDP_TIMEOUT)
#define BTP_HIDP_CONN_TIMEOUT2        (5000000/BTP_HIDP_TIMEOUT)



typedef enum{
	BTP_HIDP_BUSY_NONE                = 0x0000,
	BTP_HIDP_BUSY_SEND_CONNECT_CMD    = 0x0001,
	BTP_HIDP_BUSY_SEND_DISCCONN_CMD   = 0x0002,
	BTP_HIDP_BUSY_SEND_GETREPORT_RSP  = 0x0004,
	BTP_HIDP_BUSY_SEND_SETREPORT_RSP  = 0x0008,
	BTP_HIDP_BUSY_SEND_GETIDLE_RSP    = 0x0004,
	BTP_HIDP_BUSY_SEND_SETIDLE_RSP    = 0x0008,
	BTP_HIDP_BUSY_SEND_GETPROTOCOL_RSP = 0x0004,
	BTP_HIDP_BUSY_SEND_SETPROTOCOL_RSP = 0x0008,
	BTP_HIDP_BUSY_SEND_CONTROL_RSP     = 0x0010,
}BTP_HIDP_BUSYS_ENUM;

/* HIDP header masks */
#define BTP_HIDP_HEADER_TRANS_MASK			0xf0
#define BTP_HIDP_HEADER_PARAM_MASK			0x0f

/* HIDP transaction types */
#define BTP_HIDP_TRANS_HANDSHAKE			0x00
#define BTP_HIDP_TRANS_HID_CONTROL			0x10
#define BTP_HIDP_TRANS_GET_REPORT			0x40
#define BTP_HIDP_TRANS_SET_REPORT			0x50
#define BTP_HIDP_TRANS_GET_PROTOCOL			0x60
#define BTP_HIDP_TRANS_SET_PROTOCOL			0x70
#define BTP_HIDP_TRANS_GET_IDLE			    0x80
#define BTP_HIDP_TRANS_SET_IDLE			    0x90
#define BTP_HIDP_TRANS_DATA				    0xa0
#define BTP_HIDP_TRANS_DATC				    0xb0

/* HIDP handshake results */
#define BTP_HIDP_HSHK_SUCCESSFUL			0x00
#define BTP_HIDP_HSHK_NOT_READY			    0x01
#define BTP_HIDP_HSHK_ERR_INVALID_REPORT_ID		0x02
#define BTP_HIDP_HSHK_ERR_UNSUPPORTED_REQUEST	0x03
#define BTP_HIDP_HSHK_ERR_INVALID_PARAMETER		0x04
#define BTP_HIDP_HSHK_ERR_UNKNOWN			0x0e
#define BTP_HIDP_HSHK_ERR_FATAL			    0x0f

/* HIDP control operation parameters */
#define BTP_HIDP_CTRL_NOP				    0x00
#define BTP_HIDP_CTRL_HARD_RESET			0x01
#define BTP_HIDP_CTRL_SOFT_RESET			0x02
#define BTP_HIDP_CTRL_SUSPEND			    0x03
#define BTP_HIDP_CTRL_EXIT_SUSPEND			0x04
#define BTP_HIDP_CTRL_VIRTUAL_CABLE_UNPLUG  0x05

/* HIDP data transaction headers */
#define BTP_HIDP_DATA_RSRVD_MASK			0x0c
#define BTP_HIDP_DATA_RTYPE_OTHER			0x00
#define BTP_HIDP_DATA_RTYPE_INPUT			0x01
#define BTP_HIDP_DATA_RTYPE_OUPUT			0x02
#define BTP_HIDP_DATA_RTYPE_FEATURE			0x03

/* HIDP protocol header parameters */
#define BTP_HIDP_PROTO_BOOT				    0x00
#define BTP_HIDP_PROTO_REPORT			    0x01
#define BTP_HIDP_PROTO_MODE_REPORT_WITH_FALLBACK_TO_BOOT  0x02

#define BTP_HIDP_VIRTUAL_CABLE_UNPLUG		0
#define BTP_HIDP_BOOT_PROTOCOL_MODE			1
#define BTP_HIDP_BLUETOOTH_VENDOR_ID		9
#define BTP_HIDP_WAITING_FOR_RETURN			10
#define BTP_HIDP_WAITING_FOR_SEND_ACK		11


#define BTP_HID_REPORT_ID_UNDECLARED        0
#define BTP_HID_REPORT_ID_VALID             1
#define BTP_HID_REPORT_ID_INVALID           2

#define BTP_HID_CONTROL_PARAM_SUSPEND             0x03              // Go to reduced power mode.
#define BTP_HID_CONTROL_PARAM_EXIT_SUSPEND        0x04                  // Exit reduced power mode.
#define BTP_HID_CONTROL_PARAM_VIRTUAL_CABLE_UNPLUG 0x05

#define BTP_HID_BOOT_MODE_KEYBOARD_ID 1
#define BTP_HID_BOOT_MODE_MOUSE_ID    2


typedef struct
{
    uint16  len;
    uint16  channel;
    uint08  dat[1];
} btp_l2cap_data_packet_header_t;


typedef void (*btp_hid_output_callback_t)(uint16 aclHandle, uint16 report_type, uint16 report_id, uint16 datalen, uint08* pData);
typedef void (*btp_hid_control_callback_t)(uint16 aclHandle, uint16 evtID);
typedef void (*btp_hid_set_report_callback_t)(uint16 aclHandle, uint08 report_id, uint08 *pData, uint16 datalen);
typedef uint08 (*btp_hid_get_report_callback_t)(uint16 aclHandle, uint08 report_type, uint08 report_id, uint16* report_len, uint08* pOutData);
typedef void (*btp_hid_connection_callback_t)(int connected);

typedef struct
{
    btp_hid_output_callback_t        output;
	btp_hid_control_callback_t       control;
    btp_hid_set_report_callback_t    set_report;
    btp_hid_get_report_callback_t    get_report;
    btp_hid_connection_callback_t    connection;
} btp_hid_callback_t;

typedef enum
{
    BTP_HIDP_CLOSED,
    BTP_HIDP_CLOSING,
    BTP_HIDP_CONNECTING,
    BTP_HIDP_CONNECTED,
}btp_hid_profile_state_t;

#if 0
typedef enum {
    HID_REPORT_TYPE_RESERVED = 0,
    HID_REPORT_TYPE_INPUT,
    HID_REPORT_TYPE_OUTPUT,
    HID_REPORT_TYPE_FEATURE
} btp_hid_report_type_t;
#endif
typedef enum{
    HID_SUBEVENT_SUSPEND = 0,
	HID_SUBEVENT_EXIT_SUSPEND,
	HID_SUBEVENT_VIRTUAL_CABLE_UNPLUG,
} btp_hid_control_type_t;

typedef struct
{
    btp_hid_profile_state_t  state;

	uint16  ctlScid;
	uint16  intScid;
	uint16  aclHandle;
    uint16  usrID;
	uint16  protocol_Mode;

    uint16  report_type;
    uint16    report_id;
    uint16    expected_report_size;
    uint16    report_size;
	uint16    report_status;
	
    btp_hid_callback_t   hid_callback;

    /* hid flags */
    uint32  initiator: 1;
    uint32  rsv:      31;

	uint08  reportData;
	uint16  timeout;
	uint16  busys;
	tlkapi_timer_t  timer;
}btp_hid_item_t;

typedef struct{
	uint08 curIndex;
    btp_hid_item_t item[TLK_BT_HIDP_MAX_NUMB];
}btp_hidp_ctrl_t;

typedef enum
{
    BTP_HID_EVENT_CONNECTED,
    BTP_HID_EVENT_DISCONNECTED,
}btp_hid_device_event_t;

typedef struct
{
    uint08 l2cap_handle_interupt;
    uint08 l2cap_handle_control;
}connected_para_t;


/*
 * Data type for HID device
 */

typedef union
{
    connected_para_t conn_para;
}btp_hid_device_event_data_t;


/******************************************************************************
 * Function: HID device initial interface.
 * Descript: Defines the interface for initial hid connected control block exist.
 * Params: [IN]callback--callback function
 * Return: Returning TLK_ENONE is success.
 *******************************************************************************/
uint08 btp_hid_device_init(btp_hid_callback_t *callback, uint16 protocolMode);

/******************************************************************************
 * Function: HID connect interface.
 * Descript: Defines hid connect interface.
 * Params: [IN]handle--ACL handle
 * Return: Returning TLK_ENONE is success.
 *******************************************************************************/
uint08 btp_hid_device_open(uint16 handle);

/******************************************************************************
 * Function: HID disconnect interface.
 * Descript: Defines hid disconnect interface.
 * Params: [IN]handle--ACL handle
 * Return: Returning TLK_ENONE is success.
 *******************************************************************************/
uint08 btp_hid_device_close(uint16 handle);

/******************************************************************************
 * Function: HID send device report data interface.
 * Descript: Defines send the device data.
 * Params: [IN]pData--the user data
 *         [IN]datalen--the data length
 * Return: Returning The total number of hid control block.
 *******************************************************************************/
uint08 btp_hid_device_report(uint8_t *pData, int datalen, uint16 aclHandle);

/******************************************************************************
 * Function: HID device destroy interface.
 * Descript: Defines destory the hid control block.
 * Params: [IN]handle--ACL handle
 * Return: Returning TLK_ENONE is success.
 *******************************************************************************/
uint08 btp_hid_device_destory(uint16 handle);

/******************************************************************************
 * Function: HID check the hid Idle control block num interface.
 * Descript: Defines check how many hid idle control block exist.
 * Params:
 * Return: Returning The total number of hid control block.
 *******************************************************************************/
uint08 btp_hidp_getIdleCount(void);

/******************************************************************************
 * Function: HID Get the hid idle control block.
 * Descript: Defines get a new hid control block in resource.
 * Params:
 * Return: Returning The new hid control block.
 *******************************************************************************/
btp_hid_item_t *btp_hidp_getIdleNode(void);

/******************************************************************************
 * Function: HID Get the HID Used control block.
 * Descript: Defines get the client or server Used hid control block 
 *           in resource via aclhandle and usrid. 
 * Params:
 *       @aclHandle[IN]--The aclHandle to search the hid control block.
 * Return: Returning The used hid control block.
 *******************************************************************************/
btp_hid_item_t *btp_hidp_getAnyUsedNode(uint16 aclHandle);
btp_hid_item_t *btp_hidp_getAnyUsedNodeByChnID(uint16 aclHandle, uint16 cid);
btp_hid_item_t *btp_hidp_getUsedNodeByIntcid(uint16 aclHandle, uint16 scid);



/******************************************************************************
 * Function: HID Get the hidp Used control block.
 * Descript: Defines get the special Connected hid control block 
 *           in resource via aclhandle and scid. 
 * Params:
 *       @aclHandle[IN]--The aclHandle to  search the hid control block.
 *       @scid[IN]--The scid to search the hid control block.
 * Return: Returning The connected hid control block.
 *******************************************************************************/
btp_hid_item_t *btp_hidp_getUsedNodeByScid(uint16 aclHandle, uint16 scid);
btp_hid_item_t *btp_hidp_getConnNodeByIntcid(uint16 aclHandle, uint16 scid);


/******************************************************************************
 * Function: HID Get the hidp Connected control block.
 * Descript: Defines get the special Connected hid control block 
 *           in resource via cid. 
 * Params:
 *       @aclHandle[IN]--The cid to search the hid control block.
 * Return: Returning The connected hid control block.
 *******************************************************************************/
btp_hid_item_t *btp_hidp_getConnNode(uint16 aclHandle);
btp_hid_item_t *btp_hidp_getConnNodeByScid(uint16 aclHandle, uint16 scid);

/******************************************************************************
 * Function: HID Get the hidp Connected control block.
 * Descript: Defines get the special Connected hid control block 
 *           in resource via cid. 
 * Params:
 *       @cid[IN]--The cid to search the hid control block.
 * Return: Returning The connected hid control block.
 *******************************************************************************/
btp_hid_item_t *btp_hidp_getConnNodeByChnID(uint16 cid);

/******************************************************************************
 * Function: HID Get the hidp Busy control block.
 * Descript: Defines get the special busy hid control block
 *           in resource via cid.
 * Params:
 *       @aclHandle[IN]--The aclHandle to search the hid control block.
 * Return: Returning The busy hid control block.
 *******************************************************************************/
btp_hid_item_t *btp_hidp_getBusyNode(uint16 aclHandle);

/******************************************************************************
 * Function: HID reset the control block.
 * Descript: Defines reset the special busy hid control block
 * Params:
 *       @pItem[IN]--The hid control block to reset.
 * Return: null.
 *******************************************************************************/
void btp_hidp_resetNode(btp_hid_item_t *pItem);

/******************************************************************************
 * Function: HID send the hidp connect evt.
 * Descript: Defines send the connect evt to uplayer 
 * Params:
 *       @cid[IN]--The hid control block.
 * Return: Returning The connected hid control block.
 *******************************************************************************/
void btp_hidp_send_connectEvt(btp_hid_item_t *pHid);

/******************************************************************************
 * Function: HID send the hidp idsconnect evt.
 * Descript: Defines send the disconnect evt to uplayer
 * Params:
 *       @pHid[IN]--The hid control block.
 * Return: Returning The connected hid control block.
 *******************************************************************************/
void btp_hidp_send_disconnectEvt(btp_hid_item_t *pHid);

/******************************************************************************
 * Function: HID Get the hidp Connected boot mode.
 * Descript: Defines get the special hid boot mode 
 * Params:
 *       @aclhandle[IN]--The aclhandle to search the hid control block.
 * Return: Returning The connected hid control block.
 *******************************************************************************/
int btp_hid_device_in_boot_protocol_mode(uint16 aclhandle);

/******************************************************************************
 * Function: HID send the hid message via interrupt.
 * Descript: Defines send the hid message. 
 * Params:
 *       @aclhandle[IN]--The aclhandle to search the hid control block.
 *       @message[IN]--The message
 *       @message_len[IN]--The message length
 *       @flag[IN]--The flag
 * Return: NULL.
 *******************************************************************************/
void hid_device_send_interrupt_message(uint16_t aclhandle, const uint8_t * message, uint16_t message_len, uint32 flag);


/******************************************************************************
 * Function: HID send the hid message via control.
 * Descript: Defines send the hid message. 
 * Params:
 *       @aclhandle[IN]--The aclhandle to search the hid control block.
 *       @message[IN]--The message
 *       @message_len[IN]--The message length
 *       @flag[IN]--The flag
 * Return: NULL.
 *******************************************************************************/
void hid_device_send_control_message(uint16_t aclHandle, const uint8_t * message, uint16_t message_len, uint32 flag);


#endif //BTP_HID_H
