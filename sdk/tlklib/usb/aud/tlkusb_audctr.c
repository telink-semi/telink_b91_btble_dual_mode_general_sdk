/********************************************************************************************************
 * @file     tlkusb_audCtr.c
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
#if (TLKUSB_AUD_ENABLE)
#include "drivers.h"
#include "tlklib/usb/aud/tlkusb_audDefine.h"
#include "tlklib/usb/aud/tlkusb_audDesc.h"
#include "tlklib/usb/aud/tlkusb_aud.h"
#include "tlklib/usb/aud/tlkusb_audCtr.h"
#include "tlklib/usb/aud/tlkusb_audSpk.h"
#include "tlklib/usb/aud/tlkusb_audMic.h"


static int  tlkusb_audctrl_init(void);
static void tlkusb_audctrl_reset(void);
static void tlkusb_audctrl_deinit(void);
static void tlkusb_audctrl_handler(void);
static int  tlkusb_audctrl_getClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_audctrl_setClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_audctrl_getClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb);
static int  tlkusb_audctrl_setClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb);
static int  tlkusb_audctrl_getInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_audctrl_setInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb);

const tlkusb_modCtrl_t sTlkUsbAudModCtrl = {
	tlkusb_audctrl_init, //Init
	tlkusb_audctrl_reset, //Reset
	tlkusb_audctrl_deinit, //Deinit
	tlkusb_audctrl_handler, //Handler
	tlkusb_audctrl_getClassInf, //GetClassInf
	tlkusb_audctrl_setClassInf, //SetClassInf
	tlkusb_audctrl_getClassEdp, //GetClassEdp
	tlkusb_audctrl_setClassEdp, //SetClassEdp
	tlkusb_audctrl_getInterface, //GetInterface
	tlkusb_audctrl_setInterface, //SetInterface
};

uint08 sTlkUsbAudMicEnable = 0;


static int tlkusb_audctrl_init(void)
{
	sTlkUsbAudMicEnable = 0;
	reg_usb_ep_irq_mask = BIT(7);			//audio in interrupt enable
	plic_interrupt_enable(IRQ11_USB_ENDPOINT);
	plic_set_priority(IRQ11_USB_ENDPOINT, 1);
	reg_usb_ep7_buf_addr = 0x40; //64 - 0x60
	
	return TLK_ENONE;
}
static void tlkusb_audctrl_reset(void)
{
	sTlkUsbAudMicEnable = 0;
}
static void tlkusb_audctrl_deinit(void)
{
	plic_interrupt_disable(IRQ11_USB_ENDPOINT);
	
}
static void tlkusb_audctrl_handler(void)
{
	
}

static int tlkusb_audctrl_getClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	int ret = -TLK_ENOSUPPORT;
	uint08 entity = (pSetup->wIndex >> 8);
	uint08 value  = (pSetup->wValue >> 8) & 0xff;
	switch(entity){
		#if (TLKUSB_AUD_SPK_ENABLE)
		case USB_SPEAKER_FEATURE_UNIT_ID:
			ret = tlkusb_audspk_getCmdDeal(pSetup->bRequest, value);
			break;
		#endif
		#if (TLKUSB_AUD_MIC_ENABLE)
		case USB_MIC_FEATURE_UNIT_ID:
			ret = tlkusb_audmic_getCmdDeal(pSetup->bRequest, value);
			break;
		#endif
		default:
			break;
	}
	return ret;
}
static int tlkusb_audctrl_setClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	int ret;
	uint08 value_h = (pSetup->wValue >> 8) & 0xff;
	uint08 Entity = (pSetup->wIndex >> 8) & 0xff;
	
	ret = -TLK_ENOSUPPORT;
	switch(Entity){
		#if (TLKUSB_AUD_SPK_ENABLE)
		case USB_SPEAKER_FEATURE_UNIT_ID:
			ret = tlkusb_audspk_setCmdDeal(value_h);
			break;
		#endif
		#if (TLKUSB_AUD_MIC_ENABLE)
		case USB_MIC_FEATURE_UNIT_ID:
			ret = tlkusb_audmic_setCmdDeal(value_h);
			break;
		#endif
		default:
			break;
	}
	return ret;
}
static int tlkusb_audctrl_getClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb)
{
	return TLK_ENONE;
}
static int tlkusb_audctrl_setClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb)
{
	return TLK_ENONE;
}
static int tlkusb_audctrl_getInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	uint08 infNum = (pSetup->wIndex) & 0x07;
	usbhw_write_ctrl_ep_data(sTlkUsbAudMicEnable);
	return TLK_ENONE;
}
static int tlkusb_audctrl_setInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	uint08 enable = (pSetup->wValue) & 0xff;
	uint08 infNum = (pSetup->wIndex) & 0x07;
	if(infNum == TLKUSB_AUD_INF_MIC){
		sTlkUsbAudMicEnable = enable;
		if(enable){
			reg_usb_ep_ptr(TLKUSB_AUD_EDP_MIC) = 0;
			reg_usb_ep_ctrl(TLKUSB_AUD_EDP_MIC) = 0x01; //ACK first packet
		}
	}
	return TLK_ENONE;
}



#endif //#if (TLKUSB_AUD_ENABLE)
