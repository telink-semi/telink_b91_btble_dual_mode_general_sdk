/********************************************************************************************************
 * @file     tlkdrv_icodec.c
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
#include "tlkdrv_codec.h"
#if (TLKDRV_CODEC_ICODEC_ENABLE)
#include "tlkdrv_icodec.h"
#include "drivers.h"

extern void audio_codec_adc_power_down(void);
extern void audio_i2s_set_pin(void);
extern void aduio_set_chn_wl(audio_channel_wl_mode_e chn_wl);
extern void audio_tx_dma_chain_init (dma_chn_e chn,unsigned short * out_buff,unsigned int buff_size);
extern void audio_tx_dma_chain_init_abort (dma_chn_e chn,unsigned short * out_buff,unsigned int buff_size);
extern void audio_rx_dma_chain_init_abort (dma_chn_e chn,unsigned short * in_buff, unsigned int buff_size);

static bool tlkdrv_icodec_isOpen(uint08 subDev);
static int tlkdrv_icodec_init(uint08 subDev);
static int tlkdrv_icodec_open(uint08 subDev);
static int tlkdrv_icodec_pause(uint08 subDev);
static int tlkdrv_icodec_close(uint08 subDev);
static int tlkdrv_icodec_reset(uint08 subDev);
static int tlkdrv_icodec_config(uint08 subDev, uint08 opcode, uint32 param0, uint32 param1);

static int tlkdrv_icodec_setMuteDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_getMuteDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_setVolumeDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_getVolumeDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_setChannelDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_getChannelDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_setBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_getBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_setSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_getSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1);

static int tlkdrv_icodec_enable(uint08 bitDepth, uint08 channel, uint32 sampleRate, bool enMic, bool enSpk);
static void tlkdrv_icodec_disable(void);


extern uint16  gTlkDrvCodecSpkBuffLen;
extern uint16  gTlkDrvCodecMicBuffLen;
extern uint08 *gpTlkDrvCodecSpkBuffer;
extern uint08 *gpTlkDrvCodecMicBuffer;
extern aduio_i2s_codec_config_t audio_i2s_codec_config;


const tlkdrv_codec_modinf_t gcTlkDrvIcodecInf = {
	tlkdrv_icodec_isOpen, //IsOpen
	tlkdrv_icodec_init, //Init
	tlkdrv_icodec_open, //Open
	tlkdrv_icodec_pause, //Pause
	tlkdrv_icodec_close, //Close
	tlkdrv_icodec_reset, //Reset
	tlkdrv_icodec_config, //Config
};
static tlkdrv_icodec_t sTlkDrvIcodecCtrl = {0};


static bool tlkdrv_icodec_isOpen(uint08 subDev)
{
	if(!sTlkDrvIcodecCtrl.isInit || !sTlkDrvIcodecCtrl.isOpen) return false;
	if(subDev == TLKDRV_CODEC_SUBDEV_MIC){
		if(sTlkDrvIcodecCtrl.micIsEn) return true;
		else return false;
	}else if(subDev == TLKDRV_CODEC_SUBDEV_SPK){
		if(sTlkDrvIcodecCtrl.spkIsEn) return true;
		else return false;
	}else{
		return true;
	}
}
static int tlkdrv_icodec_init(uint08 subDev)
{
	tmemset(&sTlkDrvIcodecCtrl, 0, sizeof(tlkdrv_icodec_t));
	
	sTlkDrvIcodecCtrl.isInit = true;
	sTlkDrvIcodecCtrl.isOpen = false;
	sTlkDrvIcodecCtrl.isMute = false;
	sTlkDrvIcodecCtrl.micVol = 100;
	sTlkDrvIcodecCtrl.channel = 0x01;
	sTlkDrvIcodecCtrl.bitDepth = 16;
	sTlkDrvIcodecCtrl.sampleRate = 48000;
	
	return TLK_ENONE;
}
static int tlkdrv_icodec_open(uint08 subDev)
{
	if(!sTlkDrvIcodecCtrl.isInit) return -TLK_ESTATUS;
	if(sTlkDrvIcodecCtrl.isOpen) return -TLK_EREPEAT;
	
	if((subDev & TLKDRV_CODEC_SUBDEV_MIC) != 0) sTlkDrvIcodecCtrl.micIsEn = true;
	else sTlkDrvIcodecCtrl.micIsEn = false;
	if((subDev & TLKDRV_CODEC_SUBDEV_SPK) != 0) sTlkDrvIcodecCtrl.spkIsEn = true;
	else sTlkDrvIcodecCtrl.spkIsEn = false;

	tlkdrv_icodec_enable(sTlkDrvIcodecCtrl.bitDepth, sTlkDrvIcodecCtrl.channel,
		sTlkDrvIcodecCtrl.sampleRate, sTlkDrvIcodecCtrl.micIsEn, sTlkDrvIcodecCtrl.spkIsEn);
	if((subDev & TLKDRV_CODEC_SUBDEV_MIC) != 0){
		audio_codec_adc_enable(1);
	}else{
		audio_codec_adc_enable(0);
	}
	
	sTlkDrvIcodecCtrl.isOpen = true;
	return TLK_ENONE;
}
static int tlkdrv_icodec_pause(uint08 subDev)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_icodec_close(uint08 subDev)
{
	if(!sTlkDrvIcodecCtrl.isInit || !sTlkDrvIcodecCtrl.isOpen) return -TLK_ESTATUS;

	tlkdrv_icodec_disable();
	
	sTlkDrvIcodecCtrl.isOpen = false;
	sTlkDrvIcodecCtrl.micIsEn = false;
	sTlkDrvIcodecCtrl.spkIsEn = false;
	
	return TLK_ENONE;
}
static int tlkdrv_icodec_reset(uint08 subDev)
{
	bool isOpen = sTlkDrvIcodecCtrl.isOpen;	
	if(isOpen) tlkdrv_icodec_close(subDev);
	if(isOpen) tlkdrv_icodec_open(subDev);
	return TLK_ENONE;
}
static int tlkdrv_icodec_config(uint08 subDev, uint08 opcode, uint32 param0, uint32 param1)
{
	int ret = -TLK_ENOSUPPORT;
	switch(opcode){
		case TLKDRV_CODEC_OPCODE_SET_MUTE:  //param:[uint08]isMute
			ret = tlkdrv_icodec_setMuteDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_MUTE:
			ret = tlkdrv_icodec_getMuteDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_VOLUME: //param:[uint08]volume
			ret = tlkdrv_icodec_setVolumeDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_VOLUME:
			ret = tlkdrv_icodec_getVolumeDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_CHANNEL: //param:[uint08]chnCnt-1/2/3
			ret = tlkdrv_icodec_setChannelDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_CHANNEL:
			ret = tlkdrv_icodec_getChannelDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_BIT_DEPTH: //param:[uint08]bitDepth-8,16,20,24,32
			ret = tlkdrv_icodec_setBitDepthDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_BIT_DEPTH:
			ret = tlkdrv_icodec_getBitDepthDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_SAMPLE_RATE: //param:[uint32]bitDepth-8,16,20,24,32
			ret = tlkdrv_icodec_setSampleRateDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_SAMPLE_RATE:
			ret = tlkdrv_icodec_getSampleRateDeal(subDev, param0, param1);
			break;
	}
	return ret;
}


static int tlkdrv_icodec_setMuteDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 mute = param0 & 0xFF;
	if(mute != 0x00 && mute != 0x01) return -TLK_EPARAM;
	sTlkDrvIcodecCtrl.isMute = mute;
	return TLK_ENONE;
}
static int tlkdrv_icodec_getMuteDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrvIcodecCtrl.isMute;
}
static int tlkdrv_icodec_setVolumeDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 micVol = param0 & 0xFF;
	if(micVol > 100) return -TLK_EPARAM;
	sTlkDrvIcodecCtrl.micVol = micVol;
	return TLK_ENONE;
}
static int tlkdrv_icodec_getVolumeDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrvIcodecCtrl.micVol;
}
static int tlkdrv_icodec_setChannelDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 channel = param0 & 0xFF;
	if(channel != 0x01 && channel != 0x02 && channel != 0x03) return -TLK_EPARAM;
	sTlkDrvIcodecCtrl.channel = channel;
	return TLK_ENONE;
}
static int tlkdrv_icodec_getChannelDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrvIcodecCtrl.channel;
}
static int tlkdrv_icodec_setBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 bitDapth = param0 & 0xFF;
	if(bitDapth != 8 && bitDapth != 16 && bitDapth != 20 && bitDapth != 24 && bitDapth != 32){
		return -TLK_EPARAM;
	}
	if(bitDapth != 16) return -TLK_ENOSUPPORT;
	sTlkDrvIcodecCtrl.bitDepth = 16;
	return TLK_ENONE;
}
static int tlkdrv_icodec_getBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrvIcodecCtrl.bitDepth;
}
static int tlkdrv_icodec_setSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint32 sampleRate = param0;
	if(sampleRate != 8000 && sampleRate != 16000 && sampleRate != 32000 && sampleRate != 44100 && sampleRate != 48000){
		return -TLK_EPARAM;
	}
	sTlkDrvIcodecCtrl.sampleRate = sampleRate;
	return TLK_ENONE;
}
static int tlkdrv_icodec_getSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrvIcodecCtrl.sampleRate;
}



static int tlkdrv_icodec_enable(uint08 bitDepth, uint08 channel, uint32 sampleRate, bool enMic, bool enSpk)
{
	int rateIndex;

	if(bitDepth != 16 && bitDepth != 24 && bitDepth != 20 && channel != 0x01 && channel != 0x02 && channel != 0x03){
		return -TLK_ENOSUPPORT;
	}
	if(bitDepth == 24){
		if(channel == 0x03) bitDepth = STEREO_BIT_24;
		else bitDepth = MONO_BIT_24;
	}else if(bitDepth == 16){
		if(channel == 0x03) bitDepth = STEREO_BIT_16;
		else bitDepth = MONO_BIT_16;
	}else{
		if(channel == 0x03) bitDepth = STEREO_BIT_20;
		else bitDepth = MONO_BIT_20;
	}
	
//	tlkapi_sendU32s(TLKDRV_CODEC_DEBUG_ENABLE, "tlkdrv_icodec_enable: 002", bitDepth, channel, sampleRate, 0);
	
	audio_reset();
	codec_reset();
	
	audio_tx_dma_dis();
	audio_rx_dma_dis();
	
	audio_set_codec_supply(CODEC_2P8V);
	
	rateIndex = audio_sample_rate_to_index(sampleRate);
	if(enSpk && gpTlkDrvCodecSpkBuffer != nullptr && gTlkDrvCodecSpkBuffLen != 0){
		audio_tx_dma_chain_init_abort(TLKDRV_CODEC_SPK_DMA, (uint16*)gpTlkDrvCodecSpkBuffer, gTlkDrvCodecSpkBuffLen);
	}
	if(enMic && gpTlkDrvCodecMicBuffer != nullptr && gTlkDrvCodecMicBuffLen != 0){
		audio_rx_dma_chain_init_abort(TLKDRV_CODEC_MIC_DMA, (uint16*)gpTlkDrvCodecMicBuffer, gTlkDrvCodecMicBuffLen);
	}

	aduio_set_chn_wl(bitDepth);
	audio_set_codec_clk(1, 16);////from ppl 192/16=12M
	audio_mux_config(CODEC_I2S, audio_i2s_codec_config.audio_in_mode, audio_i2s_codec_config.audio_in_mode, audio_i2s_codec_config.audio_out_mode);
	audio_i2s_config(I2S_I2S_MODE, audio_i2s_codec_config.i2s_data_select, I2S_M_CODEC_S, audio_i2s_codec_config.i2s_data_invert_select);

	audio_set_i2s_clock(rateIndex, AUDIO_RATE_LT_L1, 0);
	audio_clk_en(1, 1);
	
	reg_audio_codec_vic_ctr = FLD_AUDIO_CODEC_SLEEP_ANALOG;//active analog sleep mode

	while(!(reg_audio_codec_stat_ctr & FLD_AUDIO_CODEC_PON_ACK));//wait codec can be configed
	audio_codec_dac_config(I2S_M_CODEC_S, rateIndex, audio_i2s_codec_config.codec_data_select, MCU_WREG);
	audio_codec_adc_config(I2S_M_CODEC_S, AMIC_IN_TO_BUF, rateIndex, audio_i2s_codec_config.codec_data_select, MCU_WREG);

	audio_data_fifo0_path_sel(I2S_DATA_IN_FIFO, I2S_OUT);

	audio_set_codec_adc_wnf(CODEC_ADC_WNF_INACTIVE);
	if(enMic){
		audio_codec_adc_enable(1);
	}else{
		audio_codec_adc_enable(0);
	}

	return TLK_ENONE;
}
static void tlkdrv_icodec_disable(void)
{
//	tlkapi_sendData(TLKDRV_CODEC_DEBUG_ENABLE, "tlkdrv_icodec_disable: 001", 0, 0);
	
	audio_codec_pwr_down();
	audio_codec_adc_power_down();
	audio_codec_dac_power_down();
	audio_codec_adc_enable(0);

	audio_reset();
	codec_reset();

	audio_tx_dma_dis();
	audio_rx_dma_dis();
	
	audio_set_codec_supply(CODEC_1P8V);
}



#endif //#if (TLKDRV_CODEC_ICODEC_ENABLE)

