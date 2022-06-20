/********************************************************************************************************
 * @file     tlkusb_audSpk.c
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
#include "tlklib/usb/aud/tlkusb_audDefine.h"
#if (TLKUSB_AUD_SPK_ENABLE)
#include "tlklib/usb/aud/tlkusb_audDesc.h"
#include "tlklib/usb/aud/tlkusb_aud.h"
#include "tlklib/usb/aud/tlkusb_audSpk.h"
#include "tlklib/usb/aud/tlkusb_audMic.h"

#include "drivers.h"


int tlkusb_audspk_setCmdDeal(int type);
int tlkusb_audspk_getCmdDeal(int req, int type);		


static tlkusb_audspk_ctrl_t sTlkUsbAudSpkCtrl;

int tlkusb_audspk_init(void)
{
	tmemset(&sTlkUsbAudSpkCtrl, 0, sizeof(tlkusb_audspk_ctrl_t));
	
	tlkusb_audspk_setVolume(TLKUSB_AUDSPK_VOL_MAX);

	return TLK_ENONE;
}

int tlkusb_audspk_d2hClassInfHandler(tlkusb_setup_req_t *pSetup, uint08 infNum)
{
	int ret = -TLK_ENOSUPPORT;
	uint08 Entity = (pSetup->wIndex >> 8);
	uint08 value_h = (pSetup->wValue >> 8) & 0xff;
	switch(Entity){
		case USB_SPEAKER_FEATURE_UNIT_ID:
			ret = tlkusb_audspk_getCmdDeal(pSetup->bRequest, value_h);
			break;
		default:
			break;
	}
	
	return ret;
}
int tlkusb_audspk_d2hClassEdpHandler(tlkusb_setup_req_t *pSetup, uint08 edpNum)
{
	return -TLK_ENOSUPPORT;
}

int tlkusb_audspk_h2dClassInfHandler(tlkusb_setup_req_t *pSetup, uint08 infNum)
{
	int ret = -TLK_ENOSUPPORT;
	uint08 property = pSetup->bRequest;
	uint08 value_l = (pSetup->wValue) & 0xff;
	uint08 value_h = (pSetup->wValue >> 8) & 0xff;
	uint08 Entity = (pSetup->wIndex >> 8) & 0xff;
	
	switch(Entity){
		case USB_SPEAKER_FEATURE_UNIT_ID:
			ret = tlkusb_audspk_setCmdDeal(value_h);
			break;
		default:
			break;
	}
	return ret;
}
int tlkusb_audspk_h2dClassEdpHandler(tlkusb_setup_req_t *pSetup, uint08 edpNum)
{
	return -TLK_ENOSUPPORT;
}


uint tlkusb_audspk_getVolume(void)
{
	return (sTlkUsbAudSpkCtrl.mute << 7) | (sTlkUsbAudSpkCtrl.volStep & 0x7f);
}
void tlkusb_audspk_setVolume(uint16 volume)
{
	sTlkUsbAudSpkCtrl.curVol = volume;
	if(volume < TLKUSB_AUDMIC_VOL_MIN){
		sTlkUsbAudSpkCtrl.volStep = 0;
	}else{
		sTlkUsbAudSpkCtrl.volStep = (volume - TLKUSB_AUDMIC_VOL_MIN) / TLKUSB_AUDMIC_VOL_RES;
	}
}
void tlkusb_audspk_enterMute(bool enable)
{
	sTlkUsbAudSpkCtrl.mute = enable;
}




int tlkusb_audspk_setCmdDeal(int type)
{
	if(type == AUDIO_FEATURE_MUTE){
		sTlkUsbAudSpkCtrl.mute = usbhw_read_ctrl_ep_data();
	}else if(type == AUDIO_FEATURE_VOLUME){
		u16 val = usbhw_read_ctrl_ep_u16();
		tlkusb_audspk_setVolume(val);
	}else{
		return -TLK_ENOSUPPORT;
	}
	return TLK_ENONE;
}
int tlkusb_audspk_getCmdDeal(int req, int type)
{
	if(type == AUDIO_FEATURE_MUTE){
		usbhw_write_ctrl_ep_data(sTlkUsbAudSpkCtrl.mute);
	}else if(type == AUDIO_FEATURE_VOLUME){
		switch(req){
			case AUDIO_REQ_GetCurrent:
				usbhw_write_ctrl_ep_u16(sTlkUsbAudSpkCtrl.curVol);
				break;
			case AUDIO_REQ_GetMinimum:
				usbhw_write_ctrl_ep_u16(TLKUSB_AUDSPK_VOL_MIN);
				break;
			case AUDIO_REQ_GetMaximum:
				usbhw_write_ctrl_ep_u16(TLKUSB_AUDSPK_VOL_MAX);
				break;
			case AUDIO_REQ_GetResolution:
				usbhw_write_ctrl_ep_u16(TLKUSB_AUDSPK_VOL_RES);
				break;
			default:
				return -TLK_ENOSUPPORT;
		}
	}else{
		return -TLK_ENOSUPPORT;
	}
	return TLK_ENONE;
}




#endif //#if (TLKUSB_AUD_SPK_ENABLE)

