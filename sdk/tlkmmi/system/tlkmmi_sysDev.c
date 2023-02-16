/********************************************************************************************************
 * @file     tlkmmi_sysDev.c
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
#if (TLKMMI_SYSTEM_ENABLE)
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi/system/tlkmmi_sys.h"
#include "tlkmmi/system/tlkmmi_sysDev.h"
#include "drivers.h"
#if (TLK_MDI_KEY_ENABLE)
#include "tlkmdi/misc/tlkmdi_key.h"
#endif
#if (TLK_MDI_LED_ENABLE)
#include "tlkmdi/misc/tlkmdi_led.h"
#endif

#if (TLK_MDI_KEY_ENABLE)
static void tlkmmi_sys_devKeyEventCB(uint08 keyID, uint08 evtID, uint08 isPress);
#endif


int tlkmmi_sys_devInit(void)
{
	#if (TLK_MDI_KEY_ENABLE)
	tlkmdi_key_insert(0x04, TLKMDI_KEY_EVTMSK_ALL, GPIO_PD5, 0, GPIO_PIN_PULLUP_1M, tlkmmi_sys_devKeyEventCB);
	tlkmdi_key_insert(0x05, TLKMDI_KEY_EVTMSK_ALL, GPIO_PE7, 0, GPIO_PIN_PULLUP_1M, tlkmmi_sys_devKeyEventCB);
	#endif
	#if (TLK_MDI_LED_ENABLE)
	tlkmdi_led_insert(0x01, GPIO_PC3, GPIO_PIN_PULLUP_10K, 1);
	tlkmdi_led_insert(0x02, GPIO_PD4, GPIO_PIN_PULLUP_10K, 1);
	tlkmdi_led_auto(0x01, true, 15, 200, 200, true);
	tlkmdi_led_auto(0x02, true, 3,  500, 500, false);
	#endif
	
	return TLK_ENONE;
}

void tlkmmi_sys_devHandler(void)
{
	
}



#if (TLK_MDI_KEY_ENABLE)
static void tlkmmi_sys_devKeyEventCB(uint08 keyID, uint08 evtID, uint08 isPress)
{
	tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_devKeyEventCB: keyID-%d, evtID-%d, isPress-%d",
		keyID, evtID, isPress);	
}
#endif


#endif //#if (TLKMMI_SYSTEM_ENABLE)

