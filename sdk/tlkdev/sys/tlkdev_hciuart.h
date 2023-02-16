/********************************************************************************************************
 * @file     tlkdev_hciuart.h
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

#ifndef TLKDEV_HCIUART_H
#define TLKDEV_HCIUART_H

#if (TLK_DEV_HCIUART_ENABLE) 




int  tlkdev_hciuart_init(void);
void tlkdev_hciuart_handler(void);



#define UART_DMA_SIZE  PENDING_BUF_LEN/4//  0x400


#define	UART_MANUAL_FLOW_CTR_RTS_STOP				gpio_set_high_level(UART_RTS_PIN)
#define	UART_MANUAL_FLOW_CTR_RTS_START				gpio_set_low_level(UART_RTS_PIN)



#define UART_DMA_TX     DMA3
#define UART_DMA_RX     DMA2


#endif

#endif //TLKDEV_HCIUART_H

