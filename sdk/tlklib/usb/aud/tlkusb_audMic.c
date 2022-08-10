/********************************************************************************************************
 * @file     tlkusb_audMic.c
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
#if (TLKUSB_AUD_MIC_ENABLE)
#include "tlklib/usb/aud/tlkusb_audDesc.h"
#include "tlklib/usb/aud/tlkusb_aud.h"
#include "tlklib/usb/aud/tlkusb_audSpk.h"
#include "tlklib/usb/aud/tlkusb_audMic.h"

#include "string.h"
#include "drivers.h"



static tlkusb_audmic_ctrl_t sTlkUsbAudMicCtrl;



int tlkusb_audmic_init(void)
{
	tmemset(&sTlkUsbAudMicCtrl, 0, sizeof(tlkusb_audmic_ctrl_t));

	sTlkUsbAudMicCtrl.sampleRate = TLKUSB_AUD_MIC_SAMPLE_RATE;
	if(TLKUSB_AUD_MIC_CHANNEL_COUNT == 1) {
		//SET_FLD(reg_usb_ep_ctrl(USB_EDP_MIC), FLD_USB_EP_EOF_ISO | FLD_USB_EP_MONO);
	}
	
	sTlkUsbAudMicCtrl.curVol = TLKUSB_AUDMIC_VOL_DEF;
	
	return TLK_ENONE;
}


int tlkusb_audmic_d2hClassInfHandler(tlkusb_setup_req_t *pSetup, uint08 infNum)
{
	return -TLK_ENOSUPPORT;
}
int tlkusb_audmic_d2hClassEdpHandler(tlkusb_setup_req_t *pSetup, uint08 edpNum)
{
	int ret;
	uint08 property = pSetup->bRequest;
	uint08 ep_ctrl = pSetup->wValue >> 8;

	ret = -TLK_ENOSUPPORT;
	if(ep_ctrl == AUDIO_EPCONTROL_SamplingFreq){
		switch(property){
			case AUDIO_REQ_GetCurrent:
				ret = TLK_ENONE;
				usbhw_write_ctrl_ep_data(TLKUSB_AUD_MIC_SAMPLE_RATE & 0xff);
				usbhw_write_ctrl_ep_data(TLKUSB_AUD_MIC_SAMPLE_RATE >> 8);
				usbhw_write_ctrl_ep_data(TLKUSB_AUD_MIC_SAMPLE_RATE >> 16);
				break;
		}
	}
	
	return ret;
}

int tlkusb_audmic_h2dClassInfHandler(tlkusb_setup_req_t *pSetup, uint08 infNum)
{
	return -TLK_ENOSUPPORT;
}
int tlkusb_audmic_h2dClassEdpHandler(tlkusb_setup_req_t *pSetup, uint08 edpNum)
{
	return -TLK_ENOSUPPORT;
}


uint tlkusb_audmic_getVolume(void)
{
	return (sTlkUsbAudMicCtrl.mute << 7) | (sTlkUsbAudMicCtrl.volStep & 0x7f);
}
void tlkusb_audmic_setVolume(sint16 volume)
{
	sTlkUsbAudMicCtrl.curVol = volume;
	if(volume < TLKUSB_AUDMIC_VOL_MIN){
		sTlkUsbAudMicCtrl.volStep = 0;
	}else{
		sTlkUsbAudMicCtrl.volStep = (volume - TLKUSB_AUDMIC_VOL_MIN) / TLKUSB_AUDMIC_VOL_RES;
	}
}
void tlkusb_audmic_enterMute(bool enable)
{
	sTlkUsbAudMicCtrl.mute = enable;
}
		

int tlkusb_audmic_setInfCmdDeal(int type)
{
	if(type == AUDIO_FEATURE_MUTE){
		sTlkUsbAudMicCtrl.mute = usbhw_read_ctrl_ep_data();
	}else if(type == AUDIO_FEATURE_VOLUME){
		u16 val = usbhw_read_ctrl_ep_u16();
		tlkusb_audmic_setVolume(val);
	}else{
		
	}
	return TLK_ENONE;
}

int tlkusb_audmic_getInfCmdDeal(int req, int type)
{
	if(type == AUDIO_FEATURE_MUTE){
		usbhw_write_ctrl_ep_data(sTlkUsbAudMicCtrl.mute);
	}else if(type == AUDIO_FEATURE_VOLUME){
		switch (req) {
			case AUDIO_REQ_GetCurrent:
				usbhw_write_ctrl_ep_u16(sTlkUsbAudMicCtrl.curVol);
				break;
			case AUDIO_REQ_GetMinimum:
				usbhw_write_ctrl_ep_u16(TLKUSB_AUDMIC_VOL_MIN);
				break;
			case AUDIO_REQ_GetMaximum:
				usbhw_write_ctrl_ep_u16(TLKUSB_AUDMIC_VOL_MAX);
				break;
			case AUDIO_REQ_GetResolution:
				usbhw_write_ctrl_ep_u16(TLKUSB_AUDMIC_VOL_RES);
				break;
			default:
				return -TLK_ENOSUPPORT;
		}
	}else{
		return -TLK_ENOSUPPORT;
	}
	return TLK_ENONE;
}

int tlkusb_audmic_setEdpCmdDeal(int type)
{
	if(type == AUDIO_EPCONTROL_SamplingFreq){
		uint32 value = usbhw_read_ctrl_ep_data();
		sTlkUsbAudMicCtrl.sampleRate = value;
		value = usbhw_read_ctrl_ep_data();
		sTlkUsbAudMicCtrl.sampleRate |= value << 8;
		value = usbhw_read_ctrl_ep_data();
		sTlkUsbAudMicCtrl.sampleRate |= value << 16;
		// TODO: Sample Rate Changed
	}
	return TLK_ENONE;
}





#endif //#if (TLKUSB_AUD_MIC_ENABLE)

