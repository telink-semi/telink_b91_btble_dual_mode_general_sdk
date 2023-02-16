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
#include "tlkdev/tlkdev.h"
#include "tlkdev/sys/tlkdev_codec.h"
#include "drivers.h"

static void tlkdrv_codec_micDataZoom(uint08 *pData, uint16 dataLen);
static void tlkdrv_codec_spkDataZoom(uint08 *pData, uint16 dataLen);

__attribute__((aligned(4))) uint16 gTlkDevCodecSpkBuffer[TLK_DEV_SPK_BUFF_SIZE/2];
__attribute__((aligned(4))) uint16 gTlkDevCodecMicBuffer[TLK_DEV_MIC_BUFF_SIZE/2];
static tlkdev_codec_t sTlkDevCodecCtrl;


int tlkdev_codec_init(void)
{
	tmemset(&sTlkDevCodecCtrl, 0, sizeof(tlkdev_codec_t));
	
	sTlkDevCodecCtrl.spkChannel = 1;
	sTlkDevCodecCtrl.micChannel = 1;
	
	tlkdrv_codec_setSpkBuffer((uint08*)gTlkDevCodecSpkBuffer, TLK_DEV_SPK_BUFF_SIZE);
	tlkdrv_codec_setMicBuffer((uint08*)gTlkDevCodecMicBuffer, TLK_DEV_MIC_BUFF_SIZE);

	tlkdrv_codec_paInit();
	
	tlkdrv_codec_mount(TLKDRV_CODEC_DEV_INNER, TLKDRV_CODEC_SUBDEV_BOTH);
//	tlkdrv_codec_mount(TLKDRV_CODEC_DEV_RTL2108, TLKDRV_CODEC_SUBDEV_BOTH);
	
	tlkdev_codec_setSpkVolume(100);
	tlkdev_codec_setMicVolume(100);
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
	if(ret == TLK_ENONE) tlkdev_codec_paOpen();
	return ret;
}
int tlkdev_codec_close(void)
{
	tlkdev_codec_paClose();
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
	if(ret == TLK_ENONE) tlkdev_codec_paOpen();
	return ret;
}

bool tlkdev_codec_paIsOpen(void)
{
	return tlkdrv_codec_paIsOpen();
}
void tlkdev_codec_paOpen(void)
{
	tlkdrv_codec_paOpen();
}
void tlkdev_codec_paClose(void)
{
	tlkdrv_codec_paClose();
}

void tlkdev_codec_muteSpk(void)
{
	tlkdrv_codec_muteSpk();
}

void tlkdev_codec_setSpkStatus(bool isMute)
{
	if(tlkdrv_codec_setSpkStatus(isMute) != TLK_ENONE){
		sTlkDevCodecCtrl.spkIsMute = true;
	}else{
		sTlkDevCodecCtrl.spkIsMute = false;
	}
}
void tlkdev_codec_setMicStatus(bool isMute)
{
	if(tlkdrv_codec_setMicStatus(isMute) != TLK_ENONE){
		sTlkDevCodecCtrl.micIsMute = true;
	}else{
		sTlkDevCodecCtrl.micIsMute = false;
	}
}

void tlkdev_codec_setSpkVolume(uint volume)
{
	if(volume > 100) volume = 100;
	if(tlkdrv_codec_setSpkVolume(volume) != TLK_ENONE){
		sTlkDevCodecCtrl.spkVolume = volume;
	}else{
		sTlkDevCodecCtrl.spkVolume = 100;
	}
	sTlkDevCodecCtrl.selSpkVolume = (volume<<7)/100;
	sTlkDevCodecCtrl.spkIsMute = false;
}
void tlkdev_codec_setMicVolume(uint volume)
{
	if(volume > 100) volume = 100;
	if(tlkdrv_codec_setMicVolume(volume) != TLK_ENONE){
		sTlkDevCodecCtrl.micVolume = volume;
	}else{
		sTlkDevCodecCtrl.micVolume = 100;
	}
	sTlkDevCodecCtrl.selMicVolume = (volume<<7)/100;
	sTlkDevCodecCtrl.micIsMute = false;
}

uint tlkdev_codec_getSpkVolume(void)
{
	int ret = tlkdrv_codec_getSpkVolume();
	if(ret >= 0) return (uint)ret;
	else return sTlkDevCodecCtrl.spkVolume;
}
uint tlkdev_codec_getMicVolume(void)
{
	int ret = tlkdrv_codec_getMicVolume();
	if(ret >= 0) return (uint)ret;
	else return sTlkDevCodecCtrl.micVolume;
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
	bool isSucc = tlkdrv_codec_readMicData(pBuff, buffLen, pOffset);
	if(isSucc){
		tlkdrv_codec_micDataZoom(pBuff, buffLen);
	}
	return isSucc;
}

void tlkdev_codec_muteSpkBuff(void)
{
	tlkdrv_codec_muteSpkBuff();
}
void tlkdev_codec_zeroSpkBuff(uint16 zeroLen, bool isInc)
{
	tlkdrv_codec_zeroSpkBuff(zeroLen, isInc);
}
bool tlkdev_codec_fillSpkBuff(uint08 *pData, uint16 dataLen)
{
	tlkdrv_codec_spkDataZoom(pData, dataLen);
	return tlkdrv_codec_fillSpkBuff(pData, dataLen);
}
bool tlkdev_codec_backReadSpkData(uint08 *pBuff, uint16 buffLen, uint16 offset, bool isBack)
{
	return tlkdrv_codec_backReadSpkData(pBuff, buffLen, offset, isBack);
}


static void tlkdrv_codec_micDataZoom(uint08 *pData, uint16 dataLen)
{
	if(pData == nullptr || dataLen == 0 || (dataLen & 0x01) != 0) return;
	if(sTlkDevCodecCtrl.micIsMute){
		tmemset(pData, 0, dataLen);
	}else if(sTlkDevCodecCtrl.micVolume != 100){
		uint16 index;
		for(index=0; index<(dataLen>>1); index++){
			pData[index] = ((uint32)pData[index] * sTlkDevCodecCtrl.selMicVolume) >> 7;
		}
	}
}
static void tlkdrv_codec_spkDataZoom(uint08 *pData, uint16 dataLen)
{
	if(pData == nullptr || dataLen == 0 || (dataLen & 0x01) != 0) return;
	if(sTlkDevCodecCtrl.spkIsMute){
		tmemset(pData, 0, dataLen);
	}else if(sTlkDevCodecCtrl.spkVolume != 100){
		uint16 index;
		for(index=0; index<(dataLen>>1); index++){
			pData[index] = ((uint32)pData[index] * sTlkDevCodecCtrl.selSpkVolume) >> 7;
		}
	}
}


#endif //#if (TLK_DEV_CODEC_ENABLE)

