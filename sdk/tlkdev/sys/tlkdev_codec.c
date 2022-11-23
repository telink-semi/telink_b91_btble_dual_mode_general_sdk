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
#include "tlkapi/tlkapi_stdio.h"
#if (TLK_DEV_CODEC_ENABLE)
#include "tlkdrv/ext/codec/tlkdrv_codec.h"
#include "tlkdev/tlkdev_stdio.h"
#include "tlkdev/sys/tlkdev_codec.h"
#include "drivers.h"


__attribute__((aligned(4))) uint16 gTlkDevCodecSpkBuffer[TLK_DEV_SPK_BUFF_SIZE/2];
__attribute__((aligned(4))) uint16 gTlkDevCodecMicBuffer[TLK_DEV_MIC_BUFF_SIZE/2];


int tlkdev_codec_init(void)
{
	audio_set_codec_in_path_a_d_gain(CODEC_IN_D_GAIN_0_DB, CODEC_IN_A_GAIN_8_DB); //mic
	audio_set_codec_out_path_a_d_gain(CODEC_OUT_D_GAIN_0_DB, CODEC_OUT_A_GAIN_0_DB); //spk

	tlkdrv_codec_setSpkBuffer((uint08*)gTlkDevCodecSpkBuffer, TLK_DEV_SPK_BUFF_SIZE);
	tlkdrv_codec_setMicBuffer((uint08*)gTlkDevCodecMicBuffer, TLK_DEV_MIC_BUFF_SIZE);
	
	tlkdrv_codec_mount(TLKDRV_CODEC_DEV_INNER, TLKDRV_CODEC_SUBDEV_BOTH);
//	tlkdrv_codec_mount(TLKDRV_CODEC_DEV_RTL2108, TLKDRV_CODEC_SUBDEV_BOTH);
	
	tlkdrv_codec_setVolume(100);
	tlkdrv_codec_setChannel(1);
	tlkdrv_codec_setBitDepth(16);
	tlkdrv_codec_setSampleRate(48000);
		
	return TLK_ENONE;
}


int tlkdev_codec_open(TLKDEV_CODEC_SUBDEV_ENUM subDev, uint08 channel, uint08 bitDepth, uint32 sampleRate)
{
	int ret = TLK_ENONE;
	if(ret == TLK_ENONE) ret = tlkdrv_codec_setChannel(channel);
	if(ret == TLK_ENONE) ret = tlkdrv_codec_setBitDepth(bitDepth);
	if(ret == TLK_ENONE) ret = tlkdrv_codec_setSampleRate(sampleRate);	
	if(ret == TLK_ENONE) ret = tlkdrv_codec_open(subDev);
	return ret;
}
int tlkdev_codec_close(void)
{
	return tlkdrv_codec_close();
}
int tlkdev_codec_extOpen(TLKDEV_CODEC_SUBDEV_ENUM subDev, uint08 spkChannel, uint08 spkBitDepth,
	uint32 spkSampleRate, uint08 micChannel, uint08 micBitDepth, uint32 micSampleRate)
{
	int ret = TLK_ENONE;
	if(ret == TLK_ENONE){
		if(subDev == TLKDEV_CODEC_SUBDEV_SPK){
			ret = tlkdrv_codec_setSpkChannel(spkChannel);
		}else if(subDev == TLKDEV_CODEC_SUBDEV_MIC){
			ret = tlkdrv_codec_setMicChannel(micChannel);
		}else if(spkChannel == micChannel){
			ret = tlkdrv_codec_setChannel(spkChannel);
		}else{
			return -TLK_EPARAM;
		}
	}
	if(ret == TLK_ENONE){
		if(subDev == TLKDEV_CODEC_SUBDEV_SPK){
			ret = tlkdrv_codec_setSpkBitDepth(spkBitDepth);
		}else if(subDev == TLKDEV_CODEC_SUBDEV_MIC){
			ret = tlkdrv_codec_setMicBitDepth(micBitDepth);
		}else if(spkBitDepth == micBitDepth){
			ret = tlkdrv_codec_setBitDepth(spkBitDepth);
		}else{
			return -TLK_EPARAM;
		}
	}
	if(ret == TLK_ENONE){
		if(subDev == TLKDEV_CODEC_SUBDEV_SPK){
			ret = tlkdrv_codec_setSpkSampleRate(spkSampleRate);
		}else if(subDev == TLKDEV_CODEC_SUBDEV_MIC){
			ret = tlkdrv_codec_setMicSampleRate(micSampleRate);
		}else if(spkSampleRate == micSampleRate){
			ret = tlkdrv_codec_setSampleRate(spkSampleRate);
		}else{
			return -TLK_EPARAM;
		}
	}
	if(ret == TLK_ENONE) ret = tlkdrv_codec_open(subDev);
	return ret;
}


uint tlkdev_codec_getSampleRate(void)
{
	return tlkdrv_codec_getSampleRate();
}
uint tlkdev_codec_getChannel(void)
{
	return tlkdrv_codec_getChannel();
}


void tlkdev_codec_muteSpk(void)
{
	tlkdrv_codec_muteSpk();
}

uint tlkdev_codec_getSpkOffset(void)
{
	return tlkdrv_codec_getSpkOffset();
}
uint tlkdev_codec_getMicOffset(void)
{
	return tlkdrv_codec_getMicOffset();
}
void tlkdev_codec_setSpkOffset(uint16 offset)
{
	tlkdrv_codec_setSpkOffset(offset);
}
void tlkdev_codec_setMicOffset(uint16 offset)
{
	tlkdrv_codec_setMicOffset(offset);
}

void tlkdev_codec_setSpkBuffer(uint08 *pBuffer, uint16 buffLen)
{
	tlkdrv_codec_setSpkBuffer(pBuffer, buffLen);
}
void tlkdev_codec_setMicBuffer(uint08 *pBuffer, uint16 buffLen)
{
	tlkdrv_codec_setMicBuffer(pBuffer, buffLen);
}

uint tlkdev_codec_getSpkBuffLen(void)
{
	return tlkdrv_codec_getSpkBuffLen();
}
uint tlkdev_codec_getMicBuffLen(void)
{
	return tlkdrv_codec_getMicBuffLen();
}
uint tlkdev_codec_getSpkIdleLen(void)
{
	return tlkdrv_codec_getSpkIdleLen();
}
uint tlkdev_codec_getSpkDataLen(void)
{
	return tlkdrv_codec_getSpkDataLen();
}
uint tlkdev_codec_getMicDataLen(void)
{
	return tlkdrv_codec_getMicDataLen();
}

bool tlkdev_codec_readSpkData(uint08 *pBuffer, uint16 buffLen, uint16 offset)
{
	return tlkdrv_codec_readSpkData(pBuffer, buffLen, offset);
}
bool tlkdev_codec_readMicData(uint08 *pBuff, uint16 buffLen, uint16 *pOffset)
{
	return tlkdrv_codec_readMicData(pBuff, buffLen, pOffset);
}

void tlkdev_codec_zeroSpkBuff(uint16 zeroLen, bool isInc)
{
	tlkdrv_codec_zeroSpkBuff(zeroLen, isInc);
}
bool tlkdev_codec_fillSpkBuff(uint08 *pData, uint16 dataLen)
{
	return tlkdrv_codec_fillSpkBuff(pData, dataLen);
}
bool tlkdev_codec_backReadSpkData(uint08 *pBuff, uint16 buffLen, uint16 offset, bool isBack)
{
	return tlkdrv_codec_backReadSpkData(pBuff, buffLen, offset, isBack);
}


#endif //#if (TLK_DEV_CODEC_ENABLE)

