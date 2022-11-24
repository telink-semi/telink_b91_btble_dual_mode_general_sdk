/********************************************************************************************************
 * @file     tlkmdi_debug.c
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
#include "tlkdev/tlkdev_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#if (TLK_MDI_DEBUG_ENABLE)
#include "tlkmdi_debug.h"
#include "drivers.h"


#define TLKMDI_DEBUG_DBG_FLAG         ((TLK_MAJOR_DBGID_MDI_MISC << 24) | (TLK_MINOR_DBGID_MDI_MISC << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_DEBUG_DBG_SIGN         "[MDI]"

#if (TLKMDI_DEBUG_STACK_CHECK_ENABLE)
static uint32 sTlkMdiDbgStackCheckTimer = 0;
#endif


int tlkmdi_debug_init(void)
{
	tlkapi_debug_init();
	#if (TLKMDI_DEBUG_STACK_CHECK_ENABLE)
	tlkapi_chip_stackInit();
	#endif
	
	return TLK_ENONE;
}


bool tlkmdi_debug_pmIsBusy(void)
{
	return tlkapi_debug_isBusy();
}

void tlkmdi_debug_handler(void)
{
	tlkapi_debug_handler();
	#if (TLKMDI_DEBUG_STACK_CHECK_ENABLE)
	if(sTlkMdiDbgStackCheckTimer == 0 || clock_time_exceed(sTlkMdiDbgStackCheckTimer, 3000000)){
		uint length = tlkapi_chip_stackCheck();
		sTlkMdiDbgStackCheckTimer = clock_time()|1;
		tlkapi_trace(TLKMDI_DEBUG_DBG_FLAG, TLKMDI_DEBUG_DBG_SIGN, "CurChipStackLength: depth-%d, isOver-%d", length, 
			tlkapi_chip_stackOverflow());
	}
	#endif
}



#endif //#if (TLK_MDI_DEBUG_ENABLE)

