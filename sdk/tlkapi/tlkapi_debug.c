/********************************************************************************************************
 * @file     tlkapi_debug.c
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
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "string.h"
#include "drivers.h"
#include "tlk_config.h"
#include "tlk_debug.h"
#include "tlkapi/tlkapi_type.h"
#include "tlkapi/tlkapi_error.h"
#include "tlkapi/tlkapi_qfifo.h"
#include "tlkapi/tlkapi_debug.h"
#include "tlkapi/tlkapi_string.h"


#if (TLK_CFG_DBG_ENABLE)


#define TLKAPI_DEBUG_ITEM_SIZE       146
#define TLKAPI_DEBUG_ITEM_NUMB       16


extern void tlk_debug_init(void);
extern bool tlk_debug_dbgIsEnable(unsigned int flags, unsigned int printFlag);
#if (TLK_CFG_VCD_ENABLE)
extern bool tlk_debug_vcdIsEnable(unsigned int flags);
#endif
extern bool tlk_debug_dbgIsEnable1(unsigned int flags);
extern char *tlk_debug_getDbgSign(unsigned int flags);

#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_UDB)
extern int tlkusb_udb_sendData(uint08 *pData, uint08 dataLen);
#endif
#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_GSUART)
static void tlkapi_debug_putchar(uint08 byte);
#endif
static void tlkapi_debug_common(uint flags, char *pSign, char *pHead, const char *format, va_list args);


static uint16 sTlkApiDebugSerial;
static uint08 sTlkApiDebugBuffer[(TLKAPI_DEBUG_ITEM_SIZE+2)*TLKAPI_DEBUG_ITEM_NUMB];
static tlkapi_qfifo_t sTlkApiDebugFifo;

#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_UDB)
static uint16 sTlkApiDebugOffset;
#endif
#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_GSUART)
static uint32 sTlkApiDebugGpioPin;
static uint32 sTlkApiDebugBitIntv;
#endif
#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_UART)
static uint08 sTlkApiUartSendIsBusy = false;
#endif
#if (TLK_CFG_VCD_ENABLE)
static void tlkapi_vcd_process(void);
#endif


int tlkapi_debug_init(void)
{
	tlk_debug_init();
	tlkapi_qfifo_init(&sTlkApiDebugFifo, TLKAPI_DEBUG_ITEM_NUMB, (TLKAPI_DEBUG_ITEM_SIZE+2),
		sTlkApiDebugBuffer, (TLKAPI_DEBUG_ITEM_SIZE+2)*TLKAPI_DEBUG_ITEM_NUMB);

	#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_GSUART)
		sTlkApiDebugGpioPin = TLKAPI_DEBUG_GPIO_PIN;
		sTlkApiDebugBitIntv = SYSTEM_TIMER_TICK_1S/TLKAPI_DEBUG_BAUD_RATE;
		gpio_function_en(sTlkApiDebugGpioPin);
		gpio_set_up_down_res(sTlkApiDebugGpioPin, GPIO_PIN_PULLUP_1M);
		gpio_output_en(sTlkApiDebugGpioPin);
		gpio_set_high_level(sTlkApiDebugGpioPin);
	#endif
	#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_UART)
		unsigned short div;
		unsigned char bwpc;

		sTlkApiUartSendIsBusy = false;
		
		uart_reset(TLKAPI_DEBUG_UART_PORT);
		
		gpio_set_up_down_res(TLKAPI_DEBUG_UART_TX_PIN, GPIO_PIN_PULLUP_10K);
		gpio_input_en(TLKAPI_DEBUG_UART_TX_PIN);
		#if (TLKAPI_DEBUG_UART_TX_PIN == TLKAPI_DEBUG_UART_TX_PIN)
		unsigned char mask = (unsigned char)~(BIT(5)|BIT(4));
		reg_gpio_func_mux(TLKAPI_DEBUG_UART_TX_PIN)=(reg_gpio_func_mux(TLKAPI_DEBUG_UART_TX_PIN) & mask);
		#else
		#error "Refer 'uart_set_fuc_pin' to select and set the PIN"
		#endif
		gpio_function_dis(TLKAPI_DEBUG_UART_TX_PIN);
		
		uart_cal_div_and_bwpc(TLKAPI_DEBUG_UART_BAUDRATE, sys_clk.pclk*1000*1000, &div, &bwpc);
		uart_init(TLKAPI_DEBUG_UART_PORT, div, bwpc, UART_PARITY_NONE, UART_STOP_BIT_ONE);	
		uart_set_dma_rx_timeout(TLKAPI_DEBUG_UART_PORT, bwpc, 12, UART_BW_MUL1);		

		uart_set_tx_dma_config(TLKAPI_DEBUG_UART_PORT, TLKAPI_DEBUG_UART_TX_DMA);
		
		uart_clr_tx_done(TLKAPI_DEBUG_UART_PORT);

		dma_clr_irq_mask(TLKAPI_DEBUG_UART_TX_DMA, TC_MASK|ABT_MASK|ERR_MASK);

		uart_set_irq_mask(TLKAPI_DEBUG_UART_PORT, UART_TXDONE_MASK); 
		
		plic_interrupt_enable(IRQ19_UART0);
		plic_set_priority(IRQ19_UART0, 2);
	#endif
	
	return TLK_ENONE;
}
void tlkapi_debug_deinit(void)
{
	
}

int tlkapi_debug_sprintf(char *pOut, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	return vsprintf(pOut, format, args);
}

_attribute_noinline_
void tlkapi_debug_warn(uint flags, char *pSign, const char *format, ...)
{
	if(!tlk_debug_dbgIsEnable(flags, TLK_DEBUG_DBG_FLAG_WARN)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	va_list args;
	va_start(args, format);
	tlkapi_debug_common(flags, pSign, TLKAPI_WARN_HEAD, format, args);
	va_end(args);
}
_attribute_noinline_
void tlkapi_debug_info(uint flags, char *pSign, const char *format, ...)
{
	if(!tlk_debug_dbgIsEnable(flags, TLK_DEBUG_DBG_FLAG_INFO)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	va_list args;
	va_start(args, format);
	tlkapi_debug_common(flags, pSign, TLKAPI_INFO_HEAD, format, args);
	va_end(args);
}
_attribute_noinline_
void tlkapi_debug_trace(uint flags, char *pSign, const char *format, ...)
{
	if(!tlk_debug_dbgIsEnable(flags, TLK_DEBUG_DBG_FLAG_TRACE)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	va_list args;
	va_start(args, format);
	tlkapi_debug_common(flags, pSign, TLKAPI_TRACE_HEAD, format, args);
	va_end(args);
}
_attribute_noinline_
void tlkapi_debug_fatal(uint flags, char *pSign, const char *format, ...)
{
	if(!tlk_debug_dbgIsEnable(flags, TLK_DEBUG_DBG_FLAG_FATAL)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	va_list args;
	va_start(args, format);
	tlkapi_debug_common(flags, pSign, TLKAPI_FATAL_HEAD, format, args);
	va_end(args);
}
_attribute_noinline_
void tlkapi_debug_error(uint flags, char *pSign, const char *format, ...)
{
	if(!tlk_debug_dbgIsEnable(flags, TLK_DEBUG_DBG_FLAG_ERROR)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	va_list args;
	va_start(args, format);
	tlkapi_debug_common(flags, pSign, TLKAPI_ERROR_HEAD, format, args);
	va_end(args);
}
_attribute_noinline_
void tlkapi_debug_array(uint flags, char *pSign, char *pInfo, uint08 *pData, uint16 dataLen)
{
	uint08 *pBuff;
	uint16 index;
	uint16 serial;
	
	if(!tlk_debug_dbgIsEnable(flags, TLK_DEBUG_DBG_FLAG_ARRAY)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);

	pBuff = tlkapi_qfifo_getBuff(&sTlkApiDebugFifo);
	if(pBuff == nullptr) return;
	
	serial = sTlkApiDebugSerial ++;
	printf("[%04x]",serial);
	if(pSign != nullptr) printf(pSign);
	printf(TLKAPI_ARRAY_HEAD);
	if(pInfo != nullptr) printf(pInfo);
	printf("(%d)", dataLen);
	for(index=0; index<dataLen; index++){
		printf("%02x ", pData[index]);
	}
	
	dataLen = ((uint16)pBuff[1] << 8) | pBuff[0];
	if(dataLen != 0) tlkapi_qfifo_dropBuff(&sTlkApiDebugFifo);
}
_attribute_noinline_
void tlkapi_debug_assert(uint flags, bool isAssert, char *pSign, const char *format, ...)
{
	if(!tlk_debug_dbgIsEnable(flags, TLK_DEBUG_DBG_FLAG_ASSERT)) return;
	uint08 count = 8;
	while(count--){
		tlkapi_debug_sendData(flags, (char*)format, 0, 0);
		tlkapi_debug_delayForPrint(100000);
	}
	while(true){}
}

bool tlkapi_debug_isBusy(void)
{
	#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_UART)
	if(sTlkApiUartSendIsBusy) return true;
	#endif
	if(sTlkApiDebugFifo.wptr == sTlkApiDebugFifo.rptr) return false;
	return true;
}


#if (TLK_USB_UDB_ENABLE)
_attribute_ram_code_sec_noinline_
#endif
void tlkapi_debug_reset(void)
{
	sTlkApiDebugFifo.wptr = 0;
	sTlkApiDebugFifo.rptr = 0;
	sTlkApiDebugFifo.full = false;	
}


#if (TLK_USB_UDB_ENABLE)
_attribute_ram_code_sec_noinline_ 
#endif
void tlkapi_debug_handler(void)
{
#if (TLK_CFG_VCD_ENABLE)
	tlkapi_vcd_process();
#endif
#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_UDB)
	int ret;
	uint08 *pData;
	uint16 offset;
	uint08 sendLen;
	uint16 dataLen;
	
	pData = tlkapi_qfifo_getData(&sTlkApiDebugFifo);
	if(pData == nullptr) return;
	
	dataLen = ((uint16)pData[1] << 8) | pData[0];
	if(dataLen == 0 || dataLen+2 > TLKAPI_DEBUG_ITEM_SIZE || sTlkApiDebugOffset >= dataLen){
		sTlkApiDebugOffset = 0;
		tlkapi_qfifo_dropData(&sTlkApiDebugFifo);
		return;
	}
	
	offset = 2+sTlkApiDebugOffset;
	if(sTlkApiDebugOffset+64 <= dataLen) sendLen = 64;
	else sendLen = dataLen-sTlkApiDebugOffset;

	ret = tlkusb_udb_sendData(pData+offset, sendLen);
	if(ret != -TLK_EBUSY) sTlkApiDebugOffset += sendLen;
		
	if(sTlkApiDebugOffset >= dataLen){
		pData[0] = 0x00;
		pData[1] = 0x00;
		pData[2] = 0x00;
		pData[3] = 0x00;
		sTlkApiDebugOffset = 0;
		tlkapi_qfifo_dropData(&sTlkApiDebugFifo);
	}
#elif(TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_GSUART)
	int index;
	uint08 *pData;
	uint16 dataLen;
	
	pData = tlkapi_qfifo_getData(&sTlkApiDebugFifo);
	if(pData == nullptr) return;
	
	dataLen = ((uint16)pData[1] << 8) | pData[0];
	if(dataLen > TLKAPI_DEBUG_ITEM_SIZE) dataLen = TLKAPI_DEBUG_ITEM_SIZE;
	for(index=0; index<dataLen; index++){
		tlkapi_debug_putchar(pData[2+index]);
	}
	pData[0] = 0x00;
	pData[1] = 0x00;
	tlkapi_qfifo_dropData(&sTlkApiDebugFifo);
#elif(TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_UART)
	if(!sTlkApiUartSendIsBusy && !tlkapi_qfifo_isEmpty(&sTlkApiDebugFifo)){
		uint08 * pData = tlkapi_qfifo_getData(&sTlkApiDebugFifo);
		if(pData == nullptr || (pData[0] == 0 && pData[1] == 0) || pData[2] != 0 || pData[3] != 0){
			if(pData != nullptr){
				pData[0] = 0; pData[1] = 0; pData[2] = 0; pData[3] = 0;
			}
			tlkapi_qfifo_dropData(&sTlkApiDebugFifo);
		}else{
			uint16 dataLen = ((uint16)pData[1] << 8) | pData[0];
			if(dataLen == 0 || dataLen > TLKAPI_DEBUG_ITEM_SIZE){
				tlkapi_qfifo_dropData(&sTlkApiDebugFifo);
			}else{
				sTlkApiUartSendIsBusy = true;
				uart_send_dma(TLKAPI_DEBUG_UART_PORT, pData+4, dataLen);
			}
		}
	}
#endif
}

_attribute_ram_code_sec_noinline_ 
void tlkapi_debug_sendData(uint flags, char *pStr, uint08 *pData, uint16 dataLen)
{
	uint08 *pBuff;
	uint16 strLen;
	uint16 tempVar;
	uint16 buffLen;
	uint16 serial;
	uint08 signLen;
	char *pDbgSign;

	if(!tlk_debug_dbgIsEnable1(flags)) return;
	

	serial = sTlkApiDebugSerial ++;
	
	pBuff = tlkapi_qfifo_takeBuff(&sTlkApiDebugFifo);
	if(pBuff == nullptr) return;

	strLen = tstrlen(pStr);	
	pDbgSign = tlk_debug_getDbgSign(flags);
	if(pDbgSign == nullptr) signLen = 0;
	else signLen = tstrlen(pDbgSign);

	buffLen = 0;
	pBuff[buffLen++] = 0x00; //(5+extLen+strLen+dataLen) & 0xFF;
	pBuff[buffLen++] = 0x00; //((5+extLen+strLen+dataLen) & 0xFF00) >> 8;
	#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_UDB)
	pBuff[buffLen++] = 0x82;
	pBuff[buffLen++] = 0x08;
	pBuff[buffLen++] = 0x22;
	pBuff[buffLen++] = 0x00;
	pBuff[buffLen++] = 0x00;
	#elif (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_UART)
	pBuff[buffLen++] = 0x00; //(5+extLen+strLen+dataLen) & 0xFF;
	pBuff[buffLen++] = 0x00; //((5+extLen+strLen+dataLen) & 0xFF00) >> 8;
	#endif
	// Add Serial
	pBuff[buffLen++] = '[';
	tempVar = (serial & 0xF000)>>12;
	if(tempVar <= 9) tempVar = '0'+tempVar;
	else tempVar = 'a'+(tempVar-10);
	pBuff[buffLen++] = tempVar;
	tempVar = (serial & 0x0F00)>>8;
	if(tempVar <= 9) tempVar = '0'+tempVar;
	else tempVar = 'a'+(tempVar-10);
	pBuff[buffLen++] = tempVar;
	tempVar = (serial & 0x00F0)>>4;
	if(tempVar <= 9) tempVar = '0'+tempVar;
	else tempVar = 'a'+(tempVar-10);
	pBuff[buffLen++] = tempVar;
	tempVar = (serial & 0x000F)>>0;
	if(tempVar <= 9) tempVar = '0'+tempVar;
	else tempVar = 'a'+(tempVar-10);
	pBuff[buffLen++] = tempVar;
	pBuff[buffLen++] = ']';
	
	if(buffLen+signLen+16 > TLKAPI_DEBUG_ITEM_SIZE) return;
	if(signLen != 0){
		tmemcpy(pBuff+buffLen, pDbgSign, signLen);
		buffLen += signLen;
	}
	
	if(buffLen+strLen+4 > TLKAPI_DEBUG_ITEM_SIZE) strLen = TLKAPI_DEBUG_ITEM_SIZE-buffLen-4;
	if(strLen != 0){
		tmemcpy(pBuff+buffLen, pStr, strLen);
		buffLen += strLen;
	}
	if(dataLen != 0){
		pBuff[buffLen++] = ':';
		tempVar = tlkapi_arrayToStr(pData, dataLen, (char*)(pBuff+buffLen), TLKAPI_DEBUG_ITEM_SIZE-2-buffLen, ' ');
		buffLen += tempVar;
	}
	#if (TLKAPI_DEBUG_CHANNEL != TLKAPI_DEBUG_CHANNEL_UDB)
	pBuff[buffLen++] = '\r';
	pBuff[buffLen++] = '\n';
	#endif
	pBuff[0] = ((buffLen-2) & 0x00FF);
	pBuff[1] = ((buffLen-2) & 0xFF00) >> 8;
}

#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_UDB)
_attribute_retention_code_ 
void tlkapi_debug_sendStatus(uint08 status, uint08 buffNumb, uint08 *pData, uint16 dataLen)
{
	uint08 *pBuff;
	uint16 buffLen;

	pBuff = tlkapi_qfifo_takeBuff(&sTlkApiDebugFifo);
	if(pBuff == nullptr) return;

	if(dataLen+2 > TLKAPI_DEBUG_ITEM_SIZE) dataLen = TLKAPI_DEBUG_ITEM_SIZE-2;
	
	buffLen = 0;
	pBuff[buffLen++] = (dataLen+2) & 0xFF;
	pBuff[buffLen++] = ((dataLen+2) & 0xFF00) >> 8;
	pBuff[buffLen++] = status;
	pBuff[buffLen++] = buffNumb;
	if(dataLen != 0){
		tmemcpy(pBuff+buffLen, pData, dataLen);
		buffLen += dataLen;
	}
}
#elif (TLK_USB_UDB_ENABLE)
_attribute_retention_code_ 
void tlkapi_debug_sendStatus(uint08 status, uint08 buffNumb, uint08 *pData, uint16 dataLen)
{
	
}
#endif
_attribute_ram_code_sec_noinline_ 
void tlkapi_debug_sendU08s(uint flags, void *pStr, uint08 val0, uint08 val1, uint08 val2, uint08 val3)
{
	uint08 buffer[4];
	buffer[0] = val0;
	buffer[1] = val1;
	buffer[2] = val2;
	buffer[3] = val3;
	tlkapi_debug_sendData(flags, pStr, (uint08*)buffer, 4);
}
_attribute_ram_code_sec_noinline_ 
void tlkapi_debug_sendU16s(uint flags, void *pStr, uint16 val0, uint16 val1, uint16 val2, uint16 val3)
{
	uint16 buffer[4];
	buffer[0] = val0;
	buffer[1] = val1;
	buffer[2] = val2;
	buffer[3] = val3;
	tlkapi_debug_sendData(flags, pStr, (uint08*)buffer, 8);
}
_attribute_ram_code_sec_noinline_ 
void tlkapi_debug_sendU32s(uint flags, void *pStr, uint32 val0, uint32 val1, uint32 val2, uint32 val3)
{
	uint32 buffer[4];
	buffer[0] = val0;
	buffer[1] = val1;
	buffer[2] = val2;
	buffer[3] = val3;
	tlkapi_debug_sendData(flags, pStr, (uint08*)buffer, 16);
}


void tlkapi_debug_delayForPrint(uint32 us)
{
	uint32 timer = clock_time();
	while(true){
		#if (TLK_USB_UDB_ENABLE)
		extern void tlkusb_process(void);
		tlkusb_process();
		#endif
		tlkapi_debug_handler();
		if(clock_time_exceed(timer, us)) break;
	}
}

static void tlkapi_debug_common(uint flags, char *pSign, char *pHead, const char *format, va_list args)
{
	uint08 *pBuff;
	uint16 serial;
	uint16 dataLen;

	serial = sTlkApiDebugSerial ++;

	pBuff = tlkapi_qfifo_getBuff(&sTlkApiDebugFifo);
	if(pBuff == nullptr) return;
	
	printf("[%04x]",serial);
	if(pSign != nullptr) printf(pSign);
	if(pHead != nullptr) printf(pHead);
	vprintf(format, args);
	#if (TLKAPI_DEBUG_CHANNEL != TLKAPI_DEBUG_CHANNEL_UDB)
	printf("\r\n");
	#endif
	
	dataLen = ((uint16)pBuff[1] << 8) | pBuff[0];
	if(dataLen != 0) tlkapi_qfifo_dropBuff(&sTlkApiDebugFifo);
}



#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_GSUART)
static void tlkapi_debug_putchar(uint08 byte)
{
	uint08 index = 0;
	uint32 time1 = 0;
	uint32 time2 = 0;
	uint08 bits[10] = {0};
	uint08 bit0 = reg_gpio_out(sTlkApiDebugGpioPin) & (~(sTlkApiDebugGpioPin & 0xff));
	uint08 bit1 = reg_gpio_out(sTlkApiDebugGpioPin) | (sTlkApiDebugGpioPin & 0xff);

	bits[0] = bit0;
	bits[1] = (byte & 0x01)? bit1 : bit0;
	bits[2] = ((byte>>1) & 0x01)? bit1 : bit0;
	bits[3] = ((byte>>2) & 0x01)? bit1 : bit0;
	bits[4] = ((byte>>3) & 0x01)? bit1 : bit0;
	bits[5] = ((byte>>4) & 0x01)? bit1 : bit0;
	bits[6] = ((byte>>5) & 0x01)? bit1 : bit0;
	bits[7] = ((byte>>6) & 0x01)? bit1 : bit0;
	bits[8] = ((byte>>7) & 0x01)? bit1 : bit0;
	bits[9] = bit1;

	uint32 r = core_disable_interrupt();
	time1 = reg_system_tick;
	for(index=0; index<10; index++){
		time2 = time1;
		while(time1-time2 < sTlkApiDebugBitIntv){
			time1 = reg_system_tick;
		}
		reg_gpio_out(sTlkApiDebugGpioPin) = bits[index];
	}
	core_restore_interrupt(r);
}
#endif

#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_UART)
_attribute_ram_code_sec_ 
void uart0_irq_handler(void)
{
	if(uart_get_irq_status(TLKAPI_DEBUG_UART_PORT, UART_TXDONE)){
	    uart_clr_tx_done(TLKAPI_DEBUG_UART_PORT);
		if(sTlkApiUartSendIsBusy){
			sTlkApiUartSendIsBusy = false;
			uint08 *pData = tlkapi_qfifo_getData(&sTlkApiDebugFifo);
			if(pData != nullptr){
				pData[0] = 0; pData[1] = 0; pData[2] = 0; pData[3] = 0;
			}
			tlkapi_qfifo_dropData(&sTlkApiDebugFifo);
			pData = tlkapi_qfifo_getData(&sTlkApiDebugFifo);
			if(pData == nullptr || (pData[0] == 0 && pData[1] == 0) || pData[2] != 0 || pData[3] != 0){
				if(pData != nullptr){
					pData[0] = 0; pData[1] = 0; pData[2] = 0; pData[3] = 0;
				}
				tlkapi_qfifo_dropData(&sTlkApiDebugFifo);
			}else{
				uint16 dataLen = ((uint16)pData[1] << 8) | pData[0];
				if(dataLen == 0 || dataLen > TLKAPI_DEBUG_ITEM_SIZE){
					tlkapi_qfifo_dropData(&sTlkApiDebugFifo);
				}else{
					sTlkApiUartSendIsBusy = true;
					uart_send_dma(TLKAPI_DEBUG_UART_PORT, pData+4, dataLen);
				}
			}
		}
	}
}
#endif


#endif

__attribute__((used)) int _write(int fd, const unsigned char *buf, int size)
{
#if (TLK_CFG_DBG_ENABLE)
	uint08 *pBuff;
	uint16 copyLen;
	uint16 dataLen;
	uint08 headLen;
		
	(void)fd;
	
	pBuff = tlkapi_qfifo_getBuff(&sTlkApiDebugFifo);
	if(pBuff == nullptr) return size;
	
	dataLen = ((uint16)pBuff[1] << 8) | pBuff[0];
	if(dataLen != 0){
		#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_UART)
		dataLen += 4;
		#else
		dataLen += 2;
		#endif
		#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_UDB)
		if(dataLen < 7) return size;
		#endif
	}else{
		#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_UART)
		headLen = 4;
		#else
		headLen = 2;
		#endif
		#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_UDB)
		pBuff[headLen++] = 0x82;
		pBuff[headLen++] = 0x08;
		pBuff[headLen++] = 0x22;
		pBuff[headLen++] = 0x00;
		pBuff[headLen++] = 0x00;
		#endif
		dataLen += headLen;
	}
	
	if(dataLen >= TLKAPI_DEBUG_ITEM_SIZE) return size;
	if(dataLen+size <= TLKAPI_DEBUG_ITEM_SIZE) copyLen = size;
	else copyLen = TLKAPI_DEBUG_ITEM_SIZE-dataLen;
	if(copyLen != 0) tmemcpy(pBuff+dataLen, buf, copyLen);
	dataLen += copyLen;
	
	pBuff[0] = ((dataLen-2) & 0x00FF);
	pBuff[1] = ((dataLen-2) & 0xFF00) >> 8;
#endif
    return size;
}

#if (TLK_CFG_VCD_ENABLE)

#if (TLK_USB_VCD_ENABLE)
#define tlkapi_vcd_rcd(d)   reg_usb_ep8_dat=d   //VCD Record
#else
#define tlkapi_vcd_rcd
#endif

static uint32 sTlkApiVcdTicks;

_attribute_ram_code_sec_noinline_
void tlkapi_vcd_ref(void)
{
	uint32 r = core_disable_interrupt();
	tlkapi_vcd_rcd(0x20);
	int t=clock_time();
	tlkapi_vcd_rcd(t);
	tlkapi_vcd_rcd(t>>8);
	tlkapi_vcd_rcd(t>>16);
	core_restore_interrupt(r);
}
// 4-byte sync word: 00 00 00 00
_attribute_ram_code_sec_noinline_
void tlkapi_vcd_sync(bool enable)
{
	if(enable){
		uint32 r = core_disable_interrupt();
		tlkapi_vcd_rcd(0);
		tlkapi_vcd_rcd(0);
		tlkapi_vcd_rcd(0);
		tlkapi_vcd_rcd(0);
		core_restore_interrupt(r);
	}
}      
//4-byte (001_id-5bits) id0: timestamp align with hardware gpio output; id1-31: user define
_attribute_ram_code_sec_noinline_
void tlkapi_vcd_tick(uint flags, uint08 id)
{
	if(tlk_debug_vcdIsEnable(flags)){
		uint32 r = core_disable_interrupt();
		tlkapi_vcd_rcd(0x20|(id&31));
		int t=clock_time();
		tlkapi_vcd_rcd(t);
		tlkapi_vcd_rcd(t>>8);
		tlkapi_vcd_rcd(t>>16);
		core_restore_interrupt(r);
	}
}
//1-byte (01x_id-5bits) 1-bit data: b=0 or 1.
_attribute_ram_code_sec_noinline_
void tlkapi_vcd_level(uint flags, uint08 id, uint08 level)
{
	if(tlk_debug_vcdIsEnable(flags)){
		uint32 r = core_disable_interrupt();
		tlkapi_vcd_rcd(((level)?0x60:0x40)|(id&31));
		int t=clock_time();
		tlkapi_vcd_rcd(t);
		tlkapi_vcd_rcd(t>>8);
		tlkapi_vcd_rcd(t>>16);
		core_restore_interrupt(r);
	}
}
//1-byte (000_id-5bits)
_attribute_ram_code_sec_noinline_
void tlkapi_vcd_event(uint flags, uint08 id)
{
	if(tlk_debug_vcdIsEnable(flags)){
		uint32 r = core_disable_interrupt();
		tlkapi_vcd_rcd(0x00|(id&31));
		core_restore_interrupt(r);
	}
}
//2-byte (10-id-6bits) 8-bit data
_attribute_ram_code_sec_noinline_
void tlkapi_vcd_byte(uint flags, uint08 id, uint08 value)
{
	if(tlk_debug_vcdIsEnable(flags)){
		uint32 r = core_disable_interrupt();
		tlkapi_vcd_rcd(0x80|(id&63));
		tlkapi_vcd_rcd(value);
		core_restore_interrupt(r);
	}
}
//3-byte (11-id-6bits) 16-bit data
_attribute_ram_code_sec_noinline_
void tlkapi_vcd_word(uint flags, uint08 id, uint16 value)
{
	if(tlk_debug_vcdIsEnable(flags)){
		uint32 r = core_disable_interrupt();
		tlkapi_vcd_rcd(0xc0|(id&63));
		tlkapi_vcd_rcd(value);
		tlkapi_vcd_rcd((value)>>8);
		core_restore_interrupt(r);
	}
}
static void tlkapi_vcd_process(void)
{
	if(sTlkApiVcdTicks == 0 || clock_time_exceed(sTlkApiVcdTicks, 10000)){
		sTlkApiVcdTicks = clock_time() | 1;
		tlkapi_vcd_ref();
		tlkapi_vcd_sync(true); //SL_STACK_VCD_EN
	}
}

#endif


#if !(TLK_CFG_DBG_ENABLE)

void tlkapi_debug_handler(void)
{

}
_attribute_ram_code_sec_noinline_
void tlkapi_debug_default(void)
{
	
}
void __attribute__((unused))tlkapi_debug_warn(uint flags, char *pSign, const char *format, ...) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkapi_debug_info(uint flags, char *pSign, const char *format, ...) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkapi_debug_trace(uint flags, char *pSign, const char *format, ...) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkapi_debug_fatal(uint flags, char *pSign, const char *format, ...) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkapi_debug_error(uint flags, char *pSign, const char *format, ...) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkapi_debug_array(uint flags, char *pSign, char *pInfo, uint08 *pData, uint16 dataLen) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkapi_debug_assert(uint flags, bool isAssert, char *pSign, const char *format, ...) __attribute__((weak, alias("tlkapi_debug_default")));
int  __attribute__((unused))tlkapi_debug_sprintf(char *pOut, const char *format, ...) __attribute__((weak, alias("tlkapi_debug_default")));

void __attribute__((unused))tlkapi_debug_sendData(uint flags, char *pStr, uint08 *pData, uint16 dataLen) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkapi_debug_sendU08s(uint flags, void *pStr, uint08 val0, uint08 val1, uint08 val2, uint08 val3) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkapi_debug_sendU16s(uint flags, void *pStr, uint16 val0, uint16 val1, uint16 val2, uint16 val3) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkapi_debug_sendU32s(uint flags, void *pStr, uint32 val0, uint32 val1, uint32 val2, uint32 val3) __attribute__((weak, alias("tlkapi_debug_default")));

void __attribute__((unused))tlkapi_debug_sendStatus(uint08 status, uint08 buffNumb, uint08 *pData, uint16 dataLen) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkapi_debug_delayForPrint(uint32 us) __attribute__((weak, alias("tlkapi_debug_default")));

#endif


#if !(TLK_CFG_VCD_ENABLE)

_attribute_ram_code_sec_noinline_
void tlkapi_vcd_default(void){}
void __attribute__((unused))tlkapi_vcd_ref(void) __attribute__((weak, alias("tlkapi_vcd_default")));
void __attribute__((unused))tlkapi_vcd_sync(bool enable) __attribute__((weak, alias("tlkapi_vcd_default")));
void __attribute__((unused))tlkapi_vcd_tick(uint flags, uint08 id) __attribute__((weak, alias("tlkapi_vcd_default")));
void __attribute__((unused))tlkapi_vcd_level(uint flags, uint08 id, uint08 level) __attribute__((weak, alias("tlkapi_vcd_default")));
void __attribute__((unused))tlkapi_vcd_event(uint flags, uint08 id) __attribute__((weak, alias("tlkapi_vcd_default")));
void __attribute__((unused))tlkapi_vcd_byte(uint flags, uint08 id, uint08 value) __attribute__((weak, alias("tlkapi_vcd_default")));
void __attribute__((unused))tlkapi_vcd_word(uint flags, uint08 id, uint16 value) __attribute__((weak, alias("tlkapi_vcd_default")));


#endif


