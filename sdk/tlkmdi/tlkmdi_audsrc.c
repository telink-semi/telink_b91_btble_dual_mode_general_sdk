/********************************************************************************************************
 * @file     tlkmdi_audsrc.c
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
#if (TLKMDI_CFG_AUDSRC_ENABLE)
#include "drivers.h"
#include "tlkmdi/tlkmdi_audio.h"
#include "tlkalg/audio/sbc/tlkalg_sbc.h"
#include "tlkmdi/tlkmdi_audmp3.h"
#include "tlkmdi/tlkmdi_audsrc.h"

#include "tlkprt/tlkprt_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/a2dp/btp_a2dp.h"
#include "tlkstk/bt/btp/avrcp/btp_avrcp.h"


#define TLKMDI_AUDSRC_DBG_FLAG         (TLKMDI_SRC_DBG_ENABLE | TLKMDI_DBG_FLAG) 
#define TLKMDI_AUDSRC_DBG_SIGN         TLKMDI_DBG_SIGN


//uint16 gTlkDevMicBuffer[TLK_DEV_MIC_BUFF_SIZE/2];
#if (TLKMDI_CFG_REUSE_SPKBUFF_ENABLE)
extern uint16 gTlkDevSpkBuffer[];
#endif

static int  tlkmdi_audsrc_a2dpStatusEvt(uint08 *pData, uint16 dataLen);
static void tlkmdi_audsrc_keyChangedEvt(uint16 aclHandle, uint08 keyID, uint08 isPress);

//extern void btp_avrcp_setvolume(uint16 handle, uint08 volume);
extern int  hci_rxfifo_half_full(void);
extern bool btp_a2dpsrc_isInStream(uint16 handle);

extern int  btp_a2dpsrc_setSampleRate(uint16 aclHandle, uint32 sampleRate);

static void tlkmdi_src_mp3Handler(void);
static void tlkmdi_src_fillHandler(void);
static void tlkmdi_src_resetParam(int sample_rate);

static tlkmdi_audsrc_ctrl_t sTlkMdiSrcCtrl;

static uint08 *spTlkMdiSrcPktBuff = nullptr;
static uint08 *spTlkMdiSrcEncBuff = nullptr;


extern int hci_rxfifo_half_full(void);


/******************************************************************************
 * Function: tlkmdi_audplay_init
 * Descript: Initialize playback parameters and logic. 
 * Params: None.
 * Return: 'TLK_ENONE' means success, otherwise failure.
*******************************************************************************/
int tlkmdi_audsrc_init(void)
{
	tmemset(&sTlkMdiSrcCtrl, 0, sizeof(tlkmdi_audsrc_ctrl_t));
	
	btp_event_regCB(BTP_EVTID_A2DPSRC_STATUS_CHANGED, tlkmdi_audsrc_a2dpStatusEvt);
	btp_avrcp_regKeyChangeCB(tlkmdi_audsrc_keyChangedEvt);
	
	return TLK_ENONE;
}

static int tlkmdi_audsrc_a2dpStatusEvt(uint08 *pData, uint16 dataLen)
{
	btp_a2dpStatusChangeEvt_t *pEvt;

	pEvt = (btp_a2dpStatusChangeEvt_t*)pData;
	if(sTlkMdiSrcCtrl.enable && sTlkMdiSrcCtrl.handle != pEvt->handle){
		tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_a2dpStatusEvt: other device request");
		if(pEvt->status == BTP_A2DP_STATUS_STREAM) btp_a2dpsrc_suspend(pEvt->handle);
		return -TLK_EFAIL;
	}

	if(pEvt->status == BTP_A2DP_STATUS_STREAM){
		uint sampleRate = btp_a2dpsrc_getSampleRate(pEvt->handle);
		if(sTlkMdiSrcCtrl.waitStart != 0 && sampleRate != tlkmdi_mp3_getSampleRate()){
			tlkmdi_audsrc_close(pEvt->handle);
			return -TLK_EFAIL;
		}
		sTlkMdiSrcCtrl.sampleRate = sampleRate;
		if(sTlkMdiSrcCtrl.waitStart == 0){
			tlkmdi_audio_sendStartEvt(TLKPRT_COMM_AUDIO_CHN_A2DP_SRC, pEvt->handle);
		}
		sTlkMdiSrcCtrl.waitStart = 0;
		sTlkMdiSrcCtrl.waitTimer = 0;
	}else{
		tlkmdi_audsrc_switch(pEvt->handle, TLK_STATE_CLOSED);
		tlkmdi_audio_sendCloseEvt(TLKPRT_COMM_AUDIO_CHN_A2DP_SRC, pEvt->handle);
	}
	return TLK_ENONE;
}
static void tlkmdi_audsrc_keyChangedEvt(uint16 aclHandle, uint08 keyID, uint08 isPress)
{
	tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_keyChangedEvt:{handle-%d,keyID-%d,isPress-%d}", aclHandle, keyID, isPress);
	if(!isPress){
		if(keyID == BTP_AVRCP_KEYID_PLAY){
			if(!sTlkMdiSrcCtrl.enable){
				tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_avrcpStatusEvt: play execute");
				tlkmdi_audsrc_start(aclHandle, 0xFFFFFFFF);
			}else{
				tlkapi_error(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_avrcpStatusEvt: play failure");
			}
		}else if(keyID == BTP_AVRCP_KEYID_STOP || keyID == BTP_AVRCP_KEYID_PAUSE){
			if(sTlkMdiSrcCtrl.enable){
				tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_avrcpStatusEvt: stop or pause execute");
				tlkmdi_audsrc_close(aclHandle);
			}else{
				tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_avrcpStatusEvt: stop or pause failure");
			}
		}else if(keyID == BTP_AVRCP_KEYID_FORWARD){
			if(sTlkMdiSrcCtrl.enable) tlkmdi_audsrc_toNext();
		}else if(keyID == BTP_AVRCP_KEYID_BACKWARD){
			if(sTlkMdiSrcCtrl.enable) tlkmdi_audsrc_toPrev();
		}
	}
}


int tlkmdi_audsrc_start(uint16 handle, uint32 param)
{
	return btp_a2dpsrc_start(handle);
}
int tlkmdi_audsrc_close(uint16 handle)
{
	if(sTlkMdiSrcCtrl.handle != handle){
		tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_close: enable handle");
		return -TLK_EHANDLE;
	}
	tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_close start: enable handle");
	sTlkMdiSrcCtrl.waitStart = 0;
	tlkmdi_audsrc_switch(sTlkMdiSrcCtrl.handle, TLK_STATE_CLOSED);
	return btp_a2dpsrc_suspend(sTlkMdiSrcCtrl.handle);
}

void tlkmdi_audsrc_timer(void)
{
	if(sTlkMdiSrcCtrl.waitStart == 1){
		int ret = btp_a2dpsrc_setSampleRate(sTlkMdiSrcCtrl.handle, tlkmdi_mp3_getSampleRate());
		if(ret == TLK_ENONE){
			sTlkMdiSrcCtrl.waitTimer = 0;
			sTlkMdiSrcCtrl.waitStart = 0;
		}else if(ret == -TLK_EBUSY){
			sTlkMdiSrcCtrl.waitTimer = clock_time()|1;
			sTlkMdiSrcCtrl.waitStart = 2;
		}else{
			tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_src_mp3Handler: disable - set sampleRate");
			sTlkMdiSrcCtrl.runing = false;
			sTlkMdiSrcCtrl.waitStart = 3;
			tlkmdi_audsrc_close(sTlkMdiSrcCtrl.handle);
		}
	}
	if(sTlkMdiSrcCtrl.waitStart != 0 && sTlkMdiSrcCtrl.waitStart != 3 && sTlkMdiSrcCtrl.waitTimer != 0 
		&& clock_time_exceed(sTlkMdiSrcCtrl.waitTimer, TLKMDI_SRC_WAIT_RECFG_TIMEOUT)){
		sTlkMdiSrcCtrl.runing = false;
		sTlkMdiSrcCtrl.waitStart = 3;
		tlkmdi_audsrc_close(sTlkMdiSrcCtrl.handle);
	}
}

bool tlkmdi_audsrc_toNext(void)
{
	uint16 index;
	if(!sTlkMdiSrcCtrl.runing) return false;
	tlkdev_spk_mute();
	tlkmdi_mp3_updateEnable(true);
	index = tlkmdi_mp3_getPlayIndex();
	tlkmdi_audio_sendPlayOverEvt(TLKPRT_COMM_AUDIO_CHN_PLAY, index);
	sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_IDLE;
	sTlkMdiSrcCtrl.playIndex = tlkmdi_mp3_getNextIndex();
	return true;
}
bool tlkmdi_audsrc_toPrev(void)
{
	uint16 index;
	if(!sTlkMdiSrcCtrl.runing) return false;
	tlkdev_spk_mute();
	tlkmdi_mp3_updateEnable(true);
	index = tlkmdi_mp3_getPlayIndex();
	tlkmdi_audio_sendPlayOverEvt(TLKPRT_COMM_AUDIO_CHN_PLAY, index);
	sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_IDLE;
	sTlkMdiSrcCtrl.playIndex = tlkmdi_mp3_getPrevIndex();
	return true;
}

/******************************************************************************
 * Function: tlkmdi_audsrc_switch
 * Descript: Change A2DP SRC status. 
 * Params: @handle[IN]--The handle.
 *         @status[IN]--The status.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audsrc_switch(uint16 handle, uint08 status)
{
	bool enable;
	bool isSucc = true;

	if(status == TLK_STATE_OPENED) enable = true;
	else enable = false;
	if(handle == 0 && enable) enable = false;
	if(!enable) tlkmdi_mp3_updateEnable(false);
	if(status == TLK_STATE_CLOSED && sTlkMdiSrcCtrl.handle != 0){
		btp_a2dpsrc_suspend(sTlkMdiSrcCtrl.handle);
	}
	if(sTlkMdiSrcCtrl.enable == enable) return true;
	
	if(!enable) sTlkMdiSrcCtrl.enable = false;
	if(!tlkmdi_mp3_enable(enable) && enable){
		tlkmdi_audsrc_close(handle);
		tlkapi_error(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_switch: enable failure - %d", enable);
		return false;
	}
	
	if(enable){
		#if (TLKMDI_CFG_REUSE_SPKBUFF_ENABLE)
		spTlkMdiSrcPktBuff = (uint08*)gTlkDevSpkBuffer;
		spTlkMdiSrcEncBuff = (((uint08*)gTlkDevSpkBuffer)+TLKMDI_SRC_PKT_BUFF_SIZE+8);
		#else
		if(spTlkMdiSrcPktBuff == nullptr) spTlkMdiSrcPktBuff = (uint08*)tlkapi_malloc(TLKMDI_SRC_PKT_BUFF_SIZE); 
		if(spTlkMdiSrcEncBuff == nullptr) spTlkMdiSrcEncBuff = (uint08*)tlkapi_malloc(TLKMDI_SRC_SBC_ENC_BUFF_SIZE);
		if(spTlkMdiSrcPktBuff == nullptr || spTlkMdiSrcEncBuff == nullptr){
			isSucc = false;
			enable = false;
			tlkmdi_mp3_enable(false);
		}
		#endif
	}
	if(!enable){
		#if !(TLKMDI_CFG_REUSE_SPKBUFF_ENABLE)
		if(spTlkMdiSrcPktBuff != nullptr){
			tlkapi_free(spTlkMdiSrcPktBuff);
			spTlkMdiSrcPktBuff = nullptr;
		}
		if(spTlkMdiSrcEncBuff != nullptr){
			tlkapi_free(spTlkMdiSrcEncBuff);
			spTlkMdiSrcEncBuff = nullptr;
		}
		#endif
	}

	sTlkMdiSrcCtrl.enable = enable;
	sTlkMdiSrcCtrl.runing = enable;
	sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_IDLE;
	sTlkMdiSrcCtrl.handle = handle;
	tlkalg_sbc_encInit(spTlkMdiSrcEncBuff);
	
	sTlkMdiSrcCtrl.refTime = clock_time();
	sTlkMdiSrcCtrl.lagTime = 0;
	sTlkMdiSrcCtrl.timeStamp = 0;
	sTlkMdiSrcCtrl.unitTime = 0;
	sTlkMdiSrcCtrl.waitTimer = 0;
	sTlkMdiSrcCtrl.waitStart = 0;
	sTlkMdiSrcCtrl.playIndex = tlkmdi_mp3_getPlayIndex();

	tlkmdi_mp3_startUpdate();
	
	if(enable){
		#if TLK_CFG_PTS_ENABLE
		uint16 bitpool = 53;
		uint16 blocks = 16;
		#endif
		tlkmdi_src_resetParam(44100);
		#if TLK_CFG_PTS_ENABLE
		bitpool = btp_a2dpsrc_getbitpool(sTlkMdiSrcCtrl.handle);
		blocks = btp_a2dpsrc_getblock(sTlkMdiSrcCtrl.handle);
		tlkalg_sbc_encSetBitpool(blocks, bitpool);
		tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_switch: bitpool-0x%x, blocks-%d", bitpool, blocks);
		#endif
		tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_A2DP_SRC, TLK_STATE_OPENED);
	}else{
		tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_A2DP_SRC, TLK_STATE_CLOSED);
	}

	tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_switch: 0x%x, status-%d, isSucc-%d", handle, status, isSucc);
	
	return isSucc;
}

/******************************************************************************
 * Function: tlkmdi_audsrc_isBusy
 * Descript: Is A2DP source block enable. 
 * Params: None.
 * Return: true or false.
*******************************************************************************/
bool tlkmdi_audsrc_isBusy(void)
{
	return (sTlkMdiSrcCtrl.enable && sTlkMdiSrcCtrl.runing);
}

/******************************************************************************
 * Function: tlkmdi_audsrc_intval
 * Descript: Get A2DP source Interval which will determine the time 
 *           when to start the next timer. 
 * Params: None.
 * Return: The interval value.
*******************************************************************************/
uint tlkmdi_audsrc_intval(void)
{
	if(sTlkMdiSrcCtrl.waitStart != 0){
		return 5000;
	}else if(sTlkMdiSrcCtrl.lagTime >= sTlkMdiSrcCtrl.unitTime){
		return 500;
	}else if(tlkmdi_mp3_getPcmDataLen() < 1024){
		return 1000;
	}else if(sTlkMdiSrcCtrl.unitTime > 1500){
		return 1500;
	}else{
		return sTlkMdiSrcCtrl.unitTime;
	}
}

/******************************************************************************
 * Function: tlkmdi_audsrc_irqProc
 * Descript: Call A2DP SRC mp3 handler and fill handler. 
 * Params: None.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audsrc_irqProc(void)
{
//	my_dump_str_data(TLKMDI_SNK_DBG_ENABLE, "tlkmdi_audsrc_irqProc:", 0, 0);
	if(!sTlkMdiSrcCtrl.enable) return false;
	tlkmdi_src_mp3Handler();
	tlkmdi_src_fillHandler();
	if(sTlkMdiSrcCtrl.runing) return true;
	else return false;
}




#define TLKMDI_SRC_TICK_PER_US    16
static void tlkmdi_src_resetParam(int sample_rate)
{
	if(sample_rate == 0) sample_rate = 44100;
	sTlkMdiSrcCtrl.unitTime = (1000000*128*TLKMDI_SRC_TICK_PER_US+sample_rate/2)/sample_rate;
	sTlkMdiSrcCtrl.seqNumber = 0;
	sTlkMdiSrcCtrl.timeStamp = 0;
	sTlkMdiSrcCtrl.refTime = clock_time();
	sTlkMdiSrcCtrl.lagTime = 0;
	sTlkMdiSrcCtrl.sndFrame = 0;
}


static bool tlkmdi_src_start(uint16 index)
{
	bool isSucc;

	tlkdev_spk_mute();
	tlkmdi_mp3_updateEnable(true);
	isSucc = tlkmdi_mp3_play(index);
	if(!isSucc){
		tlkmdi_mp3_updateEnable(false);
		sTlkMdiSrcCtrl.runing = false;
		sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_PLAY;
	}else{
		sTlkMdiSrcCtrl.runing = true;
		sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_IDLE;
	}
	return isSucc;
}

static void tlkmdi_src_mp3Handler(void) //in irq
{
	if(!sTlkMdiSrcCtrl.runing) return;
	if(sTlkMdiSrcCtrl.mp3State == TLKMDI_MP3_STATUS_IDLE){
		tlkmdi_src_start(sTlkMdiSrcCtrl.playIndex);
		if(!sTlkMdiSrcCtrl.runing) return;
		if(tlkmdi_mp3_getSampleRate() != sTlkMdiSrcCtrl.sampleRate){
			sTlkMdiSrcCtrl.waitStart = 1;
			sTlkMdiSrcCtrl.waitTimer = clock_time()|1;
		}
		sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_PLAY;
		tlkmdi_audio_sendPlayStartEvt(TLKPRT_COMM_AUDIO_CHN_A2DP_SRC, sTlkMdiSrcCtrl.playIndex);
	}
	if(sTlkMdiSrcCtrl.mp3State == TLKMDI_MP3_STATUS_WAIT || sTlkMdiSrcCtrl.mp3State == TLKMDI_MP3_STATUS_DONE){
		uint16 index = tlkmdi_mp3_getPlayIndex();
		tlkdev_spk_mute();
		if(sTlkMdiSrcCtrl.mp3State == TLKMDI_MP3_STATUS_DONE){
			sTlkMdiSrcCtrl.runing = false;
		}else{
			sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_IDLE;
		}
		tlkmdi_audio_sendPlayOverEvt(TLKPRT_COMM_AUDIO_CHN_A2DP_SRC, index);
	}else if(sTlkMdiSrcCtrl.mp3State == TLKMDI_MP3_STATUS_PLAY){
		tlkmdi_mp3_decode();
		if(tlkmdi_mp3_isOver()){
			tlkmdi_mp3_reset();
			if(tlkmdi_mp3_indexIsOver()){
				tlkmdi_mp3_setPlayIndex(0);
				sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_DONE;
			}else{
				sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_WAIT;
				sTlkMdiSrcCtrl.playIndex = tlkmdi_mp3_getNextPlay();
			}
		}
	}
}
static void tlkmdi_src_fillHandler(void)
{
	uint08 volume;
	uint08 frameSize;
	uint16 readLens;
	uint32 refTime;

//	tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "=== app_audio_a2dpSrcPlayProcs 001");
	if(!sTlkMdiSrcCtrl.runing) return;
	if(sTlkMdiSrcCtrl.unitTime == 0 || sTlkMdiSrcCtrl.handle == 0){  
		sTlkMdiSrcCtrl.refTime = clock_time(); 
		return; 
	}
	if(sTlkMdiSrcCtrl.waitTimer != 0){
		sTlkMdiSrcCtrl.refTime = clock_time();
		return;
	}
		
	frameSize = 85; // set as 77 if btpool is 32;
	
//	tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "=== app_audio_a2dpSrcPlayProcs 002");
	refTime = clock_time();
	sTlkMdiSrcCtrl.lagTime += refTime-sTlkMdiSrcCtrl.refTime;
	sTlkMdiSrcCtrl.refTime = refTime;
	if(sTlkMdiSrcCtrl.lagTime > 8*sTlkMdiSrcCtrl.unitTime){
		sTlkMdiSrcCtrl.lagTime = 8*sTlkMdiSrcCtrl.unitTime;
	}
	
	readLens = tlkmdi_mp3_getChannels() == 2 ? 128*4 : 128 *2;
	if(sTlkMdiSrcCtrl.sndFrame < TLKMDI_SRC_FRAME_NUMB && sTlkMdiSrcCtrl.lagTime >= sTlkMdiSrcCtrl.unitTime
		&& tlkmdi_mp3_getPcmDataLen() >= readLens) 
	{
		uint16 value;
		uint16 index;
		uint16 offset;
		uint16 pcm[256];
		uint08 *pBuffer;
		
		tlkmdi_mp3_getPcmData((uint08*)pcm, readLens);
		sTlkMdiSrcCtrl.lagTime -= sTlkMdiSrcCtrl.unitTime;  
		sint16 *p =(sint16*)pcm;
				
//		tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "=== app_audio_a2dpSrcPlayProcs 004");
		volume = tlkmdi_audio_getHeadsetVolume(false);
		for(index=0; index<256; index++){
			p[index] = (p[index]*volume)>>TLKMDI_AUDIO_VOLUME_EXPAND;
		}
		if(readLens == 256){
			for(index=0; index<128; index++){
				value = pcm[127-index];
				offset = 254-(index << 1);
				pcm[offset+0] = value;
				pcm[offset+1] = value;
			}
		}
		pBuffer = spTlkMdiSrcPktBuff+(1+sTlkMdiSrcCtrl.sndFrame*frameSize);
		tlkalg_sbc_encJoint((uint08*)pcm, 128*4, pBuffer);
		sTlkMdiSrcCtrl.sndFrame ++;
	}
	if(sTlkMdiSrcCtrl.sndFrame >= TLKMDI_SRC_FRAME_NUMB && !hci_rxfifo_half_full()){// retry send last send fail pkt;
		uint16 pktLen;
		pktLen = 1+frameSize*TLKMDI_SRC_FRAME_NUMB;
		spTlkMdiSrcPktBuff[0] = TLKMDI_SRC_FRAME_NUMB;
		btp_a2dpsrc_sendMediaData(sTlkMdiSrcCtrl.handle, sTlkMdiSrcCtrl.seqNumber, sTlkMdiSrcCtrl.timeStamp, spTlkMdiSrcPktBuff, pktLen);
		sTlkMdiSrcCtrl.sndFrame = 0;
		sTlkMdiSrcCtrl.seqNumber ++;
		sTlkMdiSrcCtrl.timeStamp += 8;
	}
}

#endif //#if (TLKMDI_CFG_AUDSRC_ENABLE)
