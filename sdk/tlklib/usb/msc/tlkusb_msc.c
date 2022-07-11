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
#if (TLK_USB_MSC_ENABLE)
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
static uint08 sTlkUsbMscUnitCount = 0;
static tlkusb_msc_unit_t *spTlkUsbMscUnit[TLKUSB_MSC_UNIT_COUNT];





int tlkusb_msc_init(void)
{
	return TLK_ENONE;
}


int tlkusb_msc_appendDisk(tlkusb_msc_unit_t *pUnit)
{
	if(pUnit == nullptr || pUnit->blkSize == 0 || (pUnit->blkSize & 0x3F) != 0
		|| pUnit->blkCount < 100 || pUnit->Read == nullptr || pUnit->Write == nullptr){
		return -TLK_EPARAM;
	}
	spTlkUsbMscUnit[sTlkUsbMscUnitCount++] = pUnit;
	return TLK_ENONE;
}


uint08 tlkusb_msc_getDiskCount(void)
{
	return sTlkUsbMscUnitCount;
}
tlkusb_msc_unit_t *tlkusb_msc_getDisk(uint08 lun)
{
	if(lun >= sTlkUsbMscUnitCount) return nullptr;
	return spTlkUsbMscUnit[lun];
}









#endif //#if (TLK_USB_MSC_ENABLE)


