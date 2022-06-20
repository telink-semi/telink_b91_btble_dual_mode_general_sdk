/********************************************************************************************************
 * @file     tlkmdi_audio.h
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
#ifndef TLKMDI_AUDIO_H
#define TLKMDI_AUDIO_H

#if (TLK_MDI_AUD_ENABLE)


typedef enum{
	TLKMDI_AUDIO_EVTID_NONE = 0,
	TLKMDI_AUDIO_EVTID_START,
	TLKMDI_AUDIO_EVTID_CLOSE,
	TLKMDI_AUDIO_EVTID_PLAY_START,
	TLKMDI_AUDIO_EVTID_PLAY_OVER,
	TLKMDI_AUDIO_EVTID_VOLUME_CHANGE,
	TLKMDI_AUDIO_EVTID_STATUS_CHANGE,
}TLKMDI_AUDIO_EVTID_ENUM;

int tlkmdi_audio_init();

int tlkmdi_audio_sendStartEvt(uint08 audChn, uint16 handle);
int tlkmdi_audio_sendCloseEvt(uint08 audChn, uint16 handle);

int tlkmdi_audio_sendPlayStartEvt(uint08 audChn, uint16 playIndex);
int tlkmdi_audio_sendPlayOverEvt(uint08 audChn, uint16 playIndex);

int tlkmdi_audio_sendVolumeChangeEvt(uint08 audChn, uint08 volume);
int tlkmdi_audio_sendStatusChangeEvt(uint08 audChn, uint08 status);


uint tlkmdi_audio_getToneVolume(bool isRaw);
void tlkmdi_audio_toneVolumeInc(uint08 step);
void tlkmdi_audio_toneVolumeDec(uint08 step);
void tlkmdi_audio_setToneVolume(uint08 volume);

uint tlkmdi_audio_getMusicVolume(bool isRaw);
void tlkmdi_audio_musicVolumeInc(uint08 step);
void tlkmdi_audio_musicVolumeDec(uint08 step);
void tlkmdi_audio_setMusicVolume(uint08 volume);

uint tlkmdi_audio_getVoiceVolume(bool isRaw);
void tlkmdi_audio_voiceVolumeInc(uint08 step);
void tlkmdi_audio_voiceVolumeDec(uint08 step);
void tlkmdi_audio_setVoiceVolume(uint08 volume);

uint tlkmdi_audio_getHeadsetVolume(bool isRaw);
void tlkmdi_audio_headsetVolumeInc(uint08 step);
void tlkmdi_audio_headsetVolumeDec(uint08 step);
void tlkmdi_audio_setHeadsetVolume(uint08 volume);


#endif //#if (TLK_MDI_AUD_ENABLE)

#endif //TLKMDI_AUD_HF_H

