/********************************************************************************************************
 * @file     tlkmdi_audio.c
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
#include "tlkmdi/tlkmdi_stdio.h"
#if (TLK_MDI_AUD_ENABLE)
#include "tlkmdi/tlkmdi_event.h"
#include "tlkmdi/tlkmdi_audio.h"
#include "tlkprt/tlkprt_stdio.h"
#include "tlkstk/bt/btp/avrcp/btp_avrcp.h"


static bool tlkmdi_audio_volumeInc(uint08 *pSrcVolume, uint08 *pCalVolume, uint08 step);
static bool tlkmdi_audio_volumeDec(uint08 *pSrcVolume, uint08 *pCalVolume, uint08 step);
static bool tlkmdi_audio_setVolume(uint08 *pSrcVolume, uint08 *pCalVolume, uint08 volume);
static void tlkmdi_audio_getRawVolume(uint08 srcVolume, uint08 *pCalVolume);
static void tlkmdi_audio_volumeChangeEvt(uint16 aclHandle, uint08 volume);


static uint08 sTlkMdiAudioToneVolume = TLKMDI_AUDIO_VOLUME_DEF;
static uint08 sTlkMdiAudioMusicVolume = TLKMDI_AUDIO_VOLUME_DEF;
static uint08 sTlkMdiAudioVoiceVolume = TLKMDI_AUDIO_VOLUME_DEF;
static uint08 sTlkMdiAudioHeadsetVolume = TLKMDI_AUDIO_VOLUME_DEF;

int tlkmdi_audio_init() {
    btp_avrcp_regVolumeChangeCB(tlkmdi_audio_volumeChangeEvt);
    return TLK_ENONE;
}

int tlkmdi_audio_sendStartEvt(uint08 audChn, uint16 handle)
{
	uint08 buffLen;
	uint08 buffer[4];
	buffLen = 0;
	buffer[buffLen++] = audChn;
	buffer[buffLen++] = (handle & 0x00FF);
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	return tlkmdi_sendAudioEvent(TLKMDI_AUDIO_EVTID_START, buffer, buffLen);
}
int tlkmdi_audio_sendCloseEvt(uint08 audChn, uint16 handle)
{
	uint08 buffLen;
	uint08 buffer[4];
	buffLen = 0;
	buffer[buffLen++] = audChn;
	buffer[buffLen++] = (handle & 0x00FF);
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	return tlkmdi_sendAudioEvent(TLKMDI_AUDIO_EVTID_CLOSE, buffer, buffLen);
}

int tlkmdi_audio_sendPlayStartEvt(uint08 audChn, uint16 playIndex)
{
	uint08 buffLen;
	uint08 buffer[4];
	buffLen = 0;
	buffer[buffLen++] = audChn;
	buffer[buffLen++] = (playIndex & 0x00FF);
	buffer[buffLen++] = (playIndex & 0xFF00) >> 8;
	return tlkmdi_sendAudioEvent(TLKMDI_AUDIO_EVTID_PLAY_START, buffer, buffLen);
}
int tlkmdi_audio_sendPlayOverEvt(uint08 audChn, uint16 playIndex)
{
	uint08 buffLen;
	uint08 buffer[4];
	buffLen = 0;
	buffer[buffLen++] = audChn;
	buffer[buffLen++] = (playIndex & 0x00FF);
	buffer[buffLen++] = (playIndex & 0xFF00) >> 8;
	return tlkmdi_sendAudioEvent(TLKMDI_AUDIO_EVTID_PLAY_OVER, buffer, buffLen);
}

int tlkmdi_audio_sendVolumeChangeEvt(uint08 audChn, uint08 volume)
{
	uint08 buffLen;
	uint08 buffer[4];
	buffLen = 0;
	buffer[buffLen++] = audChn;
	buffer[buffLen++] = volume;
	return tlkmdi_sendAudioEvent(TLKMDI_AUDIO_EVTID_VOLUME_CHANGE, buffer, buffLen);
}
int tlkmdi_audio_sendStatusChangeEvt(uint08 audChn, uint08 status)
{
	uint08 buffLen;
	uint08 buffer[4];
	buffLen = 0;
	buffer[buffLen++] = audChn;
	buffer[buffLen++] = status;
	return tlkmdi_sendAudioEvent(TLKMDI_AUDIO_EVTID_STATUS_CHANGE, buffer, buffLen);
}



uint tlkmdi_audio_getToneVolume(bool isRaw)
{
	uint08 volume;
	if(!isRaw) volume = sTlkMdiAudioToneVolume;
	else tlkmdi_audio_getRawVolume(sTlkMdiAudioToneVolume, &volume);
	return volume;
}
void tlkmdi_audio_toneVolumeInc(uint08 step)
{
	uint08 calVolume;
	if(tlkmdi_audio_volumeInc(&sTlkMdiAudioToneVolume, &calVolume, step)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_TONE, calVolume);
	}
}
void tlkmdi_audio_toneVolumeDec(uint08 step)
{
	uint08 calVolume;
	if(tlkmdi_audio_volumeDec(&sTlkMdiAudioToneVolume, &calVolume, step)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_TONE, calVolume);
	}
}
void tlkmdi_audio_setToneVolume(uint08 volume)
{
	uint08 calVolume;
	if(tlkmdi_audio_setVolume(&sTlkMdiAudioToneVolume, &calVolume, volume)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_TONE, calVolume);
	}
}

uint tlkmdi_audio_getMusicVolume(bool isRaw)
{
	uint08 volume;
	if(!isRaw) volume = sTlkMdiAudioMusicVolume;
	else tlkmdi_audio_getRawVolume(sTlkMdiAudioMusicVolume, &volume);
	return volume;
}
void tlkmdi_audio_musicVolumeInc(uint08 step)
{
	uint08 calVolume;
	if(tlkmdi_audio_volumeInc(&sTlkMdiAudioMusicVolume, &calVolume, step)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_MUSIC, calVolume);
	}
}
void tlkmdi_audio_musicVolumeDec(uint08 step)
{
	uint08 calVolume;
	if(tlkmdi_audio_volumeDec(&sTlkMdiAudioMusicVolume, &calVolume, step)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_MUSIC, calVolume);
	}
}
void tlkmdi_audio_setMusicVolume(uint08 volume)
{
	uint08 calVolume;
	if(tlkmdi_audio_setVolume(&sTlkMdiAudioMusicVolume, &calVolume, volume)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_MUSIC, calVolume);
	}
}

uint tlkmdi_audio_getVoiceVolume(bool isRaw)
{
	uint08 volume;
	if(!isRaw) volume = sTlkMdiAudioVoiceVolume;
	else tlkmdi_audio_getRawVolume(sTlkMdiAudioVoiceVolume, &volume);
	return volume;
}
void tlkmdi_audio_voiceVolumeInc(uint08 step)
{
	uint08 calVolume;
	if(tlkmdi_audio_volumeInc(&sTlkMdiAudioVoiceVolume, &calVolume, step)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_VOICE, calVolume);
	}
}
void tlkmdi_audio_voiceVolumeDec(uint08 step)
{
	uint08 calVolume;
	if(tlkmdi_audio_volumeDec(&sTlkMdiAudioVoiceVolume, &calVolume, step)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_VOICE, calVolume);
	}
}
void tlkmdi_audio_setVoiceVolume(uint08 volume)
{
	uint08 calVolume;
	if(tlkmdi_audio_setVolume(&sTlkMdiAudioVoiceVolume, &calVolume, volume)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_VOICE, calVolume);
	}
}

uint tlkmdi_audio_getHeadsetVolume(bool isRaw)
{
	uint08 volume;
	if(!isRaw) volume = sTlkMdiAudioHeadsetVolume;
	else tlkmdi_audio_getRawVolume(sTlkMdiAudioHeadsetVolume, &volume);
	return volume;
}
void tlkmdi_audio_headsetVolumeInc(uint08 step)
{
	uint08 calVolume;
	if(tlkmdi_audio_volumeInc(&sTlkMdiAudioHeadsetVolume, &calVolume, step)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_HEADSET, calVolume);
	}
}
void tlkmdi_audio_headsetVolumeDec(uint08 step)
{
	uint08 calVolume;
	if(tlkmdi_audio_volumeDec(&sTlkMdiAudioHeadsetVolume, &calVolume, step)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_HEADSET, calVolume);
	}
}
void tlkmdi_audio_setHeadsetVolume(uint08 volume)
{
	uint08 calVolume;
	if(tlkmdi_audio_setVolume(&sTlkMdiAudioHeadsetVolume, &calVolume, volume)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_HEADSET, calVolume);
	}
}

static void tlkmdi_audio_getRawVolume(uint08 srcVolume, uint08 *pCalVolume)
{
	if(srcVolume > 128) srcVolume = 128;
	*pCalVolume = (((uint16)srcVolume*25)>>5) + 1; //((uint16)srcVolume*100)/128;
	
}
static bool tlkmdi_audio_volumeInc(uint08 *pSrcVolume, uint08 *pCalVolume, uint08 step)
{
	uint08 valDiff;
	uint08 rawValue;
	uint08 calValue;

	if(step == 0) return false;
	step = ((uint16)step<<TLKMDI_AUDIO_VOLUME_EXPAND)/100;
	rawValue = *pSrcVolume;
	if(rawValue+step <= 128) valDiff = step;
	else valDiff = 128-rawValue;
	if(valDiff == 0) return false;
	rawValue += valDiff;
	calValue = ((uint16)rawValue*100)>>TLKMDI_AUDIO_VOLUME_EXPAND;
	if(rawValue != 0 && calValue == 0) calValue = 1;
	if(pSrcVolume != nullptr) *pSrcVolume = rawValue;
	if(pCalVolume != nullptr) *pCalVolume = calValue;
	return true;
}
static bool tlkmdi_audio_volumeDec(uint08 *pSrcVolume, uint08 *pCalVolume, uint08 step)
{
	uint08 valDiff;
	uint08 rawValue;
	uint08 calValue;

	if(step == 0) return false;
	step = ((uint16)step<<TLKMDI_AUDIO_VOLUME_EXPAND)/100;
	rawValue = *pSrcVolume;
	if(rawValue < step) valDiff = rawValue;
	else valDiff = step;
	if(valDiff == 0) return false;
	rawValue -= valDiff;
	calValue = ((uint16)rawValue*100)>>TLKMDI_AUDIO_VOLUME_EXPAND;
	if(rawValue != 0 && calValue == 0) calValue = 1;
	if(pSrcVolume != nullptr) *pSrcVolume = rawValue;
	if(pCalVolume != nullptr) *pCalVolume = calValue;
	return true;
}
static bool tlkmdi_audio_setVolume(uint08 *pSrcVolume, uint08 *pCalVolume, uint08 volume)
{
	uint08 rawValue;
	uint08 calValue;

	rawValue = *pSrcVolume;
	if(volume > 100) volume = 100;
	calValue = (volume<<TLKMDI_AUDIO_VOLUME_EXPAND)/100;
	if(rawValue == calValue) return false;
	if(pSrcVolume != nullptr) *pSrcVolume = calValue;
	if(pCalVolume != nullptr) *pCalVolume = volume;
	return true;
}

static void tlkmdi_audio_volumeChangeEvt(uint16 aclHandle, uint08 volume) {
    sTlkMdiAudioMusicVolume = volume;
}


#endif //#if (TLK_MDI_AUD_ENABLE)

