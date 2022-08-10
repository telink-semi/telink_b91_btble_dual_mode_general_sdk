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
#if (TLK_MDI_AUDIO_ENABLE)
#include "tlkmdi/tlkmdi_btacl.h"
#include "tlkmdi/tlkmdi_event.h"
#include "tlkmdi/tlkmdi_audio.h"
#include "tlkprt/tlkprt_stdio.h"
#include "tlkstk/bt/btp/avrcp/btp_avrcp.h"

#define TLKMDI_AUDIO_ANDROID_VOLUME_STEP_MAX      16
#define TLKMDI_AUDIO_IOS_VOLUME_STEP_MAX          17
#define TLKMDI_AUDIO_MUSIC_PLAYER_VOLUME_STEP_MAX 33
#define TLKMDI_AUDIO_TONE_VOLUME_STEP_MAX         26

#define TLKMDI_AUDIO_ANDROID_TOTAL_VOLUME_STEP       TLKMDI_AUDIO_ANDROID_VOLUME_STEP_MAX
#define TLKMDI_AUDIO_IOS_TOTAL_VOLUME_STEP           TLKMDI_AUDIO_IOS_VOLUME_STEP_MAX
#define TLKMDI_AUDIO_MUSIC_PLAYER_TOTAL_VOLUME_STEP  TLKMDI_AUDIO_MUSIC_PLAYER_VOLUME_STEP_MAX
#define TLKMDI_AUDIO_TONE_TOTAL_VOLUME_STEP          TLKMDI_AUDIO_TONE_VOLUME_STEP_MAX


#define TLKMDI_AUDIO_VENDOR_IOS_DEVICE          1
#define TLKMDI_AUDIO_VENDOR_ANDROID_DEVICE      0

//#define TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE       1

#define TLKMDI_AUD_DBG_FLAG         (TLKMDI_AUD_DBG_ENABLE | TLKMDI_DBG_FLAG) 
#define TLKMDI_AUD_DBG_SIGN         TLKMDI_DBG_SIGN

#ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
static bool tlkmdi_audio_volumeInc(uint08 *pSrcVolume, uint08 *pCalVolume, uint08 step);
static bool tlkmdi_audio_volumeDec(uint08 *pSrcVolume, uint08 *pCalVolume, uint08 step);
static bool tlkmdi_audio_setVolume(uint08 *pSrcVolume, uint08 *pCalVolume, uint08 volume);
static void tlkmdi_audio_getRawVolume(uint08 srcVolume, uint08 *pCalVolume);
#else
static void tlkmdi_audio_getRawVolumeNew(uint08 srcVolume, uint08 *pCalVolume, uint16 type);
static bool tlkmdi_audio_volumeIncNew(uint08 *pSrcVolume, uint08 *pCalVolume, uint16 type);
static bool tlkmdi_audio_volumeDecNew(uint08 *pSrcVolume, uint08 *pCalVolume, uint16 type);
static bool tlkmdi_audio_setVolumeNew(uint08 *pSrcVolume, uint08 *pCalVolume, uint08 volume, uint16 type);
static void tlkmdi_audio_getAudioParamter(uint16 audioType, uint08* pAudioParamter, uint16* step);
static bool tlkmdi_audio_calAudio(uint16 type, uint08* pSrcVolume, uint08* pCalVolume, uint08 *vol, int index, int step);
static void tlkmdi_audio_getAudioUsrTable(uint16 type, uint08 *usrvol, uint16 pVendor);
#endif
static void tlkmdi_audio_volumeChangeEvt(uint16 aclHandle, uint08 volume);
static uint08 sTlkMdiAudioToneVolume = TLKMDI_AUDIO_VOLUME_DEF;
static uint08 sTlkMdiAudioMusicVolume = TLKMDI_AUDIO_VOLUME_DEF;
static uint08 sTlkMdiAudioVoiceVolume = TLKMDI_AUDIO_VOLUME_DEF;
static uint08 sTlkMdiAudioHeadsetVolume = TLKMDI_AUDIO_VOLUME_DEF;

extern uint16 gTlkMmiAudioCurHandle;

#if TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
static uint08 bt_tone_real_vol_table[TLKMDI_AUDIO_TONE_TOTAL_VOLUME_STEP] =
{
 0,
 5,
 10,
 15,
 20,
 25,
 30,
 35,
 40,
 45,
 50,
 55,
 60,
 65,
 70,
 75,
 80,
 85,
 90,
 95,
 100,
 105,
 110,
 115,
 120,
 127,
};

static uint08 bt_music_player_real_vol_table[TLKMDI_AUDIO_MUSIC_PLAYER_TOTAL_VOLUME_STEP] =
{
  0,   //-99 0 0
  4,   //-64 6 1
  8,   //-58 8 2
  12,  //-50 7 3
  16,  //-43 5 4
  20,  //-38 5 5
  24,  //-33 4 6
  28,  //-29 4 7
  32,  //-25 4 8
  36,  //-21 4 9
  40,  //-17 3 10
  44,  //-14 2 11
  48,  //-12 2 12
  52,  //-10 2 13
  56,  //-8 2 14
  60,  //-6 2 15
  64,  //-4 0 16
  68,
  72,
  76,
  80,
  84,
  88,
  92,
  96,
  100,
  104,
  108,
  112,
  116,
  120,
  124,
  127
};

static uint08 bt_a2dp_real_vol_table_ios[TLKMDI_AUDIO_IOS_TOTAL_VOLUME_STEP] =
{
 0,  //-99 0 0
 1,  //-64 6 1
 2,  //-58 8 2
 5,  //-50 7 3
 9,  //-43 5 4
 14,  //-38 5 5
 20,  //-33 4 6
 27,  //-29 4 7
 35,  //-25 4 8
 44,  //-21 4 9
 54, //-17 3 10
 65, //-14 2 11
 77, //-12 2 12
 90, //-10 2 13
 114, //-8 2 14
 120, //-6 2 15
 127    //-4 0 16
};

static uint08 bt_a2dp_real_vol_table_android[TLKMDI_AUDIO_ANDROID_TOTAL_VOLUME_STEP] =
{
 0,  //-99 0 0
 2,  //-58 8 2
 5,  //-50 7 3
 9,  //-43 5 4
 14,  //-38 5 5
 20,  //-33 4 6
 31,  //-29 4 7
 50,  //-25 4 8
 68,  //-21 4 9
 85, //-17 3 10
 99, //-14 2 11
 109, //-12 2 12
 115, //-10 2 13
 120, //-8 2 14
 124, //-6 2 15
 127  //-4 0 16
};

static uint08 bt_voice_real_vol_table_ios[TLKMDI_AUDIO_IOS_TOTAL_VOLUME_STEP] =
{
 0,  //-99 0 0
 1,  //-64 6 1
 2,  //-58 8 2
 5,  //-50 7 3
 9,  //-43 5 4
 14,  //-38 5 5
 20,  //-33 4 6
 27,  //-29 4 7
 35,  //-25 4 8
 44,  //-21 4 9
 54, //-17 3 10
 65, //-14 2 11
 77, //-12 2 12
 90, //-10 2 13
 104, //-8 2 14
 115, //-6 2 15
 127  //-4 0 16
};

static uint08 bt_voice_real_vol_table_android[TLKMDI_AUDIO_ANDROID_TOTAL_VOLUME_STEP] =
{
 0,  //-99 0 0
 2,  //-58 8 2
 5,  //-50 7 3
 9,  //-43 5 4
 14,  //-38 5 5
 20,  //-33 4 6
 27,  //-29 4 7
 35,  //-25 4 8
 44,  //-21 4 9
 54, //-17 3 10
 65, //-14 2 11
 77, //-12 2 12
 90, //-10 2 13
 104, //-8 2 14
 115, //-6 2 15
 127  //-4 0 16
};


static uint08 usr_tone_vol_table[TLKMDI_AUDIO_TONE_TOTAL_VOLUME_STEP] =
{
 0,
 4,
 8,
 12,
 16,
 20,
 24,
 28,
 32,
 36,
 40,
 44,
 48,
 52,
 56,
 60,
 64,
 68,
 72,
 76,
 80,
 84,
 88,
 92,
 96,
 100,
};


static uint08 usr_music_player_vol_table[TLKMDI_AUDIO_MUSIC_PLAYER_TOTAL_VOLUME_STEP] =
{
 0,
 3,
 6,
 9,
 12,
 15,
 18,
 21,
 24,
 27,
 30,
 33,
 36,
 39,
 42,
 45,
 48,
 51,
 54,
 57,
 60,
 63,
 66,
 69,
 72,
 75,
 78,
 81,
 84,
 88,
 92,
 96,
 100,
};


static uint08 usr_vol_table_ios[TLKMDI_AUDIO_IOS_TOTAL_VOLUME_STEP] =
{
 0,
 6,
 12,
 18,
 24,
 30,
 36,
 42,
 48,
 54,
 60,
 66,
 72,
 78,
 84,
 90,
 100,
};

static uint08 usr_vol_table_android[TLKMDI_AUDIO_ANDROID_TOTAL_VOLUME_STEP] =
{
 0,
 7,
 14,
 21,
 28,
 35,
 41,
 48,
 55,
 62,
 69,
 76,
 83,
 90,
 97,
 100,
};
#endif

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
	uint08 volume = 0;
	if(!isRaw) volume = sTlkMdiAudioToneVolume;
	else {
        #ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
		tlkmdi_audio_getRawVolume(sTlkMdiAudioToneVolume, &volume);
        #else
	    tlkmdi_audio_getRawVolumeNew(sTlkMdiAudioToneVolume, &volume, TLKPRT_COMM_VOLUME_TYPE_TONE);
		#endif
	}
	return volume;
}
void tlkmdi_audio_toneVolumeInc(uint08 step)
{
	uint08 calVolume = 0;
	#ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
	if(tlkmdi_audio_volumeInc(&sTlkMdiAudioToneVolume, &calVolume, step)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_TONE, calVolume);
	}
	#else
	if(tlkmdi_audio_volumeIncNew(&sTlkMdiAudioToneVolume, &calVolume, TLKPRT_COMM_VOLUME_TYPE_TONE)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_TONE, calVolume);
	}
	#endif
}
void tlkmdi_audio_toneVolumeDec(uint08 step)
{
	uint08 calVolume = 0;
	#ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
	if(tlkmdi_audio_volumeDec(&sTlkMdiAudioToneVolume, &calVolume, step)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_TONE, calVolume);
	}
	#else
	if(tlkmdi_audio_volumeDecNew(&sTlkMdiAudioToneVolume, &calVolume, TLKPRT_COMM_VOLUME_TYPE_TONE)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_TONE, calVolume);
	}
	#endif
}
void tlkmdi_audio_setToneVolume(uint08 volume)
{
	uint08 calVolume = 0;
	#ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
	if(tlkmdi_audio_setVolume(&sTlkMdiAudioToneVolume, &calVolume, volume)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_TONE, calVolume);
	}
	#else
	if(tlkmdi_audio_setVolumeNew(&sTlkMdiAudioToneVolume, &calVolume, volume, TLKPRT_COMM_VOLUME_TYPE_TONE)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_TONE, calVolume);
	}
	#endif
}

uint tlkmdi_audio_getMusicVolume(bool isRaw)
{
	uint08 volume = 0;
	if(!isRaw) volume = sTlkMdiAudioMusicVolume;
	else {
        #ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
		tlkmdi_audio_getRawVolume(sTlkMdiAudioMusicVolume, &volume);
		#else
	    tlkmdi_audio_getRawVolumeNew(sTlkMdiAudioMusicVolume, &volume, TLKPRT_COMM_VOLUME_TYPE_MUSIC);
		#endif
	}
	return volume;
}
void tlkmdi_audio_musicVolumeInc(uint08 step)
{
	uint08 calVolume = 0;
	#ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
	if(tlkmdi_audio_volumeInc(&sTlkMdiAudioMusicVolume, &calVolume, step)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_MUSIC, calVolume);
	}
	#else
	tlkapi_trace(TLKMDI_AUD_DBG_FLAG, TLKMDI_AUD_DBG_SIGN, "Music volume inc start");
	if(tlkmdi_audio_volumeIncNew(&sTlkMdiAudioMusicVolume, &calVolume, TLKPRT_COMM_VOLUME_TYPE_MUSIC)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_MUSIC, calVolume);
	}
	#endif
}
void tlkmdi_audio_musicVolumeDec(uint08 step)
{
	uint08 calVolume = 0;
	#ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
	if(tlkmdi_audio_volumeDec(&sTlkMdiAudioMusicVolume, &calVolume, step)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_MUSIC, calVolume);
	}
	#else
	if(tlkmdi_audio_volumeDecNew(&sTlkMdiAudioMusicVolume, &calVolume, TLKPRT_COMM_VOLUME_TYPE_MUSIC)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_MUSIC, calVolume);
	}
	#endif
}
void tlkmdi_audio_setMusicVolume(uint08 volume)
{
	uint08 calVolume = 0;
	#ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
	if(tlkmdi_audio_setVolume(&sTlkMdiAudioMusicVolume, &calVolume, volume)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_MUSIC, calVolume);
	}
	#else
	if(tlkmdi_audio_setVolumeNew(&sTlkMdiAudioMusicVolume, &calVolume, volume, TLKPRT_COMM_VOLUME_TYPE_MUSIC)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_MUSIC, calVolume);
	}
	#endif
}

uint tlkmdi_audio_getVoiceVolume(bool isRaw)
{
	uint08 volume = 0;
	if(!isRaw) volume = sTlkMdiAudioVoiceVolume;
	else {
        #ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
		tlkmdi_audio_getRawVolume(sTlkMdiAudioVoiceVolume, &volume);
		#else
	    tlkmdi_audio_getRawVolumeNew(sTlkMdiAudioVoiceVolume, &volume, TLKPRT_COMM_VOLUME_TYPE_VOICE);
		#endif
	}
	return volume;
}
void tlkmdi_audio_voiceVolumeInc(uint08 step)
{
	uint08 calVolume = 0;
	#ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
	if(tlkmdi_audio_volumeInc(&sTlkMdiAudioVoiceVolume, &calVolume, step)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_VOICE, calVolume);
	}
	#else
	if(tlkmdi_audio_volumeIncNew(&sTlkMdiAudioVoiceVolume, &calVolume, TLKPRT_COMM_VOLUME_TYPE_VOICE)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_VOICE, calVolume);
	}
	#endif
}
void tlkmdi_audio_voiceVolumeDec(uint08 step)
{
	uint08 calVolume = 0;
	#ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
	if(tlkmdi_audio_volumeDec(&sTlkMdiAudioVoiceVolume, &calVolume, step)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_VOICE, calVolume);
	}
	#else
	if(tlkmdi_audio_volumeDecNew(&sTlkMdiAudioVoiceVolume, &calVolume, TLKPRT_COMM_VOLUME_TYPE_VOICE)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_VOICE, calVolume);
	}
	#endif
}
void tlkmdi_audio_setVoiceVolume(uint08 volume)
{
	uint08 calVolume = 0;
    #ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
	if(tlkmdi_audio_setVolume(&sTlkMdiAudioVoiceVolume, &calVolume, volume)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_VOICE, calVolume);
	}
	#else
	if(tlkmdi_audio_setVolumeNew(&sTlkMdiAudioVoiceVolume, &calVolume, volume, TLKPRT_COMM_VOLUME_TYPE_VOICE)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_VOICE, calVolume);
	}
	#endif
}

uint tlkmdi_audio_getHeadsetVolume(bool isRaw)
{
	uint08 volume = 0;
	if(!isRaw) volume = sTlkMdiAudioHeadsetVolume;
	else {
	    #ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
		tlkmdi_audio_getRawVolume(sTlkMdiAudioHeadsetVolume, &volume);
		#else
	    tlkmdi_audio_getRawVolumeNew(sTlkMdiAudioHeadsetVolume, &volume, TLKPRT_COMM_VOLUME_TYPE_HEADSET);
		#endif
	}
	return volume;
}
void tlkmdi_audio_headsetVolumeInc(uint08 step)
{
	uint08 calVolume = 0;
	#ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
	if(tlkmdi_audio_volumeInc(&sTlkMdiAudioHeadsetVolume, &calVolume, step)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_HEADSET, calVolume);
	}
	#else
	if(tlkmdi_audio_volumeIncNew(&sTlkMdiAudioHeadsetVolume, &calVolume, TLKPRT_COMM_VOLUME_TYPE_HEADSET)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_HEADSET, calVolume);
	}
	#endif
}
void tlkmdi_audio_headsetVolumeDec(uint08 step)
{
	uint08 calVolume = 0;
	#ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
	if(tlkmdi_audio_volumeDec(&sTlkMdiAudioHeadsetVolume, &calVolume, step)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_HEADSET, calVolume);
	}
	#else
	if(tlkmdi_audio_volumeDecNew(&sTlkMdiAudioHeadsetVolume, &calVolume, TLKPRT_COMM_VOLUME_TYPE_HEADSET)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_HEADSET, calVolume);
	}
	#endif
}
void tlkmdi_audio_setHeadsetVolume(uint08 volume)
{
	uint08 calVolume = 0;
	#ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
	if(tlkmdi_audio_setVolume(&sTlkMdiAudioHeadsetVolume, &calVolume, volume)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_HEADSET, calVolume);
	}
	#else
	if(tlkmdi_audio_setVolumeNew(&sTlkMdiAudioHeadsetVolume, &calVolume, volume, TLKPRT_COMM_VOLUME_TYPE_HEADSET)){
		tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_HEADSET, calVolume);
	}
	#endif
}
#ifndef TLKMDI_AUDIO_VOLUME_NEWCAL_ENABLE
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
	if(*pSrcVolume > 128) *pSrcVolume = 128;
	if(*pCalVolume > 100) *pCalVolume = 100;
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
	if(*pSrcVolume < 0) *pSrcVolume = 0;
	if(*pCalVolume < 0) *pCalVolume = 0;
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
#else
static void tlkmdi_audio_getRawVolumeNew(uint08 srcVolume, uint08 *pCalVolume, uint16 type)
{
    int i;
    uint16 step = 0;
	uint08 *vol = NULL;

	tlkapi_trace(TLKMDI_AUD_DBG_FLAG, TLKMDI_AUD_DBG_SIGN, "tlkmdi_audio_getRawVolumeNew start srcVol %d, calVol %d", srcVolume, *pCalVolume);

	tlkmdi_audio_getAudioParamter(type, vol, &step);
    if (vol == NULL || step == 0){
        return;
	}

	//assure the step index
	for(i = 1; i < step; i++){
        if (vol[i] > srcVolume && vol[i - 1] <= srcVolume){
			i = i - 1;
            break;
		}
	}

	tlkmdi_audio_calAudio(type, &srcVolume, pCalVolume, vol, i, step);

	tlkapi_trace(TLKMDI_AUD_DBG_FLAG, TLKMDI_AUD_DBG_SIGN, "tlkmdi_audio_getRawVolumeNew srcVol %d, calVol %d", srcVolume, *pCalVolume);
}
static bool tlkmdi_audio_volumeIncNew(uint08 *pSrcVolume, uint08 *pCalVolume, uint16 type)
{
    int i;
    uint16 step = 0;
	uint08 *vol = NULL;

	tlkapi_trace(TLKMDI_AUD_DBG_FLAG, TLKMDI_AUD_DBG_SIGN, "tlkmdi_audio_volumeIncNew start srcVol %d, calVol %d", *pSrcVolume, *pCalVolume);
	tlkmdi_audio_getAudioParamter(type, vol, &step);
    if (vol == NULL || step == 0){
        return false;
	}

    for(i = 1; i < step; i++){
        if (vol[i] > *pSrcVolume && vol[i - 1] <= *pSrcVolume){
            break;
		}
	}

	tlkmdi_audio_calAudio(type, pSrcVolume, pCalVolume, vol, i, step);

	tlkapi_trace(TLKMDI_AUD_DBG_FLAG, TLKMDI_AUD_DBG_SIGN, "tlkmdi_audio_volumeIncNew srcVol %d, calVol %d", *pSrcVolume, *pCalVolume);

	return true;
}
static bool tlkmdi_audio_volumeDecNew(uint08 *pSrcVolume, uint08 *pCalVolume, uint16 type)
{
    int i;
    uint16 step = 0;
	uint08 *vol = NULL;

    tlkapi_trace(TLKMDI_AUD_DBG_FLAG, TLKMDI_AUD_DBG_SIGN, "tlkmdi_audio_volumeDecNew srcVol %d, calVol %d", *pSrcVolume, *pCalVolume);
	tlkmdi_audio_getAudioParamter(type, vol, &step);
    if (vol == NULL || step == 0){
        return false;
	}

    for(i = step - 1; i > 0; i--){
        if (vol[i] >= *pSrcVolume && vol[i - 1] < *pSrcVolume){
			i = i - 1;
            break;
		}
	}
	tlkmdi_audio_calAudio(type, pSrcVolume, pCalVolume, vol, i, step);

	tlkapi_trace(TLKMDI_AUD_DBG_FLAG, TLKMDI_AUD_DBG_SIGN, "tlkmdi_audio_volumeDecNew srcVol %d, calVol %d", *pSrcVolume, *pCalVolume);

	return true;
}
static bool tlkmdi_audio_setVolumeNew(uint08 *pSrcVolume, uint08 *pCalVolume, uint08 volume, uint16 type)
{
    int i;
    uint16 step = 0;
	uint08 *vol = NULL;
	tlkapi_trace(TLKMDI_AUD_DBG_FLAG, TLKMDI_AUD_DBG_SIGN, "tlkmdi_audio_setVolumeNew start srcVol %d, calVol %d", *pSrcVolume, *pCalVolume);

	tlkmdi_audio_getAudioParamter(type, vol, &step);
    if (vol == NULL || step == 0){
        return false;
	}

    for(i = 1; i < step; i++){
        if (vol[i] > volume && vol[i - 1] <= volume){
			i = i - 1;
            break;
		}
	}
	tlkmdi_audio_calAudio(type, pSrcVolume, pCalVolume, vol, i, step);
	tlkapi_trace(TLKMDI_AUD_DBG_FLAG, TLKMDI_AUD_DBG_SIGN, "tlkmdi_audio_setVolumeNew srcVol %d, calVol %d", *pSrcVolume, *pCalVolume);

	return true;
}

static void tlkmdi_audio_getAudioParamter(uint16 audioType, uint08* pAudioParamter, uint16* step)
{
    uint16 pVendor = 0;

	
	pVendor = tlkmdi_btacl_get_peer_devType(gTlkMmiAudioCurHandle);

	if (audioType == TLKPRT_COMM_VOLUME_TYPE_TONE){
		pAudioParamter = &bt_tone_real_vol_table[0];
		*step = TLKMDI_AUDIO_TONE_VOLUME_STEP_MAX;
	} else if (audioType == TLKPRT_COMM_VOLUME_TYPE_MUSIC){
		pAudioParamter = &bt_music_player_real_vol_table[0];
		*step = TLKMDI_AUDIO_MUSIC_PLAYER_VOLUME_STEP_MAX;
	} else {
	    if (pVendor == TLKMDI_AUDIO_VENDOR_ANDROID_DEVICE){
            if (audioType == TLKPRT_COMM_VOLUME_TYPE_VOICE){
		        pAudioParamter = &bt_voice_real_vol_table_android[0];
		        *step = TLKMDI_AUDIO_ANDROID_VOLUME_STEP_MAX;
		    } else if (audioType == TLKPRT_COMM_VOLUME_TYPE_HEADSET){
		        pAudioParamter = &bt_a2dp_real_vol_table_android[0];
		        *step = TLKMDI_AUDIO_ANDROID_VOLUME_STEP_MAX;
		    }
	    } else {
            if (audioType == TLKPRT_COMM_VOLUME_TYPE_VOICE){
		        pAudioParamter = &bt_voice_real_vol_table_ios[0];
		        *step = TLKMDI_AUDIO_ANDROID_VOLUME_STEP_MAX;
	        } else {
		        pAudioParamter = &bt_a2dp_real_vol_table_ios[0];
		        *step = TLKMDI_AUDIO_ANDROID_VOLUME_STEP_MAX;
	        }
	    }
	}
}

static bool tlkmdi_audio_calAudio(uint16 type, uint08* pSrcVolume, uint08* pCalVolume, uint08 *vol, int index, int step)
{
    uint08* usrTable = NULL;
    uint16 pVendor = 0;

	tlkapi_trace(TLKMDI_AUD_DBG_FLAG, TLKMDI_AUD_DBG_SIGN, "tlkmdi_audio_calAudio enter!");
	pVendor = tlkmdi_btacl_get_peer_devType(gTlkMmiAudioCurHandle);
    tlkmdi_audio_getAudioUsrTable(type, usrTable, pVendor);
	if (usrTable == NULL){
		tlkapi_trace(TLKMDI_AUD_DBG_FLAG, TLKMDI_AUD_DBG_SIGN, "tlkmdi_audio_calAudio User Volume Table is NULL");
        return false;
	}
	
	if (*pSrcVolume <= vol[0]) {
		*pCalVolume = usrTable[0];
		*pSrcVolume = vol[0];
	} else if (*pSrcVolume >= vol[step - 1]){
		*pCalVolume = usrTable[step - 1];
		*pSrcVolume = vol[step - 1];
	}else {
		*pCalVolume = usrTable[index];
		*pSrcVolume = vol[index];
	}
	return true;
}

static void tlkmdi_audio_getAudioUsrTable(uint16 type, uint08 *usrVol, uint16 pVendor)
{
	if (type == TLKPRT_COMM_VOLUME_TYPE_TONE){
		usrVol = &usr_tone_vol_table[0];
	} else if(type == TLKPRT_COMM_VOLUME_TYPE_MUSIC){
		usrVol = &usr_music_player_vol_table[0];
	} else {
		if (pVendor == TLKMDI_AUDIO_VENDOR_ANDROID_DEVICE){
			usrVol = &usr_vol_table_android[0];
	    } else {
	    	usrVol = &usr_vol_table_ios[0];
	    }
    }
}

#endif

static void tlkmdi_audio_volumeChangeEvt(uint16 aclHandle, uint08 volume) {
    sTlkMdiAudioMusicVolume = volume;
}


#endif //#if (TLK_MDI_AUDIO_ENABLE)

