/********************************************************************************************************
 * @file     tlkdev_codec.c
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

#include "drivers.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdev/tlkdev_stdio.h"
#include "tlkdev/sys/tlkdev_codec.h"
#include "tlkdev/sys/tlkdev_spk.h"
#include "tlkdev/sys/tlkdev_mic.h"




extern void audio_codec_config2(audio_channel_wl_mode_e channel_wl,int sample_rate, uint32 * speaker_buff, int speaker_size, uint32 *mic_buff, int mic_size);
static void tlkdev_codec_setInnerSampleRate(uint32 sampleRate);

extern uint08 gTlkDevSpkBuffer[];
extern uint08 gTlkDevMicBuffer[];

static tlkdev_codec_t sTlkDevCodec = {0};


int tlkdev_codec_init(uint08 mode, uint08 codecSel)
{
	uint08 chnMode;
	
	if(sTlkDevCodec.isInit && sTlkDevCodec.mode == mode && sTlkDevCodec.sel == codecSel){
		tlkapi_error(TLKDEV_DBG_FLAG, TLKDEV_DBG_SIGN, "tlkdev_codec_init: mode-%d,sel-%d", mode, codecSel);
		return TLK_ENONE;
	}

	chnMode = MONO_BIT_16;   /// To be consistent with the drive macro definition

	sTlkDevCodec.isInit = true;
	sTlkDevCodec.mode = mode;
	sTlkDevCodec.sel  = codecSel;
	if(sTlkDevCodec.sampleRate == 0) sTlkDevCodec.sampleRate = 44100;
	if(mode == TLKDEV_CODEC_MODE_DOUBLE) chnMode = STEREO_BIT_16;
	

	if(codecSel == TLKDEV_CODEC_SELC_OUTER){
		tlkapi_trace(TLKDEV_DBG_FLAG, TLKDEV_DBG_SIGN, "tlkdev_codec_init: outer");
		audio_codec_config (chnMode, sTlkDevCodec.sampleRate, (uint32 *)gTlkDevSpkBuffer, TLK_DEV_SPK_BUFF_SIZE , (uint32*)gTlkDevMicBuffer, TLK_DEV_MIC_BUFF_SIZE);
	}else{
		if(mode == TLKDEV_CODEC_MODE_DOUBLE){
			sTlkDevCodec.sampleRate = 48000;
			audio_codec_config(chnMode,sTlkDevCodec.sampleRate, (uint32 *)gTlkDevSpkBuffer, TLK_DEV_SPK_BUFF_SIZE , (uint32*)gTlkDevMicBuffer, TLK_DEV_MIC_BUFF_SIZE);
			tlkapi_trace(TLKDEV_DBG_FLAG, TLKDEV_DBG_SIGN, "tlkdev_codec_init: Stereo sample rate-%d", sTlkDevCodec.sampleRate);
			return TLK_ENONE;
		}
		
		tlkapi_trace(TLKDEV_DBG_FLAG, TLKDEV_DBG_SIGN, "tlkdev_codec_init: sampleRate-%d", sTlkDevCodec.sampleRate);
		
		audio_codec_config(chnMode, sTlkDevCodec.sampleRate, (uint32*)gTlkDevSpkBuffer, TLK_DEV_SPK_BUFF_SIZE , (uint32*)gTlkDevMicBuffer, TLK_DEV_MIC_BUFF_SIZE);
		audio_codec_adc_enable(0);
	}

	tlkdev_mic_reset();
	tlkdev_spk_reset();

	return TLK_ENONE;
}
int tlkdev_codec_init2(uint08 mode, uint08 codecSel)
{
	sTlkDevCodec.isInit = true;
	sTlkDevCodec.mode = mode;
	sTlkDevCodec.sel  = codecSel;
	
	audio_codec_config2(sTlkDevCodec.mode, sTlkDevCodec.sampleRate, (uint32*)gTlkDevSpkBuffer, TLK_DEV_SPK_BUFF_SIZE, (uint32*)gTlkDevMicBuffer, TLK_DEV_MIC_BUFF_SIZE);
	audio_codec_adc_enable(0);
	
	tlkdev_mic_reset();
	tlkdev_spk_reset();

	return TLK_ENONE;
}

void tlkdev_codec_reset(void)
{
	sTlkDevCodec.isInit = false;
}

void tlkdev_codec_setSampleRate(uint32 sampleRate)
{
	if(sTlkDevCodec.sampleRate == sampleRate) return;

	if(sampleRate == 0){
		sampleRate = 44100;
		tlkapi_error(TLKDEV_DBG_FLAG, TLKDEV_DBG_SIGN, "tlkdev_codec_init: sampleRate-%d", sTlkDevCodec.sampleRate);
	}
	tlkapi_trace(TLKDEV_DBG_FLAG, TLKDEV_DBG_SIGN, "tlkdev_codec_init: sampleRate changed %d-%d", sTlkDevCodec.sampleRate, sampleRate);

	sTlkDevCodec.sampleRate = sampleRate;
	if(sTlkDevCodec.sel == TLKDEV_CODEC_SELC_OUTER)
	{
		int rate = audio_sample_rate_to_index(sTlkDevCodec.sampleRate);
		audio_set_i2s_clock(rate, 0, 0); //must
	}
	else
	{
		tlkdev_codec_setInnerSampleRate(sTlkDevCodec.sampleRate);
	}
}


_attribute_no_inline_
void tlkdev_codec_config_stereo(uint32 sample_rate)
{
	aduio_set_chn_wl(STEREO_BIT_16);

	int rate = 	audio_sample_rate_to_index (sample_rate);

//	analog_write_reg8 (0x02, 0xc4);		//flash 2.8V trim   codec ?
	audio_set_codec_supply(CODEC_2P8V);

	audio_set_codec_clk(1,16);////from ppl 192/16=12M

	audio_mux_config(CODEC_I2S, BIT_16_STEREO, BIT_16_STEREO, BIT_16_STEREO_FIFO0);

	audio_i2s_config(I2S_I2S_MODE, I2S_BIT_16_DATA, I2S_M_CODEC_S,0);

	audio_set_i2s_clock(rate, AUDIO_RATE_EQUAL, 0);

	audio_clk_en(1,1);

	reg_audio_codec_vic_ctr = FLD_AUDIO_CODEC_SLEEP_ANALOG;//active analog sleep mode

	while(!(reg_audio_codec_stat_ctr&FLD_AUDIO_CODEC_PON_ACK));//wait codec can be configed

	audio_codec_dac_config(I2S_M_CODEC_S, rate, CODEC_BIT_16_DATA, MCU_WREG);

	audio_codec_adc_config(I2S_M_CODEC_S, AMIC_IN_TO_BUF, rate, CODEC_BIT_16_DATA,MCU_WREG);

	audio_data_fifo0_path_sel(I2S_DATA_IN_FIFO,I2S_OUT);

	audio_tx_dma_chain_init(DMA3, (u16*)gTlkDevSpkBuffer, TLK_DEV_SPK_BUFF_SIZE);
	audio_rx_dma_chain_init(DMA2, (u16*)gTlkDevMicBuffer, TLK_DEV_MIC_BUFF_SIZE);
}

static void tlkdev_codec_setInnerSampleRate(uint32 sampleRate)
{
	int rate = audio_sample_rate_to_index(sampleRate);

	BM_SET(reg_audio_codec_dac_ctr,FLD_AUDIO_CODEC_DAC_SOFT_MUTE); //dac mute
	//reg_audio_codec_dac_ctr = FLD_AUDIO_CODEC_DAC_SOFT_MUTE;
	//delay_ms(2);
	
    audio_set_i2s_clock(rate, AUDIO_RATE_LT_L0, 1); //must

	//reg_audio_codec_dac_freq_ctr=(FLD_AUDIO_CODEC_DAC_FREQ&rate);
	audio_write_codec_reg(0x0b, rate);//dac sample rate: 48k
	audio_write_codec_reg(0x0f, 0x10|rate);//dac sample rate: 48k

	//while ( !(audio_read_codec_reg(0x00) == 0x98));//wait codec adc/dac locked
	//delay_ms(2);

    BM_CLR(reg_audio_codec_dac_ctr,FLD_AUDIO_CODEC_DAC_SOFT_MUTE); //dac unmute
	//reg_audio_codec_dac_ctr = 0;		//dac unmute  must
}




