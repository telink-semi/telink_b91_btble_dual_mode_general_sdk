/********************************************************************************************************
 * @file     tlkusb_msc.c
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
#if (TLKUSB_MSC_ENABLE)
#include "drivers.h"
#include "tlklib/usb/msc/tlkusb_mscDefine.h"
#include "tlklib/usb/msc/tlkusb_msc.h"
#include "tlklib/usb/msc/tlkusb_mscDesc.h"
#include "tlklib/usb/msc/tlkusb_mscCtrl.h"



extern const tlkusb_modCtrl_t sTlkUsbMscModCtrl;
extern const tlkusb_modDesc_t sTlkUsbMscModDesc;
const tlkusb_module_t gTlkUsbMscModule = {
	TLKUSB_MODTYPE_MSC,
	&sTlkUsbMscModDesc,
	&sTlkUsbMscModCtrl,
};


int tlkusb_msc_init(void)
{
	return TLK_ENONE;
}


extern void mass_storage_irq_handler();
extern void dcd_int_handler(uint08 rhport);
_attribute_retention_code_
void tlkusb_mscirq_handler(void)
{
	#if (TLK_DEV_XT2602E_ENABLE)
	mass_storage_irq_handler();
	#elif (TLK_DEV_XTSD04G_ENABLE)
	dcd_int_handler(0);
	#endif
}



#endif //#if (TLKUSB_MSC_ENABLE)


