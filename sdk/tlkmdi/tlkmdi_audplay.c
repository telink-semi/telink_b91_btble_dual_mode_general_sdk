/********************************************************************************************************
 * @file     tlkmdi_play.c
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
#if (TLKMDI_CFG_AUDPLAY_ENABLE)
#include "tlkmdi/tlkmdi_audio.h"
#include "tlkprt/tlkprt_stdio.h"
#include "tlklib/fs/filesystem.h"
#include "tlkapi/tlkapi_file.h"
#include "tlkmdi/tlkmdi_audmp3.h"
#include "tlkmdi/tlkmdi_audplay.h"
#include "tlkapi/tlkapi_file.h"



#include "tlkalg/audio/mixer/tlkalg_2chmix.h"


#define TLKMDI_AUDPLAY_DBG_FLAG         (TLKMDI_PLAY_DBG_ENABLE | TLKMDI_DBG_FLAG) 
#define TLKMDI_AUDPLAY_DBG_SIGN         TLKMDI_DBG_SIGN


#define TLKMDI_PLAY_VOLUME_STEP           3




static void tlkmdi_play_mp3Handler(void);
static void tlkmdi_play_fillHandler(void);


typedef struct{
	uint08 runing;
	uint08 status;
	uint08 enable;
	uint08 curState;
	uint16 playIndex;
	uint16 sampleRate;
}tlkmdi_play_ctrl_t;



static tlkmdi_play_ctrl_t sTlkMdiPlayCtrl;


/******************************************************************************
 * Function: tlkmdi_audplay_init
 * Descript: Initialize playback parameters and logic. 
 * Params: None.
 * Return: 'TLK_ENONE' means success, otherwise failure.
*******************************************************************************/
int tlkmdi_audplay_init(void)
{
	tmemset(&sTlkMdiPlayCtrl, 0, sizeof(tlkmdi_play_ctrl_t));
	sTlkMdiPlayCtrl.playIndex = tlkmdi_mp3_getPlayIndex();
	return TLK_ENONE;
}

int tlkmdi_audplay_start(uint16 handle, uint32 param)
{
	int ret;
	if(sTlkMdiPlayCtrl.enable) return TLK_ENONE;
	ret = tlkmdi_audio_sendStartEvt(TLKPRT_COMM_AUDIO_CHN_PLAY, handle);
	if(ret != TLK_ENONE) return ret;
	if((param & 0x80000000) != 0) sTlkMdiPlayCtrl.playIndex = tlkmdi_mp3_getNextIndex();
	else if((param & 0x40000000) != 0) sTlkMdiPlayCtrl.playIndex = tlkmdi_mp3_getPrevIndex();
	else sTlkMdiPlayCtrl.playIndex = tlkmdi_mp3_getPlayIndex();
	return TLK_ENONE;
}
int tlkmdi_audplay_close(uint16 handle)
{
	if(!sTlkMdiPlayCtrl.enable) return TLK_ENONE;
//	tlkmdi_audplay_switch(handle, TLK_STATE_CLOSED);
	tlkmdi_audio_sendCloseEvt(TLKPRT_COMM_AUDIO_CHN_PLAY, handle);
	return TLK_ENONE;
}


/******************************************************************************
 * Function: tlkmdi_audplay_toNext
 * Descript: Play the next music. 
 * Params: None.
 * Return: Return true is success, otherwise is failure.
*******************************************************************************/
bool tlkmdi_audplay_toNext(void)
{
	uint16 index = tlkmdi_mp3_getPlayIndex();
	if(!sTlkMdiPlayCtrl.enable){
		if(tlkmdi_audplay_start(TLK_INVALID_HANDLE, 0x80000000) == TLK_ENONE){
			return true;
		}else{
			return false;
		}
	}
	if(sTlkMdiPlayCtrl.curState != TLKMDI_MP3_STATUS_IDLE){
		tlkmdi_audio_sendPlayOverEvt(TLKPRT_COMM_AUDIO_CHN_PLAY, index);
	}
	sTlkMdiPlayCtrl.curState = TLKMDI_MP3_STATUS_IDLE;
	sTlkMdiPlayCtrl.playIndex = tlkmdi_mp3_getNextIndex();
	tlkapi_trace(TLKMDI_AUDPLAY_DBG_FLAG, TLKMDI_AUDPLAY_DBG_SIGN, "tlkmdi_audplay_toNext: %d %d", index, sTlkMdiPlayCtrl.playIndex);
	return true;
}

/******************************************************************************
 * Function: tlkmdi_audplay_toPrev
 * Descript: Play the prev' music. 
 * Params: None.
 * Return: Return true is success, otherwise is failure.
*******************************************************************************/
bool tlkmdi_audplay_toPrev(void)
{
	uint16 index = tlkmdi_mp3_getPlayIndex();
	if(!sTlkMdiPlayCtrl.enable){
		if(tlkmdi_audplay_start(TLK_INVALID_HANDLE, 0x40000000) == TLK_ENONE){
			return true;
		}else{
			return false;
		}
	}
	if(sTlkMdiPlayCtrl.curState != TLKMDI_MP3_STATUS_IDLE){
		tlkmdi_audio_sendPlayOverEvt(TLKPRT_COMM_AUDIO_CHN_PLAY, index);
	}
	sTlkMdiPlayCtrl.curState = TLKMDI_MP3_STATUS_IDLE;
	sTlkMdiPlayCtrl.playIndex = tlkmdi_mp3_getPrevIndex();
	tlkapi_trace(TLKMDI_AUDPLAY_DBG_FLAG, TLKMDI_AUDPLAY_DBG_SIGN, "tlkmdi_audplay_toPrev: %d %d", index, sTlkMdiPlayCtrl.playIndex);
	return true;
}

/******************************************************************************
 * Function: tlkmdi_audplay_switch
 * Descript: Change the audio status. 
 * Params: None.
 * Return: Return true is success, otherwise is failure.
*******************************************************************************/
bool tlkmdi_audplay_switch(uint16 handle, uint08 status)
{
	bool enable;

	tlkapi_trace(TLKMDI_AUDPLAY_DBG_FLAG, TLKMDI_AUDPLAY_DBG_SIGN, "tlkmdi_audplay_switch: %d", status);

	if(sTlkMdiPlayCtrl.status != status){
		sTlkMdiPlayCtrl.status = status;
		tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_PLAY, status);
	}
	
	if(status == TLK_STATE_OPENED) enable = true;
	else enable = false;
	if(!enable) tlkmdi_mp3_updateEnable(false);
	if(sTlkMdiPlayCtrl.enable == enable) return true;
	
	if(!enable) sTlkMdiPlayCtrl.enable = false;
	if(!tlkmdi_mp3_enable(enable) && enable){
		tlkapi_trace(TLKMDI_AUDPLAY_DBG_FLAG, TLKMDI_AUDPLAY_DBG_SIGN, "tlkmdi_audplay_switch: enable failure - %d", enable);
		sTlkMdiPlayCtrl.status = TLK_STATE_CLOSED;
		tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_PLAY, sTlkMdiPlayCtrl.status);
		return false;
	}
	if(enable){
		tlkdev_codec_init(TLKDEV_CODEC_MODE_SINGLE, TLKDEV_CODEC_SELC_INNER);
	}
	
	tlkdev_spk_mute();
	tlkmdi_mp3_startUpdate();
	
	sTlkMdiPlayCtrl.enable = enable;
	sTlkMdiPlayCtrl.runing = enable;
	sTlkMdiPlayCtrl.curState = TLKMDI_MP3_STATUS_IDLE;
	sTlkMdiPlayCtrl.playIndex = tlkmdi_mp3_getPlayIndex();
	sTlkMdiPlayCtrl.sampleRate = tlkmdi_mp3_getSampleRate();
	if(sTlkMdiPlayCtrl.enable) tlkdev_codec_setSampleRate(sTlkMdiPlayCtrl.sampleRate);

	tlkapi_trace(TLKMDI_AUDPLAY_DBG_FLAG, TLKMDI_AUDPLAY_DBG_SIGN, "tlkmdi_audplay_switch: success - %d", enable);
	
	return true;
}

/******************************************************************************
 * Function: tlkmdi_audplay_isBusy
 * Descript: Verify the audio control block is enable. 
 * Params: None.
 * Return: 'true' means success, otherwise failure.
*******************************************************************************/
bool tlkmdi_audplay_isBusy(void)
{
	return (sTlkMdiPlayCtrl.enable && sTlkMdiPlayCtrl.runing);
}

/******************************************************************************
 * Function: tlkmdi_audplay_isBusy
 * Descript: Get the audio play interval, this value will determine 
 *           what the time to restart the timer . 
 * Params: None.
 * Return: interval value.
*******************************************************************************/
uint tlkmdi_audplay_intval(void)
{
	if(!sTlkMdiPlayCtrl.runing) return 0;
	if(tlkdev_spk_dataLen() < (TLK_DEV_SPK_BUFF_SIZE>>1)){
		return 1000;
	}else if(tlkmdi_mp3_getPcmDataLen() < 1024){
		return 800;
	}else{
		return 2000;
	}
}


/******************************************************************************
 * Function: tlkmdi_audplay_irqProc
 * Descript: Trigger audio handler. 
 * Params: None.
 * Return: Return true is success, otherwise is failure.
*******************************************************************************/
bool tlkmdi_audplay_irqProc(void)
{
//	my_dump_str_data(TLKMDI_PLAY_DBG_ENABLE, "tlkmdi_audplay_irqProc:", 0, 0);
	if(!sTlkMdiPlayCtrl.enable) return false;
	tlkmdi_play_mp3Handler();
	tlkmdi_play_fillHandler();
	if(sTlkMdiPlayCtrl.runing) return true;
	else return false;
}


bool tlkmdi_play_start(uint16 index)
{
	bool isSucc;

	tlkdev_spk_mute();
	tlkmdi_mp3_updateEnable(true);
	isSucc = tlkmdi_mp3_play(index);
	if(!isSucc){
		tlkmdi_mp3_updateEnable(false);
		sTlkMdiPlayCtrl.runing = false;
		sTlkMdiPlayCtrl.curState = TLKMDI_MP3_STATUS_PLAY;
	}else{
		sTlkMdiPlayCtrl.runing = true;
		sTlkMdiPlayCtrl.curState = TLKMDI_MP3_STATUS_IDLE;
	}
	return isSucc;
}


/******************************************************************************
 * Function: tlkmdi_play_setParam
 * Descript: Set the audio Paramter. 
 * Params: None.
 * Return: None.
*******************************************************************************/
void tlkmdi_play_setParam(uint16 fileIndex)
{
	sTlkMdiPlayCtrl.curState = TLKMDI_MP3_STATUS_IDLE;
	sTlkMdiPlayCtrl.playIndex = fileIndex;
}


static void tlkmdi_play_mp3Handler(void)
{
	if(!sTlkMdiPlayCtrl.runing) return;
	if(sTlkMdiPlayCtrl.curState == TLKMDI_MP3_STATUS_IDLE){
		tlkapi_trace(TLKMDI_AUDPLAY_DBG_FLAG, TLKMDI_AUDPLAY_DBG_SIGN, "tlkmdi_play_mp3Handler: new play");
		tlkmdi_play_start(sTlkMdiPlayCtrl.playIndex);
		if(!sTlkMdiPlayCtrl.runing) return;
		if(tlkmdi_mp3_getSampleRate() != sTlkMdiPlayCtrl.sampleRate){
			sTlkMdiPlayCtrl.sampleRate = tlkmdi_mp3_getSampleRate();
			tlkdev_codec_setSampleRate(sTlkMdiPlayCtrl.sampleRate);
		}
		sTlkMdiPlayCtrl.curState = TLKMDI_MP3_STATUS_PLAY;
		tlkmdi_audio_sendPlayStartEvt(TLKPRT_COMM_AUDIO_CHN_PLAY, sTlkMdiPlayCtrl.playIndex);
	}
	if(sTlkMdiPlayCtrl.curState == TLKMDI_MP3_STATUS_WAIT || sTlkMdiPlayCtrl.curState == TLKMDI_MP3_STATUS_DONE){
		uint16 dropLen = (tlkmdi_mp3_getChannels() == 2) ? 960 : 480;//10ms
		if(tlkdev_spk_dataLen() >= dropLen){
			tlkdev_spk_fillZero(0, false);
		}else{
			uint16 index = tlkmdi_mp3_getPlayIndex();
			tlkdev_spk_mute();
			if(sTlkMdiPlayCtrl.curState == TLKMDI_MP3_STATUS_DONE){
				sTlkMdiPlayCtrl.runing = false;
			}else{
				sTlkMdiPlayCtrl.curState = TLKMDI_MP3_STATUS_IDLE;
			}
			tlkmdi_audio_sendPlayOverEvt(TLKPRT_COMM_AUDIO_CHN_PLAY, index);
		}
	}else if(sTlkMdiPlayCtrl.curState == TLKMDI_MP3_STATUS_PLAY){
		int ret;
		ret = tlkmdi_mp3_decode();
		if(ret != TLK_ENONE){
			tlkdev_spk_fillZero(0, 0);
		}
		if(tlkmdi_mp3_isOver()){
			tlkmdi_mp3_reset();
//			my_dump_str_data(TLKMDI_PLAY_DBG_ENABLE, "tlkmdi_play_mp3Handler: over", 0, 0);
			if(tlkmdi_mp3_indexIsOver()){
				tlkmdi_mp3_setPlayIndex(0);
				sTlkMdiPlayCtrl.curState = TLKMDI_MP3_STATUS_DONE;
			}else{
				sTlkMdiPlayCtrl.curState = TLKMDI_MP3_STATUS_WAIT;
				sTlkMdiPlayCtrl.playIndex = tlkmdi_mp3_getNextPlay();
			}
		}
	}
}
static void tlkmdi_play_fillHandler(void)
{
	uint16 index;
	uint08 volume;
	uint32 needLen;
	uint16 pcm[256];
	sint16 *ptr = NULL;

	if(!sTlkMdiPlayCtrl.runing) return;
//	tlkapi_trace(TLKMDI_AUDPLAY_DBG_FLAG, TLKMDI_AUDPLAY_DBG_SIGN, "tlkmdi_play_fillHandler:");
	
	needLen = tlkmdi_mp3_getChannels() == 2 ? 128*4 : 128*2;
	if(tlkmdi_mp3_getPcmDataLen() < needLen || tlkdev_spk_idleLen() <= needLen){
		return;
	}
	
	while(true){
		ptr = (sint16*)pcm;
		tlkmdi_mp3_getPcmData((uint08*)pcm, needLen);
		volume = tlkmdi_audio_getMusicVolume(false);
		for(index=0; index<256; index++){
			ptr[index] = (ptr[index]*volume) >> TLKMDI_AUDIO_VOLUME_EXPAND;
		}
		if(needLen == 512){
			short stereo_pcm[128*2];
			for(index=0; index<128; index++){
				stereo_pcm[index] = pcm[index*2];
				stereo_pcm[index+128] = pcm[index*2+1];
			}
			tlkalg_2chnmix(&stereo_pcm[0], &stereo_pcm[128], (short*)pcm, 1, 128);
		}
		#if TLK_ALG_EQ_ENABLE
		ptr = (signed short *)pcm;
		eq_para.eq_type = EQ_TYPE_MUSIC;
		eq_para.eq_channel = EQ_CHANNEL_LEFT;
		eq_proc_tws_music(eq_para, ptr, ptr, 128, 0);
		#endif
		tlkdev_spk_play((uint08*)pcm, 256);
		if(tlkmdi_mp3_getPcmDataLen() < needLen || tlkdev_spk_idleLen() <= needLen){
			return; //break;
		}
	}
}


#endif //#if (TLKMDI_CFG_AUDPLAY_ENABLE)
