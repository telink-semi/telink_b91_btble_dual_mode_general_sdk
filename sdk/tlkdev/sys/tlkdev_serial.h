/********************************************************************************************************
 * @file     tlkdev_serial.h
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

#ifndef TLKDEV_SERIAL_H
#define TLKDEV_SERIAL_H

#if (TLK_DEV_SERIAL_ENABLE)


typedef void(*tlkdev_serial_recvCB)(uint08 *pFrame, uint16 frmLen);


int  tlkdev_serial_init(void);

void tlkdev_serial_regCB(tlkdev_serial_recvCB cb);

bool tlkdev_serial_isBusy(void);
void tlkdev_serial_wakeup(void);

void tlkdev_serial_handler(void);

int  tlkdev_serial_send(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen);
void tlkdev_serial_setBaudrate(uint32 baudrate);

#else //TLK_DEV_SERIAL_ENABLE

#include "tlkstk/hci/bt_hci.h"

#if TLKAPP_HCI_UART_MODE
int  tlkdev_serial_init(void);
void tlkdev_serial_handler(void);
//typedef int(*tlkdev_serial_sendCB)(uint08 *pFrame, uint16 frmLen);
void tlkdev_serial_regCB(tlk_hci_serial_sendCB cb);


#define UART_DMA_SIZE  PENDING_BUF_LEN/4//  0x400


#define	UART_MANUAL_FLOW_CTR_RTS_STOP				gpio_set_high_level(UART_RTS_PIN)
#define	UART_MANUAL_FLOW_CTR_RTS_START				gpio_set_low_level(UART_RTS_PIN)



#define UART_DMA_TX     DMA3
#define UART_DMA_RX     DMA2
#endif
#endif

#endif //TLKDEV_SERIAL_H

