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


#define TLKDEV_SERIAL_PORT               UART1
#define TLKDEV_SERIAL_TX_PIN             UART1_TX_PD6
#define TLKDEV_SERIAL_RX_PIN             UART1_RX_PD7
#define TLKDEV_SERIAL_BAUDRATE           1500000//921600//115200

#define TLKDEV_SERIAL_DMA_ENABLE         1

#define TLKDEV_SERIAL_DMA_TX             DMA4
#define TLKDEV_SERIAL_DMA_RX             DMA5


typedef void(*tlkdev_serial_recvCB)(uint08 *pFrame, uint16 frmLen);


int  tlkdev_serial_init(void);

void tlkdev_serial_clear(void);

void tlkdev_serial_regCB(tlkdev_serial_recvCB cb);

bool tlkdev_serial_isBusy(void);
void tlkdev_serial_wakeup(void);

void tlkdev_serial_handler(void);

int  tlkdev_serial_send(uint08 *pData, uint16 dataLen);

uint tlkdev_serial_sfifoSingleLen(void);
bool tlkdev_serial_sfifoIsMore60(uint16 dataLen);
bool tlkdev_serial_sfifoIsMore80(uint16 dataLen);

void tlkdev_serial_setRecvFifo(tlkapi_qfifo_t *pFifo);
void tlkdev_serial_setBaudrate(uint32 baudrate);



#endif //TLK_DEV_SERIAL_ENABLE

#endif //TLKDEV_SERIAL_H

