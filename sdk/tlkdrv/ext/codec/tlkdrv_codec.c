/********************************************************************************************************
 * @file     tlkdrv_codec.c
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
#include "tlkdrv_rtl2108.h"
#include "drivers.h"


static const tlkdrv_codec_modinf_t *tlkdrv_codec_getDev(uint08 dev);


extern const tlkdrv_codec_modinf_t gcTlkDrvIcodecInf;
extern const tlkdrv_codec_modinf_t gcTlkDrvRtl2108Inf;
static const tlkdrv_codec_modinf_t *spTlkDrvCodecModinf[TLKDRV_CODEC_DEV_MAX] = {
	&gcTlkDrvIcodecInf,
	#if (TLKDRV_CODEC_RTL2108_ENABLE)
	&gcTlkDrvRtl2108Inf,
	#else
	nullptr,
	#endif
};
//static uint08 sTlkDrvCodecMajorDev = TLKDRV_CODEC_DEV_INNER;
//static uint08 sTlkDrvCodecMinorDev = TLKDRV_CODEC_SUBDEV_BOTH;
static uint08 sTlkDrvCodecSpkIsMute = true;
static uint08 sTlkDrvCodecMajorDev = 0xFF; //Unknown
static uint08 sTlkDrvCodecMinorDev = 0xFF; //Unknown
static uint08 sTlkDrvCodecMicVol = 100;
static uint08 sTlkDrvCodecSpkVol = 100;
static uint16 gTlkDrvCodecSpkOffset = 0;
static uint16 gTlkDrvCodecMicOffset = 0;
uint16  gTlkDrvCodecSpkBuffLen = 0;
uint16  gTlkDrvCodecMicBuffLen = 0;
uint08 *gpTlkDrvCodecSpkBuffer = nullptr;
uint08 *gpTlkDrvCodecMicBuffer = nullptr;


int tlkdrv_codec_mount(TLKDRV_CODEC_DEV_ENUM dev, TLKDRV_CODEC_SUBDEV_ENUM subDev)
{
	int ret;
	const tlkdrv_codec_modinf_t *pModInf;
	pModInf = tlkdrv_codec_getDev(dev);
	if(pModInf == nullptr || pModInf->Init == nullptr) return -TLK_ENOSUPPORT;
	ret = pModInf->Init(subDev);
	if(ret == TLK_ENONE){
		sTlkDrvCodecMajorDev = dev;
		sTlkDrvCodecMinorDev = subDev;
	}
	return ret;
}

bool tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_ENUM subDev)
{
	const tlkdrv_codec_modinf_t *pModInf;
	pModInf = tlkdrv_codec_getDev(sTlkDrvCodecMajorDev);
	if(pModInf == nullptr || pModInf->IsOpen == nullptr) return false;
	return pModInf->IsOpen(subDev);
}

int tlkdrv_codec_open(TLKDRV_CODEC_SUBDEV_ENUM subDev)
{
	int ret;
	const tlkdrv_codec_modinf_t *pModInf;

	tlkapi_sendData(TLKDRV_CODEC_DEBUG_ENABLE, "=== tlkdrv_codec_open 001", &sTlkDrvCodecMajorDev, 1);
	if(subDev == TLKDRV_CODEC_SUBDEV_DEF) subDev = sTlkDrvCodecMinorDev;
	pModInf = tlkdrv_codec_getDev(sTlkDrvCodecMajorDev);
	if(pModInf == nullptr || pModInf->Open == nullptr) return -TLK_ENOSUPPORT;
	ret = pModInf->Open(subDev);
	tlkapi_sendData(TLKDRV_CODEC_DEBUG_ENABLE, "=== tlkdrv_codec_open 002", 0, 0);
	if(ret == TLK_ENONE){
		gTlkDrvCodecSpkOffset = 0;
		gTlkDrvCodecMicOffset = 0;
		tlkdrv_codec_muteSpk();
	}
	return TLK_ENONE;
}
int tlkdrv_codec_pause(void)
{
	const tlkdrv_codec_modinf_t *pModInf;
	pModInf = tlkdrv_codec_getDev(sTlkDrvCodecMajorDev);
	if(pModInf == nullptr || pModInf->Pause == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Pause(sTlkDrvCodecMinorDev);
}
int tlkdrv_codec_close(void)
{
	const tlkdrv_codec_modinf_t *pModInf;
	tlkapi_sendData(TLKDRV_CODEC_DEBUG_ENABLE, "=== tlkdrv_codec_close 003", 0, 0);
	pModInf = tlkdrv_codec_getDev(sTlkDrvCodecMajorDev);
	if(pModInf == nullptr || pModInf->Close == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Close(sTlkDrvCodecMinorDev);
}
int tlkdrv_codec_reset(void)
{
	const tlkdrv_codec_modinf_t *pModInf;
	pModInf = tlkdrv_codec_getDev(sTlkDrvCodecMajorDev);
	if(pModInf == nullptr || pModInf->Reset == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Reset(sTlkDrvCodecMinorDev);
}
int tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_ENUM subDev, uint08 opcode, uint32 param0, uint32 param1)
{
	const tlkdrv_codec_modinf_t *pModInf;
	if(subDev == TLKDRV_CODEC_SUBDEV_DEF) subDev = sTlkDrvCodecMinorDev;
	pModInf = tlkdrv_codec_getDev(sTlkDrvCodecMajorDev);
	if(pModInf == nullptr || pModInf->Config == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Config(subDev, opcode, param0, param1);
}


int tlkdrv_codec_setMute(void)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_DEF, TLKDRV_CODEC_OPCODE_SET_MUTE, 0, 0);
}
int tlkdrv_codec_setMicMute(void)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_MIC, TLKDRV_CODEC_OPCODE_SET_MUTE, 0, 0);
}
int tlkdrv_codec_setSpkMute(void)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_SPK, TLKDRV_CODEC_OPCODE_SET_MUTE, 0, 0);
}

int tlkdrv_codec_setVolume(uint08 volume)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_DEF, TLKDRV_CODEC_OPCODE_SET_VOLUME, volume, 0);
}
int tlkdrv_codec_setMicVolume(uint08 volume)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_MIC, TLKDRV_CODEC_OPCODE_SET_VOLUME, volume, 0);
}
int tlkdrv_codec_setSpkVolume(uint08 volume)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_SPK, TLKDRV_CODEC_OPCODE_SET_VOLUME, volume, 0);
}

int tlkdrv_codec_setChannel(uint08 channel)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_DEF, TLKDRV_CODEC_OPCODE_SET_CHANNEL, channel, 0);
}
int tlkdrv_codec_setMicChannel(uint08 channel)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_MIC, TLKDRV_CODEC_OPCODE_SET_CHANNEL, channel, 0);
}
int tlkdrv_codec_setSpkChannel(uint08 channel)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_SPK, TLKDRV_CODEC_OPCODE_SET_CHANNEL, channel, 0);
}

int tlkdrv_codec_setBitDepth(uint08 bitDepth)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_DEF, TLKDRV_CODEC_OPCODE_SET_BIT_DEPTH, bitDepth, 0);
}
int tlkdrv_codec_setMicBitDepth(uint08 bitDepth)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_MIC, TLKDRV_CODEC_OPCODE_SET_BIT_DEPTH, bitDepth, 0);
}
int tlkdrv_codec_setSpkBitDepth(uint08 bitDepth)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_SPK, TLKDRV_CODEC_OPCODE_SET_BIT_DEPTH, bitDepth, 0);
}

int tlkdrv_codec_setSampleRate(uint32 sampleRate)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_DEF, TLKDRV_CODEC_OPCODE_SET_SAMPLE_RATE, sampleRate, 0);
}
int tlkdrv_codec_setMicSampleRate(uint32 sampleRate)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_MIC, TLKDRV_CODEC_OPCODE_SET_SAMPLE_RATE, sampleRate, 0);
}
int tlkdrv_codec_setSpkSampleRate(uint32 sampleRate)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_SPK, TLKDRV_CODEC_OPCODE_SET_SAMPLE_RATE, sampleRate, 0);
}


void tlkdrv_codec_muteSpk(void)
{
	if(sTlkDrvCodecSpkIsMute) return;
	if(gTlkDrvCodecSpkBuffLen != 0){
		tmemset(gpTlkDrvCodecSpkBuffer, 0, gTlkDrvCodecSpkBuffLen);
	}
	sTlkDrvCodecSpkIsMute = true;
}

int tlkdrv_codec_setSoftSpkVol(uint08 volume)
{
	if(volume > 100) return TLK_EPARAM;
	sTlkDrvCodecMicVol = volume;
	return TLK_ENONE;
}
int tlkdrv_codec_setSoftMicVol(uint08 volume)
{
	if(volume > 100) return TLK_EPARAM;
	sTlkDrvCodecSpkVol = volume;
	return TLK_ENONE;
}

void tlkdrv_codec_setSpkBuffer(uint08 *pBuffer, uint16 buffLen)
{
	if(buffLen != 0 && buffLen < 512) return;
	gTlkDrvCodecSpkBuffLen = buffLen;
	gpTlkDrvCodecSpkBuffer = pBuffer;
}
void tlkdrv_codec_setMicBuffer(uint08 *pBuffer, uint16 buffLen)
{
	if(buffLen != 0 && buffLen < 512) return;
	gTlkDrvCodecMicBuffLen = buffLen;
	gpTlkDrvCodecMicBuffer = pBuffer;
}

uint tlkdrv_codec_getSpkOffset(void)
{
	return gTlkDrvCodecSpkOffset;
}
uint tlkdrv_codec_getMicOffset(void)
{
	return gTlkDrvCodecMicOffset;
}
void tlkdrv_codec_setSpkOffset(uint16 offset)
{
	if(offset >= gTlkDrvCodecSpkBuffLen) return;
	gTlkDrvCodecSpkOffset = offset;
}
void tlkdrv_codec_setMicOffset(uint16 offset)
{
	if(offset >= gTlkDrvCodecMicBuffLen) return;
	gTlkDrvCodecMicOffset = offset;
}


uint tlkdrv_codec_getSpkIdleLen(void)
{
	uint16 unUsed;
	uint32 wptr;
	uint32 rptr;
	
	if(gTlkDrvCodecSpkBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_SPK)) return 0;
	
	wptr = gTlkDrvCodecSpkOffset;
	rptr = (audio_get_tx_dma_rptr(TLKDRV_CODEC_SPK_DMA))-((uint32)gpTlkDrvCodecSpkBuffer);
	
	if(rptr > wptr) unUsed = rptr-wptr;
	else unUsed = gTlkDrvCodecSpkBuffLen+rptr-wptr;

	if(unUsed+180 >= gTlkDrvCodecSpkBuffLen){
		unUsed -= 180;
		gTlkDrvCodecSpkOffset += 180;
		if(gTlkDrvCodecSpkOffset >= gTlkDrvCodecSpkBuffLen){
			gTlkDrvCodecSpkOffset -= gTlkDrvCodecSpkBuffLen;
		}
		tlkapi_sendData(TLKDRV_CODEC_DEBUG_ENABLE, "tlkdrv_codec_getSpkIdleLen: seek 180B", 0, 0);
	}
	
	return unUsed;
}
uint tlkdrv_codec_getSpkDataLen(void)
{
	uint16 used;
	uint32 wptr;
	uint32 rptr;

	if(gTlkDrvCodecSpkBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_SPK)) return 0;
		
	wptr = gTlkDrvCodecSpkOffset;
	rptr = (audio_get_tx_dma_rptr(TLKDRV_CODEC_SPK_DMA))-((uint32)gpTlkDrvCodecSpkBuffer);
	
	if(wptr > rptr) used = wptr-rptr;
	else used = gTlkDrvCodecSpkBuffLen+wptr-rptr;
	
	return used;
}
uint tlkdrv_codec_getMicDataLen(void)
{
	uint16 used;
	uint32 wptr;
	uint32 rptr;

	if(gTlkDrvCodecMicBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_MIC)) return 0;
	
	rptr = gTlkDrvCodecMicOffset;
	wptr = (audio_get_rx_dma_wptr(TLKDRV_CODEC_MIC_DMA))-((uint32)gpTlkDrvCodecMicBuffer);
	
	if(wptr > rptr) used = wptr-rptr;
	else used = gTlkDrvCodecMicBuffLen+wptr-rptr;
	
	return used;
}

bool tlkdrv_codec_readMicData(uint08 *pBuffer, uint16 buffLen, uint16 *pOffset)
{
	uint32 wptr;
	uint32 rptr;
	uint16 dataLen;
	uint16 tempLen;

	if(gTlkDrvCodecMicBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_MIC)) return false;

	rptr = gTlkDrvCodecMicOffset;
	wptr = (uint32)(audio_get_rx_dma_wptr(TLKDRV_CODEC_MIC_DMA) - (uint32)gpTlkDrvCodecMicBuffer);
	if(wptr >= gTlkDrvCodecMicBuffLen){
		wptr = 0;
		return false;
	}
	
	if(wptr >= rptr) dataLen = wptr-rptr;
	else dataLen = gTlkDrvCodecMicBuffLen+wptr-rptr;
	if(dataLen < buffLen) return false;
		
	if(rptr+buffLen <= gTlkDrvCodecMicBuffLen) tempLen = buffLen;
	else tempLen = gTlkDrvCodecMicBuffLen-rptr;
	if(tempLen != 0) tmemcpy(pBuffer, ((uint08*)gpTlkDrvCodecMicBuffer)+rptr, tempLen);
	if(tempLen == buffLen){
		rptr += tempLen;
	}else{
		rptr = buffLen-tempLen;
		tmemcpy(pBuffer+tempLen, ((uint08*)gpTlkDrvCodecMicBuffer), rptr);
	}
	if(pOffset != NULL) *pOffset = dataLen;
	if(rptr >= gTlkDrvCodecMicBuffLen) rptr = 0;
	gTlkDrvCodecMicOffset = rptr;
	return true;
}

//"dataLen == 0x0000":Fills all idle buffers; "dataLen == 0xFFFF":Fill all buffers.
void tlkdrv_codec_zeroSpkBuff(uint16 zeroLen, bool isInc)
{
	if(gTlkDrvCodecSpkBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_SPK)) return;
	if(zeroLen == 0xFFFF){
		tlkdrv_codec_muteSpk();
	}else{
		uint16 woffset;
		uint08 *pBuffer = (uint08*)gpTlkDrvCodecSpkBuffer;
		uint16 tempLen = tlkdrv_codec_getSpkIdleLen();
		if(zeroLen == 0 || zeroLen > tempLen) zeroLen = tempLen;
		if(zeroLen == 0 || zeroLen > gTlkDrvCodecSpkBuffLen) return;
		woffset = gTlkDrvCodecSpkOffset;
		if(woffset+zeroLen <= gTlkDrvCodecSpkBuffLen) tempLen = zeroLen;
		else tempLen = gTlkDrvCodecSpkBuffLen-woffset;
		if(tempLen != 0) tmemset(pBuffer+woffset, 0, tempLen);
		woffset += tempLen;
		zeroLen -= tempLen;
		if(zeroLen != 0){
			woffset = zeroLen;
			tmemset(pBuffer, 0, zeroLen);
		}
		if(isInc){
			gTlkDrvCodecSpkOffset = woffset;
		} 
	}
}

uint tlkdrv_codec_getSpkIdleLen0(void)
{
	uint32 wptr;
	uint32 rptr;
	uint16 unUsed;

	if(gTlkDrvCodecSpkBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_SPK)) return 0;
	
	wptr = gTlkDrvCodecSpkOffset;
	rptr = (audio_get_tx_dma_rptr(TLKDRV_CODEC_SPK_DMA))-((uint32)gpTlkDrvCodecSpkBuffer);
	
	if(rptr > wptr) unUsed = rptr-wptr;
	else unUsed = gTlkDrvCodecSpkBuffLen+rptr-wptr;

	return unUsed;
}

bool tlkdrv_codec_fillSpkBuff(uint08 *pData, uint16 dataLen)
{
	uint32 wptr;
	uint32 rptr;
	uint16 unUsed;
	uint16 offset;
	uint08 *pBuffer = (uint08*)gpTlkDrvCodecSpkBuffer;

	if(gTlkDrvCodecSpkBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_SPK)) return false;
	if(pData == NULL || dataLen == 0 || (dataLen & 0x01) != 0) return false;

	if(sTlkDrvCodecSpkIsMute) sTlkDrvCodecSpkIsMute = false;

	tlkapi_sendData(TLKDRV_CODEC_DEBUG_ENABLE, "=== spkFillBuff 03", 0, 0);
	wptr = gTlkDrvCodecSpkOffset;
	rptr = (audio_get_tx_dma_rptr(TLKDRV_CODEC_SPK_DMA))-((uint32)gpTlkDrvCodecSpkBuffer);
	

	if(rptr > wptr) unUsed = rptr-wptr;
	else unUsed = gTlkDrvCodecSpkBuffLen+rptr-wptr;

	tlkapi_sendU32s(TLKDRV_CODEC_DEBUG_ENABLE, "fill1:", rptr, wptr, unUsed, dataLen);

	if(unUsed <= dataLen){
		tlkapi_sendU32s(TLKDRV_CODEC_DEBUG_ENABLE, "fill1:", rptr, wptr, unUsed, dataLen);
		return false;
	}
	
	tlkapi_sendData(TLKDRV_CODEC_DEBUG_ENABLE, "=== spkFillBuff", 0, 0);
	tlkapi_sendData(TLKDRV_CODEC_DEBUG_ENABLE, "fill data: ok", 0, 0);

	if(unUsed == gTlkDrvCodecSpkBuffLen){
		wptr = rptr+32;
	}
	if(wptr >= gTlkDrvCodecSpkBuffLen){
		wptr = 0;
	}

	if(wptr+dataLen >  gTlkDrvCodecSpkBuffLen){
		offset = gTlkDrvCodecSpkBuffLen-wptr;
	}else{
		offset = dataLen;
	}
	memcpy(pBuffer+wptr, pData, offset);
	if(offset < dataLen){
		memcpy(pBuffer, pData+offset, dataLen-offset);
	}
	
	wptr += dataLen;
	if(wptr >= gTlkDrvCodecSpkBuffLen){
		wptr -= gTlkDrvCodecSpkBuffLen;
	}
	gTlkDrvCodecSpkOffset = wptr;
		
	return true;
}

bool tlkdrv_codec_backReadSpkData(uint08 *pBuffer, uint16 buffLen, uint16 offset, bool isBack)
{
	uint32 rptr;
	uint16 tempLen;

	if(gTlkDrvCodecSpkBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_SPK)) return false;
	if(buffLen == 0 || buffLen > gTlkDrvCodecSpkBuffLen || offset >= gTlkDrvCodecSpkBuffLen){
		tlkapi_sendData(TLKDRV_CODEC_DEBUG_ENABLE, "tlkdrv_codec_backReadSpkData: fault", 0, 0);
		return false;
	}
	rptr = (uint32)((audio_get_tx_dma_rptr(TLKDRV_CODEC_SPK_DMA))-((uint32)gpTlkDrvCodecSpkBuffer));
	if(offset != 0){
		if(!isBack){
			rptr += offset;
			if(rptr >= gTlkDrvCodecSpkBuffLen) rptr -= gTlkDrvCodecSpkBuffLen;
		}else{
			if(rptr >= offset) rptr -= offset;
			else rptr = gTlkDrvCodecSpkBuffLen+rptr-offset;
		}
	}
	if(rptr >= gTlkDrvCodecSpkBuffLen) rptr = 0;
//	if(rptr >= buffLen) rptr -= buffLen;
//	else rptr = gTlkDrvCodecSpkBuffLen+rptr-buffLen;
		
	if(rptr+buffLen <= gTlkDrvCodecSpkBuffLen) tempLen = buffLen;
	else tempLen = gTlkDrvCodecSpkBuffLen-rptr;
	if(tempLen != 0) tmemcpy(pBuffer, ((uint08*)(gpTlkDrvCodecSpkBuffer))+rptr, tempLen);
	if(tempLen < buffLen) tmemcpy(pBuffer+tempLen, (uint08*)gpTlkDrvCodecSpkBuffer, buffLen-tempLen);

//	tlkapi_sendU32s(TLKDRV_CODEC_DEBUG_ENABLE, "backRead:", offset, rptr, gTlkDrvCodecSpkOffset, tempLen);

	return true;
}



static const tlkdrv_codec_modinf_t *tlkdrv_codec_getDev(uint08 dev)
{
	if(dev >= TLKDRV_CODEC_DEV_MAX) return nullptr;
	return spTlkDrvCodecModinf[dev];
}

