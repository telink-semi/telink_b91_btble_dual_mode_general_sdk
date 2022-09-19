/********************************************************************************************************
 * @file     tlkmmi_audioCtrl.c
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
#include "tlkdev/tlkdev_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi/tlkmmi_stdio.h"
#if (TLKMMI_AUDIO_ENABLE)
#include "tlkmmi/tlkmmi_adapt.h"
#include "tlkprt/tlkprt_comm.h"
#include "tlkmmi/audio/tlkmmi_audio.h"
#include "tlkmmi/audio/tlkmmi_audioInfo.h"
#include "tlkmmi/audio/tlkmmi_audioComm.h"
#include "tlkmmi/audio/tlkmmi_audioCtrl.h"
#include "tlkmmi/audio/tlkmmi_audioStatus.h"
#include "tlkmmi/audio/tlkmmi_audioModinf.h"

#include "string.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/bth/bth_handle.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkstk/bt/btp/a2dp/btp_a2dp.h"


extern int btp_avrcp_notyVolume(uint08 volume);


static void tlkmmi_audio_eventCB(uint08 majorID, uint08 minorID, uint08 *pData, uint08 dataLen);
static bool tlkmmi_audio_ctrlTimer(tlkapi_timer_t *pTimer, uint32 userArg);
static bool tlkmmi_audio_ctrlProcs(tlkapi_procs_t *pProcs, uint32 userArg);
static int  tlkmmi_audio_sendProgressEvt(uint08 optype, uint16 progress);
static void tlkmmi_audio_sendSongChangeEvt(void);
static void tlkmmi_audio_sendStatusChangeEvt(void);
static void tlkmmi_audio_sendVolumeChangeEvt(void);

static void tlkmmi_audio_startEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_closeEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_playStartEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_playOverEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_volumeChangeEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_statusChangeEvtDeal(uint08 *pData, uint08 dataLen);


extern uint08 gTlkMmiAudioCurOptype;
extern uint16 gTlkMmiAudioCurHandle;
extern tlkapi_timer_t gTlkMmiAudioCurTimer;

tlkmmi_audio_ctrl_t gTlkMmiAudioCtrl;


/******************************************************************************
 * Function: tlkmmi_audio_ctrlInit
 * Descript: Initial the audio control info. 
 * Params: None.
 * Return: Return TLK_ENONE is success, other is false.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_ctrlInit(void)
{
	uint08 volume;
	uint08 enable;
	uint16 interval;

	tmemset(&gTlkMmiAudioCtrl, 0, sizeof(tlkmmi_audio_ctrl_t));

	tlkmdi_event_regCB(TLKMDI_EVENT_MAJOR_AUDIO, tlkmmi_audio_eventCB);
	tlkmmi_adapt_initTimer(&gTlkMmiAudioCtrl.timer, tlkmmi_audio_ctrlTimer, NULL, TLKMMI_AUDIO_TIMEOUT);
	tlkmmi_adapt_initProcs(&gTlkMmiAudioCtrl.procs, tlkmmi_audio_ctrlProcs, NULL);
	
	volume = 0;
	tlkmdi_audio_infoGetVolume(TLKPRT_COMM_VOLUME_TYPE_TONE, &volume);
	tlkmdi_audio_setToneVolume(volume);
	tlkmdi_audio_infoGetVolume(TLKPRT_COMM_VOLUME_TYPE_MUSIC, &volume);
	tlkmdi_audio_setMusicVolume(volume);
//	btp_avrcp_setSpkVolume(volume);
	tlkmdi_audio_infoGetVolume(TLKPRT_COMM_VOLUME_TYPE_VOICE, &volume);
	tlkmdi_audio_setVoiceVolume(volume);
	btp_hfphf_setSpkVolume(volume);
	tlkmdi_audio_infoGetVolume(TLKPRT_COMM_VOLUME_TYPE_HEADSET, &volume);
	tlkmdi_audio_setHeadsetVolume(volume);
	

	if(tlkmdi_audio_infoGetReport(&enable, &interval) == TLK_ENONE){
		gTlkMmiAudioCtrl.report.enable = enable;
		gTlkMmiAudioCtrl.report.interval = (interval*100)/TLKMMI_AUDIO_TIMEOUT_MS;
		gTlkMmiAudioCtrl.report.timeout = gTlkMmiAudioCtrl.report.interval;
	}

	return TLK_ENONE;
}

void tlkmmi_audio_validOptype(uint08 *pOptype, uint16 *pHandle)
{
	uint16 srcHandle;
	uint16 snkHandle;
	
	srcHandle = btp_a2dp_getSrcHandle();
	snkHandle = btp_a2dp_getSnkHandle();
//	snkHandle = 0;
	if(srcHandle == 0 && snkHandle == 0){
		if(pOptype != nullptr) *pOptype = TLKMMI_AUDIO_OPTYPE_PLAY;
		if(pHandle != nullptr) *pHandle = TLK_INVALID_HANDLE;
	}else{
		if(srcHandle != 0 && (snkHandle == 0 || TLKMMI_AUDIO_SRC_PRIORITY >= TLKMMI_AUDIO_SNK_PRIORITY)){
			if(pOptype != nullptr) *pOptype = TLKMMI_AUDIO_OPTYPE_SRC;
			if(pHandle != nullptr) *pHandle = srcHandle;
		}else{
			if(pOptype != nullptr) *pOptype = TLKMMI_AUDIO_OPTYPE_SNK;
			if(pHandle != nullptr) *pHandle = snkHandle;
		}
	}
}
/******************************************************************************
 * Function: tlkmmi_audio_startPlay
 * Descript: Insert the music fileindex into the audio status control list,
 *           and switch to play this music.
 * Params:
 *         @fileIndex[IN]--The music file index to playing.
 * Return: true/false.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_startPlay(void)
{
	if(gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_SNK 
		|| gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_SRC
		|| gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_PLAY
		|| gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_NONE){
		uint08 optype;
		uint16 handle;
		tlkmmi_audio_validOptype(&optype, &handle);
		if(gTlkMmiAudioCurOptype != optype){
			return tlkmmi_audio_modinfStart(optype, handle, 0);
		}
		return TLK_ENONE;
	}
	return -TLK_EFAIL;
}
/******************************************************************************
 * Function: tlkmmi_audio_closePlay
 * Descript: Suspend the current music and adjust the audio handle and it's 
 *           operation type out of the audio status control list.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_closePlay(void)
{
	if(gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_SNK 
		|| gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_SRC
		|| gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_PLAY){
		tlkmmi_audio_modinfClose(gTlkMmiAudioCurOptype, gTlkMmiAudioCurHandle);
	}
}
/******************************************************************************
 * Function: tlkmmi_audio_playNext
 * Descript: play next music.
 * Params: None.
 * Return: true is success/false if failure.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_playNext(void)
{
	if(gTlkMmiAudioCurOptype != TLKMMI_AUDIO_OPTYPE_NONE){
		return tlkmmi_audio_modinfToNext(gTlkMmiAudioCurOptype);
	}else{
		uint08 optype;
		uint16 handle;
		tlkmmi_audio_validOptype(&optype, &handle);
		if(tlkmmi_audio_modinfStart(optype, handle, 0x80000000) != TLK_ENONE) return false;
		return true;
	}
}
/******************************************************************************
 * Function: tlkmmi_audio_playNext
 * Descript: play pre music.
 * Params: None.
 * Return: true is success/false if failure.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_playPrev(void)
{
	if(gTlkMmiAudioCurOptype != TLKMMI_AUDIO_OPTYPE_NONE){
		return tlkmmi_audio_modinfToPrev(gTlkMmiAudioCurOptype);
	}else{
		uint08 optype;
		uint16 handle;
		tlkmmi_audio_validOptype(&optype, &handle);
		if(tlkmmi_audio_modinfStart(optype, handle, 0x40000000) != TLK_ENONE) return false;
		return true;
	}
}

/******************************************************************************
 * Function: tlkmmi_audio_isLocalPlay
 * Descript: Checks whether the current playback is local.
 * Params: None.
 * Return: TRUE-local play, false-other play.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_isLocalPlay(void)
{
	if(gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_PLAY) return true;
	else return false;
}
/******************************************************************************
 * Function: tlkmmi_audio_startLocalPlay
 * Descript: Suspend the current music and adjust the audio handle and it's 
 *           operation type out of the audio status control list.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_startLocalPlay(void)
{
	tlkmmi_audio_modinfStart(TLKMMI_AUDIO_OPTYPE_PLAY, TLK_INVALID_HANDLE, 0xFFFFFFFF);
}
/******************************************************************************
 * Function: tlkmmi_audio_stopLocalPlay
 * Descript: Suspend the current music and adjust the audio handle and it's 
 *           operation type out of the audio status control list.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_stopLocalPlay(void)
{
	tlkmmi_audio_modinfClose(TLKMMI_AUDIO_OPTYPE_PLAY, TLK_INVALID_HANDLE);
}


/******************************************************************************
 * Function: tlkmmi_audio_startTone
 * Descript: start the play tone.
 * Params: None.
 * Return: true is success/false if failure.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_startTone(uint16 fileIndex, uint16 playCount)
{
	uint32 param = (((uint32)playCount)<<16) | fileIndex;
	return tlkmmi_audio_modinfStart(TLKMMI_AUDIO_OPTYPE_TONE, TLK_INVALID_HANDLE, param);
}

/******************************************************************************
 * Function: tlkmmi_audio_stopTone
 * Descript: stop the play tone.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_stopTone(void)
{
	tlkmmi_audio_modinfClose(TLKMMI_AUDIO_OPTYPE_TONE, TLK_INVALID_HANDLE);
}

/******************************************************************************
 * Function: tlkmmi_audio_getCurChannel
 * Descript: Get current audio channel.
 * Params: @channel[OUT]--The audio channel.
 * Return: Return the TLK_ENONE means success, other value is failure.
 * Others: None.
*******************************************************************************/
//int tlkmmi_audio_getCurChn(void)
int tlkmmi_audio_getCurChannel(uint08 *pChannel)
{
	tlkmmi_audio_statusItem_t *pStatus;

	if(pChannel == nullptr) return -TLK_EPARAM;

	pStatus = tlkmmi_audio_getCurStatus();
	if(pStatus == nullptr){
		*pChannel = TLKPRT_COMM_AUDIO_CHN_NONE;
	}else{
		tlkmmi_audio_optypeToChannel(pStatus->optype, pChannel);
	}
	if((*pChannel) == TLKPRT_COMM_AUDIO_CHN_NONE) return -TLK_EFAIL;
	return TLK_ENONE;
}

int tlkmmi_audio_getCurVolType(uint08 *pVolType)
{
	switch(gTlkMmiAudioCurOptype){
		case TLKMMI_AUDIO_OPTYPE_TONE:
			*pVolType = TLKPRT_COMM_VOLUME_TYPE_TONE;
			break;
		case TLKMMI_AUDIO_OPTYPE_HF:
		case TLKMMI_AUDIO_OPTYPE_AG:
		case TLKMMI_AUDIO_OPTYPE_SCO:
			*pVolType = TLKPRT_COMM_VOLUME_TYPE_VOICE;
			break;
		case TLKMMI_AUDIO_OPTYPE_SRC:
			*pVolType = TLKPRT_COMM_VOLUME_TYPE_HEADSET;
			break;
		case TLKMMI_AUDIO_OPTYPE_SNK:
		case TLKMMI_AUDIO_OPTYPE_PLAY:
			*pVolType = TLKPRT_COMM_VOLUME_TYPE_MUSIC;
			break;
		default:
			*pVolType = TLKPRT_COMM_VOLUME_TYPE_AUTO;
			break;
	}
	return TLK_ENONE;
}


/******************************************************************************
 * Function: tlkmmi_audio_getVolume
 * Descript: Get audio volume.
 * Params: 
 *         @voltype[IN]--The audio channel.
 *         @pVolume[OUT]--The audio volume.
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
uint tlkmmi_audio_getVolume(uint08 voltype, uint08 *pVolType)
{
	if(voltype > TLKPRT_COMM_VOLUME_TYPE_TONE) return 0;
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_AUTO){
		tlkmmi_audio_getCurVolType(&voltype);
	}
	if(pVolType != nullptr) *pVolType = voltype;
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_TONE){
		return tlkmdi_audio_getToneVolume(true);
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_VOICE){
		return tlkmdi_audio_getVoiceVolume(true);
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_MUSIC){
		return tlkmdi_audio_getMusicVolume(true);
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_HEADSET){
		return tlkmdi_audio_getHeadsetVolume(true);
	}else{
		return 0;
	}
}
/******************************************************************************
 * Function: tlkmmi_audio_volumeInc
 * Descript: adjust audio channel increment.
 * Params: @channel[IN]--The audio channel.
 * Return: Return true means success, other value is failure.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_volumeInc(uint08 voltype)
{
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_AUTO){
		tlkmmi_audio_getCurVolType(&voltype);
	}
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_TONE){
		tlkmdi_audio_toneVolumeInc(TLKMMI_AUDIO_VOLUME_TONE_STEP);
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_VOICE){
		tlkmdi_audio_voiceVolumeInc(TLKMMI_AUDIO_VOLUME_VOICE_STEP);
		btp_hfphf_setSpkVolume(tlkmdi_audio_getVoiceVolume(true));
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_HEADSET){
		tlkmdi_audio_headsetVolumeInc(TLKMMI_AUDIO_VOLUME_HEADSET_STEP);
	}else{
		tlkmdi_audio_musicVolumeInc(TLKMMI_AUDIO_VOLUME_MUSIC_STEP);
		btp_avrcp_notyVolume(tlkmdi_audio_getMusicVolume(true));
	}
}
/******************************************************************************
 * Function: tlkmmi_audio_volumeDec
 * Descript: adjust audio channel decrement.
 * Params: @channel[IN]--The audio channel.
 * Return: Return true means success, other value is failure.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_volumeDec(uint08 voltype)
{
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_AUTO){
		tlkmmi_audio_getCurVolType(&voltype);
	}
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_TONE){
		tlkmdi_audio_toneVolumeDec(TLKMMI_AUDIO_VOLUME_TONE_STEP);
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_VOICE){
		tlkmdi_audio_voiceVolumeDec(TLKMMI_AUDIO_VOLUME_VOICE_STEP);
		btp_hfphf_setSpkVolume(tlkmdi_audio_getVoiceVolume(true));
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_HEADSET){
		tlkmdi_audio_headsetVolumeDec(TLKMMI_AUDIO_VOLUME_HEADSET_STEP);
	}else{
		tlkmdi_audio_musicVolumeDec(TLKMMI_AUDIO_VOLUME_MUSIC_STEP);
		btp_avrcp_notyVolume(tlkmdi_audio_getMusicVolume(true));
	}
}
/******************************************************************************
 * Function: tlkmmi_audio_setVolume
 * Descript: Set audio volume.
 * Params: 
 *         @channel[IN]--The audio channel.
 *         @volume[IN]--The audio volume.
 * Return: Return true is success/other is failure.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_setVolume(uint08 voltype, uint08 volume)
{
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_AUTO){
		tlkmmi_audio_getCurVolType(&voltype);
	}
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_TONE){
		tlkmdi_audio_setToneVolume(volume);
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_VOICE){
		tlkmdi_audio_setVoiceVolume(volume);
		btp_hfphf_setSpkVolume(tlkmdi_audio_getVoiceVolume(true));
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_HEADSET){
		tlkmdi_audio_setHeadsetVolume(volume);
	}else{
		tlkmdi_audio_setMusicVolume(volume);
		btp_avrcp_notyVolume(tlkmdi_audio_getMusicVolume(true));
	}
}

/******************************************************************************
 * Function: tlkmdi_audio_setReport
 * Descript: Set audio volume.
 * Params: 
 *         @enable[IN]--enable the audio info report.
 *         @interval[IN]--The value How often report.
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_audio_setReport(uint08 enable, uint16 interval)
{
	if(enable == true){
		if(interval < 5 || interval > 1800) return -TLK_EPARAM;
	}
	if(tlkmdi_audio_infoSetReport(enable, interval) != TLK_ENONE) return -TLK_EFAIL;
	
	gTlkMmiAudioCtrl.report.enable = enable;
	if(enable){
		gTlkMmiAudioCtrl.report.interval = (interval*100)/TLKMMI_AUDIO_TIMEOUT_MS;
		gTlkMmiAudioCtrl.report.timeout = gTlkMmiAudioCtrl.report.interval;
	}
	tlkmmi_adapt_insertTimer(&gTlkMmiAudioCtrl.timer);
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_audio_channelToOptype
 * Descript: channel relect to a optype.
 * Params: 
 *         @pChannel[IN]--The channel of audio info.
 *         @optype[IN]--the audio operate.
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_channelToOptype(uint08 channel, uint08 *pOptype)
{
	uint08 optype;
	
	if(pOptype == nullptr) return -TLK_EPARAM;
	if(channel == TLKPRT_COMM_AUDIO_CHN_NONE){
		optype = gTlkMmiAudioCurOptype;
	}else if(channel == TLKPRT_COMM_AUDIO_CHN_PLAY){
		optype = TLKMMI_AUDIO_OPTYPE_PLAY;
	}else if(channel == TLKPRT_COMM_AUDIO_CHN_TONE){
		optype = TLKMMI_AUDIO_OPTYPE_TONE;
	}else if(channel == TLKPRT_COMM_AUDIO_CHN_SCO){
		optype = TLKMMI_AUDIO_OPTYPE_SCO;
	}else if(channel == TLKPRT_COMM_AUDIO_CHN_HFP_HF){
		optype = TLKMMI_AUDIO_OPTYPE_HF;
	}else if(channel == TLKPRT_COMM_AUDIO_CHN_HFP_AG){
		optype = TLKMMI_AUDIO_OPTYPE_AG;
	}else if(channel == TLKPRT_COMM_AUDIO_CHN_A2DP_SRC){
		optype = TLKMMI_AUDIO_OPTYPE_SRC;
	}else if(channel == TLKPRT_COMM_AUDIO_CHN_A2DP_SNK){
		optype = TLKMMI_AUDIO_OPTYPE_SNK;
	}else{
		optype = TLKMMI_AUDIO_OPTYPE_NONE;
	}
	*pOptype = optype;
	if(optype == TLKMMI_AUDIO_OPTYPE_NONE) return -TLK_EFAIL;
	else return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_audio_optypeToChannel
 * Descript: optype relect to a Channel.
 * Params: 
 *         @optype[IN]--the audio operate.
 *         @pChannel[IN]--The channel of audio info.
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_optypeToChannel(uint08 optype, uint08 *pChannel)
{
	uint08 channel;
	if(pChannel == nullptr) return -TLK_EPARAM;
	if(optype == TLKMMI_AUDIO_OPTYPE_PLAY){
		channel = TLKPRT_COMM_AUDIO_CHN_PLAY;
	}else if(optype == TLKMMI_AUDIO_OPTYPE_TONE){
		channel = TLKPRT_COMM_AUDIO_CHN_TONE;
	}else if(optype == TLKMMI_AUDIO_OPTYPE_SNK){
		channel = TLKPRT_COMM_AUDIO_CHN_A2DP_SNK;
	}else if(optype == TLKMMI_AUDIO_OPTYPE_SRC){
		channel = TLKPRT_COMM_AUDIO_CHN_A2DP_SRC;
	}else if(optype == TLKMMI_AUDIO_OPTYPE_HF){
		channel = TLKPRT_COMM_AUDIO_CHN_HFP_HF;
	}else if(optype == TLKMMI_AUDIO_OPTYPE_AG){
		channel = TLKPRT_COMM_AUDIO_CHN_HFP_AG;
	}else if(optype == TLKMMI_AUDIO_OPTYPE_SCO){
		channel = TLKPRT_COMM_AUDIO_CHN_SCO;
	}else{
		channel = TLKPRT_COMM_AUDIO_CHN_NONE;
	}
	*pChannel = channel;
	if(channel == TLKPRT_COMM_AUDIO_CHN_NONE) return -TLK_EFAIL;
	else return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_audio_optypeChanged
 * Descript: Handle audio optype change.
 * Params: 
 *         @Newoptype[IN]--the new audio operate.
 *         @newHandle[IN]--The new handle of audio.
 *         @oldOptype[IN]--The old audio operate.
 *         @oldHandle[IN]--THe old handle of audio.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_optypeChanged(uint08 newOptype, uint16 newHandle, uint08 oldOptype, uint16 oldHandle)
{
	gTlkMmiAudioCurHandle = newHandle;
	gTlkMmiAudioCurOptype = newOptype;
	tlkmmi_adapt_insertTimer(&gTlkMmiAudioCurTimer);
	tlkmmi_adapt_insertTimer(&gTlkMmiAudioCtrl.timer);
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_optypeChanged: gTlkMmiAudioCurOptype -%d", gTlkMmiAudioCurOptype);

	tlkmmi_audio_infoSave();
}


static void tlkmmi_audio_eventCB(uint08 majorID, uint08 minorID, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_eventCB: minorID-%d", minorID);
	if(minorID == TLKMDI_AUDIO_EVTID_START){
		tlkmmi_audio_startEvtDeal(pData, dataLen);
	}else if(minorID == TLKMDI_AUDIO_EVTID_CLOSE){
		tlkmmi_audio_closeEvtDeal(pData, dataLen);
	}else if(minorID == TLKMDI_AUDIO_EVTID_PLAY_OVER){
		tlkmmi_audio_playOverEvtDeal(pData, dataLen);
	}else if(minorID == TLKMDI_AUDIO_EVTID_PLAY_START){
		tlkmmi_audio_playStartEvtDeal(pData, dataLen);
	}else if(minorID == TLKMDI_AUDIO_EVTID_VOLUME_CHANGE){
		tlkmmi_audio_volumeChangeEvtDeal(pData, dataLen);
	}else if(minorID == TLKMDI_AUDIO_EVTID_STATUS_CHANGE){
		tlkmmi_audio_statusChangeEvtDeal(pData, dataLen);
	}
	if(gTlkMmiAudioCtrl.report.busys != TLKMMI_AUDIO_REPORT_BUSY_NONE){
		tlkmmi_adapt_appendProcs(&gTlkMmiAudioCtrl.procs);
	}
}


static void tlkmmi_audio_startEvtDeal(uint08 *pData, uint08 dataLen)
{
	int ret;
	uint16 handle;
	uint08 optype;
	uint08 channel;
	optype = 0;
	channel = pData[0];
	handle = ((uint16)pData[2]<<8) | pData[1];
	if(tlkmmi_audio_channelToOptype(channel, &optype) != TLK_ENONE) return;
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_startEvtDeal: channel-%d, optype-%d", channel, optype);

	ret = tlkmmi_audio_insertStatus(handle, optype);
	if(ret != TLK_ENONE && ret != -TLK_EREPEAT){
		tlkapi_error(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_startEvtDeal: failure - channel-%d, optype-%d", channel, optype);
		tlkmmi_audio_modinfClose(optype, handle);
	}
}
static void tlkmmi_audio_closeEvtDeal(uint08 *pData, uint08 dataLen)
{
	uint16 handle;
	uint08 optype;
	uint08 channel;
	optype = 0;
	channel = pData[0];
	handle = ((uint16)pData[2]<<8) | pData[1];
	if(tlkmmi_audio_channelToOptype(channel, &optype) != TLK_ENONE) return;
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_closeEvtDeal: channel-%d,optype-%d,handle-%d", channel, optype, handle);
	tlkmmi_audio_removeStatus(handle, optype);
}
static void tlkmmi_audio_playStartEvtDeal(uint08 *pData, uint08 dataLen)
{
	uint16 index = ((uint16)pData[2]<<8)|pData[1];
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_playStartEvtDeal: index-%d", index);
	gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_PROGRESS;
	gTlkMmiAudioCtrl.report.newIndex = index;
	gTlkMmiAudioCtrl.report.busys |= TLKMMI_AUDIO_REPORT_BUSY_SONG_CHANGE;
}
static void tlkmmi_audio_playOverEvtDeal(uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_playOverEvtDeal");
	if(gTlkMmiAudioCtrl.report.enable) gTlkMmiAudioCtrl.report.busys |= TLKMMI_AUDIO_REPORT_BUSY_PROGR100;
	gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_PROGR000;
	gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_PROGRESS;
	gTlkMmiAudioCtrl.report.newIndex = 0;
}
static void tlkmmi_audio_volumeChangeEvtDeal(uint08 *pData, uint08 dataLen)
{
//	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_volumeChangeEvtDeal");
	gTlkMmiAudioCtrl.report.volType = pData[0];
	gTlkMmiAudioCtrl.report.volValue = pData[1];
	tlkmmi_audio_sendVolumeChangeEvt();
	tlkmdi_audio_infoSetVolume(pData[0], pData[1]);
	tlkmmi_adapt_insertTimer(&gTlkMmiAudioCtrl.timer);
}
static void tlkmmi_audio_statusChangeEvtDeal(uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_statusChangeEvtDeal");
	if(gTlkMmiAudioCtrl.report.statusChn0 == 0){
		gTlkMmiAudioCtrl.report.statusChn0 = pData[0];
		gTlkMmiAudioCtrl.report.statusVal0 = pData[1];
	}else if(gTlkMmiAudioCtrl.report.statusChn1 == 0){
		gTlkMmiAudioCtrl.report.statusChn1 = pData[0];
		gTlkMmiAudioCtrl.report.statusVal1 = pData[1];
	}
	tlkmmi_audio_sendStatusChangeEvt();
}
static bool tlkmmi_audio_ctrlTimer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	bool isBusy = false;
	if(gTlkMmiAudioCtrl.report.enable && (gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_PLAY 
		|| gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_SRC)){
		if(gTlkMmiAudioCtrl.report.timeout != 0) gTlkMmiAudioCtrl.report.timeout --;
		if(gTlkMmiAudioCtrl.report.timeout == 0){
			gTlkMmiAudioCtrl.report.timeout = gTlkMmiAudioCtrl.report.interval;
			gTlkMmiAudioCtrl.report.busys |= TLKMMI_AUDIO_REPORT_BUSY_PROGRESS;
			tlkmmi_adapt_appendProcs(&gTlkMmiAudioCtrl.procs);
		}
		isBusy = true;
	}
	
	if(tlkmmi_audio_infoIsChange()){
		if(tlkmmi_audio_infoIsUpdate()){
			tlkmmi_audio_infoSave();
		}
		isBusy = true;
	}
	return isBusy;
}
static bool tlkmmi_audio_ctrlProcs(tlkapi_procs_t *pProcs, uint32 userArg)
{
	if((gTlkMmiAudioCtrl.report.busys & TLKMMI_AUDIO_REPORT_BUSY_STATUS_CHANGE) != 0){
		tlkmmi_audio_sendStatusChangeEvt();
	}
	if((gTlkMmiAudioCtrl.report.busys & TLKMMI_AUDIO_REPORT_BUSY_VOLUME_CHANGE) != 0){
		tlkmmi_audio_sendVolumeChangeEvt();
	}
	
	if((gTlkMmiAudioCtrl.report.busys & TLKMMI_AUDIO_REPORT_BUSY_PROGR100) != 0){
		if(tlkmmi_audio_sendProgressEvt(gTlkMmiAudioCurOptype, 1000) == TLK_ENONE){
			gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_PROGR100;
		}
	}else if((gTlkMmiAudioCtrl.report.busys & TLKMMI_AUDIO_REPORT_BUSY_SONG_CHANGE) != 0){
		tlkmmi_audio_sendSongChangeEvt();
	}else if((gTlkMmiAudioCtrl.report.busys & TLKMMI_AUDIO_REPORT_BUSY_PROGR000) != 0){
		if(tlkmmi_audio_sendProgressEvt(gTlkMmiAudioCurOptype, 0) == TLK_ENONE){
			gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_PROGR000;
		}
	}else if((gTlkMmiAudioCtrl.report.busys & TLKMMI_AUDIO_REPORT_BUSY_PROGRESS) != 0){
		#if (TLK_MDI_MP3_ENABLE)
		if(tlkmmi_audio_sendProgressEvt(gTlkMmiAudioCurOptype, tlkmdi_mp3_getProgress()) == TLK_ENONE){
			gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_PROGRESS;
		}
		#else
		gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_PROGRESS;
		#endif
	}
	
	if(gTlkMmiAudioCtrl.report.busys != TLKMMI_AUDIO_REPORT_BUSY_NONE) return true;
	return false;
}


static int tlkmmi_audio_sendProgressEvt(uint08 optype, uint16 progress)
{
	uint08 channel;
	uint08 buffLen;
	uint08 buffer[4];
	
	tlkmmi_audio_optypeToChannel(optype, &channel);
	if(channel == TLKPRT_COMM_AUDIO_CHN_NONE) channel = 0;

	buffLen = 0;
	buffer[buffLen++] = channel;
	buffer[buffLen++] = (progress & 0xFF);
	buffer[buffLen++] = (progress & 0xFF00) >> 8;
	return tlkmdi_comm_sendAudioEvt(TLKPRT_COMM_EVTID_AUDIO_PROGRESS_REPORT, buffer, buffLen);
}
static void tlkmmi_audio_sendSongChangeEvt(void)
{
	uint08 length = 0;
	uint08 codec = 0;
	uint08 *pName = nullptr;
	uint08 buffLen;
	uint08 buffer[86];

//	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_sendSongChangeEvt 01");
	buffLen = 0;
	if(gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_PLAY){
		buffer[buffLen++] = TLKPRT_COMM_AUDIO_CHN_PLAY;
	}else if(gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_SRC){
		buffer[buffLen++] = TLKPRT_COMM_AUDIO_CHN_A2DP_SRC;
	}else{
		gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_SONG_CHANGE;
		return;
	}
//	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_sendSongChangeEvt 02");
	buffer[buffLen++] = gTlkMmiAudioCtrl.report.newIndex & 0xFF;
	buffer[buffLen++] = (gTlkMmiAudioCtrl.report.newIndex & 0xFF00) >> 8;
	#if (TLK_MDI_MP3_ENABLE)
	if(!tlkmdi_mp3_isEnable()){
		length = 0;
	}else{
		pName = tlkmdi_mp3_getFileName(&length);
		codec = tlkmdi_mp3_getFNameCode();
	}
	#else
	length = 0;
	#endif
	if(length > 80) length = 80;
	buffer[buffLen++] = length;
	buffer[buffLen++] = codec;
	if(length != 0){
		tmemcpy(buffer+buffLen, pName, length);
		buffLen += length;
	}
//	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_sendSongChangeEvt 03");
	if(tlkmdi_comm_sendAudioEvt(TLKPRT_COMM_EVTID_AUDIO_SONG_CHANGE, buffer, buffLen) == TLK_ENONE){
		gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_SONG_CHANGE;
		if(gTlkMmiAudioCtrl.report.enable) gTlkMmiAudioCtrl.report.busys |= TLKMMI_AUDIO_REPORT_BUSY_PROGR000;
	}else{
		gTlkMmiAudioCtrl.report.busys |= TLKMMI_AUDIO_REPORT_BUSY_SONG_CHANGE;
	}
	tlkmmi_adapt_appendProcs(&gTlkMmiAudioCtrl.procs);
}
static void tlkmmi_audio_sendStatusChangeEvt(void)
{
	uint08 buffer[2];
	if(gTlkMmiAudioCtrl.report.statusChn0 != 0){
		buffer[0] = gTlkMmiAudioCtrl.report.statusChn0;
		buffer[1] = gTlkMmiAudioCtrl.report.statusVal0;
		if(tlkmdi_comm_sendAudioEvt(TLKPRT_COMM_EVTID_AUDIO_STATUS_CHANGE, buffer, 2) == TLK_ENONE){
			gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_STATUS_CHANGE;
			gTlkMmiAudioCtrl.report.statusChn0 = gTlkMmiAudioCtrl.report.statusChn1;
			gTlkMmiAudioCtrl.report.statusVal0 = gTlkMmiAudioCtrl.report.statusVal1;
			gTlkMmiAudioCtrl.report.statusChn1 = 0;
			gTlkMmiAudioCtrl.report.statusVal1 = 0;
		}
	}
	if(gTlkMmiAudioCtrl.report.statusChn0 != 0){
		buffer[0] = gTlkMmiAudioCtrl.report.statusChn0;
		buffer[1] = gTlkMmiAudioCtrl.report.statusVal0;
		if(tlkmdi_comm_sendAudioEvt(TLKPRT_COMM_EVTID_AUDIO_STATUS_CHANGE, buffer, 2) == TLK_ENONE){
			gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_STATUS_CHANGE;
			gTlkMmiAudioCtrl.report.statusChn0 = 0;
			gTlkMmiAudioCtrl.report.statusVal0 = 0;
		}
	}
	if(gTlkMmiAudioCtrl.report.statusChn0 != 0){
		gTlkMmiAudioCtrl.report.busys |= TLKMMI_AUDIO_REPORT_BUSY_STATUS_CHANGE;
		tlkmmi_adapt_appendProcs(&gTlkMmiAudioCtrl.procs);
	}
}
static void tlkmmi_audio_sendVolumeChangeEvt(void)
{
	uint08 buffer[2];
	buffer[0] = gTlkMmiAudioCtrl.report.volType;
	buffer[1] = gTlkMmiAudioCtrl.report.volValue;
	if(tlkmdi_comm_sendAudioEvt(TLKPRT_COMM_EVTID_AUDIO_VOLUME_CHANGE, buffer, 2) == TLK_ENONE){
		gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_VOLUME_CHANGE;
	}else{
		gTlkMmiAudioCtrl.report.busys|= TLKMMI_AUDIO_REPORT_BUSY_VOLUME_CHANGE;
		tlkmmi_adapt_appendProcs(&gTlkMmiAudioCtrl.procs);
	}
}


#endif //#if (TLKMMI_AUDIO_ENABLE)

