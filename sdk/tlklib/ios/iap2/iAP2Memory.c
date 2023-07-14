/********************************************************************************************************
 * @file     iAP2Memory.c
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
#if (TLK_STK_BTP_ENABLE)
#include "tlkstk/bt/btp/btp_stdio.h"
#if (TLKBTP_CFG_IAP_ENABLE)
#include "iAP2Inner.h"
#include "iAP2Memory.h"


static uint08 sIap2MemBuff[IAP2_MEMORY_BUFF_SIZE];
volatile static tlkapi_mem_t sIap2MemCtrl = 0;


void iap2_memInit(void)
{
	sIap2MemCtrl = tlkapi_mem_init(false, false, sIap2MemBuff, IAP2_MEMORY_BUFF_SIZE);
}

void *iap2_malloc(uint32 size)
{
	void *ptr;
	if(sIap2MemCtrl == 0){
		sIap2MemCtrl = tlkapi_mem_init(false, false, sIap2MemBuff, IAP2_MEMORY_BUFF_SIZE);
	}
	ptr = tlkapi_mem_malloc(sIap2MemCtrl, size);
	if(ptr == nullptr){
		tlkapi_error(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "iap2_malloc: failure - size[%d]", size);
	}
	return ptr;
}
void *iap2_calloc(uint32 size)
{
	void *ptr;
	if(sIap2MemCtrl == 0){
		sIap2MemCtrl = tlkapi_mem_init(false, false, sIap2MemBuff, IAP2_MEMORY_BUFF_SIZE);
	}
	ptr = tlkapi_mem_calloc(sIap2MemCtrl, size);
	if(ptr == nullptr){
		tlkapi_error(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "iap2_calloc: failure - size[%d]", size);
	}
	return ptr;
}
void iap2_free(void *ptr)
{
	tlkapi_mem_free(sIap2MemCtrl, ptr);
}






#endif //#if (TLKBTP_CFG_IAP_ENABLE)
#endif //#if (TLK_STK_BTP_ENABLE)

