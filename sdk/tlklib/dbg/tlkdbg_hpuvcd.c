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
bool sTlkHpuVcdIsOpen = false;

#if (TLK_DEV_SERIAL_ENABLE)
extern bool tlkdev_serial_sfifoIsMore80(uint16 dataLen);
#endif
#if (TLK_CFG_COMM_ENABLE)
extern uint tlkmdi_comm_getSingleDatPktUnitLen(void);
extern int  tlkmdi_comm_sendDat(uint08 datID, uint16 numb, uint08 *pData, uint16 dataLen);
#endif
static void tlkdbg_hpuvcd_push(uint08 value, uint08 *pBuffer, uint08 *pOffset);

//static bool tlkdbg_hpuvcd_timer(tlkapi_timer_t *pTimer, uint32 userArg);


static uint32 sTlkDbgHpuVcdTicks;
static uint08 sTlkdbgHpuVcdBuffer[TLKDBG_HPU_VCD_BUFFER_SIZE];
static tlkapi_fifo_t sTlkDbgHpuVcdFifo;
//static tlkapi_timer_t sTlkDbgHpuVcdTimer;


void tlkdbg_hpuvcd_init(void)
{
	tlkapi_fifo_init(&sTlkDbgHpuVcdFifo, false, false, sTlkdbgHpuVcdBuffer, TLKDBG_HPU_VCD_BUFFER_SIZE);
//	tlkapi_timer_initNode(&sTlkDbgHpuVcdTimer, tlkdbg_hpuvcd_timer, nullptr, 5000);
}


void tlkdbg_hpuvcd_reset(void)
{
	tlkapi_fifo_clear(&sTlkDbgHpuVcdFifo);
}
bool tlkdbg_hpuvcd_isBusy(void)
{
	if(sTlkHpuVcdIsOpen == false || tlkapi_fifo_isEmpty(&sTlkDbgHpuVcdFifo)) return false;
	else return true;
}
void tlkdbg_hpuvcd_handler(void)
{
	int ret;
	uint readLen;
	uint08 buffer[TLKDBG_HPU_VCD_CACHE_SIZE];

	if(sTlkHpuVcdIsOpen == false) return;

	if(sTlkDbgHpuVcdTicks == 0 || clock_time_exceed(sTlkDbgHpuVcdTicks, 10000)){
		sTlkDbgHpuVcdTicks = clock_time() | 1;
		tlkdbg_hpuvcd_sync();
	}

	#if (TLK_CFG_COMM_ENABLE)
	readLen = tlkmdi_comm_getSingleDatPktUnitLen();
	#else
	readLen = TLKDBG_HPU_VCD_CACHE_SIZE;
	#endif
	if(readLen > TLKDBG_HPU_VCD_CACHE_SIZE) readLen = TLKDBG_HPU_VCD_CACHE_SIZE;
	while(readLen != 0 && !tlkapi_fifo_isEmpty(&sTlkDbgHpuVcdFifo)){
		ret = tlkapi_fifo_readCommon(&sTlkDbgHpuVcdFifo, buffer, readLen, false);
		if(ret <= 0) break;
		readLen = ret;
		#if (TLK_CFG_COMM_ENABLE)
		#if (TLK_DEV_SERIAL_ENABLE)
		if(tlkdev_serial_sfifoIsMore80(readLen)) break;
		#endif
		ret = tlkmdi_comm_sendDat(TLKPRT_COMM_SYS_DAT_PORT, TLKPRT_COMM_SYS_DAT_VCD, buffer, readLen);
		#else
		ret = -TLK_ENOSUPPORT;
		#endif
		if(ret == TLK_ENONE || ret == -TLK_ENOSUPPORT){
			tlkapi_fifo_chgReadPos(&sTlkDbgHpuVcdFifo, readLen);
		}
	}
}

//bool tlkdbg_hpuvcd_timer(tlkapi_timer_t *pTimer, uint32 userArg)
//{
//	tlkdbg_hpuvcd_handler();
//	return true;
//}




_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_sync(void)
{
	if(sTlkHpuVcdIsOpen == false) return;

	uint08 buffLen = 0;
	uint08 buffer[16];
	core_interrupt_disable();
	int tick=clock_time();
	buffer[buffLen++] = TLKDBG_HPU_VCD_HEAD_SIGN;
	buffer[buffLen++] = (TLKDBG_HPU_VCD_TYPE_SYNC << 4) | 0x04;
	tlkdbg_hpuvcd_push((tick & 0xFF), buffer, &buffLen);
	tlkdbg_hpuvcd_push((tick & 0xFF00) >> 8, buffer, &buffLen);
	tlkdbg_hpuvcd_push((tick & 0xFF0000) >> 16, buffer, &buffLen);
	tlkdbg_hpuvcd_push((tick & 0xFF000000) >> 24, buffer, &buffLen);
	tlkapi_fifo_write(&sTlkDbgHpuVcdFifo, buffer, buffLen);
	core_interrupt_restore();
}
//4-byte (001_id-5bits) id0: timestamp align with hardware gpio output; id1-31: user define
_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_tick(uint08 id)
{
	if(sTlkHpuVcdIsOpen == false) return;

	uint08 buffLen = 0;
	uint08 buffer[16];
	core_interrupt_disable();
	int tick=clock_time();
	buffer[buffLen++] = TLKDBG_HPU_VCD_HEAD_SIGN;
	buffer[buffLen++] = (TLKDBG_HPU_VCD_TYPE_TICK << 4) | 0x05;
	tlkdbg_hpuvcd_push(id, buffer, &buffLen);
	tlkdbg_hpuvcd_push((tick & 0xFF), buffer, &buffLen);
	tlkdbg_hpuvcd_push((tick & 0xFF00) >> 8, buffer, &buffLen);
	tlkdbg_hpuvcd_push((tick & 0xFF0000) >> 16, buffer, &buffLen);
	tlkdbg_hpuvcd_push((tick & 0xFF000000) >> 24, buffer, &buffLen);
	tlkapi_fifo_write(&sTlkDbgHpuVcdFifo, buffer, buffLen);
	core_interrupt_restore();
}
//1-byte (01x_id-5bits) 1-bit data: b=0 or 1.
_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_level(uint08 id, uint08 level)
{
	if(sTlkHpuVcdIsOpen == false) return;
	
	uint08 buffLen = 0;
	uint08 buffer[16];
	core_interrupt_disable();
	int tick=clock_time();
	if(level) id |= 0x80;
	else id &= 0x7F;
	buffer[buffLen++] = TLKDBG_HPU_VCD_HEAD_SIGN;
	buffer[buffLen++] = (TLKDBG_HPU_VCD_TYPE_LEVEL << 4) | 0x05;
	tlkdbg_hpuvcd_push(id, buffer, &buffLen);
	tlkdbg_hpuvcd_push((tick & 0xFF), buffer, &buffLen);
	tlkdbg_hpuvcd_push((tick & 0xFF00) >> 8, buffer, &buffLen);
	tlkdbg_hpuvcd_push((tick & 0xFF0000) >> 16, buffer, &buffLen);
	tlkdbg_hpuvcd_push((tick & 0xFF000000) >> 24, buffer, &buffLen);
	tlkapi_fifo_write(&sTlkDbgHpuVcdFifo, buffer, buffLen);
	core_interrupt_restore();
}
//1-byte (000_id-5bits)
_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_event(uint08 id)
{
	if(sTlkHpuVcdIsOpen == false) return;

	uint08 buffLen = 0;
	uint08 buffer[16];
	core_interrupt_disable();
	buffer[buffLen++] = TLKDBG_HPU_VCD_HEAD_SIGN;
	buffer[buffLen++] = (TLKDBG_HPU_VCD_TYPE_EVENT << 4) | 0x01;
	tlkdbg_hpuvcd_push(id, buffer, &buffLen);
	tlkapi_fifo_write(&sTlkDbgHpuVcdFifo, buffer, buffLen);
	core_interrupt_restore();
}
//2-byte (10-id-6bits) 8-bit data
_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_byte(uint08 id, uint08 value)
{
	if(sTlkHpuVcdIsOpen == false) return;

	uint08 buffLen = 0;
	uint08 buffer[16];
	core_interrupt_disable();
	buffer[buffLen++] = TLKDBG_HPU_VCD_HEAD_SIGN;
	buffer[buffLen++] = (TLKDBG_HPU_VCD_TYPE_BYTE << 4) | 0x02;
	tlkdbg_hpuvcd_push(id, buffer, &buffLen);
	tlkdbg_hpuvcd_push((value & 0xFF), buffer, &buffLen);
	tlkapi_fifo_write(&sTlkDbgHpuVcdFifo, buffer, buffLen);
	core_interrupt_restore();
}
//3-byte (11-id-6bits) 16-bit data
_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_word(uint08 id, uint16 value)
{
	if(sTlkHpuVcdIsOpen == false) return;

	uint08 buffLen = 0;
	uint08 buffer[16];
	core_interrupt_disable();
	buffer[buffLen++] = TLKDBG_HPU_VCD_HEAD_SIGN;
	buffer[buffLen++] = (TLKDBG_HPU_VCD_TYPE_WORD << 4) | 0x03; //
	tlkdbg_hpuvcd_push(id, buffer, &buffLen);
	tlkdbg_hpuvcd_push((value & 0xFF), buffer, &buffLen);
	tlkdbg_hpuvcd_push((value & 0xFF00) >> 8, buffer, &buffLen);
	tlkapi_fifo_write(&sTlkDbgHpuVcdFifo, buffer, buffLen);
	core_interrupt_restore();
}

void tlkdbg_hpuvcd_setOpen(bool isOpen)
{
	sTlkHpuVcdIsOpen = isOpen;
	tlkapi_fifo_clear(&sTlkDbgHpuVcdFifo);
}

_attribute_ram_code_sec_noinline_
static void tlkdbg_hpuvcd_push(uint08 value, uint08 *pBuffer, uint08 *pOffset)
{
	uint08 offset = *pOffset;
	if(value == TLKDBG_HPU_VCD_HEAD_SIGN){
		pBuffer[offset++] = TLKDBG_HPU_VCD_ESCAPE_SIGN;
		pBuffer[offset++] = 0x01;
	}else if(value == TLKDBG_HPU_VCD_ESCAPE_SIGN){
		pBuffer[offset++] = TLKDBG_HPU_VCD_ESCAPE_SIGN;
		pBuffer[offset++] = 0x02;
	}else{
		pBuffer[offset++] = value;
	}
	*pOffset = offset;
}


#endif //#if (TLKDBG_CFG_HPU_VCD_ENABLE)
#endif //#if (TLK_CFG_VCD_ENABLE)

