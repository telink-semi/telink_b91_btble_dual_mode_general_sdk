/********************************************************************************************************
 * @file     tlkusb.c
 *
 * @brief    This is the header file for BTBLE SDK
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
#include "tlklib/usb/tlkusb_stdio.h"
#include "tlklib/usb/tlkusb_core.h"
#include "tlklib/usb/tlkusb_desc.h"
#if (TLK_CFG_USB_ENABLE)
#include "drivers.h"
#if (TLKUSB_UDB_ENABLE)
#include "tlklib/usb/udb/tlkusb_udb.h"
#endif
#if (TLKUSB_AUD_ENABLE)
#include "tlklib/usb/aud/tlkusb_aud.h"
#endif

extern uint08 gTlkUsbCurModType;


int tlkusb_init(uint16 usbID)
{
	gpio_input_en(GPIO_PA5);
	gpio_input_en(GPIO_PA6);
	
	usb_set_pin_en();
	
	tlkusb_core_init(usbID);
	#if (TLKUSB_UDB_ENABLE)
	tlkusb_udb_init();
	#endif
	#if (TLKUSB_AUD_ENABLE)
	tlkusb_aud_init();
	#endif
	

	return TLK_ENONE;
}


void tlkusb_process(void)
{
	tlkusb_core_handler();
}

bool tlkusb_setModule(uint08 modtype) //TLKUSB_MODTYPE_ENUM
{
	if(gTlkUsbCurModType == modtype) return true;
	
//	usbhw_disable_manual_interrupt();
	usb_dp_pullup_en(false);
	
	tlkusb_module_deinit(gTlkUsbCurModType);
	if(tlkusb_module_init(modtype) != TLK_ENONE) return false;
	gTlkUsbCurModType = modtype;
	
	usb_dp_pullup_en(true);
	return true;
}

#if (TLK_USB_MSC_ENABLE)
_attribute_retention_code_
void tlkusb_irq_handler(void)
{
//	#if (TLK_DEV_XT2602E_ENABLE)
//	mass_storage_irq_handler();
//	#else (TLK_DEV_XTSD04G_ENABLE)
//	dcd_int_handler(0);
//	#endif
}
#endif



#endif //#if (TLK_CFG_USB_ENABLE)

