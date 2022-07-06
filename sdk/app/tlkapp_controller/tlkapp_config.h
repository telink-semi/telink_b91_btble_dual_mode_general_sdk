/********************************************************************************************************
 * @file     tlkapp_config.h
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
#ifndef TLKAPP_CONFIG_H
#define TLKAPP_CONFIG_H




#define TLKAPP_DBG_ENABLE       1
#define TLKAPP_DBG_FLAG         (TLKAPP_DBG_ENABLE | TLKAPI_DBG_FLAG_ALL)
#define TLKAPP_DBG_SIGN         "[APP]"


#define TLKAPP_WAKEUP_PIN        GPIO_PE1


#define TLKAPP_MEM_TOTAL_SIZE         (40*1024) //MP3(36740)+SrcEnc(4048) = 36740+4048=40788  -- Worst scenario: Music playing on the headphone





/**********************************************************************************
 *BLE configuration
***********************************************************************************/
#define	MAX_BT_ACL_LINK							2				//BT LINK:  acl
#define	MAX_BLE_LINK							2				//BLE LINK: adv & acl
#define	EM_BT_AUDIOBUF_SIZE						256
#define ACL_DATA_BUF_SIZE            			800


#define BLMS_MAX_CONN_MASTER_NUM				0
#define	BLMS_MAX_CONN_SLAVE_NUM					1


#define TLK_STK_BT_ENABLE        1


/******************************************************************************
 * Macro: TLKAPP_ENABLE
 * Descr: WON'T CHANGE
*******************************************************************************/
#ifndef TLKAPP_GENERAL_ENABLE
#define TLKAPP_GENERAL_ENABLE    0
#endif
#ifndef TLKAPP_CONTROLLER_ENABLE
#define TLKAPP_CONTROLLER_ENABLE 0
#endif
/******************************************************************************
 * Macro: TLK_CONTROLLER_ENABLE
 * Descr:
*******************************************************************************/
#define TLKAPP_HCI_UART_MODE      1
#define TLKAPP_HCI_USB_MODE       0

#if TLKAPP_HCI_UART_MODE
#define TLK_DEV_SERIAL_ENABLE        (0)
//baudrate of UART
#define UART_BAUDRATE   115200
/*! HCI Transport configuration  */
#if 0//UART0
#define UART_ID         UART0
#define UART_TX_PIN     UART0_TX_PD2
#define UART_RX_PIN     UART0_RX_PD3
#define UART_CTS_PIN    UART0_CTS_PA1
#define UART_RTS_PIN    UART0_RTS_PA2
#define UART_IRQ        IRQ19_UART0
#define UART_IRQHandler uart0_irq_handler
#else
#define UART_ID         UART1
#define UART_TX_PIN     UART1_TX_PE0
#define UART_RX_PIN     UART1_RX_PE2
#define UART_CTS_PIN    UART1_CTS_PE1
#define UART_RTS_PIN    UART1_RTS_PE3
#define UART_IRQ        IRQ18_UART1
#define UART_IRQHandler uart1_irq_handler
#endif
/////////////////////  FLOW CONTROL //////////////////////////////////
#define	UART_FLOW_CTR				0
#define CTS_STOP_VOLT               1 //0 :Low level stops TX.  1 :High level stops TX.
#endif

/******************************************************************************
 * Macro: CONTROLLER MOUDLE CONFIGE
 * Descr:
*******************************************************************************/
#define TLK_AFH_ENABLE                        (1&&TLK_STK_BT_ENABLE)
#define TLK_AFH_CLS_ENABLE                    (1&&TLK_STK_BT_ENABLE)
#define TLK_PWC_ENABLE                        (1&&TLK_STK_BT_ENABLE)
#define TLK_SNIFF_ENABLE                      (1&&TLK_STK_BT_ENABLE)
#define TLK_SCO_ENABLE                        (1&&TLK_STK_BT_ENABLE)




#define TLK_CFG_FS_ENABLE        0
#define TLK_STK_BTH_ENABLE       0
#define TLK_STK_BTP_ENABLE       0



#endif //TLKAPP_CONFIG_H