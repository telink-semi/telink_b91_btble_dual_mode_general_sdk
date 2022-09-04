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
#if (TLK_USB_UDB_ENABLE)
#include "tlklib/usb/udb/tlkusb_udb.h"
#endif
#if (TLK_USB_AUD_ENABLE)
#include "tlklib/usb/aud/tlkusb_aud.h"
#endif
#if (TLK_USB_CDC_ENABLE)
#include "tlklib/usb/cdc/tlkusb_cdc.h"
#endif
#if (TLK_USB_HID_ENABLE)
#include "tlklib/usb/aud/tlkusb_hid.h"
#endif
#if (TLK_USB_MSC_ENABLE)
#include "tlklib/usb/msc/tlkusb_msc.h"
#endif
#if (TLK_USB_USR_ENABLE)
#include "tlklib/usb/usr/tlkusb_usr.h"
#endif


extern uint08 gTlkUsbCurModType;


int tlkusb_init(uint16 usbID)
{
	//set USB ID
//	REG_ADDR8(0x1401f4) = 0x65;
//	REG_ADDR16(0x1401fe) = 0x08d0;
//	REG_ADDR8(0x1401f4) = 0x00;

	gpio_input_en(GPIO_PA5);
	gpio_input_en(GPIO_PA6);
	
	usb_set_pin_en();
	
	tlkusb_core_init(usbID);
	#if (TLK_USB_UDB_ENABLE)
	tlkusb_udb_init();
	#endif
	#if (TLK_USB_AUD_ENABLE)
	tlkusb_aud_init();
	#endif
	#if (TLK_USB_CDC_ENABLE)
	tlkusb_cdc_init();
	#endif
	#if (TLK_USB_HID_ENABLE)
	tlkusb_hid_init();
	#endif
	#if (TLK_USB_USR_ENABLE)
	tlkusb_usr_init();
	#endif
	#if (TLK_USB_MSC_ENABLE)
	tlkusb_msc_init();
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





#endif //#if (TLK_CFG_USB_ENABLE)

