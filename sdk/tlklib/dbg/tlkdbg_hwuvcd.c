/********************************************************************************************************
 * @file	tlkdbg_hwuvcd.c
 *
 * @brief	This is the source file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *
 *******************************************************************************************************/
#include "tlk_config.h"
#if (TLK_CFG_VCD_ENABLE)
#include <stdio.h>
#include <stdarg.h>
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdbg.h"
#include "tlkdbg_config.h"
#if (TLKDBG_CFG_HWU_VCD_ENABLE)
#include "tlkdbg_define.h"
#include "tlkdbg_hwuvcd.h"
#include "tlksys/prt/tlkpto_stdio.h"
#include "drivers.h"


//HWU - GPIO simulate UART

static void tlkdbg_hwuvcd_send(uint08 *pData, uint16 dataLen);
static void tlkdbg_hwuvcd_putchar(uint08 byte);
//static bool tlkdbg_hwuvcd_timer(tlkapi_timer_t *pTimer, uint32 userArg);

static uint32 sTlkDbgHwuVcdGpioPin;
static uint32 sTlkDbgHwuVcdBitIntv;

static uint08 sTlkdbgHwuVcdBuffer[TLKDBG_HWU_VCD_BUFFER_SIZE];
static tlkapi_fifo_t sTlkDbgHwuVcdFifo;
//static tlkapi_timer_t sTlkDbgHwuVcdTimer;



void tlkdbg_hwuvcd_init(void)
{
	tlkapi_fifo_init(&sTlkDbgHwuVcdFifo, false, false, sTlkdbgHwuVcdBuffer, TLKDBG_HWU_VCD_BUFFER_SIZE);
//	tlkapi_timer_initNode(&sTlkDbgHwuVcdTimer, tlkdbg_hwuvcd_timer, nullptr, 1000);

	sTlkDbgHwuVcdGpioPin = TLKDBG_HWU_VCD_GPIO_PIN;
	sTlkDbgHwuVcdBitIntv = SYSTEM_TIMER_TICK_1S/TLKDBG_HWU_VCD_BAUD_RATE;
	gpio_function_en(sTlkDbgHwuVcdGpioPin);
	gpio_set_up_down_res(sTlkDbgHwuVcdGpioPin, GPIO_PIN_PULLUP_1M);
	gpio_output_en(sTlkDbgHwuVcdGpioPin);
	gpio_set_high_level(sTlkDbgHwuVcdGpioPin);
}


void tlkdbg_hwuvcd_reset(void)
{
	tlkapi_fifo_clear(&sTlkDbgHwuVcdFifo);
}
bool tlkdbg_hwuvcd_isBusy(void)
{
	if(tlkapi_fifo_isEmpty(&sTlkDbgHwuVcdFifo)) return false;
	else return true;
}
void tlkdbg_hwuvcd_handler(void)
{
	int ret;
	uint08 buffer[TLKDBG_HWU_VCD_CACHE_SIZE];
	while(!tlkapi_fifo_isEmpty(&sTlkDbgHwuVcdFifo)){
		ret = tlkapi_fifo_read(&sTlkDbgHwuVcdFifo, buffer, TLKDBG_HWU_VCD_CACHE_SIZE);
		if(ret <= 0) break;
		tlkdbg_hwuvcd_send(buffer, ret);
	}
}

//static bool tlkdbg_hwuvcd_timer(tlkapi_timer_t *pTimer, uint32 userArg)
//{
//	tlkdbg_hwuvcd_handler();
//	return true;
//}


_attribute_ram_code_sec_noinline_
void tlkdbg_hwuvcd_ref(void)
{
	uint08 buffLen = 0;
	uint08 buffer[4];
	core_interrupt_disable();
	int t=clock_time();
	buffer[buffLen++] = 0x20;
	buffer[buffLen++] = t & 0xFF;
	buffer[buffLen++] = (t & 0xFF00) >> 8;
	buffer[buffLen++] = (t & 0xFF0000) >> 16;
	tlkapi_fifo_write(&sTlkDbgHwuVcdFifo, buffer, buffLen);
	core_interrupt_restore();
}
// 4-byte sync word: 00 00 00 00
_attribute_ram_code_sec_noinline_
void tlkdbg_hwuvcd_sync(void)
{
	uint08 buffLen = 0;
	uint08 buffer[4];
	core_interrupt_disable();
	buffer[buffLen++] = 0x00;
	buffer[buffLen++] = 0x00;
	buffer[buffLen++] = 0x00;
	buffer[buffLen++] = 0x00;
	tlkapi_fifo_write(&sTlkDbgHwuVcdFifo, buffer, buffLen);
	core_interrupt_restore();
}      
//4-byte (001_id-5bits) id0: timestamp align with hardware gpio output; id1-31: user define
_attribute_ram_code_sec_noinline_
void tlkdbg_hwuvcd_tick(uint08 id)
{
	uint08 buffLen = 0;
	uint08 buffer[4];
	core_interrupt_disable();
	int t=clock_time();
	buffer[buffLen++] = 0x20 | (id&31);
	buffer[buffLen++] = t & 0xFF;
	buffer[buffLen++] = (t & 0xFF00) >> 8;
	buffer[buffLen++] = (t & 0xFF0000) >> 16;
	tlkapi_fifo_write(&sTlkDbgHwuVcdFifo, buffer, buffLen);
	core_interrupt_restore();
}
//1-byte (01x_id-5bits) 1-bit data: b=0 or 1.
_attribute_ram_code_sec_noinline_
void tlkdbg_hwuvcd_level(uint08 id, uint08 level)
{
	uint08 buffLen = 0;
	uint08 buffer[4];
	core_interrupt_disable();
	int t=clock_time();
	buffer[buffLen++] = ((level) ? 0x60:0x40) | (id&31);
	buffer[buffLen++] = t & 0xFF;
	buffer[buffLen++] = (t & 0xFF00) >> 8;
	buffer[buffLen++] = (t & 0xFF0000) >> 16;
	tlkapi_fifo_write(&sTlkDbgHwuVcdFifo, buffer, buffLen);
	core_interrupt_restore();
}
//1-byte (000_id-5bits)
_attribute_ram_code_sec_noinline_
void tlkdbg_hwuvcd_event(uint08 id)
{
	uint08 buffLen = 0;
	uint08 buffer[4];
	core_interrupt_disable();
	buffer[buffLen++] = 0x00 | (id&31);
	tlkapi_fifo_write(&sTlkDbgHwuVcdFifo, buffer, buffLen);
	core_interrupt_restore();
}
//2-byte (10-id-6bits) 8-bit data
_attribute_ram_code_sec_noinline_
void tlkdbg_hwuvcd_byte(uint08 id, uint08 value)
{
	uint08 buffLen = 0;
	uint08 buffer[4];
	core_interrupt_disable();
	buffer[buffLen++] = 0x80 | (id&63);
	buffer[buffLen++] = value;
	tlkapi_fifo_write(&sTlkDbgHwuVcdFifo, buffer, buffLen);
	core_interrupt_restore();
}
//3-byte (11-id-6bits) 16-bit data
_attribute_ram_code_sec_noinline_
void tlkdbg_hwuvcd_word(uint08 id, uint16 value)
{
	uint08 buffLen = 0;
	uint08 buffer[4];
	core_interrupt_disable();
	buffer[buffLen++] = 0x80 | (id&63);
	buffer[buffLen++] = value & 0xFF;
	buffer[buffLen++] = (value & 0xFF00) >> 8;
	tlkapi_fifo_write(&sTlkDbgHwuVcdFifo, buffer, buffLen);
	core_interrupt_restore();
}


static void tlkdbg_hwuvcd_send(uint08 *pData, uint16 dataLen)
{
	uint16 index;
	for(index=0; index<dataLen; index++){
		tlkdbg_hwuvcd_putchar(pData[index]);
	}
}
static void tlkdbg_hwuvcd_putchar(uint08 byte)
{
	uint08 index = 0;
	uint32 time1 = 0;
	uint32 time2 = 0;
	uint08 bits[10] = {0};
	uint08 bit0 = reg_gpio_out(sTlkDbgHwuVcdGpioPin) & (~(sTlkDbgHwuVcdGpioPin & 0xff));
	uint08 bit1 = reg_gpio_out(sTlkDbgHwuVcdGpioPin) | (sTlkDbgHwuVcdGpioPin & 0xff);

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

	core_interrupt_disable();
	time1 = reg_system_tick;
	for(index=0; index<10; index++){
		time2 = time1;
		while(time1-time2 < sTlkDbgHwuVcdBitIntv){
			time1 = reg_system_tick;
		}
		reg_gpio_out(sTlkDbgHwuVcdGpioPin) = bits[index];
	}
	core_interrupt_restore();
}




#endif //#if (TLKDBG_CFG_HWU_VCD_ENABLE)
#endif //#if (TLK_CFG_VCD_ENABLE)

