/********************************************************************************************************
 * @file     tlkapi_chip.c
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

#include "stdbool.h"
#include "string.h"
#include "drivers.h"
#include "tlkapi/tlkapi_type.h"
#include "tlkapi/tlkapi_error.h"
#include "tlkapi/tlkapi_chip.h"

#include "tlkstk/inner/tlkstk_myudb.h"


#if (TLKAPI_CHIP_STACK_CHECK_ENABLE)
#define TLKAPI_CHIP_STACK_CHACK_OFFSET    256
#define TLKAPI_CHIP_STACK_CHACK_SIGN      0xAA5533CC
extern unsigned long _STACK_TOP, _BSS_VMA_END, _BSS_VMA_START, _ZERO_BSS_BEGIN;
static uint32 sTlkApiChipStackStart = 0;
static uint32 sTlkApiChipStackCurLen = 0;
static uint16 sTlkApiChipStackLength = 0;
#endif


_attribute_retention_code_  //switch clock must execute in ram_code
void tlkapi_chip_switchClock(TLKAPI_CHIP_CLOCK_ENUM clock)
{
	/* HCLK & PCLK can not change, because some important peripherals clock is derive from HCLK & PCLK,
	 * such as UART/IIC/SPI. If HCLK & PCLK are changed, make sure that peripherals clock should re_init */
	if(clock == TLKAPI_CHIP_CLOCK_96M){
	    CCLK_96M_HCLK_48M_PCLK_24M_48M_MSPI;
	}else if(clock == TLKAPI_CHIP_CLOCK_48M){
	    CCLK_48M_HCLK_48M_PCLK_24M;
	}else{
		//consider later
	}
}

#if (TLKAPI_CHIP_STACK_CHECK_ENABLE)
void tlkapi_chip_stackInit(void)
{
	uint32 index;
	uint32 saddr;
	uint32 eaddr;
	uint32 length;
	volatile uint32 *pStart;

	eaddr = (unsigned long)&_STACK_TOP;
	saddr = (unsigned long)&_BSS_VMA_END;

	if(eaddr < 0x1000 || saddr >= eaddr) return;
	eaddr = (eaddr - 0) & 0xFFFFFFFC;
	saddr = (saddr + 4) & 0xFFFFFFFC;
	
	sTlkApiChipStackStart = saddr;
	if(saddr+TLKAPI_CHIP_STACK_CHACK_OFFSET >= eaddr){
		sTlkApiChipStackCurLen = 0;
		sTlkApiChipStackLength = 0;
	}else{
		sTlkApiChipStackLength = eaddr-saddr;
		sTlkApiChipStackCurLen = TLKAPI_CHIP_STACK_CHACK_OFFSET;
	}
	
	length = (sTlkApiChipStackLength-sTlkApiChipStackCurLen)>>2;
	pStart = (volatile uint32*)sTlkApiChipStackStart;
	for(index=0; index<length; index++){
		pStart[index] = TLKAPI_CHIP_STACK_CHACK_SIGN;
	}
}
uint tlkapi_chip_stackCheck(void)
{
	uint32 length;
	volatile uint32 *pStart;
	
	if(sTlkApiChipStackCurLen >= sTlkApiChipStackLength) return sTlkApiChipStackCurLen;
	length = (sTlkApiChipStackLength-sTlkApiChipStackCurLen)>>2;
	if(length <= 2){
		sTlkApiChipStackCurLen = sTlkApiChipStackLength;
	}else{
		uint32 index;
		pStart = (volatile uint32*)(sTlkApiChipStackStart);
		for(index=length-1; index>1; index--){
			if(pStart[index] == TLKAPI_CHIP_STACK_CHACK_SIGN && pStart[index-1] == TLKAPI_CHIP_STACK_CHACK_SIGN){
				break;
			}
		}
		if(index <= 1) sTlkApiChipStackCurLen = sTlkApiChipStackLength;
		else sTlkApiChipStackCurLen += (length-index-1)<<2;
	}
	return sTlkApiChipStackCurLen;
}
uint tlkapi_chip_stackDepth(void)
{
	return sTlkApiChipStackLength;
}
bool tlkapi_chip_stackOverflow(void)
{
	if(sTlkApiChipStackCurLen == sTlkApiChipStackLength) return true;
	else return false;
}
#endif


extern unsigned int trng_rand(void);
void tlkapi_random(uint08 *pBuff, uint16 buffLen)
{
	int i;
	unsigned int randNums = 0;
    /* if len is odd */
	for(i=0; i<buffLen; i++ ){
		if((i & 3) == 0) randNums = trng_rand();
		pBuff[i] = randNums & 0xff;
		randNums >>=8;
	}
}


