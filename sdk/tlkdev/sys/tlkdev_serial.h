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


typedef void(*TlkDevSerialRecvCB)(uint08 *pData, uint16 dataLen);


int tlkdev_serial_init(void);

int tlkdev_serial_mount(uint08 port, uint32 baudRate, uint16 txPin,
	uint16 rxPin, uint08 txDma, uint08 rxDma, uint16 rtsPin, uint16 ctsPin);
int tlkdev_serial_unmount(void);

int tlkdev_serial_setBaudrate(uint32 baudRate);

int tlkdev_serial_setRxFifo(uint08 *pBuffer, uint16 buffLen);
int tlkdev_serial_setTxQFifo(uint16 fnumb, uint16 fsize, uint08 *pBuffer, uint16 buffLen);
int tlkdev_serial_setRxQFifo(uint16 fnumb, uint16 fsize, uint08 *pBuffer, uint16 buffLen);

int tlkdev_serial_open(void);
int tlkdev_serial_close(void);

int tlkdev_serial_send(uint08 *pData, uint16 dataLen);
int tlkdev_serial_read(uint08 *pBuff, uint16 buffLen);

void tlkdev_serial_clear(void);
void tlkdev_serial_regCB(TlkDevSerialRecvCB cb);
bool tlkdev_serial_sfifoIsMore60(uint16 dataLen);
bool tlkdev_serial_sfifoIsMore80(uint16 dataLen);

bool tlkdev_serial_isBusy(void);
void tlkdev_serial_wakeup(uint wakeSrc);

void tlkdev_serial_handler(void);




#endif //TLK_DEV_SERIAL_ENABLE

#endif //TLKDEV_SERIAL_H

