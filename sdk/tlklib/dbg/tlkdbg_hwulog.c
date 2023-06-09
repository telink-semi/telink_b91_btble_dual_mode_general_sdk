/********************************************************************************************************
 * @file     tlkdbg_hwulog.c
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
#include "tlk_config.h"
#if (TLK_CFG_DBG_ENABLE)
#include <stdio.h>
#include <stdarg.h>
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdbg.h"
#include "tlkdbg_config.h"
#if (TLKDBG_CFG_HWU_LOG_ENABLE)
#include "tlkdbg_define.h"
#include "tlkdbg_hwulog.h"
#include "drivers.h"

//HWU - GPIO simulate UART

#define TLKDBG_HWULOG_NEWLINE_MODE1_ENABLE         1


static uint08 sTlkDbgHwuLogIsBusy;
static uint16 sTlkDbgHwuLogSerial;
static uint08 sTlkDbgHwuLogCache[TLKDBG_HWU_LOG_CACHE_SIZE+4];
static uint08 sTlkDbgHwuLogBuffer[TLKDBG_HWU_LOG_BUFFER_SIZE];
__attribute__((aligned(4)))
static uint08 sTlkDbgHwuLogSendBuff[TLKDBG_HWU_LOG_SND_CACHE_SIZE+4];
static tlkapi_fifo_t sTlkDbgHwuLogFifo;


void tlkdbg_hwulog_init(void)
{
	unsigned short div;
	unsigned char bwpc;

	sTlkDbgHwuLogIsBusy = false;
	sTlkDbgHwuLogSerial = 0;
	tlkapi_fifo_init(&sTlkDbgHwuLogFifo, false, false, sTlkDbgHwuLogBuffer, TLKDBG_HWU_LOG_BUFFER_SIZE);
		
	uart_reset(TLKDBG_HWULOG_UART_PORT);
	uart_set_pin(TLKDBG_HWULOG_UART_PORT, TLKDBG_HWULOG_UART_TX_PIN, GPIO_NONE_PIN);
	
	uart_cal_div_and_bwpc(TLKDBG_HWULOG_UART_BAUDRATE, sys_clk.pclk*1000*1000, &div, &bwpc);
	uart_init(TLKDBG_HWULOG_UART_PORT, div, bwpc, UART_PARITY_NONE, UART_STOP_BIT_ONE);	
	
	uart_clr_irq_status(TLKDBG_HWULOG_UART_PORT, UART_TXDONE_IRQ_STATUS);
	uart_set_irq_mask(TLKDBG_HWULOG_UART_PORT, UART_TXDONE_MASK); 
	uart_set_irq_mask(TLKDBG_HWULOG_UART_PORT, UART_ERR_IRQ_MASK);
}

void tlkdbg_hwulog_reset(void)
{
	sTlkDbgHwuLogIsBusy = false;
	sTlkDbgHwuLogSerial = 0;
	tlkapi_fifo_clear(&sTlkDbgHwuLogFifo);
}

bool tlkdbg_hwulog_isBusy(void)
{
	if(tlkapi_fifo_isEmpty(&sTlkDbgHwuLogFifo)) return false;
	else return true;
}


void tlkdbg_hwulog_handler(void)
{
	int ret;
	if(!tlkapi_fifo_isEmpty(&sTlkDbgHwuLogFifo)){
		ret = tlkapi_fifo_read(&sTlkDbgHwuLogFifo, sTlkDbgHwuLogSendBuff, TLKDBG_HWU_LOG_SND_CACHE_SIZE);
		if(ret > 0){
			uart_send(TLKDBG_HWULOG_UART_PORT, sTlkDbgHwuLogSendBuff, ret);
		}
	}
}


void tlkdbg_hwulog_print(char *pSign, char *pHead, char *fileName, uint lineNumb, const char *format, va_list args)
{
	uint16 serial;
	uint16 dataLen;

	serial = sTlkDbgHwuLogSerial ++;

	tlkdbg_setPrintBuffer(sTlkDbgHwuLogCache, TLKDBG_HWU_LOG_CACHE_SIZE);
	
	printf("[%04x]",serial);
	if(pSign != nullptr) printf(pSign);
	if(pHead != nullptr) printf(pHead);
	if(fileName != nullptr){
		printf("(%s//%03d)", fileName, lineNumb);
	}
	vprintf(format, args);

	tlkdbg_setPrintBuffer(nullptr, 0);
	
	uint32 r = core_disable_interrupt();
	dataLen = ((uint16)sTlkDbgHwuLogCache[1] << 8) | sTlkDbgHwuLogCache[0];
	#if (TLKDBG_HWULOG_NEWLINE_MODE1_ENABLE)
	sTlkDbgHwuLogCache[dataLen+2+0] = '\r';
	sTlkDbgHwuLogCache[dataLen+2+1] = '\n';
	tlkapi_fifo_write(&sTlkDbgHwuLogFifo, sTlkDbgHwuLogCache+2, dataLen+2);
	#else
	sTlkDbgHwuLogCache[dataLen+2+0] = '\n';
	tlkapi_fifo_write(&sTlkDbgHwuLogFifo, sTlkDbgHwuLogCache+2, dataLen+1);
	#endif
	core_restore_interrupt(r);
}

void tlkdbg_hwulog_array(char *pSign, char *pHead, char *fileName, uint lineNumb, const char *format, uint08 *pData, uint16 dataLen)
{
	uint16 index;
	uint16 serial;
	uint16 optLen;

	serial = sTlkDbgHwuLogSerial ++;

	if(tlkapi_fifo_idleLen(&sTlkDbgHwuLogFifo) < 128) return;
	
	tlkdbg_setPrintBuffer(sTlkDbgHwuLogCache, TLKDBG_HWU_LOG_CACHE_SIZE);
		
	printf("[%04x]",serial);
	if(pSign != nullptr) printf(pSign);
	if(pHead != nullptr) printf(pHead);
	if(fileName != nullptr){
		printf("(%s//%03d)", fileName, lineNumb);
	}
	printf("(%d)", dataLen);
	for(index=0; index<dataLen; index++){
		printf("%02x ", pData[index]);
	}
	
	tlkdbg_setPrintBuffer(nullptr, 0);
	
	uint32 r = core_disable_interrupt();
	optLen = ((uint16)sTlkDbgHwuLogCache[1] << 8) | sTlkDbgHwuLogCache[0];
	#if (TLKDBG_HWULOG_NEWLINE_MODE1_ENABLE)
	sTlkDbgHwuLogCache[optLen+2+0] = '\r';
	sTlkDbgHwuLogCache[optLen+2+1] = '\n';
	tlkapi_fifo_write(&sTlkDbgHwuLogFifo, sTlkDbgHwuLogCache+2, optLen+2);
	#else
	sTlkDbgHwuLogCache[optLen+2+0] = '\n';
	tlkapi_fifo_write(&sTlkDbgHwuLogFifo, sTlkDbgHwuLogCache+2, optLen+1);
	#endif
	core_restore_interrupt(r);
}




_attribute_ram_code_sec_noinline_ 
void tlkdbg_hwulog_sendU08s(char *pSign, void *pStr, uint08 val0, uint08 val1, uint08 val2, uint08 val3)
{
	uint08 buffer[4];
	buffer[0] = val0;
	buffer[1] = val1;
	buffer[2] = val2;
	buffer[3] = val3;
	tlkdbg_hwulog_sendData(pSign, pStr, (uint08*)buffer, 4);
}
_attribute_ram_code_sec_noinline_ 
void tlkdbg_hwulog_sendU16s(char *pSign, void *pStr, uint16 val0, uint16 val1, uint16 val2, uint16 val3)
{
	uint16 buffer[4];
	buffer[0] = val0;
	buffer[1] = val1;
	buffer[2] = val2;
	buffer[3] = val3;
	tlkdbg_hwulog_sendData(pSign, pStr, (uint08*)buffer, 8);
}
_attribute_ram_code_sec_noinline_ 
void tlkdbg_hwulog_sendU32s(char *pSign, void *pStr, uint32 val0, uint32 val1, uint32 val2, uint32 val3)
{
	uint32 buffer[4];
	buffer[0] = val0;
	buffer[1] = val1;
	buffer[2] = val2;
	buffer[3] = val3;
	tlkdbg_hwulog_sendData(pSign, pStr, (uint08*)buffer, 16);
}


_attribute_ram_code_sec_noinline_ 
void tlkdbg_hwulog_sendData(char *pSign, char *pStr, uint08 *pData, uint16 dataLen)
{
	uint16 strLen;
	uint16 tempVar;
	uint16 buffLen;
	uint16 serial;
	uint08 signLen;
	uint08 pBuff[TLKDBG_HWU_LOG_IRQ_CACHE_SIZE+2];

	serial = sTlkDbgHwuLogSerial ++;
	
	strLen = tstrlen(pStr);	
	if(pSign == nullptr) signLen = 0;
	else signLen = tstrlen(pSign);

	buffLen = 0;
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
	
	if(buffLen+signLen+16 > TLKDBG_HWU_LOG_IRQ_CACHE_SIZE) return;
	if(signLen != 0){
		tmemcpy(pBuff+buffLen, pSign, signLen);
		buffLen += signLen;
	}
	
	if(buffLen+strLen+4 > TLKDBG_HWU_LOG_IRQ_CACHE_SIZE) strLen = TLKDBG_HWU_LOG_IRQ_CACHE_SIZE-buffLen-4;
	if(strLen != 0){
		tmemcpy(pBuff+buffLen, pStr, strLen);
		buffLen += strLen;
	}
	if(dataLen != 0){
		pBuff[buffLen++] = ':';
		tempVar = tlkapi_arrayToStr(pData, dataLen, (char*)(pBuff+buffLen), TLKDBG_HWU_LOG_IRQ_CACHE_SIZE-2-buffLen, ' ');
		buffLen += tempVar;
	}
	uint32 r = core_disable_interrupt();
	#if (TLKDBG_HWULOG_NEWLINE_MODE1_ENABLE)
	pBuff[buffLen++] = '\r';
	#endif
	pBuff[buffLen++] = '\n';
	tlkapi_fifo_write(&sTlkDbgHwuLogFifo, pBuff, buffLen);	
	core_restore_interrupt(r);
}




#endif //#if (TLKDBG_CFG_HWU_LOG_ENABLE)
#endif //#if (TLK_CFG_DBG_ENABLE)
