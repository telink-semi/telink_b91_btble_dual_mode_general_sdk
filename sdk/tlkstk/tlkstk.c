/********************************************************************************************************
 * @file     tlkstk.c
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
#include "tlkstk/bt/btc/btc_stdio.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk.h"


extern int  btble_init(void);;
extern void btble_sdk_main_loop(void);

extern bool bth_isBusy(void);


int tlkstk_init(void)
{
	btble_init();

	btc_init();
#if TLK_STK_BTH_ENABLE
	bth_init();
#endif
#if TLK_STK_BTP_ENABLE
	btp_init();
#endif

	return TLK_ENONE;
}

void tlkstk_process(void)
{
	btble_sdk_main_loop();
	
#if TLK_STK_BTH_ENABLE
	bth_process();
#endif
#if TLK_STK_BTP_ENABLE
	btp_process();
#endif
}


bool tlkstk_pmIsBusy(void)
{
	return bth_pmIsBusy();
}


