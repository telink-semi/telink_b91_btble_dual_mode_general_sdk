/********************************************************************************************************
 * @file     bt_hci.h
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


#ifndef BT_HCI_H_
#define BT_HCI_H_


#include "tlkstk/inner/tlkstk_inner.h"

/**
 *  @brief  Definition for HCI packet type & HCI packet indicator
 */
typedef enum hci_type_e {
	HCI_TYPE_CMD = 0x01,
	HCI_TYPE_ACL_DATA,
	HCI_TYPE_SCO_DATA,
	HCI_TYPE_EVENT,
    HCI_TYPE_ISO_DATA,
} hci_type_t;



extern struct evt_mask hci_rsvd_evt_msk ;
extern struct evt_mask hci_env_tag;
extern struct evt_mask hci_env_tag_page_2;


u8 * bt_hci_get_event_buffer(void);
int bt_hci_send_data (u32 h, u8 *para, int n);
u8 * my_uart_tx_get_buffer (void);
u8 * my_uart_rx_get_buffer (void);

typedef int return_type_of_hci_command;
typedef return_type_of_hci_command (*func_hci_command_t) (unsigned char *, int);
extern return_type_of_hci_command hci_command_exec (unsigned char *p, int len);


////////////////////////////////////////////////////////////////////////////////////

int hci_controller_to_host(void);
void hci_host_to_controller(void);

int	hci_txfifo_add ( u8 *para, int n);
int	hci_txfifo_full (void);
int hci_txfifo_is_empty(void);
void hci_txfifo_set_rptr_reference (int idx);

void hci_set_tx_critical_sec_en(u8 flow);
u8  hci_get_tx_critical_sec_en(void);

u8* hci_rxfifo_get (void);
u8  hci_get_rx_acl_flow_control(void);
int hci_rxfifo_is_empty(void);
int hci_rxfifo_full (void);
void hci_set_rx_acl_flow_control(u8 flow);


#define PENDING_BUF_LEN 4096  //  4K
void data_push_controller_hci_fifo_process(u8* pending_buf,u32 pending_len);


typedef uint08(*tlk_hci_serial_sendCB)(uint08 *pBuf);
extern tlk_hci_serial_sendCB sTlk_hci_serial_sendCB;

typedef void(*tlk_hci_serial_tx_reset_sendCB)(void);
extern tlk_hci_serial_tx_reset_sendCB sTlk_hci_serial_tx_reset_sendCB;

typedef int (*tlk_hci_cmd_vendor_CB)(uint32_t data);
extern tlk_hci_cmd_vendor_CB sTlk_hci_cmd_vendor_BaudrateCB;
extern tlk_hci_cmd_vendor_CB sTlk_hci_cmd_vendor_standbyCB;

#endif // BT_HCI_H_
