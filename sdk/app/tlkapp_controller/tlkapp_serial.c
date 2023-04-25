/********************************************************************************************************
 * @file     tlkapp_serial.c
 *
 * @brief    This is the source file for BTBLE SDK
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
#include "types.h"
#include "drivers.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkapp_config.h"
#include "tlkapp_serial.h"
#include "tlkdev/sys/tlkdev_serial.h"
#include "tlkstk/hci/bt_hci.h"


static void tlkapp_serial_input(uint08 *pData, uint16 dataLen);
static int  tlkapp_serial_send(uint08 *pData, uint16 dataLen);
static void tlkapp_serial_reset(void);

__attribute__((aligned(4)))
static uint08 sTlkAppSerialRecvBuffer[TLKAPP_SERIAL_RBUFF_NUMB*(TLKAPP_SERIAL_RBUFF_SIZE+4)];
__attribute__((aligned(4)))
static uint08 sTlkAppSerialSendBuffer[TLKAPP_SERIAL_SBUFF_NUMB*(TLKAPP_SERIAL_SBUFF_SIZE+4)];


int tlkapp_serial_init(void)
{
	tlkdev_serial_mount(TLKAPP_SERIAL_PORT, TLKAPP_SERIAL_BAUDRATE,
		TLKAPP_SERIAL_TX_PIN, TLKAPP_SERIAL_RX_PIN,
		TLKAPP_SERIAL_TX_DMA, TLKAPP_SERIAL_RX_DMA, 0, 0);
	tlkdev_serial_setTxQFifo(TLKAPP_SERIAL_SBUFF_NUMB, TLKAPP_SERIAL_SBUFF_SIZE+4,
		sTlkAppSerialSendBuffer, TLKAPP_SERIAL_SBUFF_NUMB*(TLKAPP_SERIAL_SBUFF_SIZE+4));
	tlkdev_serial_setRxQFifo(TLKAPP_SERIAL_RBUFF_NUMB, TLKAPP_SERIAL_RBUFF_SIZE+4,
		sTlkAppSerialRecvBuffer, TLKAPP_SERIAL_RBUFF_NUMB*(TLKAPP_SERIAL_RBUFF_SIZE+4));
	tlkdev_serial_regCB(tlkapp_serial_input);
	tlkdev_serial_open();

	tlkbt_hci_regSendCB(tlkapp_serial_send);
	tlkbt_hci_regResetCB(tlkapp_serial_reset);

	return TLK_ENONE;
}

void tlkapp_serial_handler(void)
{
	tlkdev_serial_handler();
}


static void tlkapp_serial_input(uint08 *pData, uint16 dataLen)
{
	tlkbt_hci_recvHandler(pData, dataLen);
}

static int tlkapp_serial_send(uint08 *pData, uint16 dataLen)
{
	return tlkdev_serial_send(pData, dataLen);
}
static void tlkapp_serial_reset(void)
{
	tlkdev_serial_close();
	tlkdev_serial_open();
}

