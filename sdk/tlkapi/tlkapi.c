/********************************************************************************************************
 * @file     tlkapi.c
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
#include "tlkapi/tlkapi_stdio.h"
#include "tlkapi/tlkapi_timer.h"
#include "tlkapi/tlkapi.h"
#include "drivers.h"


#define TLKAPI_DBG_FLAG         ((TLK_MAJOR_DBGID_SYS << 24) | (TLK_MINOR_DBGID_SYS_API << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKAPI_DBG_SIGN         "[API]"


#if (TLKAPI_CHIP_STACK_CHECK_ENABLE)
static uint32 sTlkApiStackCheckTimer = 0;
#endif


int  tlkapi_init(void)
{
	#if (TLK_CFG_DBG_ENABLE)
	tlkapi_debug_init();
	#endif
	#if (TLKAPI_CHIP_STACK_CHECK_ENABLE)
	tlkapi_chip_stackInit();
	#endif
	#if (TLKAPI_TIMER_ENABLE)
	tlkapi_timer_init();
	#endif
	
	return TLK_ENONE;
}

bool tlkapi_pmIsBusy(void)
{
	#if (TLK_CFG_DBG_ENABLE)
	return tlkapi_debug_isBusy();
	#else
	return false;
	#endif
}

void tlkapi_process(void)
{
	#if (TLK_CFG_DBG_ENABLE)
	tlkapi_debug_process();
	#endif

	#if (TLKAPI_CHIP_STACK_CHECK_ENABLE)
	if(sTlkApiStackCheckTimer == 0 || clock_time_exceed(sTlkApiStackCheckTimer, 3000000)){
		uint length = tlkapi_chip_stackCheck();
		sTlkApiStackCheckTimer = clock_time()|1;
//		tlkapi_trace(TLKAPI_DBG_FLAG, TLKAPI_DBG_SIGN, "CurChipStackLength: depth-%d, isOver-%d", length, tlkapi_chip_stackOverflow());
	}
	#endif
}


