/********************************************************************************************************
 * @file     tlkmmi_lemstAcl.h
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

#ifndef TLKMMI_LEMST_ACL_H
#define TLKMMI_LEMST_ACL_H

#if (TLKMMI_LEMST_ENABLE)


#define TLKMMI_LEMST_OTA_CLIENT_ENABLE         0
#define TLKMMI_LEMST_MASTER_SMP_ENABLE         0



#define TLKMMI_LEMST_MASTER_MAX_NUM         1
#define TLKMMI_LEMST_SLAVE_MAX_NUM          0
#define TLKMMI_LEMST_MAX_CONN_NUM           (TLKMMI_LEMST_MASTER_MAX_NUM + TLKMMI_LEMST_SLAVE_MAX_NUM)

/*
 * @brief	user use this macro to enable or disable
 * 			workaround solution caused by "(ACL_xxx_TX_FIFO_SIZE * (ACL_xxx_TX_FIFO_NUM - 1)) must be less than 4096 (4K)"
 *          CAL_LL_ACL_TX_FIFO_SIZE(230) = 240, CAL_LL_ACL_TX_FIFO_SIZE(231) = 256
 *          when ACL_xxx_MAX_TX_OCTETS > 230, ACL_xxx_TX_FIFO_SIZE >= 256, ACL_xxx_TX_FIFO_NUM can only be 9
 *          if you want TX FIFO 16 or 32, can enable this macro
 */
#define TLKMMI_LEMST_WORKAROUND_TX_FIFO_4K_LIMITATION_EN          0

/**
 * @brief	connMaxRxOctets
 * refer to BLE SPEC "4.5.10 Data PDU length management" & "2.4.2.21 LL_LENGTH_REQ and LL_LENGTH_RSP"
 * usage limitation:
 * 1. should be in range of 27 ~ 251
 */
#define TLKMMI_LEMST_CONN_MAX_RX_OCTETS			27

/**
 * @brief	connMaxTxOctets
 * refer to BLE SPEC "4.5.10 Data PDU length management" & "2.4.2.21 LL_LENGTH_REQ and LL_LENGTH_RSP"
 *  in this SDK, we separate this value into 2 parts: slaveMaxTxOctets and masterMaxTxOctets,
 *  for purpose to save some SRAM costed by when slave and master use different connMaxTxOctets.
 * usage limitation:
 * 1. slaveMaxTxOctets and masterMaxTxOctets should be in range of 27 ~ 251
 */
#define TLKMMI_LEMST_MASTER_MAX_TX_OCTETS		27
#define TLKMMI_LEMST_SLAVE_MAX_TX_OCTETS			27

/********************* ACL connection LinkLayer TX & RX data FIFO allocation, Begin ************************************************/
/**
 * @brief	ACL RX buffer size & number
 *  		ACL RX buffer is shared by all connections to hold LinkLayer RF RX data.
 * usage limitation for TLKMMI_LEMST_ACL_RX_FIFO_SIZE:
 * 1. should be greater than or equal to (connMaxRxOctets + 21)
 * 2. should be be an integer multiple of 16 (16 Byte align)
 * 3. user can use formula:  size = CAL_LL_ACL_RX_FIFO_SIZE(connMaxRxOctets)
 * usage limitation for TLKMMI_LEMST_ACL_RX_FIFO_NUM:
 * 1. must be: 2^n, (power of 2)
 * 2. at least 4; recommended value: 8, 16
 */
#define TLKMMI_LEMST_ACL_RX_FIFO_SIZE               CAL_LL_ACL_RX_FIFO_SIZE(TLKMMI_LEMST_CONN_MAX_RX_OCTETS) // TLKMMI_LEMST_CONN_MAX_RX_OCTETS + 21, then 16 Byte align
#define TLKMMI_LEMST_ACL_RX_FIFO_NUM                8 // must be: 2^n


/**
 * @brief	ACL TX buffer size & number
 *  		ACL MASTER TX buffer is shared by all master connections to hold LinkLayer RF TX data.
*			ACL SLAVE  TX buffer is shared by all slave  connections to hold LinkLayer RF TX data.
 * usage limitation for TLKMMI_LEMST_ACL_MASTER_TX_FIFO_SIZE:
 * 1. should be greater than or equal to (connMaxTxOctets + 10)
 * 2. should be be an integer multiple of 16 (16 Byte align)
 * 3. user can use formula:  size = CAL_LL_ACL_TX_FIFO_SIZE(connMaxTxOctets)
 * usage limitation for TLKMMI_LEMST_ACL_MASTER_TX_FIFO_NUM:
 * 1. must be: (2^n) + 1, (power of 2, then add 1)
 * 2. at least 9; recommended value: 9, 17, 33; other value not allowed.
 * usage limitation for size * (number - 1)
 * (TLKMMI_LEMST_ACL_MASTER_TX_FIFO_SIZE * (TLKMMI_LEMST_ACL_MASTER_TX_FIFO_NUM - 1)) must be less than 4096 (4K)
 *    so when ACL TX FIFO size equal to or bigger than 256, ACL TX FIFO number can only be 9(can not use 17 or 33), cause 256*(17-1)=4096
 */
#define TLKMMI_LEMST_ACL_MASTER_TX_FIFO_SIZE        48 // TLKMMI_LEMST_MASTER_MAX_TX_OCTETS + 10, then 16 Byte align
#define TLKMMI_LEMST_ACL_MASTER_TX_FIFO_NUM         9  // must be: (2^n) + 1

/******************** ACL connection LinkLayer TX & RX data FIFO allocation, End ***************************************************/

/***************** ACL connection L2CAP layer MTU TX & RX data FIFO allocation, Begin ********************************/

/*Note:
 * if support LE Secure Connections, L2CAP buffer must >= 72.([64+6]+3)/4*4), 4B align.
 * MTU Buff size = Extra_Len(6)+ ATT_MTU_MAX
 *  1. should be greater than or equal to (ATT_MTU + 6)
 *  2. should be be an integer multiple of 4 (4 Byte align)
 */
#define TLKMMI_LEMST_ATT_MTU_MASTER_RX_MAX_SIZE     23
#define	TLKMMI_LEMST_MTU_M_BUFF_SIZE_MAX            CAL_MTU_BUFF_SIZE(TLKMMI_LEMST_ATT_MTU_MASTER_RX_MAX_SIZE)


typedef struct{
	uint16 connHandle;
	uint08 connAddr[6];
	uint08 isStart;
}tlkmmi_lemst_acl_t;

int tlkmmi_lemst_aclInit(void);


int tlkmmi_lemst_disconn(uint16 handle, uint08 *pPeerAddr);

int tlkmmi_lemst_setAclAddr(uint08 *pAddr, uint08 addrLen);

bool tlkmmi_lemst_aclVolumeInc(void);
bool tlkmmi_lemst_aclVolumeDec(void);


#endif //TLKMMI_LEMST_ENABLE

#endif //TLKMMI_LEMST_ACL_H

