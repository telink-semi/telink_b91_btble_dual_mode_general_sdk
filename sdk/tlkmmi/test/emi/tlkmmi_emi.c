/********************************************************************************************************
 * @file     tlkmmi_emi.c
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
#if (TLKMMI_TEST_ENABLE)
#include "../tlkmmi_testStdio.h"
#if (TLK_TEST_EMI_ENABLE)
#include "tlkmmi_emi.h"
#include "drivers.h"
#include "tlkstk/tlkstk_stdio.h"


extern void tlkbt_regPlicIrqClaim(plic_interrupt_claim_callback_t cb);
extern void tlkbt_set_workMode(u8 workMode);
extern void btc_enterTestMode(void);
extern int  tlkusb_init(uint16 usbID);
extern void tlkdbg_init(void);
extern void tlkusb_handler(void);

static int tlkmmi_emi_start(void);
static int tlkmmi_emi_pause(void);
static int tlkmmi_emi_close(void);
static int tlkmmi_emi_input(uint08 msgID, uint08 *pData, uint16 dataLen);
static void tlkmmi_emi_handler(void);


const tlkmmi_testModinf_t gTlkMmiEmiModinf = 
{
	tlkmmi_emi_start, //.Start
	tlkmmi_emi_pause, //.Pause
	tlkmmi_emi_close, //.Close
	tlkmmi_emi_input, //.Input
	tlkmmi_emi_handler, //Handler
};



static int tlkmmi_emi_start(void)
{
	tlkbt_set_workMode(0);
	tlkbt_regPlicIrqClaim((plic_interrupt_claim_callback_t)plic_interrupt_claim());

	tlkdbg_init();
	tlkusb_init(0x120);
	tlkstk_init();
	btc_enterTestMode();
	
	return TLK_ENONE;
}
static int tlkmmi_emi_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_emi_close(void)
{
	return TLK_ENONE;
}
static int tlkmmi_emi_input(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	return TLK_ENONE;
}
static void tlkmmi_emi_handler(void)
{
	tlkdbg_handler();
	tlkusb_handler();
	tlkstk_handler();
}



#endif //#if (TLK_TEST_EMI_ENABLE)

#endif //#if (TLKMMI_TEST_ENABLE)

