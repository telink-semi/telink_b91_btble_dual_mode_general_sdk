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


static uint08 tlkusb_audmic_readData(uint08 *pBuffer, uint08 readLen);


static uint08 sTlkUsbAudMicEnable = false;
static uint08 sTlkUsbAudMicAutoZero = false;
static uint08 sTlkUsbAudMicUnitLens = (TLKUSB_AUD_MIC_SAMPLE_RATE / 1000) * 2;
static uint08 sTlkUsbAudMicBuffer[TLKUSB_AUD_MIC_BUFFER_SIZE+4];
static tlkusb_audmic_ctrl_t sTlkUsbAudMicCtrl;


int tlkusb_audmic_init(void)
{
	tmemset(&sTlkUsbAudMicCtrl, 0, sizeof(tlkusb_audmic_ctrl_t));
	
	sTlkUsbAudMicEnable = false;
	sTlkUsbAudMicCtrl.sampleRate = TLKUSB_AUD_MIC_SAMPLE_RATE;
	sTlkUsbAudMicCtrl.curVol = TLKUSB_AUDMIC_VOL_DEF;
	tlkapi_fifo_init(&sTlkUsbAudMicCtrl.fifo, false, false, sTlkUsbAudMicBuffer, TLKUSB_AUD_MIC_BUFFER_SIZE+4);
		
	return TLK_ENONE;
}


bool tlkusb_audmic_getEnable(void)
{
	return sTlkUsbAudMicEnable;
}
void tlkusb_audmic_setEnable(bool enable)
{
	sTlkUsbAudMicEnable = enable;
	if(enable){
		reg_usb_ep_ptr(TLKUSB_AUD_EDP_MIC) = 0;
		reg_usb_ep_ctrl(TLKUSB_AUD_EDP_MIC) = 0x01; //ACK first packet
	}else{
		tlkapi_fifo_clear(&sTlkUsbAudMicCtrl.fifo);
	}
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

void tlkusb_audmic_autoZero(bool enable)
{
	sTlkUsbAudMicAutoZero = enable;
}
bool tlkusb_audmic_sendData(uint08 *pData, uint16 dataLen, bool isCover)
{
	int ret;
	if(!sTlkUsbAudMicEnable) return false;
	TLKAPI_FIFO_SET_COVER(&sTlkUsbAudMicCtrl.fifo, isCover);
	ret = tlkapi_fifo_write(&sTlkUsbAudMicCtrl.fifo, pData, dataLen);
	if(ret <= 0) return false;
	return true;
}


_attribute_ram_code_sec_noinline_
void tlkusb_audmic_fillData(void)
{
	uint08 buffLen;
	uint08 buffer[64];

	usbhw_clr_eps_irq(BIT(TLKUSB_AUD_EDP_MIC));
	reg_usb_ep_ptr(TLKUSB_AUD_EDP_MIC) = 0;
	
	buffLen = 0;
	if(sTlkUsbAudMicEnable){
		uint08 readLen = 64;
		if(readLen > sTlkUsbAudMicUnitLens) readLen = sTlkUsbAudMicUnitLens;
		buffLen = tlkusb_audmic_readData(buffer, readLen);
	}
	if(buffLen != 0){
		uint08 index;
		for(index=0; index<buffLen; index++){
			reg_usb_ep_dat(TLKUSB_AUD_EDP_MIC) = buffer[index];
		}
		reg_usb_ep_ctrl(TLKUSB_AUD_EDP_MIC) = BIT(0);
	}else if(sTlkUsbAudMicAutoZero){
		uint08 index;
		for(index=0; index<sTlkUsbAudMicUnitLens; index++){
			reg_usb_ep_dat(TLKUSB_AUD_EDP_MIC) = 0x00;
		}
		reg_usb_ep_ctrl(TLKUSB_AUD_EDP_MIC) = BIT(0);
	}
}

_attribute_ram_code_sec_noinline_
static uint08 tlkusb_audmic_readData(uint08 *pBuffer, uint08 readLen)
{
	uint16 used;
	uint16 tempLen;
	uint16 woffset;
	uint16 roffset;
	tlkapi_fifo_t *pFifo;

	pFifo = &sTlkUsbAudMicCtrl.fifo;
	
	if(pFifo->buffLen == 0) return 0;
	woffset = pFifo->woffset;
	roffset = pFifo->roffset;
	if(woffset >= roffset) used = woffset-roffset;
	else used = pFifo->buffLen+woffset-roffset;
	if(used == 0) return 0;
	if(readLen > used) readLen = used;
	
	if(roffset+readLen <= pFifo->buffLen) tempLen = readLen;
	else tempLen = pFifo->buffLen-roffset;
	tmemcpy(pBuffer, pFifo->pBuffer+roffset, tempLen);
	if(tempLen < readLen){
		tmemcpy(pBuffer+tempLen, pFifo->pBuffer, readLen-tempLen);
	}
	roffset += readLen;
	if(roffset >= pFifo->buffLen) roffset -= pFifo->buffLen;
	pFifo->roffset = roffset;
	return readLen;
}


#endif //#if (TLKUSB_AUD_MIC_ENABLE)

