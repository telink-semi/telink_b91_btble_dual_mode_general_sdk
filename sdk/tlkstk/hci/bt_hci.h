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



#define BT_HCI_PENDING_BUFF_LEN 4096  //  4K

/**
 *  @brief  Definition for HCI packet type & HCI packet indicator
 */
typedef enum{
	BT_HCI_TYPE_CMD      = 0x01,
	BT_HCI_TYPE_ACL_DATA = 0x02,
	BT_HCI_TYPE_SCO_DATA = 0x03,
	BT_HCI_TYPE_EVENT    = 0x04,
    BT_HCI_TYPE_ISO_DATA = 0x05,
}BT_HCI_TYPE_ENUM;

typedef int(*TlkBtHciSendCallback)(uint08 *pData, uint16 dataLen);
typedef void(*TlkBtHciResetCallback)(void);
typedef void(*TlkBtHciVendorCallback)(uint32 data);


typedef struct{
    uint08 type;
    uint08 unuse_l;
    uint08 unuse_h;
    uint08 len ;
}bt_hci_sco_data_t;
typedef struct{
    uint08 type;
    uint08 handle_pbf_bcf_l;
    uint08 handle_pbf_bcf_h;
    uint08 len_l ;
    uint08 len_h ;
}bt_hci_acl_data_t;


void tlkbt_hci_enableTxCritical(bool enable);
bool tlkbt_hci_TxCriticalIsenable(void);

bool tlkbt_hci_c2hFifoIsFull(void);
bool tlkbt_hci_c2hFifoIsEmpty(void);

bool tlkbt_hci_h2cFifoIsFull(void);
bool tlkbt_hci_h2cFifoIsEmpty(void);
bool tlkbt_hci_h2cFifoIsHalfFull(void);

uint08 *tlkbt_hci_getH2cBuff(void);
uint08 *tlkbt_hci_getC2hBuff(void);

void tlkbt_hci_c2hHandler(void);
void tlkbt_hci_h2cHandler(void);

int  tlkbt_hci_sendC2hData(uint32 h, uint08 *para, int n);

void tlkbt_hci_recvHandler(uint08* pending_buf, uint32 pending_len);

void tlkbt_hci_regSendCB(TlkBtHciSendCallback sendCB);
void tlkbt_hci_regResetCB(TlkBtHciResetCallback resetCB);
void tlkbt_hci_regStandByCB(TlkBtHciVendorCallback standbyCB);
void tlkbt_hci_regBaudRateCB(TlkBtHciVendorCallback baudrateCB);



#endif // BT_HCI_H_

