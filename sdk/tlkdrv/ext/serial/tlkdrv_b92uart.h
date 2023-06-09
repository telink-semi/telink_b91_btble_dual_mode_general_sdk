/********************************************************************************************************
 * @file     tlkdrv_b92uart.h
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
#ifndef TLKDRV_B92_UART_H
#define TLKDRV_B92_UART_H
#if (TLKDRV_SERIAL_B92_UART_EANBLE)



int tlkdrv_b92uart_open(uint08 port, uint32 baudRate, uint16 txPin,
	uint16 rxPin, uint08 txDma, uint08 rxDma, uint16 rtsPin, uint16 ctsPin);

int tlkdrv_b92uart_close(uint08 port, uint08 txDma, uint08 rxDma);


int tlkdrv_b92uart_sendWithoutDma(uint08 port, uint08 rtsPin, uint08 *pData, uint16 dataLen);

void tlkdrv_b92uart_irqHandler(uint08 port);


#endif //#if (TLKDRV_SERIAL_B92_UART_EANBLE)
#endif //TLKDRV_B92_UART_H

