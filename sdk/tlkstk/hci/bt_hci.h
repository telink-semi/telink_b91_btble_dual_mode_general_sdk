/********************************************************************************************************
 * @file	bt_hci.h
 *
 * @brief	This is the header file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *
 *******************************************************************************************************/
#ifndef BT_HCI_H
#define BT_HCI_H


#define TLKBT_HCI_H2C_ACL_SIZE         679 //Data(2-DH5)=679
#define TLKBT_HCI_C2H_ACL_SIZE         679
#define TLKBT_HCI_H2C_FIFO_SIZE        692 //692=TLKBT_HCI_H2C_ACL_SIZE+4(L2capHead)+5(HCI-ACL)+4(DmaLen)
#define TLKBT_HCI_H2C_FIFO_NUMB        8
#define TLKBT_HCI_C2H_FIFO_SIZE        692
#define TLKBT_HCI_C2H_FIFO_NUMB        8
#define TLKBT_HCI_H2C_CMD_FIFO_SIZE    100
#define TLKBT_HCI_H2C_CMD_FIFO_NUMB    8


/**
 *  @brief  Definition for HCI packet type & HCI packet indicator
 */
typedef enum{
	BT_HCI_TYPE_CMD      = 0x01,
	BT_HCI_TYPE_ACL_DATA = 0x02,
	BT_HCI_TYPE_SCO_DATA = 0x03,
	BT_HCI_TYPE_EVENT    = 0x04,
    BT_HCI_TYPE_ISO_DATA = 0x05,
}TLKBT_HCI_TYPE_ENUM;

typedef enum{
    TLKBT_HCI_LLID_ISO      = 0, /// Reserved for future use
    TLKBT_HCI_LLID_CONTINUE = 1, /// Continue
    TLKBT_HCI_LLID_START    = 2, /// Start
	TLKBT_HCI_LLID_COMPLETE = 3, /// Control
}TLKBT_HCI_LLID_ENUM;


typedef int(*TlkBtHciSendCallback)(uint08 *pData, uint16 dataLen);
typedef void(*TlkBtHciResetCallback)(void);
typedef int (*TlkBtHciEventCallback)(uint08 evtID, uint08 *pData, uint16 dataLen);
typedef void(*TlkBtHciAclDataCallback)(uint16 handle, uint08 *pData, uint16 dataLen);
typedef void(*TlkBtHciUserCmdCallback)(uint08 *pData, uint16 dataLen);



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


void tlkbt_hci_regEventCB(TlkBtHciEventCallback cb);
void tlkbt_hci_regAclDataCB(TlkBtHciAclDataCallback cb);
void tlkbt_hci_regUserCmdCB(TlkBtHciUserCmdCallback cb);

void tlkbt_hci_regSendCB(TlkBtHciSendCallback sendCB);
void tlkbt_hci_regResetCB(TlkBtHciResetCallback resetCB);

void tlkbt_hci_enableTxCritical(bool enable);
bool tlkbt_hci_TxCriticalIsenable(void);

bool tlkbt_hci_c2hFifoIsFull(void);
bool tlkbt_hci_c2hFifoIsEmpty(void);

bool tlkbt_hci_h2cFifoIsFull(void);
bool tlkbt_hci_h2cFifoIsEmpty(void);
bool tlkbt_hci_h2cFifoIsHalfFull(void);

uint tlkbt_hci_h2cFifoSize(void);
uint tlkbt_hci_h2cFifoNumb(void);
uint tlkbt_hci_h2cFifoUsed(void);
uint tlkbt_hci_h2cFifoUnused(void);

bool tlkbt_hci_h2cCmdFifoIsFull(void);
bool tlkbt_hci_h2cCmdFifoIsEmpty(void);
uint tlkbt_hci_h2cCmdFifoSize(void);
uint tlkbt_hci_h2cCmdFifoNumb(void);
uint tlkbt_hci_h2cCmdFifoUsed(void);
uint tlkbt_hci_h2cCmdFifoUnused(void);


uint08 *tlkbt_hci_getH2cBuff(void);
uint08 *tlkbt_hci_getC2hBuff(void);

void tlkbt_hci_c2hHandler(void);
void tlkbt_hci_h2cHandler(void);

void tlkbt_hci_exeCmdNow(void);
void tlkbt_hci_exeEvtNow(void);

int tlkbt_hci_sendH2cCmd(uint16 opcode, uint08 *pData, uint08 dataLen);
int tlkbt_hci_pushH2cExtCmd(uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen);
int tlkbt_hci_sendH2cAclData(uint16 aclHandle, uint08 *pUsrExt, uint16 extLen, 
	uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);

int tlkbt_hci_pushC2hData(uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
int tlkbt_hci_pushC2hExtData(uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen, uint08 *pExtData, uint16 extLen);
int tlkbt_hci_sendC2hScoData(uint16 scoHandle, uint08 *pData, uint08 dataLen);
int tlkbt_hci_sendC2hIsoData(uint16 isoHandle, uint08 *pData, uint16 dataLen);
int tlkbt_hci_sendC2hAclData(uint16 aclHandle, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen);
int tlkbt_hci_sendC2hEvent(uint08 evtID, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen);


void tlkbt_hci_recvHandler(uint08* pending_buf, uint32 pending_len);




typedef enum
{
    BTHCI_ECODE_NO_ERROR                        = 0x00,
    BTHCI_ECODE_UNKNOWN_HCI_COMMAND             = 0x01,
    BTHCI_ECODE_UNKNOWN_CONNECTION_ID           = 0x02,
    BTHCI_ECODE_HARDWARE_FAILURE                = 0x03,
    BTHCI_ECODE_PAGE_TIMEOUT                    = 0x04,
    BTHCI_ECODE_AUTH_FAILURE                    = 0x05,
    BTHCI_ECODE_PIN_OR_KEY_MISSING              = 0x06,
    BTHCI_ECODE_MEMORY_CAPA_EXCEED              = 0x07,
    BTHCI_ECODE_CONNECT_TIMEOUT                 = 0x08,
    BTHCI_ECODE_CONN_LIMIT_EXCEED               = 0x09,
    BTHCI_ECODE_SYNC_CONN_LIMIT_DEV_EXCEED      = 0x0A,
    BTHCI_ECODE_CONN_ALREADY_EXISTS             = 0x0B,
    BTHCI_ECODE_COMMAND_DISALLOWED              = 0x0C,
    BTHCI_ECODE_CONN_REJ_LIMITED_RESOURCES      = 0x0D,
    BTHCI_ECODE_CONN_REJ_SECURITY_REASONS       = 0x0E,
    BTHCI_ECODE_CONN_REJ_UNACCEPTABLE_BDADDR    = 0x0F,
    BTHCI_ECODE_CONN_ACCEPT_TIMEOUT_EXCEED      = 0x10,
    BTHCI_ECODE_UNSUPPORTED                     = 0x11,
    BTHCI_ECODE_INVALID_HCI_PARAM               = 0x12,
    BTHCI_ECODE_REMOTE_USER_TERM_CONN           = 0x13,
    BTHCI_ECODE_REMOTE_DEV_TERM_LOW_RESOURCES   = 0x14,
    BTHCI_ECODE_REMOTE_DEV_POWER_OFF            = 0x15,
    BTHCI_ECODE_CONN_TERM_BY_LOCAL_HOST         = 0x16,
    BTHCI_ECODE_REPEATED_ATTEMPTS               = 0x17,
    BTHCI_ECODE_PAIRING_NOT_ALLOWED             = 0x18,
    BTHCI_ECODE_UNKNOWN_LMP_PDU                 = 0x19,
    BTHCI_ECODE_UNSUPPORTED_REMOTE_FEATURE      = 0x1A,
    BTHCI_ECODE_SCO_OFFSET_REJECTED             = 0x1B,
    BTHCI_ECODE_SCO_INTERVAL_REJECTED           = 0x1C,
    BTHCI_ECODE_SCO_AIR_MODE_REJECTED           = 0x1D,
    BTHCI_ECODE_INVALID_LMP_PARAM               = 0x1E,
    BTHCI_ECODE_UNSPECIFIED_ERROR               = 0x1F,
    BTHCI_ECODE_UNSUPPORTED_LMP_PARAM_VALUE     = 0x20,
    BTHCI_ECODE_ROLE_CHANGE_NOT_ALLOWED         = 0x21,
    BTHCI_ECODE_LMP_RSP_TIMEOUT                 = 0x22,
    BTHCI_ECODE_LMP_COLLISION                   = 0x23,
    BTHCI_ECODE_LMP_PDU_NOT_ALLOWED             = 0x24,
    BTHCI_ECODE_ENC_MODE_NOT_ACCEPT             = 0x25,
    BTHCI_ECODE_LINK_KEY_CANT_CHANGE            = 0x26,
    BTHCI_ECODE_QOS_NOT_SUPPORTED               = 0x27,
    BTHCI_ECODE_INSTANT_PASSED                  = 0x28,
    BTHCI_ECODE_PAIRING_WITH_UNIT_KEY_NOT_SUPP  = 0x29,
    BTHCI_ECODE_DIFF_TRANSACTION_COLLISION      = 0x2A,
    BTHCI_ECODE_QOS_UNACCEPTABLE_PARAM          = 0x2C,
    BTHCI_ECODE_QOS_REJECTED                    = 0x2D,
    BTHCI_ECODE_CHANNEL_CLASS_NOT_SUPP          = 0x2E,
    BTHCI_ECODE_INSUFFICIENT_SECURITY           = 0x2F,
    BTHCI_ECODE_PARAM_OUT_OF_MAND_RANGE         = 0x30,
    BTHCI_ECODE_ROLE_SWITCH_PEND                = 0x32, /* LM_ROLE_SWITCH_PENDING               */
    BTHCI_ECODE_RESERVED_SLOT_VIOLATION         = 0x34, /* LM_RESERVED_SLOT_VIOLATION           */
    BTHCI_ECODE_ROLE_SWITCH_FAIL                = 0x35, /* LM_ROLE_SWITCH_FAILED                */
    BTHCI_ECODE_EIR_TOO_LARGE                   = 0x36, /* LM_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE */
    BTHCI_ECODE_SP_NOT_SUPPORTED_HOST           = 0x37,
    BTHCI_ECODE_HOST_BUSY_PAIRING               = 0x38,
    BTHCI_ECODE_CONTROLLER_BUSY                 = 0x3A,
    BTHCI_ECODE_UNACCEPTABLE_CONN_PARAM         = 0x3B,
    BTHCI_ECODE_ADV_TIMEOUT                     = 0x3C,
    BTHCI_ECODE_TERMINATED_MIC_FAILURE          = 0x3D,
    BTHCI_ECODE_CONN_FAILED_TO_BE_EST           = 0x3E,
    BTHCI_ECODE_CCA_REJ_USE_CLOCK_DRAG          = 0x40,
    BTHCI_ECODE_TYPE0_SUBMAP_NOT_DEFINED        = 0x41,
    BTHCI_ECODE_UNKNOWN_ADVERTISING_ID          = 0x42,
    BTHCI_ECODE_LIMIT_REACHED                   = 0x43,
    BTHCI_ECODE_OPERATION_CANCELED_BY_HOST      = 0x44,

	BTHCI_ECODE_TASK_ALREADY_EXIST			     = 0x80,
	BTHCI_ECODE_TASK_NOT_EXIST			     	 = 0x81,

    BTHCI_ECODE_UNDEFINED                       = 0xFF,


/*****************************************************
 ***              HW ERROR CODES                   ***
 *****************************************************/

    BTHCI_ECODE_HW_UART_OUT_OF_SYNC            = 0x00,
    BTHCI_ECODE_HW_MEM_ALLOC_FAIL              = 0x01,
}BT_HCI_ECODE_ENUM;



#endif // BT_HCI_H

