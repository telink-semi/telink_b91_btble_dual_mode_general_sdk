/********************************************************************************************************
 * @file     tlkusb_aud.c
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
#if (TLK_USB_AUD_ENABLE)
#include "drivers.h"
#include "tlklib/usb/aud/tlkusb_audDefine.h"
#include "tlklib/usb/aud/tlkusb_audDesc.h"
#include "tlklib/usb/aud/tlkusb_aud.h"
#include "tlklib/usb/aud/tlkusb_audSpk.h"
#include "tlklib/usb/aud/tlkusb_audMic.h"


extern const tlkusb_modCtrl_t sTlkUsbAudModCtrl;
extern const tlkusb_modDesc_t sTlkUsbAudModDesc;



int	sTlkUsbAudMute = 0;
uint32 sTlkUsbAudSamplRate = 16000;
const tlkusb_module_t gTlkUsbAudModule = {
	TLKUSB_MODTYPE_AUD,
	&sTlkUsbAudModDesc,
	&sTlkUsbAudModCtrl,
};


int tlkusb_aud_init(void)
{
	#if (TLKUSB_AUD_SPK_ENABLE)
	tlkusb_audspk_init();
	#endif
	#if (TLKUSB_AUD_MIC_ENABLE)
	tlkusb_audmic_init();
	#endif
	
	return TLK_ENONE;
}



_attribute_ram_code_sec_noinline_
void tlkusb_audirq_handler(void)
{
	uint08 irq = reg_usb_ep_irq_status;
	#if (TLKUSB_AUD_MIC_ENABLE)
	if(irq & FLD_USB_EDP7_IRQ){
		tlkusb_audmic_fillData();
	}
	#endif
	#if (TLKUSB_AUD_SPK_ENABLE)
	if(irq & FLD_USB_EDP6_IRQ){
		tlkusb_audspk_recvData();
	}
	#endif
}


void tlkusb_aud_setFeatureMute(int id, int mute)
{
	int b = (id >> 8) & 15;
	if((id & 0xff) < 2) b |= (1 << 4);
	if(mute) sTlkUsbAudMute |= (1 << b);
	else sTlkUsbAudMute &= ~ (1 << b);
}
uint tlkusb_aud_getFeatureMute(int id)
{
	int b = (id >> 8) & 15;
	if((id & 0xff) < 2) b |= (1 << 4);
	return (sTlkUsbAudMute >> b) & 1;
}



#endif //#if (TLK_USB_AUD_ENABLE)

