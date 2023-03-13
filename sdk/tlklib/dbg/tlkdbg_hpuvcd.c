/********************************************************************************************************
 * @file     tlkdbg_hpuvcd.c
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
#if (TLK_CFG_VCD_ENABLE)
#include <stdio.h>
#include <stdarg.h>
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdbg.h"
#include "tlkdbg_config.h"
#if (TLKDBG_CFG_HPU_VCD_ENABLE)
#include "tlkdbg_define.h"
#include "tlkdbg_hpuvcd.h"
#include "tlksys/prt/tlkpto_stdio.h"
#include "drivers.h"


//HPU - Hardware Protocol UART

extern uint tlkmdi_comm_getSingleDatPktUnitLen(void);
extern uint tlkdev_serial_sfifoSingleLen(void);
extern bool tlkdev_serial_sfifoIsMore80(uint16 dataLen);
extern int  tlkmdi_comm_sendDat(uint08 datID, uint16 numb, uint08 *pData, uint16 dataLen);

bool tlkdbg_hpuvcd_timer(tlkapi_timer_t *pTimer, uint32 userArg);

static uint32 sTlkDbgHpuVcdTicks;
static uint08 sTlkdbgHpuVcdBuffer[TLKDBG_HPU_VCD_BUFFER_SIZE];
static tlkapi_fifo_t sTlkDbgHpuVcdFifo;
//static tlkapi_timer_t sTlkDbgHpuVcdTimer;


void tlkdbg_hpuvcd_init(void)
{
	tlkapi_fifo_init(&sTlkDbgHpuVcdFifo, false, false, sTlkdbgHpuVcdBuffer, TLKDBG_HPU_VCD_BUFFER_SIZE);
//	tlkapi_timer_initNode(&sTlkDbgHpuVcdTimer, tlkdbg_hpuvcd_timer, nullptr, 1000);
}


void tlkdbg_hpuvcd_reset(void)
{
	tlkapi_fifo_clear(&sTlkDbgHpuVcdFifo);
}
bool tlkdbg_hpuvcd_isBusy(void)
{
	if(tlkapi_fifo_isEmpty(&sTlkDbgHpuVcdFifo)) return false;
	else return true;
}
void tlkdbg_hpuvcd_handler(void)
{
	int ret;
	uint readLen;
	uint08 buffer[TLKDBG_HPU_VCD_CACHE_SIZE];

	if(sTlkDbgHpuVcdTicks == 0 || clock_time_exceed(sTlkDbgHpuVcdTicks, 10000)){
		sTlkDbgHpuVcdTicks = clock_time() | 1;
		tlkdbg_hpuvcd_ref();
		tlkdbg_hpuvcd_sync(true);
	}
	
	readLen = tlkmdi_comm_getSingleDatPktUnitLen();
	if(readLen > TLKDBG_HPU_VCD_CACHE_SIZE) readLen = TLKDBG_HPU_VCD_CACHE_SIZE;
	while(readLen != 0 && !tlkapi_fifo_isEmpty(&sTlkDbgHpuVcdFifo)){
		ret = tlkapi_fifo_readCommon(&sTlkDbgHpuVcdFifo, buffer, readLen, false);
		if(ret <= 0) break;
		readLen = ret;
		if(tlkdev_serial_sfifoIsMore80(readLen)) break;
		ret = tlkmdi_comm_sendDat(TLKPRT_COMM_SYS_DAT_PORT, TLKPRT_COMM_SYS_DAT_VCD, buffer, readLen);
		if(ret == TLK_ENONE || ret == -TLK_ENOSUPPORT){
			tlkapi_fifo_chgReadPos(&sTlkDbgHpuVcdFifo, readLen);
		}
	}
}

bool tlkdbg_hpuvcd_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	tlkdbg_hpuvcd_handler();
	return true;
}


_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_ref(void)
{
	uint08 buffLen = 0;
	uint08 buffer[5];
	uint32 r = core_disable_interrupt();
	int t=clock_time();
	buffer[buffLen++] = 0x7F;
	buffer[buffLen++] = 0x20;
	buffer[buffLen++] = t & 0xFF;
	buffer[buffLen++] = (t & 0xFF00) >> 8;
	buffer[buffLen++] = (t & 0xFF0000) >> 16;
	tlkapi_fifo_write(&sTlkDbgHpuVcdFifo, buffer, buffLen);
	core_restore_interrupt(r);
}
// 4-byte sync word: 00 00 00 00
_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_sync(bool enable)
{
	uint08 buffLen = 0;
	uint08 buffer[5];
	uint32 r = core_disable_interrupt();
	buffer[buffLen++] = 0x7F;
	buffer[buffLen++] = 0x00;
	buffer[buffLen++] = 0x00;
	buffer[buffLen++] = 0x00;
	buffer[buffLen++] = 0x00;
	tlkapi_fifo_write(&sTlkDbgHpuVcdFifo, buffer, buffLen);
	core_restore_interrupt(r);
}      
//4-byte (001_id-5bits) id0: timestamp align with hardware gpio output; id1-31: user define
_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_tick(uint08 id)
{
	uint08 buffLen = 0;
	uint08 buffer[5];
	uint32 r = core_disable_interrupt();
	int t=clock_time();
	buffer[buffLen++] = 0x7F;
	buffer[buffLen++] = 0x20 | (id&31);
	buffer[buffLen++] = t & 0xFF;
	buffer[buffLen++] = (t & 0xFF00) >> 8;
	buffer[buffLen++] = (t & 0xFF0000) >> 16;
	tlkapi_fifo_write(&sTlkDbgHpuVcdFifo, buffer, buffLen);
	core_restore_interrupt(r);
}
//1-byte (01x_id-5bits) 1-bit data: b=0 or 1.
_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_level(uint08 id, uint08 level)
{
	uint08 buffLen = 0;
	uint08 buffer[5];
	uint32 r = core_disable_interrupt();
	int t=clock_time();
	buffer[buffLen++] = 0x7F;
	buffer[buffLen++] = ((level) ? 0x60:0x40) | (id&31);
	buffer[buffLen++] = t & 0xFF;
	buffer[buffLen++] = (t & 0xFF00) >> 8;
	buffer[buffLen++] = (t & 0xFF0000) >> 16;
	tlkapi_fifo_write(&sTlkDbgHpuVcdFifo, buffer, buffLen);
	core_restore_interrupt(r);
}
//1-byte (000_id-5bits)
_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_event(uint08 id)
{
	uint08 buffLen = 0;
	uint08 buffer[5];
	uint32 r = core_disable_interrupt();
	buffer[buffLen++] = 0x7F;
	buffer[buffLen++] = 0x00 | (id&31);
	buffer[buffLen++] = 0x00;
	buffer[buffLen++] = 0x00;
	buffer[buffLen++] = 0x00;
	tlkapi_fifo_write(&sTlkDbgHpuVcdFifo, buffer, buffLen);
	core_restore_interrupt(r);
}
//2-byte (10-id-6bits) 8-bit data
_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_byte(uint08 id, uint08 value)
{
	uint08 buffLen = 0;
	uint08 buffer[5];
	uint32 r = core_disable_interrupt();
	buffer[buffLen++] = 0x7F;
	buffer[buffLen++] = 0x80 | (id&63);
	buffer[buffLen++] = value;
	buffer[buffLen++] = 0x00;
	buffer[buffLen++] = 0x00;
	tlkapi_fifo_write(&sTlkDbgHpuVcdFifo, buffer, buffLen);
	core_restore_interrupt(r);
}
//3-byte (11-id-6bits) 16-bit data
_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_word(uint08 id, uint16 value)
{
	uint08 buffLen = 0;
	uint08 buffer[5];
	uint32 r = core_disable_interrupt();
	buffer[buffLen++] = 0x7F;
	buffer[buffLen++] = 0xc0 | (id&63);
	buffer[buffLen++] = value & 0xFF;
	buffer[buffLen++] = (value & 0xFF00) >> 8;
	buffer[buffLen++] = 0x00;
	tlkapi_fifo_write(&sTlkDbgHpuVcdFifo, buffer, buffLen);
	core_restore_interrupt(r);
}





#endif //#if (TLKDBG_CFG_HPU_VCD_ENABLE)
#endif //#if (TLK_CFG_VCD_ENABLE)

