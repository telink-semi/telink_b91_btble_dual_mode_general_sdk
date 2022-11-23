/********************************************************************************************************
 * @file     tlkmdi_tone.c
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
#if (TLKMDI_CFG_AUDTONE_ENABLE)
#include "tlkmdi/tlkmdi_audio.h"
#include "tlklib/fs/tlkfs.h"
#include "tlkapi/tlkapi_file.h"
#include "tlkmdi/tlkmdi_audmp3.h"
#include "tlkmdi/tlkmdi_audplay.h"
#include "tlkapi/tlkapi_file.h"

#include "tlkalg/audio/mixer/tlkalg_2chmix.h"

#include "tlkstk/inner/tlkstk_myudb.h"
#include "tlkprt/tlkprt_stdio.h"


#define TLKMDI_AUDTONE_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_AUDIO << 24) | (TLK_MINOR_DBGID_MDI_AUD_TONE << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_AUDTONE_DBG_SIGN       "[MDI]"



static int tlkmdi_tone_findFilePath(uint08 *pBuff, uint16 buffLen, bool addRoot);
static int tlkmdi_tone_findFileName(uint08 *pBuff, uint16 index);

static void tlkmdi_tone_mp3Handler(void);
static void tlkmdi_tone_fillHandler(void);


typedef struct{
	uint08 runing;
	uint08 enable;
	uint08 curState;
	uint08 playNumb;
	uint08 playCount;
	uint16 playIndex;
	uint32 sampleRate;
}tlkmdi_tone_ctrl_t;
static tlkmdi_tone_ctrl_t sTlkMdiToneCtrl;


/******************************************************************************
 * Function: tlkmdi_audplay_init
 * Descript: Initialize playback parameters and logic. 
 * Params: None.
 * Return: 'TLK_ENONE' means success, otherwise failure.
*******************************************************************************/
int tlkmdi_audtone_init(void)
{	
	tmemset(&sTlkMdiToneCtrl, 0, sizeof(tlkmdi_tone_ctrl_t));
	sTlkMdiToneCtrl.playIndex = 0;
	sTlkMdiToneCtrl.playCount = 1;
	return TLK_ENONE;
}

int tlkmdi_audtone_start(uint16 handle, uint32 param)
{
	uint16 playIndex = (param & 0x0000FFFF);
	uint16 playCount = (param & 0xFFFF0000) >> 16;
	sTlkMdiToneCtrl.playIndex = playIndex;
	sTlkMdiToneCtrl.playCount = playCount;
	sTlkMdiToneCtrl.playNumb = 0;
	if(!sTlkMdiToneCtrl.enable) sTlkMdiToneCtrl.curState = TLKMDI_MP3_STATUS_IDLE;
	tlkmdi_audio_sendStartEvt(TLKPRT_COMM_AUDIO_CHN_TONE, handle);
	return TLK_ENONE;
}
int tlkmdi_audtone_close(uint16 handle)
{
	tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_audtone_close");
	sTlkMdiToneCtrl.runing = false;
	tlkmdi_audtone_switch(handle, TLK_STATE_CLOSED);
	tlkmdi_audio_sendCloseEvt(TLKPRT_COMM_AUDIO_CHN_TONE, handle);
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_audtone_switch
 * Descript: Change Tone status. 
 * Params: @handle[IN]--The handle.
 *         @status[IN]--The status.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audtone_switch(uint16 handle, uint08 status)
{
	bool enable;

	if(status == TLK_STATE_OPENED) enable = true;
	else enable = false;
	tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_audtone_switch 01: %d %d %d", status, enable, sTlkMdiToneCtrl.enable);
	if(sTlkMdiToneCtrl.enable == enable) return true;

	tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_audtone_switch 02: %d", enable);
	if(!tlkmdi_mp3_enable(enable) && enable){
		return false;
	}
	if(enable){
		sTlkMdiToneCtrl.sampleRate = tlkmdi_mp3_getSampleRate();
		tlkdev_codec_open(TLKDEV_CODEC_SUBDEV_SPK, TLKDEV_CODEC_CHANNEL_LEFT, TLKDEV_CODEC_BITDEPTH_16, sTlkMdiToneCtrl.sampleRate);
		tlkdev_codec_setSpkOffset(640);
	}else{
		tlkdev_codec_close();
	}
	tlkdev_codec_muteSpk();

	tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_audtone_switch 03");

	if(enable) tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_TONE, TLK_STATE_OPENED);
	else tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_TONE, TLK_STATE_CLOSED);
	
	sTlkMdiToneCtrl.enable = enable;
	sTlkMdiToneCtrl.runing = enable;
	sTlkMdiToneCtrl.playNumb = 0;
	sTlkMdiToneCtrl.curState = TLKMDI_MP3_STATUS_IDLE;
	return true;
}


/******************************************************************************
 * Function: tlkmdi_audtone_isBusy
 * Descript: Is the Tone Ctrl is enable. 
 * Params: None.
 * Return: 'TLK_ENONE' means success, otherwise failure.
*******************************************************************************/
bool tlkmdi_audtone_isBusy(void)
{
	return (sTlkMdiToneCtrl.enable && sTlkMdiToneCtrl.runing);
}

/******************************************************************************
 * Function: tlkmdi_audtone_intval
 * Descript: Get Tone Interval which will determine the time 
 *           when to start the next timer. 
 * Params: None.
 * Return: The interval value.
*******************************************************************************/
uint tlkmdi_audtone_intval(void)
{
	if(!sTlkMdiToneCtrl.runing) return 0;
	if(tlkdev_codec_getSpkDataLen() < (TLK_DEV_SPK_BUFF_SIZE>>1)){
		return 1000;
	}else if(tlkmdi_mp3_getPcmDataLen() < 1024){
		return 800;
	}else{
		return 3000;
	}
}

/******************************************************************************
 * Function: tlkmdi_audtone_irqProc
 * Descript: Call Tone mp3 handler and fill handler. 
 * Params: None.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audtone_irqProc(void)
{
	tlkmdi_tone_mp3Handler();
	tlkmdi_tone_fillHandler();
	if(sTlkMdiToneCtrl.runing) return true;
	else return false;
}

/******************************************************************************
 * Function: tlkmdi_tone_setParam
 * Descript: Set the playing paramter. 
 * Params: 
 *        @playIndex[IN]--Theplay index.
 *        @playcount[IN]--The play count.
 * Return: None.
*******************************************************************************/
void tlkmdi_tone_setParam(uint16 playIndex, uint08 playCount)
{
	sTlkMdiToneCtrl.playIndex = playIndex;
	sTlkMdiToneCtrl.playCount = playCount;
}


/******************************************************************************
 * Function: tlkmdi_tone_start
 * Descript: Start tone to playing. 
 * Params:
 *         @index[IN]--The music index.
 * Return: Return TLK_ENONE is success other value is false.
*******************************************************************************/
bool tlkmdi_tone_start(uint16 index)
{
	uint08 pathLen;
	uint08 fpath[TLKMDI_MP3_PATH_SIZE+2] = {0};
	
	tlkmdi_tone_findFilePath(fpath, TLKMDI_MP3_DIR_SIZE, true);
	pathLen = tlkapi_file_strlen((FCHAR*)fpath);
	if(pathLen == 0) return false;
	
//	tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_tone_start 02");
	if(tlkmdi_tone_findFileName(fpath+pathLen, index) != TLK_ENONE){
		return false;
	}
	
//	tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_tone_start 03");
	if(!tlkmdi_mp3_start(fpath, 0)){
		return false;
	}
	
	sTlkMdiToneCtrl.playIndex = index;
	
	return true;
}

/******************************************************************************
 * Function: tlkmdi_tone_close
 * Descript: Stop tone to playing. 
 * Params: None.
 * Return: None.
*******************************************************************************/
void tlkmdi_tone_close(void)
{
	tlkdev_codec_muteSpk();
	tlkmdi_mp3_close();
	sTlkMdiToneCtrl.runing = false;
	sTlkMdiToneCtrl.curState = TLKMDI_MP3_STATUS_IDLE;
}



static void tlkmdi_tone_mp3Handler(void)
{
	if(!sTlkMdiToneCtrl.runing) return;
	if(sTlkMdiToneCtrl.curState == TLKMDI_MP3_STATUS_IDLE){
		tlkmdi_tone_start(sTlkMdiToneCtrl.playIndex);
		if(!sTlkMdiToneCtrl.runing) return;
		if(tlkmdi_mp3_getSampleRate() != sTlkMdiToneCtrl.sampleRate){
			sTlkMdiToneCtrl.sampleRate = tlkmdi_mp3_getSampleRate();
			tlkdev_codec_close();
			tlkdev_codec_open(TLKDEV_CODEC_SUBDEV_SPK, TLKDEV_CODEC_CHANNEL_LEFT, TLKDEV_CODEC_BITDEPTH_16, sTlkMdiToneCtrl.sampleRate);
			tlkdev_codec_setSpkOffset(640);
		}
		sTlkMdiToneCtrl.playNumb ++;
		sTlkMdiToneCtrl.curState = TLKMDI_MP3_STATUS_PLAY;
	}
	if(sTlkMdiToneCtrl.curState == TLKMDI_MP3_STATUS_WAIT || sTlkMdiToneCtrl.curState == TLKMDI_MP3_STATUS_DONE){
		uint16 dropLen = (tlkmdi_mp3_getChannels() == 2) ? 960 : 480;//10ms
		if(tlkdev_codec_getSpkDataLen() >= dropLen){
			tlkdev_codec_zeroSpkBuff(0, false);
		}else{
			tlkdev_codec_muteSpk();
			if(sTlkMdiToneCtrl.curState == TLKMDI_MP3_STATUS_DONE){
				sTlkMdiToneCtrl.runing = false;
				tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_tone_play is over");
			}else{
				sTlkMdiToneCtrl.curState = TLKMDI_MP3_STATUS_IDLE;
			}
		}
	}else if(sTlkMdiToneCtrl.curState == TLKMDI_MP3_STATUS_PLAY){
		int ret;
		ret = tlkmdi_mp3_decode();
		if(ret != TLK_ENONE){
			tlkdev_codec_zeroSpkBuff(0, 0);
		}
		if(tlkmdi_mp3_isOver()){
			tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_tone_play: mp3 over - %d %d",
				sTlkMdiToneCtrl.playCount, sTlkMdiToneCtrl.playNumb);
			if(sTlkMdiToneCtrl.playCount != 0 && sTlkMdiToneCtrl.playNumb >= sTlkMdiToneCtrl.playCount){
				sTlkMdiToneCtrl.curState = TLKMDI_MP3_STATUS_DONE;
			}else{
				sTlkMdiToneCtrl.curState = TLKMDI_MP3_STATUS_WAIT;
			}
		}
	}
}
static void tlkmdi_tone_fillHandler(void)
{
	uint08 volume;
	uint16 index;
	uint32 needLen;
	uint16 pcm[256];
	sint16 *ptr = NULL;

	if(!sTlkMdiToneCtrl.runing) return;
//	tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_tone_fillHandler:");
	
	needLen = tlkmdi_mp3_getChannels() == 2 ? 128*4 : 128*2;
	if(tlkmdi_mp3_getPcmDataLen() < needLen || tlkdev_codec_getSpkIdleLen() <= needLen){
		return;
	}
	
	while(true){
		ptr = (signed short*)pcm;
		tlkmdi_mp3_getPcmData((uint08*)pcm, needLen);
		volume = tlkmdi_audio_getToneVolume(false);
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
		tlkdev_codec_fillSpkBuff((uint08*)pcm, 256);
		if(tlkmdi_mp3_getPcmDataLen() < needLen || tlkdev_codec_getSpkIdleLen() <= needLen){
			return; //break;
		}
	}
}


static int tlkmdi_tone_findFilePath(uint08 *pBuff, uint16 buffLen, bool addRoot)
{
	uint08 dataLen = 0;
	
	#if !(TLK_FS_FAT_ENABLE)
		if(buffLen >= 7){
			pBuff[dataLen++] = (uint08)'/';
			pBuff[dataLen++] = (uint08)'T';
			pBuff[dataLen++] = (uint08)'O';
			pBuff[dataLen++] = (uint08)'N';
			pBuff[dataLen++] = (uint08)'E';
			if(addRoot) pBuff[dataLen++] = (uint08)'/';
			pBuff[dataLen++] = 0x00;
		}
	#else
		if(buffLen >= 18){
			pBuff[dataLen++] = (uint08)'1';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)':';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)'/';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)'T';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)'O';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)'N';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)'E';
			pBuff[dataLen++] = 0x00;
			if(addRoot){
				pBuff[dataLen++] = (uint08)'/';
				pBuff[dataLen++] = 0x00;
			}
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = 0x00;
		}
	#endif
	return dataLen;
}
static int tlkmdi_tone_findFileName(uint08 *pBuff, uint16 index)
{
	uint08 length;
	uint08 aIndex = index%10;
	uint08 bIndex = (index/10)%10;
	FCHAR* pFName = (FCHAR*)pBuff;

	length = 0;
	pFName[length++] = 0x30+bIndex;
	pFName[length++] = 0x30+aIndex;
	pFName[length++] = '.';
	pFName[length++] = 'M';
	pFName[length++] = 'P';
	pFName[length++] = '3';
	pFName[length++] = 0x00;
	return TLK_ENONE;
}


#endif //#if (TLKMDI_CFG_AUDTONE_ENABLE)

