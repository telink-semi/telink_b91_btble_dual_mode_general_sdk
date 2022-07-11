/********************************************************************************************************
 * @file     tlkmdi_sco.c
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
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkdev/tlkdev_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#if (TLKMDI_CFG_AUDSCO_ENABLE)
#include "drivers.h"
#include "tlkmdi/tlkmdi_audio.h"
#include "tlkalg/audio/sbc/tlkalg_sbc.h"
#include "tlkalg/audio/cvsd/tlkalg_cvsd.h"
#include "tlkdev/sys/tlkdev_spk.h"
#include "tlkdev/sys/tlkdev_mic.h"
#include "tlkmdi/tlkmdi_audsco.h"
#if TLK_ALG_EQ_ENABLE
#include "tlkalg/eq/tlkalg_eq.h"
#endif
#if TLK_ALG_EC_ENABLE
#include "tlkalg/audio/ec/tlkalg_ec.h"
#endif
#if TLK_ALG_AGC_ENABLE
#include "tlkalg/audio/agc/tlkalg_agc.h"
#endif

#include "tlkprt/tlkprt_stdio.h"


#define TLKMDI_AUDSCO_DBG_FLAG         (TLKMDI_SCO_DBG_ENABLE | TLKMDI_DBG_FLAG) 
#define TLKMDI_AUDSCO_DBG_SIGN         TLKMDI_DBG_SIGN


#define TLKMDI_SCO_VOLUME_STEP             6

#define TLKMDI_SCO_PACKET_LOSS_FLAG        1
#define TLKMDI_SCO_SCO_LOSS_TEST           0
#define TLKMDI_SCO_PLC_ENABLE              0
#define TLKMDI_SCO_PLC_ENABLE1             1

#define CODEC_DAC_MONO_MODE                1

#define TLKMDI_SCO_GRAD_NUMBER             12
#define TLKMDI_SCO_GRAD_VOLUME             1




extern void bt_ll_sco_data_callback_register(void *prx, void *ptx);
extern void tlkalg_2chnmix(short* pLeft, short* pRight, short* pOut, int stride, int length);


static void tlkmdi_sco_micHandler(void);
static void tlkmdi_sco_spkHandler(void);

#if TLKMDI_SCO_PLC_ENABLE
static int tlkmdi_sco_plcDecode(uint08 *ps, int len, sint16 *pd, int plc);
#endif
static int tlkmdi_sco_msbcEncode(uint08 *pSrc, uint16 srcLen, uint08 *pOut);
static int tlkmdi_sco_msbcDecode(uint08 *pSrc, uint16 srcLen, uint08 *pOut);
static int tlkmdi_sco_cvsdEncode(uint08 *pSrc, uint16 srcLen, uint08 *pOut);
static int tlkmdi_sco_cvsdDecode(uint08 *pSrc, uint16 srcLen, uint08 *pOut);

static int tlkmdi_sco_getPcmData(sint16 *pBuffer);

static void tlkmdi_sco_initCodec(bool enable);
static bool tlkmdi_sco_initBuffer(bool enable);
static bool tlkmdi_sco_mallocAlgBuffer(void);
static void tlkmdi_sco_freeAlgBuffer(void);

static int tlkmdi_sco_connectEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_sco_disconnEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_sco_codecChgEvt(uint08 *pData, uint16 dataLen);

static void tlkmdi_sco_addSpkEncFrame(uint08 id, uint08 *p, int len);
static void tlkmdi_sco_getMicEncFrame(uint08 id, uint08 *p, int len);


const unsigned char sTlkMdiScoMSBCSilencePkt[60] = {0x00,0x00,
	0xad,0x00,0x00,0xc5,0x00,0x00,0x00,0x00, 0x77,0x6d,0xb6,0xdd,0xdb,0x6d,0xb7,0x76,
	0xdb,0x6d,0xdd,0xb6,0xdb,0x77,0x6d,0xb6, 0xdd,0xdb,0x6d,0xb7,0x76,0xdb,0x6d,0xdd,
	0xb6,0xdb,0x77,0x6d,0xb6,0xdd,0xdb,0x6d, 0xb7,0x76,0xdb,0x6d,0xdd,0xb6,0xdb,0x77,
	0x6d,0xb6,0xdd,0xdb,0x6d,0xb7,0x76,0xdb, 0x6c,0x00 
};

static uint08 *sTlkMdiScoAgcBuffer = nullptr;
static tlkapi_qfifo_t sTlkMdiScoSpkFifo;
static tlkapi_qfifo_t sTlkMdiScoMicFifo;
static tlkmdi_sco_ctrl_t sTlkMdiScoCtrl;
static tlkmdi_sco_buff_t sTlkMdiScoBuff;

_attribute_data_retention_sec_ 
uint08 sTlkMdiScoCodec = TLKMDI_SCO_CODEC_ID_CVSD;
#if TLKMDI_SCO_SCO_LOSS_TEST
uint16 sTlkMdiScoErrIndex = 0;
uint16 sTlkMdiScoErrCount = 0;
#endif


/******************************************************************************
 * Function: tlkmdi_audsco_init
 * Descript: Initial the Sco block, register the Sco data callback. and register 
 *           connect and disconnect or codec callback to listen the sco status. 
 * Params: None.
 * Return: TLK_ENONE is success,other value is false.
*******************************************************************************/
int tlkmdi_audsco_init(void)
{
	bt_ll_sco_data_callback_register(tlkmdi_sco_addSpkEncFrame, tlkmdi_sco_getMicEncFrame);

	bth_event_regCB(BTH_EVTID_SCOCONN_COMPLETE, tlkmdi_sco_connectEvt);
	bth_event_regCB(BTH_EVTID_SCODISC_COMPLETE, tlkmdi_sco_disconnEvt);
	bth_event_regCB(BTH_EVTID_SCOCODEC_CHANGED, tlkmdi_sco_codecChgEvt);
	
	return TLK_ENONE;
}


/******************************************************************************
 * Function: tlkmdi_audsco_switch
 * Descript: Change the sco status. 
 * Params:
 *        @handle[IN]--The handle
 *        @status[IN]--The status.
 * Return: The interval value.
*******************************************************************************/
bool tlkmdi_audsco_switch(uint16 handle, uint08 status)
{
	bool isSucc;
	bool enable;

	if(status == TLK_STATE_OPENED) enable = true;
	else enable = false;
	if(sTlkMdiScoCtrl.enable == enable) return true;

	if(!enable) sTlkMdiScoCtrl.enable = false;
	isSucc = tlkmdi_sco_initBuffer(enable);
	if(!isSucc) enable = false;

	sTlkMdiScoCtrl.numb = 0;
	sTlkMdiScoCtrl.enable = enable;
	sTlkMdiScoCtrl.handle = handle;
	sTlkMdiScoCtrl.dropSpkNumb = 10;
	sTlkMdiScoCtrl.dropMicNumb = 10;

	tlkdev_spk_mute();
	
	tlkmdi_sco_initCodec(enable);
	if(enable){
		tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_audsco_switch: enable");
		tlkdev_codec_init(TLKDEV_CODEC_MODE_SINGLE, TLKDEV_CODEC_SELC_INNER);
		tlkdev_codec_setSampleRate(16000);
		audio_codec_adc_enable(1);
		tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_SCO, TLK_STATE_OPENED);
	}else{
		tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_audsco_switch: disable");
		audio_codec_adc_enable(0);
		tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_SCO, TLK_STATE_CLOSED);
	}

	return isSucc;
}


/******************************************************************************
 * Function: tlkmdi_audsco_init
 * Descript: verify the sco is enable. 
 * Params: None.
 * Return: true is success,other value is false.
*******************************************************************************/
bool tlkmdi_audsco_isBusy(void)
{
	return sTlkMdiScoCtrl.enable;
}

/******************************************************************************
 * Function: tlkmdi_audsco_intval
 * Descript: Get the sco interval which will determine when to start the next timer. 
 * Params: None.
 * Return: The interval value.
*******************************************************************************/
uint tlkmdi_audsco_intval(void)
{
	return 1000;
}


/******************************************************************************
 * Function: tlkmdi_audsco_irqProc
 * Descript: call the mic handler and spkhandler. 
 * Params: None.
 * Return: The true/false.
*******************************************************************************/
bool tlkmdi_audsco_irqProc(void)
{
	if(!sTlkMdiScoCtrl.enable) return false;
	tlkmdi_sco_micHandler();
	tlkmdi_sco_spkHandler();
	if(sTlkMdiScoCtrl.enable) return true;
	else return false;
}


/******************************************************************************
 * Function: tlkmdi_sco_XXXCodec
 * Descript: Get/Set the SCO codec. 
 * Params: @codec[IN]--The codec id.
 * Return: The codec id.
*******************************************************************************/
uint tlkmdi_sco_getCodec(void)
{
    return sTlkMdiScoCodec;
}

void tlkmdi_sco_setCodec(uint08 codec)
{
	tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_setCodec: %d %d", sTlkMdiScoCodec, codec);
    if(TLKMDI_SCO_CODEC_ID_CVSD == codec || TLKMDI_SCO_CODEC_ID_MSBC == codec){
        sTlkMdiScoCodec = codec;
		if(sTlkMdiScoCtrl.enable) tlkmdi_sco_initCodec(sTlkMdiScoCtrl.enable);
    }
}

static int tlkmdi_sco_connectEvt(uint08 *pData, uint16 dataLen)
{
	bth_scoConnComplateEvt_t *pEvt;

	pEvt = (bth_scoConnComplateEvt_t*)pData;
	if(pEvt->status != 0){
		tlkapi_error(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_connectEvt: failure - %d", pEvt->status);
		return TLK_ENONE;
	}
	tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_connectEvt: {status-%d,handle-%d,scoHandle-%d,linkType-%d}", 
		pEvt->status, pEvt->aclHandle, pEvt->scoHandle, pEvt->linkType);
	tlkmdi_audio_sendStartEvt(TLKPRT_COMM_AUDIO_CHN_SCO, pEvt->aclHandle);
	return TLK_ENONE;
}
static int tlkmdi_sco_disconnEvt(uint08 *pData, uint16 dataLen)
{
	bth_scoDiscComplateEvt_t *pEvt;

	pEvt = (bth_scoDiscComplateEvt_t*)pData;
	tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_disconnEvt: {status-%d,handle-%d,scoHandle-%d,linkType-%d}", 
		pEvt->reason, pEvt->aclHandle, pEvt->scoHandle, pEvt->linkType);
	tlkmdi_audsco_switch(pEvt->aclHandle, TLK_STATE_CLOSED);
	tlkmdi_audio_sendCloseEvt(TLKPRT_COMM_AUDIO_CHN_SCO, pEvt->aclHandle);
	return TLK_ENONE;
}
static int tlkmdi_sco_codecChgEvt(uint08 *pData, uint16 dataLen)
{
	bth_scoCodecChangedEvt_t *pEvt;

	pEvt = (bth_scoCodecChangedEvt_t*)pData;
	tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_codecChgEvt: %d", pEvt->codec);
	if(pEvt->codec != sTlkMdiScoCodec){
		sTlkMdiScoCodec = pEvt->codec;
		if(sTlkMdiScoCtrl.enable) tlkmdi_sco_initCodec(sTlkMdiScoCtrl.enable);
	}
	return TLK_ENONE;
}

volatile uint32 AAAA_irq_test211 = 0;
volatile uint32 AAAA_irq_test212 = 0;
volatile uint32 AAAA_irq_test213 = 0;
volatile uint32 AAAA_irq_test214 = 0;
volatile uint32 AAAA_irq_test215 = 0;
volatile uint32 AAAA_irq_test216 = 0;
volatile uint32 AAAA_irq_test217 = 0;
volatile uint32 AAAA_irq_test218 = 0;

_attribute_bt_ram_code_
static void tlkmdi_sco_addSpkEncFrame(uint08 id, uint08 *pData, int dataLen) // NOTE: This is in IRQ
{
	uint08 *pBuffer;
	
	if(pData == nullptr || dataLen == 0){ //start voice function on first valid packet
//		tlkapi_error(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_addSpkEncFrame: err");
		return;
	}
	if(!sTlkMdiScoCtrl.enable) return;

	if(sTlkMdiScoCtrl.dropSpkNumb != 0) sTlkMdiScoCtrl.dropSpkNumb --;
	if(sTlkMdiScoCtrl.dropSpkNumb != 0) return;

//	tlkapi_array(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_addSpkEncFrame", pData, 16);

	AAAA_irq_test211 ++;
	pBuffer = tlkapi_qfifo_getBuff(&sTlkMdiScoSpkFifo);
	AAAA_irq_test212 ++;
	if(pBuffer != nullptr && pData != nullptr && pData[0] == BTH_HCI_TYPE_SCO_DATA){
		uint16 tempLen;
		uint16 syncHead = pData[1] + (pData[2] << 8);
		pBuffer[0] = 0;
		pBuffer[1] = 0;
		AAAA_irq_test213 ++;
		#if TLKMDI_SCO_SCO_LOSS_TEST
		sTlkMdiScoErrIndex ++ ;
		if(sTlkMdiScoErrIndex >= 100){
//			tlkapi_error(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "p_error: %d", sTlkMdiScoErrCount);
			sTlkMdiScoErrCount = 0;
			sTlkMdiScoErrIndex = 0;
		}
		#endif
		if((syncHead & 0x3000) == BTH_HCI_SYNC_HDR_PSF_GOOD){
			tempLen = pData[3];
			if(tempLen > TLKMDI_SCO_SPK_ENC_BUFF_SIZE) tempLen = TLKMDI_SCO_SPK_ENC_BUFF_SIZE;
			if(tempLen != 0) tmemcpy(pBuffer+2, &pData[4], tempLen);
		}else{
			#if TLKMDI_SCO_SCO_LOSS_TEST
			sTlkMdiScoErrCount ++;
			#endif
            pBuffer[0] = TLKMDI_SCO_PACKET_LOSS_FLAG;
		}
		tlkapi_qfifo_dropBuff(&sTlkMdiScoSpkFifo);
		AAAA_irq_test214 ++;
	}
	AAAA_irq_test215 ++;
}

uint08 sTlkMdiMicMuteIndex = 0;
_attribute_bt_data_retention_ unsigned char byte1[4] = {0x08, 0x38, 0xc8, 0xf8};
_attribute_bt_data_retention_ unsigned char silencePkt[64] = {0x00,0x00,
	0xad,0x00,0x00,0xc5,0x00,0x00,0x00,0x00, 0x77,0x6d,0xb6,0xdd,0xdb,0x6d,0xb7,0x76,
	0xdb,0x6d,0xdd,0xb6,0xdb,0x77,0x6d,0xb6, 0xdd,0xdb,0x6d,0xb7,0x76,0xdb,0x6d,0xdd,
	0xb6,0xdb,0x77,0x6d,0xb6,0xdd,0xdb,0x6d, 0xb7,0x76,0xdb,0x6d,0xdd,0xb6,0xdb,0x77,
	0x6d,0xb6,0xdd,0xdb,0x6d,0xb7,0x76,0xdb, 0x6c,0x00,0x00,0x00,0x00,0x00};
_attribute_bt_ram_code_
static void tlkmdi_sco_getMicEncFrame(uint08 id, uint08 *pBuff, int buffLen) // NOTE: This is in IRQ
{
	uint08 *pData;

	if(!sTlkMdiScoCtrl.enable) return;
	AAAA_irq_test216 ++;
//	tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_getMicEncFrame: len - %d", len);
	pData = tlkapi_qfifo_getData(&sTlkMdiScoMicFifo);
	if(pData != nullptr){
		if(buffLen > TLKMDI_SCO_MIC_ENC_BUFF_SIZE){
			buffLen = TLKMDI_SCO_MIC_ENC_BUFF_SIZE;
//			tlkapi_error(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "===getMicEncFrame err");
		}
		tmemcpy(pBuff, pData, buffLen);
		tlkapi_qfifo_dropData(&sTlkMdiScoMicFifo);
	}else{
		if(sTlkMdiScoCodec == TLKMDI_SCO_CODEC_ID_CVSD){
			tmemset(pBuff, 0x55, buffLen);
		}else if(sTlkMdiScoCodec != TLKMDI_SCO_CODEC_ID_MSBC){
			tmemset(pBuff, 0x00, buffLen);
		}else{
			u8 copyLen = 0;
//			unsigned char byte1[4] = {0x08, 0x38, 0xc8, 0xf8};
//			unsigned char silencePkt[64] = {0x00,0x00,
//				0xad,0x00,0x00,0xc5,0x00,0x00,0x00,0x00, 0x77,0x6d,0xb6,0xdd,0xdb,0x6d,0xb7,0x76,
//				0xdb,0x6d,0xdd,0xb6,0xdb,0x77,0x6d,0xb6, 0xdd,0xdb,0x6d,0xb7,0x76,0xdb,0x6d,0xdd,
//				0xb6,0xdb,0x77,0x6d,0xb6,0xdd,0xdb,0x6d, 0xb7,0x76,0xdb,0x6d,0xdd,0xb6,0xdb,0x77,
//				0x6d,0xb6,0xdd,0xdb,0x6d,0xb7,0x76,0xdb, 0x6c,0x00,0x00,0x00,0x00,0x00};
			if(buffLen > 64) copyLen = 64;
			else copyLen = buffLen;
			silencePkt[0] = 0x01;
			silencePkt[1] = byte1[sTlkMdiMicMuteIndex & 0x03];
			sTlkMdiMicMuteIndex ++;
			tmemcpy(pBuff, silencePkt, copyLen);
		}
	}
	AAAA_irq_test217 ++;
//	tlkapi_array(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_getMicEncFrame: ", pBuff, 16);	
}

static void tlkmdi_sco_micHandler(void)
{
	uint16 offset = 0;
	uint08 *pMicPtr;
	uint08 *pBuffer;
	short spkData[256];
	short micData[256];

	pBuffer = tlkapi_qfifo_getBuff(&sTlkMdiScoMicFifo);
	if(pBuffer == nullptr || sTlkMdiScoCtrl.enc_func == nullptr) return;

	if(!tlkdev_mic_readData((uint08*)micData, 240, &offset)){
		return;
	}
	#if TLK_ALG_EC_ENABLE
	tlkdev_spk_backReadData((uint08*)spkData, 240, offset+140, true);
	pMicPtr = (uint08*)tlkalg_ec_frame((uint08*)micData, (uint08*)spkData);
	#else
	pMicPtr = micData;
	#endif
	if(pMicPtr == nullptr) return;
	
    /* mixing tone here for remote HF device or AG device */
	#if TLK_ALG_AGC_ENABLE
	sint16 pcm_agc[120];
	sint16* pcm_agc_ptr=pcm_agc;
	sint16* pcm_ptr = (sint16*)pMicPtr;
	my_agc_process(sTlkMdiScoAgcBuffer, (const short* const*)&pcm_ptr, 120, (short* const*)&pcm_agc_ptr);
	#endif
	#if TLK_ALG_EQ_ENABLE
	if(audio_codec_flag_get(CODEC_FLAG_EQ_VOICE_MIC_EN)){
		eq_para.eq_type = EQ_TYPE_VOICE_MIC;
		eq_para.eq_channel = EQ_CHANNEL_LEFT;
		eq_proc_tws_speech(eq_para, pMicPtr, pMicPtr, 120, 0);
	}
	#endif
	
	#if TLK_ALG_AGC_ENABLE
	sTlkMdiScoCtrl.enc_func((uint08*)pcm_agc, 240, (uint08*)pBuffer);
	#else
	sTlkMdiScoCtrl.enc_func((uint08*)pMicPtr, 240, (uint08*)pBuffer);
	#endif
	
	tlkapi_qfifo_dropBuff(&sTlkMdiScoMicFifo);
}
static void tlkmdi_sco_spkHandler(void)
{
	int count = 16;
	uint08 volume;
	
	if(sTlkMdiScoBuff.pTempBuffer == nullptr) return;
	//sTlkMdiScoTempBuff -- sbc 128; aac 1024

//	tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_spkHandler 001");

	while(count--){
		int np = 0;
		if(tlkdev_spk_idleLen() > 512){
			np = tlkmdi_sco_getPcmData((sint16*)sTlkMdiScoBuff.pTempBuffer); //tvoice_get_playback_data, tmusic_get_playback_data, app_music_getPlaybackData
		}
		if(np == 0) break;

//		tlkapi_array(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_spkHandler 002", sTlkMdiScoBuff.pTempBuffer, 16);
		#if TLK_ALG_EQ_ENABLE
		if(np && bt_ll_acl_alive())
		{
			short *p = nullptr;
			short stereo_pcm[128*2];
			p = (signed short *)sTlkMdiScoBuff.pTempBuffer;
			/// 0. interleave revert proc
			int samples = (audio_codec_flag_get(CODEC_FLAG_STEREO_EN)?240:120);
			if(audio_codec_flag_get(CODEC_FLAG_STEREO_EN))
			{
				for(int i = 0; i < samples/2; i++){
					stereo_pcm[i] = p[i*2];
					stereo_pcm[i + samples/2] = p[i*2 + 1];
				}
				eq_para.eq_type = EQ_TYPE_VOICE_SPK;
				eq_para.eq_channel = EQ_CHANNEL_LEFT;
				eq_proc_tws_speech(eq_para,&stereo_pcm[0], &stereo_pcm[0], samples/2, 0);
				eq_para.eq_channel = EQ_CHANNEL_RIGHT;
				eq_proc_tws_speech(eq_para,&stereo_pcm[120], &stereo_pcm[120], samples/2, 0);
				tlkalg_2chnmix(&stereo_pcm[0], &stereo_pcm[120], (sint16*)sTlkMdiScoBuff.pTempBuffer, 1, 120);
			}
			else
			{
				eq_para.eq_type = EQ_TYPE_VOICE_SPK;
				eq_para.eq_channel = EQ_CHANNEL_LEFT;
				eq_proc_tws_speech(eq_para, sTlkMdiScoBuff.pTempBuffer, sTlkMdiScoBuff.pTempBuffer, samples, 0);
			}
		}
		#endif 
		
		if(np){			
            sint16 *dec_pcm = (int16_t *)sTlkMdiScoBuff.pTempBuffer;
            int temp;
			int samples = audio_codec_flag_get(CODEC_FLAG_STEREO_EN)?240:120;
			volume = tlkmdi_audio_getVoiceVolume(false);
            for(int i = 0; i < samples; i++){
                temp = ((int)dec_pcm[i] * volume)>>TLKMDI_AUDIO_VOLUME_EXPAND;
                dec_pcm[i] = temp;
            }
			tlkdev_spk_play((uint08*)sTlkMdiScoBuff.pTempBuffer, np);
		}
	}
}


static int tlkmdi_sco_getPcmData(sint16 *pBuffer)
{
	uint08 *pData;
			
	pData = tlkapi_qfifo_getData(&sTlkMdiScoSpkFifo);
	if(pData == nullptr || sTlkMdiScoCtrl.dec_func == nullptr) return 0;
	
//	log_task(SL_APP_MUSIC_EN, SL01_task_music_dec, 1);
	#if CODEC_DAC_MONO_MODE
		if(pData[0] != TLKMDI_SCO_PACKET_LOSS_FLAG){
			sTlkMdiScoCtrl.dec_func(pData, 60, (uint08*)pBuffer);
			#if TLKMDI_SCO_PLC_ENABLE1
			tlkalg_sbc_plcGoodFrame(&gTlkalgSbcPlcState, pBuffer, pBuffer);
			#endif
		}else{				
			//my_dump_str_u32s (1, "voice plc", sTlkMdiScoCtrl.spk_enc_rptr, 0, 0, 0);
			#if TLKMDI_SCO_PLC_ENABLE1
			if (TLKMDI_SCO_CODEC_ID_MSBC == tlkmdi_sco_getCodec()){
				tlkalg_msbc_decData((uint08 *)&gTlkalgSbcPlcIndices0[0], 57, (uint08*)pBuffer);
			}else{
				sTlkMdiScoCtrl.dec_func(pData, 60, (uint08*)pBuffer);
			}
			tlkalg_sbc_plcBadFrame(&gTlkalgSbcPlcState, pBuffer, pBuffer);			
            #endif
		}
	#else
		u16 pcm[120];
		sint16 *my_pd = (sint16 *)pcm;
		if(pData[0] != TLKMDI_SCO_PACKET_LOSS_FLAG){
			sTlkMdiScoCtrl.dec_func(pData, 60, my_pd);
			#if TLKMDI_SCO_PLC_ENABLE1
			tlkalg_sbc_plcGoodFrame(&gTlkalgSbcPlcState, my_pd, my_pd);
			#endif
		}else{				
			#if TLKMDI_SCO_PLC_ENABLE1
			if (TLKMDI_SCO_CODEC_ID_MSBC == tlkmdi_sco_getCodec()){
				tlkalg_msbc_decData((uint08 *)&gTlkalgSbcPlcIndices0[0], 57, my_pd);
			}else{
				sTlkMdiScoCtrl.dec_func(pData, 60, my_pd);
			}
			tlkalg_sbc_plcBadFrame(&gTlkalgSbcPlcState, my_pd, my_pd);
			#endif
		}
		tmemcpy(pBuffer, pcm, 120*2);
	#endif
	
	if(sTlkMdiScoCtrl.dropMicNumb != 0) sTlkMdiScoCtrl.dropMicNumb --;
	if(sTlkMdiScoCtrl.dropMicNumb != 0) return 0;
	tlkapi_qfifo_dropData(&sTlkMdiScoSpkFifo);
	
//	log_task (SL_APP_MUSIC_EN, SL01_task_music_dec, 0);
	return 120 * 2;
}

#if TLKMDI_SCO_PLC_ENABLE
static int tlkmdi_sco_plcDecode (uint08 *ps, int len, sint16 *pd, int plc)
{
	static int my_voice_plc = 0;
	static sint16 my_voice_buff[120];
	if(plc){
		my_voice_plc = 2;
		//my_dump_str_u32s (1, "voice plc", sTlkMdiScoCtrl.spk_enc_rptr, my_voice_plc, 0, 0);
	}
	sTlkMdiScoCtrl.dec_func (ps, 60, pd);

	if(my_voice_plc)
	{
		my_voice_plc--;
		sint16 *pdd = pd;
		for (int i=0; i<120; i++)
		{
			pd[i] = (my_voice_buff[119 - i] * (119 - i) + pdd[i] * i) / 120;
		}
	}
	for (int i=0; i<120; i++)
	{
		my_voice_buff[i] = pd[i];
	}
	return 0;
}
#endif //#if TLKMDI_SCO_PLC_ENABLE

static int tlkmdi_sco_msbcEncode(uint08 *pSrc, uint16 srcLen, uint08 *pOut)
{
	////////////////////// msbc format ////////////////////////////////
	//msbc, 57, 240, ad_0_0
	//msbc, 57, 240, ad_75_54,_a4
	uint32 dlen;
	unsigned char byte1[4] = {0x08, 0x38, 0xc8, 0xf8};
	pOut[0] = 0x01;
	pOut[1] = byte1[sTlkMdiScoCtrl.numb & 3];
	
	tlkalg_sbc_encData((const uint08*)pSrc, srcLen, pOut + 2, 64, &dlen);
	sTlkMdiScoCtrl.numb ++;
	if(dlen == 57) return TLK_ENONE;
	else return -TLK_EFAIL;
}
static int tlkmdi_sco_msbcDecode(uint08 *pSrc, uint16 srcLen, uint08 *pOut)
{
	uint32 dlen;
	tlkalg_sbc_decData((const uint08*)pSrc + 4, srcLen, pOut, 240, &dlen, 1, 0x01);
	if(dlen == 240){
//		tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_msbcDecode: success");
		return TLK_ENONE;
	}else{
//		tlkapi_error(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_msbcDecode: failure");
		return -TLK_EFAIL;
	}
}
static int tlkmdi_sco_cvsdEncode(uint08 *pSrc, uint16 srcLen, uint08 *pOut)
{
	if(tlkalg_cvsd_enc((short *)pSrc, srcLen/2, pOut)) return TLK_ENONE;
	else return -TLK_EFAIL;
}
static int tlkmdi_sco_cvsdDecode(uint08 *pSrc, uint16 srcLen, uint08 *pOut)
{
	bool isSucc;
	if(pSrc[0] == TLKMDI_SCO_PACKET_LOSS_FLAG){
		isSucc = tlkalg_cvsd_decMute((short*)pOut, srcLen * 2);
	}else{
		isSucc = tlkalg_cvsd_dec(pSrc+2, srcLen, (short *)pOut);
	}
	if(isSucc) return TLK_ENONE;
	else return -TLK_EFAIL;
}


static void tlkmdi_sco_initCodec(bool enable)
{
    tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_initCodec,");

	if(!enable || sTlkMdiScoBuff.pEncodeBuffer == nullptr){
		tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_initCodec: disable");
		sTlkMdiScoCtrl.dec_func = nullptr;
		sTlkMdiScoCtrl.enc_func = nullptr;
		tlkalg_cvsd_init(nullptr, nullptr, 0, 0);
		tlkalg_sbc_encInit(nullptr);
		tlkalg_sbc_decInit(nullptr);
	}else{
		tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_initCodec: enable");
		if(TLKMDI_SCO_CODEC_ID_CVSD == tlkmdi_sco_getCodec()){
			tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_initCodec: CVSD init");
			tlkalg_cvsd_init(sTlkMdiScoBuff.pEncodeBuffer, sTlkMdiScoBuff.pEncodeBuffer+TLKMDI_SCO_CVSD_PARAM_SIZE,
				TLKMDI_SCO_CVSD_PARAM_SIZE, TLKMDI_SCO_CVSD_CACHE_SIZE);
			sTlkMdiScoCtrl.dec_func = tlkmdi_sco_cvsdDecode;
			sTlkMdiScoCtrl.enc_func = tlkmdi_sco_cvsdEncode;
	    }else{
	    	tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_initCodec: enter MSBC");
			tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_initCodec: SBC init");
			tlkalg_sbc_encInit(sTlkMdiScoBuff.pEncodeBuffer+TLKMDI_SCO_SBC_ENC_BUFF_OFFS);
			tlkalg_sbc_decInit(sTlkMdiScoBuff.pEncodeBuffer+TLKMDI_SCO_SBC_DEC_BUFF_OFFS);
			sTlkMdiScoCtrl.dec_func = tlkmdi_sco_msbcDecode;
			sTlkMdiScoCtrl.enc_func = tlkmdi_sco_msbcEncode;
	    }
	}
}
static bool tlkmdi_sco_initBuffer(bool enable)
{
	bool isEnable;

	isEnable = enable;
	if(isEnable && !tlkmdi_sco_mallocAlgBuffer()){
		isEnable = false;
	}
	if(!isEnable){
		tlkmdi_sco_freeAlgBuffer();
	}
	if(!isEnable){
		#if (TLK_ALG_EC_ENABLE)
		tlkalg_ec_init(nullptr, nullptr);
		#endif
		#if (TLK_ALG_AGC_ENABLE)
		sTlkMdiScoAgcBuffer = nullptr;
		my_agc_init(nullptr, 3, 30, 16000);
		#endif
		tlkapi_qfifo_reset(&sTlkMdiScoSpkFifo);
		tlkapi_qfifo_reset(&sTlkMdiScoMicFifo);
	}
	else{
		#if (TLK_ALG_EC_ENABLE)
		tlkalg_ec_init(sTlkMdiScoBuff.pSpeexBuffer+TLKMDI_SCO_SPEEX_NS_OFFS,
			sTlkMdiScoBuff.pAecmBuffer+TLKMDI_SCO_AECM_ST_OFFS);
		#endif
		#if (TLK_ALG_AGC_ENABLE)
		sTlkMdiScoAgcBuffer = sTlkMdiScoBuff.pSpeexBuffer+TLKMDI_SCO_AGC_ST_OFFS;
		my_agc_init(sTlkMdiScoAgcBuffer, 3, 30, 16000);
		#endif
		uint16 index;
		uint16 encBuffLen = TLKMDI_SCO_SPK_ENC_BUFF_SIZE*TLKMDI_SCO_SPK_ENC_BUFF_NUMB;
		uint16 decBuffLen = TLKMDI_SCO_MIC_ENC_BUFF_SIZE*TLKMDI_SCO_MIC_ENC_BUFF_NUMB;
		uint08 *pEncBuffer = sTlkMdiScoBuff.pCacheBuffer;
		uint08 *pDecBuffer = sTlkMdiScoBuff.pCacheBuffer+encBuffLen;
		tlkapi_qfifo_init(&sTlkMdiScoSpkFifo, TLKMDI_SCO_SPK_ENC_BUFF_NUMB, TLKMDI_SCO_SPK_ENC_BUFF_SIZE, pEncBuffer, encBuffLen);
		tlkapi_qfifo_init(&sTlkMdiScoMicFifo, TLKMDI_SCO_MIC_ENC_BUFF_NUMB, TLKMDI_SCO_MIC_ENC_BUFF_SIZE, pDecBuffer, decBuffLen);
		unsigned char byte1[4] = {0x08, 0x38, 0xc8, 0xf8};
		for(index=0; index<TLKMDI_SCO_MIC_ENC_BUFF_NUMB; index++){
			tmemcpy(pDecBuffer, sTlkMdiScoMSBCSilencePkt, 60);
			pDecBuffer[0] = 0x01;
			pDecBuffer[1] = byte1[index & 3];
			pDecBuffer += TLKMDI_SCO_MIC_ENC_BUFF_SIZE;
		}
	}
	if(isEnable == enable) return true;
	else return false;
}

static bool tlkmdi_sco_mallocAlgBuffer(void)
{
	bool isOK = true;

	if(sTlkMdiScoBuff.pTempBuffer == nullptr){
		sTlkMdiScoBuff.pTempBuffer = (uint08*)tlkapi_malloc(TLKMDI_SCO_TEMP_BUFFER_SIZE);
	}
	if(sTlkMdiScoBuff.pAecmBuffer == nullptr){
		sTlkMdiScoBuff.pAecmBuffer = (uint08*)tlkapi_malloc(TLKMDI_SCO_AECM_BUFFER_SIZE);
	}
	if(sTlkMdiScoBuff.pCacheBuffer == nullptr){
		sTlkMdiScoBuff.pCacheBuffer = (uint08*)tlkapi_malloc(TLKMDI_SCO_CACHE_BUFFER_SIZE);
	} 
	if(sTlkMdiScoBuff.pSpeexBuffer == nullptr){
		sTlkMdiScoBuff.pSpeexBuffer = (uint08*)tlkapi_malloc(TLKMDI_SCO_SPEEX_BUFFER_SIZE);
	}
	if(sTlkMdiScoBuff.pEncodeBuffer == nullptr){
		sTlkMdiScoBuff.pEncodeBuffer = (uint08*)tlkapi_malloc(TLKMDI_SCO_ENCODE_BUFFER_SIZE);
	}
	if(sTlkMdiScoBuff.pSpeexBuffer == nullptr || sTlkMdiScoBuff.pAecmBuffer == nullptr
		|| sTlkMdiScoBuff.pTempBuffer == nullptr || sTlkMdiScoBuff.pCacheBuffer == nullptr
		|| sTlkMdiScoBuff.pEncodeBuffer == nullptr){
		isOK = false;
	}
	return isOK;
}
static void tlkmdi_sco_freeAlgBuffer(void)
{
	if(sTlkMdiScoBuff.pTempBuffer != nullptr){
		tlkapi_free(sTlkMdiScoBuff.pTempBuffer);
		sTlkMdiScoBuff.pTempBuffer = nullptr;
	}
	if(sTlkMdiScoBuff.pAecmBuffer != nullptr){
		tlkapi_free(sTlkMdiScoBuff.pAecmBuffer);
		sTlkMdiScoBuff.pAecmBuffer = nullptr;
	}
	if(sTlkMdiScoBuff.pCacheBuffer != nullptr){
		tlkapi_free(sTlkMdiScoBuff.pCacheBuffer);
		sTlkMdiScoBuff.pCacheBuffer = nullptr;
	}
	if(sTlkMdiScoBuff.pSpeexBuffer != nullptr){
		tlkapi_free(sTlkMdiScoBuff.pSpeexBuffer);
		sTlkMdiScoBuff.pSpeexBuffer = nullptr;
	}
	if(sTlkMdiScoBuff.pEncodeBuffer != nullptr){
		tlkapi_free(sTlkMdiScoBuff.pEncodeBuffer);
		sTlkMdiScoBuff.pEncodeBuffer = nullptr;
	}
}


#endif //#if (TLKMDI_CFG_AUDSCO_ENABLE)

