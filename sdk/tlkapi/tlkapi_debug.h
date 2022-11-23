/********************************************************************************************************
 * @file     tlkapi_debug.h
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

#ifndef TLKAPI_DEBUG_H
#define TLKAPI_DEBUG_H


#define TLKAPI_DBG_WARN_FLAG        0x02
#define TLKAPI_DBG_INFO_FLAG        0x04
#define TLKAPI_DBG_TRACE_FLAG       0x08
#define TLKAPI_DBG_ERROR_FLAG       0x10
#define TLKAPI_DBG_FATAL_FLAG       0x20
#define TLKAPI_DBG_ARRAY_FLAG       0x40
#define TLKAPI_DBG_ASSERT_FLAG      0x80
#define TLKAPI_DBG_FLAG_ALL         0xFE

#define tlkapi_warn      tlkapi_debug_warn
#define tlkapi_info      tlkapi_debug_info
#define tlkapi_trace     tlkapi_debug_trace
#define tlkapi_fatal     tlkapi_debug_fatal
#define tlkapi_error     tlkapi_debug_error
#define tlkapi_array     tlkapi_debug_array
#define tlkapi_assert    tlkapi_debug_assert
#define tlkapi_sprintf   tlkapi_debug_sprintf

#define tlkapi_sendStr(flags,pStr)                        tlkapi_debug_sendData(flags,pStr,0,0)
#define tlkapi_sendData(flags,pStr,pData,dataLen)         tlkapi_debug_sendData(flags,pStr,(uint08*)pData,dataLen)
#define tlkapi_sendU08s(flags,pStr,val0,val1,val2,val3)   tlkapi_debug_sendU08s(flags,pStr,(uint08)(val0),(uint08)(val1),(uint08)(val2),(uint08)(val3))
#define tlkapi_sendU16s(flags,pStr,val0,val1,val2,val3)   tlkapi_debug_sendU16s(flags,pStr,(uint16)(val0),(uint16)(val1),(uint16)(val2),(uint16)(val3))
#define tlkapi_sendU32s(flags,pStr,val0,val1,val2,val3)   tlkapi_debug_sendU32s(flags,pStr,(uint32)(val0),(uint32)(val1),(uint32)(val2),(uint32)(val3))


void tlkapi_debug_warn(uint flags, char *pSign, const char *format, ...);
void tlkapi_debug_info(uint flags, char *pSign, const char *format, ...);
void tlkapi_debug_trace(uint flags, char *pSign, const char *format, ...);
void tlkapi_debug_fatal(uint flags, char *pSign, const char *format, ...);
void tlkapi_debug_error(uint flags, char *pSign, const char *format, ...);
void tlkapi_debug_array(uint flags, char *pSign, char *pInfo, uint08 *pData, uint16 dataLen);
void tlkapi_debug_assert(uint flags, bool isAssert, char *pSign, const char *format, ...);
int  tlkapi_debug_sprintf(char *pOut, const char *format, ...);

void tlkapi_debug_sendData(uint flags, char *pStr, uint08 *pData, uint16 dataLen);
void tlkapi_debug_sendU08s(uint flags, void *pStr, uint08 val0, uint08 val1, uint08 val2, uint08 val3);
void tlkapi_debug_sendU16s(uint flags, void *pStr, uint16 val0, uint16 val1, uint16 val2, uint16 val3);
void tlkapi_debug_sendU32s(uint flags, void *pStr, uint32 val0, uint32 val1, uint32 val2, uint32 val3);

void tlkapi_debug_handler(void);

void tlkapi_debug_sendStatus(uint08 status, uint08 buffNumb, uint08 *pData, uint16 dataLen);
void tlkapi_debug_delayForPrint(uint32 us);


void tlkapi_vcd_ref(void);
void tlkapi_vcd_sync(bool enable);
void tlkapi_vcd_tick(uint flags, uint08 id);
void tlkapi_vcd_level(uint flags, uint08 id, uint08 level);
void tlkapi_vcd_event(uint flags, uint08 id);
void tlkapi_vcd_byte(uint flags, uint08 id, uint08 value);
void tlkapi_vcd_word(uint flags, uint08 id, uint16 value);



#if (TLK_CFG_DBG_ENABLE)


#define TLKAPI_DEBUG_CHANNEL_UDB      1
#define TLKAPI_DEBUG_CHANNEL_GSUART   2 //GPIO simulate UART
#define TLKAPI_DEBUG_CHANNEL_UART     3
#if (TLK_USB_UDB_ENABLE)
#define TLKAPI_DEBUG_CHANNEL          TLKAPI_DEBUG_CHANNEL_UDB
#else
#define TLKAPI_DEBUG_CHANNEL          TLKAPI_DEBUG_CHANNEL_GSUART
//#define TLKAPI_DEBUG_CHANNEL          TLKAPI_DEBUG_CHANNEL_UART
#endif

#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_GSUART)
#define TLKAPI_DEBUG_GPIO_PIN        GPIO_PD5
#define TLKAPI_DEBUG_BAUD_RATE       1000000
#endif

#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_UART)
#define TLKAPI_DEBUG_UART_PORT       UART0
#define TLKAPI_DEBUG_UART_TX_DMA     DMA7
#define TLKAPI_DEBUG_UART_TX_PIN     UART0_TX_PD2
#define TLKAPI_DEBUG_UART_BAUDRATE   921600
#endif


#define TLKAPI_WARN_HEAD       "<WARN>"
#define TLKAPI_INFO_HEAD       "<INFO>"
#define TLKAPI_TRACE_HEAD      "<TRACE>"
#define TLKAPI_FATAL_HEAD      "<FATAL>"
#define TLKAPI_ERROR_HEAD      "<ERROR>"
#define TLKAPI_ARRAY_HEAD      "<ARRAY>"
#define TLKAPI_ASSERT_HEAD     "<ASSERT>"


int  tlkapi_debug_init(void);

void tlkapi_debug_reset(void);

bool tlkapi_debug_isBusy(void);

#endif //#if (TLK_CFG_DBG_ENABLE)

#endif //TLKAPI_DEBUG_H

