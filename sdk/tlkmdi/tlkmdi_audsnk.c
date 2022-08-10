/********************************************************************************************************
 * @file     tlkmdi_audsnk.c
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
#if (TLKMDI_CFG_AUDSNK_ENABLE)
#include "drivers.h"
#include "tlkmdi/tlkmdi_audio.h"
#include "tlkalg/audio/sbc/tlkalg_sbc.h"
#include "tlkdev/sys/tlkdev_spk.h"
#include "tlkmdi/tlkmdi_audsnk.h"
#if TLK_ALG_EQ_ENABLE
#include "tlkalg/tlkalg_eq.h"
#endif


#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/a2dp/btp_a2dp.h"
#include "tlkstk/bt/btp/avrcp/btp_avrcp.h"

#include "tlkprt/tlkprt_stdio.h"



#define TLKMDI_SNK_VOLUME_STEP           3

#define TLKMDI_AUDSNK_DBG_FLAG         (TLKMDI_SNK_DBG_ENABLE | TLKMDI_DBG_FLAG) 
#define TLKMDI_AUDSNK_DBG_SIGN         TLKMDI_DBG_SIGN


static int tlkmdi_audsnk_statusChangedEvt(uint08 *pData, uint16 dataLen);

static bool tlkmdi_snk_initBuffer(bool enable);

static void tlkmdi_snk_spkHandler(void);

static int tlkmdi_snk_parseSbcParam(uint08 *data);
static uint tlkmdi_snk_getPlaybackData(uint08 *pBuffer);
static int tlkmdi_snk_addRawFrame(uint08 id, uint08 *p, int len);
static int tlkmdi_snk_firstInit(uint32 para, uint08 *pData, int dataLen);


typedef struct{
	uint08 enable;
	uint08 firstInit;
	uint16 handle;
	uint16 frameSize;
	uint16 sampleRate;
	uint16 frameSample;
	uint32 encParma;
	uint32 runTimer;
	tlkmdi_snk_decFunc decFunc;
}tlkmdi_audsnk_ctrl_t;


static uint32  sTlkMdiSnkBuffLen;
static uint08 *spTlkMdiSnkBuffer;
static tlkapi_qfifo_t spTlkMdiSnkFifo;
static tlkmdi_audsnk_ctrl_t sTlkMdiSnkCtrl = {0};

static uint08 *spTlkMdiSnkSBCDecBuff = nullptr;
static uint08 *spTlkMdiSnkRawRcvBuff = nullptr;
static uint08 *spTlkMdiSnkTmpBuff = nullptr;



/******************************************************************************
 * Function: tlkmdi_audsnk_init
 * Descript: Initial the A2DP sink block, register the data callback. 
 * Params: None.
 * Return: TLK_ENONE is success,other value is false.
*******************************************************************************/
int tlkmdi_audsnk_init(void)
{
	spTlkMdiSnkBuffer = nullptr;
	
	tmemset(&spTlkMdiSnkFifo, 0, sizeof(tlkapi_qfifo_t));
	memset(&sTlkMdiSnkCtrl, 0, sizeof(tlkmdi_audsnk_ctrl_t));

	sTlkMdiSnkCtrl.encParma = 0;
	sTlkMdiSnkCtrl.firstInit = true;

	btp_a2dpsnk_regRecvDataCB(tlkmdi_snk_addEncFrame);
	btp_event_regCB(BTP_EVTID_A2DPSNK_STATUS_CHANGED, tlkmdi_audsnk_statusChangedEvt);
	
	return TLK_ENONE;
}

static int tlkmdi_audsnk_statusChangedEvt(uint08 *pData, uint16 dataLen)
{
	btp_a2dpStatusChangeEvt_t *pEvt;
	pEvt = (btp_a2dpStatusChangeEvt_t*)pData;
	if(pEvt->status == BTP_A2DP_STATUS_STREAM){
		tlkmdi_audio_sendStartEvt(TLKPRT_COMM_AUDIO_CHN_A2DP_SNK, pEvt->handle);
	}else{
		tlkmdi_audsnk_switch(pEvt->handle, TLK_STATE_CLOSED);
		tlkmdi_audio_sendCloseEvt(TLKPRT_COMM_AUDIO_CHN_A2DP_SNK, pEvt->handle);
	}
	return TLK_ENONE;
}

int tlkmdi_audsnk_start(uint16 handle, uint32 param)
{
	if(sTlkMdiSnkCtrl.enable) return -TLK_EREPEAT;
	btp_avrcp_sendKeyPress(handle, BTP_AVRCP_KEYID_PLAY);
	btp_avrcp_sendKeyRelease(handle, BTP_AVRCP_KEYID_PLAY);
	return TLK_ENONE;
}
int tlkmdi_audsnk_close(uint16 handle)
{
	tlkapi_trace(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "tlkmdi_audsnk_close ..");
//	if(!sTlkMdiSnkCtrl.enable) return -TLK_EREPEAT;
//	tlkmdi_audsnk_switch(handle, TLK_STATE_CLOSED);
//	btp_avrcp_sendKeyPress(handle, BTP_AVRCP_KEYID_STOP);
//	btp_avrcp_sendKeyRelease(handle, BTP_AVRCP_KEYID_STOP);
	btp_avrcp_sendKeyPress(handle, BTP_AVRCP_KEYID_PAUSE);
	btp_avrcp_sendKeyRelease(handle, BTP_AVRCP_KEYID_PAUSE);

	return TLK_ENONE;
}

void tlkmdi_audsnk_timer(void)
{
	if(sTlkMdiSnkCtrl.enable && sTlkMdiSnkCtrl.runTimer != 0 
		&& clock_time_exceed(sTlkMdiSnkCtrl.runTimer, 3000000)){
		tlkmdi_audsnk_close(sTlkMdiSnkCtrl.handle);
	}
}

bool tlkmdi_audsnk_toNext(void)
{
	if(!sTlkMdiSnkCtrl.enable) return -TLK_EREPEAT;
	btp_avrcp_sendKeyPress(sTlkMdiSnkCtrl.handle, BTP_AVRCP_KEYID_FORWARD);
	btp_avrcp_sendKeyRelease(sTlkMdiSnkCtrl.handle, BTP_AVRCP_KEYID_FORWARD);
	return TLK_ENONE;
}
bool tlkmdi_audsnk_toPrev(void)
{
	if(!sTlkMdiSnkCtrl.enable) return -TLK_EREPEAT;
	btp_avrcp_sendKeyPress(sTlkMdiSnkCtrl.handle, BTP_AVRCP_KEYID_BACKWARD);
	btp_avrcp_sendKeyRelease(sTlkMdiSnkCtrl.handle, BTP_AVRCP_KEYID_BACKWARD);
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_audsnk_switch
 * Descript: Change A2DP SNK status. 
 * Params: @handle[IN]--The handle.
 *         @status[IN]--The status.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audsnk_switch(uint16 handle, uint08 status)
{
	bool isSucc;
	bool enable;
	uint sampleRate = 0;

	if(status == TLK_STATE_OPENED) enable = true;
	else enable = false;
	if(sTlkMdiSnkCtrl.enable == enable) return true;

	if(!enable) sTlkMdiSnkCtrl.enable = false;
	if(enable){
		sampleRate = btp_a2dpsnk_getSampleRate(handle);
		if(sampleRate == 0){
			tlkapi_error(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "tlkmdi_audsnk_switch: failure - get sample rate");
			return false;
		}
	}
	
	isSucc = tlkmdi_snk_initBuffer(enable);
	if(enable && !isSucc){
		tlkmdi_audsnk_close(handle);
		tlkapi_error(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "tlkmdi_audsnk_switch: failure - buffer");
		return false;
	}
    if (!enable && (btp_a2dpsnk_getStatus(handle) == BTP_A2DP_STATUS_STREAM)){
        tlkmdi_audsnk_close(handle);
	}
	tlkdev_spk_mute();
	
	sTlkMdiSnkCtrl.handle = handle;
	sTlkMdiSnkCtrl.enable = enable;
	sTlkMdiSnkCtrl.encParma = 0;
	sTlkMdiSnkCtrl.runTimer = clock_time()|1;
	sTlkMdiSnkCtrl.firstInit = true;
	tlkapi_qfifo_clear(&spTlkMdiSnkFifo);
	if(enable){
		tlkdev_codec_init(TLKDEV_CODEC_MODE_SINGLE, TLKDEV_CODEC_SELC_INNER);
		tlkdev_codec_setSampleRate(sampleRate);
		tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_A2DP_SNK, TLK_STATE_OPENED);
	}else{
		tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_A2DP_SNK, TLK_STATE_CLOSED);
	}
	
	tlkapi_trace(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "tlkmdi_audsnk_switch: %d %d %d", handle, status, isSucc);
	
	return isSucc;
}

/******************************************************************************
 * Function: tlkmdi_audsnk_isBusy
 * Descript: Is A2DP sink block enable. 
 * Params: None.
 * Return: true or false.
*******************************************************************************/
bool tlkmdi_audsnk_isBusy(void)
{
	return sTlkMdiSnkCtrl.enable;
}

/******************************************************************************
 * Function: tlkmdi_audsnk_intval
 * Descript: Get A2DP sink Interval which will determine the time 
 *           when to start the next timer. 
 * Params: None.
 * Return: The interval value.
*******************************************************************************/
uint tlkmdi_audsnk_intval(void)
{
	return 1000;
}

/******************************************************************************
 * Function: tlkmdi_audsnk_irqProc
 * Descript: Call A2DP SNK spk handler. 
 * Params: None.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audsnk_irqProc(void)
{
//	my_dump_str_data(TLKMDI_SNK_DBG_ENABLE, "tlkmdi_audsnk_irqProc:", 0, 0);
	if(!sTlkMdiSnkCtrl.enable) return false;
	tlkmdi_snk_spkHandler();
	if(sTlkMdiSnkCtrl.enable) return true;
	else return false;
}



int tlkmdi_snk_initFifo(void)
{
	uint32 r = irq_disable();
	if(spTlkMdiSnkBuffer == nullptr || sTlkMdiSnkBuffLen == 0 || sTlkMdiSnkCtrl.frameSize == 0){
		tlkapi_qfifo_reset(&spTlkMdiSnkFifo);
	}else{
		uint16 buffNumb;
		uint16 buffSize;
		buffSize = sTlkMdiSnkCtrl.frameSize;
		buffNumb = sTlkMdiSnkBuffLen/buffSize;
		tlkapi_qfifo_init(&spTlkMdiSnkFifo, buffNumb, buffSize, spTlkMdiSnkBuffer, sTlkMdiSnkBuffLen);
	}
	irq_restore(r);
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_snk_setBuffer
 * Descript: A2DP SNK alloc buffer for audio data. 
 * Params: 
 *        @pBuffer[IN]--The data buffer.
 *        @buffLen[IN]--The buffer length.
 * Return: TLK_ENONE is success, others value is false.
*******************************************************************************/
int	tlkmdi_snk_setBuffer(uint08 *pBuffer, uint32 buffLen)
{
	if(buffLen < TLKMDI_SNK_ENC_FIFO_MIN_SIZE) return -TLK_EPARAM;
	if(pBuffer != nullptr && buffLen != 0){
		spTlkMdiSnkBuffer = pBuffer;
		sTlkMdiSnkBuffLen = buffLen;
		tlkmdi_snk_initFifo();
	}else{
		spTlkMdiSnkBuffer = 0;
		sTlkMdiSnkBuffLen = nullptr;
		tlkapi_qfifo_reset(&spTlkMdiSnkFifo);
	}
	return TLK_ENONE;
}

static bool tlkmdi_snk_initBuffer(bool enable)
{
	bool isSucc = true;
	
	if(enable){
		if(spTlkMdiSnkRawRcvBuff == NULL) spTlkMdiSnkRawRcvBuff = (uint08*)tlkapi_malloc(TLKMDI_SNK_RAW_RCV_BUFF_SIZE);
		if(spTlkMdiSnkSBCDecBuff == NULL) spTlkMdiSnkSBCDecBuff = (uint08*)tlkapi_malloc(TLKMDI_SNK_SBC_DEC_BUFF_SIZE);
		if(spTlkMdiSnkTmpBuff == nullptr) spTlkMdiSnkTmpBuff = (uint08*)tlkapi_malloc(TLKMDI_SNK_DEC_TEMP_SIZE);
		if(spTlkMdiSnkRawRcvBuff == NULL || spTlkMdiSnkSBCDecBuff == NULL || spTlkMdiSnkTmpBuff == nullptr){
			isSucc = false;
			enable = false;
		}
	}
	if(!enable){
		if(spTlkMdiSnkRawRcvBuff != NULL){
			tlkapi_free(spTlkMdiSnkRawRcvBuff);
			spTlkMdiSnkRawRcvBuff = NULL;
		}
		if(spTlkMdiSnkSBCDecBuff != NULL){
			tlkapi_free(spTlkMdiSnkSBCDecBuff);
			spTlkMdiSnkSBCDecBuff = NULL;
		}
		if(spTlkMdiSnkTmpBuff != nullptr){
			tlkapi_free(spTlkMdiSnkTmpBuff);
			spTlkMdiSnkTmpBuff = nullptr;
		}
	}
	tlkmdi_snk_setBuffer(spTlkMdiSnkRawRcvBuff, TLKMDI_SNK_RAW_RCV_BUFF_SIZE);
	tlkalg_sbc_decInit(spTlkMdiSnkSBCDecBuff);
	
	return isSucc;
}


uint tlkmdi_snk_getTimer(void)
{
	return 1000;
}



static void tlkmdi_snk_spkHandler(void)
{
	uint index;
	uint length;
	uint08 volume;
	sint16 *pData;
	
	if(spTlkMdiSnkTmpBuff == nullptr) return;

	while(true){
//		tlkapi_trace(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "tlkmdi_audsnk_irqProc: 1");
		length = 0;
		if(tlkdev_spk_idleLen() > 512){
			length = tlkmdi_snk_getPlaybackData((uint08*)spTlkMdiSnkTmpBuff);
		}
		if(length == 0) break;

		#if TLK_ALG_EQ_ENABLE
		if(length && bt_ll_acl_alive())
		{
			short *p = NULL;
			short stereo_pcm[128*2];
			p = (signed short *)spTlkMdiSnkTmpBuff;
			eq_para.eq_channel = EQ_CHANNEL_LEFT;
			eq_proc_tws_music(eq_para, spTlkMdiSnkTmpBuff, spTlkMdiSnkTmpBuff, sTlkMdiSnkCtrl.frameSample, 0);
		}
		#endif 

//		tlkapi_trace(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "tlkmdi_audsnk_irqProc: 2");
		
		volume = tlkmdi_audio_getMusicVolume(false);
		pData = (sint16*)spTlkMdiSnkTmpBuff;
		for(index=0; index<length/2; index++){
			pData[index] = (pData[index]*volume)>>TLKMDI_AUDIO_VOLUME_EXPAND;
		}
		tlkdev_spk_play(spTlkMdiSnkTmpBuff, length);
	}
}

/******************************************************************************
 * Function: tlkmdi_snk_addEncFrame
 * Descript: A2DP SNK Encode the data. 
 * Params: 
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 * Return: None.
*******************************************************************************/
void tlkmdi_snk_addEncFrame(uint08 *pData, uint16 dataLen)
{
	//60 02 04 01 80 60 01 41  02 36 ee c5 00 00 00 00
	//05 9c bd 35 b5 00 95 32  21 00 95

//	uint08 pktType;
//	uint16 seqNumber;
//	uint32 srcDataID;
//	uint32 timeStamp;
	uint08 pktNumber;

//	tlkapi_array(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "tlkmdi_snk_addEncFrame:", pData, dataLen > 16 ? 16 : dataLen);
	
//	pktType = pData[13];
	pktNumber = pData[12];
	if(pData[13] != 0x9c){			//non-sbc packet
//		tlkapi_array(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "<enc: not SBC packet>", p, 64);
		return;
	}

	sTlkMdiSnkCtrl.runTimer = clock_time()|1;
	
	uint32 param = pData[13] | (pData[14] << 8) | (pData[15] << 16);
	if(sTlkMdiSnkCtrl.firstInit || sTlkMdiSnkCtrl.encParma != param){
//		tlkapi_trace(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "==Reinit");
		tlkapi_qfifo_clear(&spTlkMdiSnkFifo);
		tlkmdi_snk_firstInit(param, pData, dataLen);
		
		if(sTlkMdiSnkCtrl.sampleRate != 0){
			tlkdev_codec_setSampleRate(sTlkMdiSnkCtrl.sampleRate);
		}
		sTlkMdiSnkCtrl.encParma = param;
		sTlkMdiSnkCtrl.firstInit = false;
		sTlkMdiSnkCtrl.decFunc = (tlkmdi_snk_decFunc)tlkalg_sbc_dec_channel0;
//		tlkapi_trace(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "==Init Param: %d %d %d %d", sTlkMdiSnkCtrl.sampleRate, sTlkMdiSnkCtrl.frameSize, sTlkMdiSnkCtrl.frameSample, 0);
	}
	
	if(pktNumber * sTlkMdiSnkCtrl.frameSize == dataLen-13){
		tlkmdi_snk_addRawFrame(pData[7], pData + 13, dataLen-13);
	}else{
		tlkapi_error(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "<enc: receive packet length error>: %d %d %d %d", sTlkMdiSnkCtrl.frameSize, dataLen, pktNumber, 0);
	}
}

int tlkmdi_snk_firstInit(uint32 param, uint08 *pData, int dataLen)
{
	tlkmdi_snk_parseSbcParam(pData+13);
	tlkmdi_snk_initFifo();
	
	#if TLK_ALG_EQ_ENABLE
	if(audio_codec_flag_get(CODEC_FLAG_EQ_MUSIC_EN)){
		eq_para.eq_type = EQ_TYPE_MUSIC;
		eq_para.eq_channel = EQ_CHANNEL_LEFT;
		eq_para.eq_nstage = eq_sys_para.music_eq_num;
		if(48000 == sTlkMdiSnkCtrl.sampleRate){
			eq_para.eq_sample_rate = 0;
		}else{
			eq_para.eq_sample_rate = 1;
		}
		eq_proc_settimg_init(eq_para);
	}
	#endif
	
	return TLK_ENONE;
}



static int tlkmdi_snk_addRawFrame(uint08 id, uint08 *pData, int dataLen)
{
	uint08 *pBuffer;
	
	if(spTlkMdiSnkBuffer == NULL) return 0;
	
	if(sTlkMdiSnkCtrl.frameSize == 0) return -TLK_ENOREADY;
	if((dataLen % sTlkMdiSnkCtrl.frameSize) != 0){
		return -TLK_EPARAM;
	}
	
	while(dataLen >= sTlkMdiSnkCtrl.frameSize){
		pBuffer = tlkapi_qfifo_getBuff(&spTlkMdiSnkFifo);
		if(pBuffer == nullptr) break;
		tmemcpy(pBuffer, pData, sTlkMdiSnkCtrl.frameSize);
		pData   += sTlkMdiSnkCtrl.frameSize;
		dataLen -= sTlkMdiSnkCtrl.frameSize;
		tlkapi_qfifo_dropBuff(&spTlkMdiSnkFifo);
	}
	
	return TLK_ENONE;
}

static int tlkmdi_snk_parseSbcParam(uint08 *data)
{
	uint08 temp;
	uint08 sbcBlock;
	uint08 subBand;
	uint08 bitPool;
	uint16 frameSize;
	uint16 sampleRate;
	
	//read frequency from data
	temp = data[1] & TLKMDI_SNK_SBC_FREQ_MASK;
    if(temp == TLKMDI_SNK_SBC_FREQ_16000) sampleRate = 16000;
    else if(temp == TLKMDI_SNK_SBC_FREQ_32000) sampleRate = 32000;
    else if(temp == TLKMDI_SNK_SBC_FREQ_44100) sampleRate = 44100;
    else sampleRate = 48000;
    
	//read block length from data
	temp = data[1] & TLKMDI_SNK_SBC_BLOCK_MASK;
    if(temp == TLKMDI_SNK_SBC_BLOCK_4) sbcBlock = 4;
    else if(temp == TLKMDI_SNK_SBC_BLOCK_8) sbcBlock = 8;
    else if(temp == TLKMDI_SNK_SBC_BLOCK_12) sbcBlock = 12;
    else sbcBlock = 16; //TLKMDI_SNK_SBC_BLOCK_16
    
	//read subbands from data
	temp = data[1] & TLKMDI_SNK_SBC_SUBBAND_MASK;
    if(temp == TLKMDI_SNK_SBC_SUBBAND_4) subBand = 4;
    else subBand = 8; //TLKMDI_SNK_SBC_SUBBAND_8
	
	//read bitpool from data
	bitPool = data[2];
	
    //read channel mode from data&caculate frame length
	temp = data[1] & TLKMDI_SNK_SBC_TYPE_MASK;
	if(temp == TLKMDI_SNK_SBC_TYPE_MONO) frameSize = 4 + subBand/2 + (sbcBlock*bitPool)/8;
	else if (temp == TLKMDI_SNK_SBC_TYPE_DUAL_MODE) frameSize = 4 + subBand + (sbcBlock*bitPool)/4;
	else if (temp == TLKMDI_SNK_SBC_TYPE_STEREO) frameSize = 4 + subBand + (sbcBlock*bitPool)/8;
	else frameSize = 4 + subBand + (sbcBlock*bitPool + subBand)/8; //TLKMDI_SNK_SBC_TYPE_JOINT_STEREO

	sTlkMdiSnkCtrl.frameSize  = frameSize;
	sTlkMdiSnkCtrl.sampleRate = sampleRate;
	sTlkMdiSnkCtrl.frameSample = 128;
	
//	my_dump_str_u32s(TLKMDI_SNK_DBG_ENABLE, "<enc: sbc param>", sampleRate, sbcBlock, subBand, bitPool);
//	my_dump_str_u32s(TLKMDI_SNK_DBG_ENABLE, "<enc: sbc fsize>", frameSize, 0, 0, 0);
		
	return TLK_ENONE;
}

static uint tlkmdi_snk_getPlaybackData(uint08 *pBuffer)
{
	uint08 *pData;
	uint16 outLen;

	if(tlkapi_qfifo_isEmpty(&spTlkMdiSnkFifo) || sTlkMdiSnkCtrl.decFunc == nullptr) return 0;
	
	//&spTlkMdiSnkFifo
	pData = tlkapi_qfifo_getData(&spTlkMdiSnkFifo);
	if(pData == nullptr){
		outLen = 0;
	}else{
		//ret =
		sTlkMdiSnkCtrl.decFunc(pData, sTlkMdiSnkCtrl.frameSample, pBuffer);
		tlkapi_qfifo_dropData(&spTlkMdiSnkFifo);
		outLen = sTlkMdiSnkCtrl.frameSample*2;
	}

	return outLen;
}



#endif //#if (TLKMDI_CFG_AUDSNK_ENABLE)

