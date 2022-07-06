/********************************************************************************************************
 * @file     tlkdev_spk.c
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
#include "tlkdev/sys/tlkdev_spk.h"




static uint16 sTlkDevSpkOffset = 0;
static bool sTlkDevSpkIsMute = true;

uint16 gTlkDevSpkBuffer[TLK_DEV_SPK_BUFF_SIZE/2];


int tlkdev_spk_init(void)
{
	sTlkDevSpkIsMute = false;
	tlkdev_spk_mute();
	
	return TLK_ENONE;
}

int tlkdev_spk_open(void)
{
	return TLK_ENONE;
}
void tlkdev_spk_close(void)
{
	
}

void tlkdev_spk_mute(void)
{
	if(sTlkDevSpkIsMute) return;
	tmemset(gTlkDevSpkBuffer, 0, sizeof(gTlkDevSpkBuffer));
	sTlkDevSpkIsMute = true;
}
bool tlkdev_spk_play(uint08 *pPcmData, uint16 dataLen)
{
	return tlkdev_spk_fillBuff(pPcmData, dataLen);
}


void tlkdev_spk_reset(void)
{
	sTlkDevSpkOffset  = audio_get_tx_dma_rptr(DMA3);
	sTlkDevSpkOffset -= ((unsigned int)gTlkDevSpkBuffer);

	sTlkDevSpkOffset += 640;
	if(sTlkDevSpkOffset >= TLK_DEV_SPK_BUFF_SIZE){
		sTlkDevSpkOffset -= TLK_DEV_SPK_BUFF_SIZE;
	}
}
uint32 tlkdev_spk_idleLen(void)
{
	uint16 unUsed;
	uint32 wptr;
	uint32 rptr;
	
	wptr = sTlkDevSpkOffset;
	rptr = (audio_get_tx_dma_rptr(DMA3))-((uint32)gTlkDevSpkBuffer);
	
	if(rptr > wptr) unUsed = rptr-wptr;
	else unUsed = TLK_DEV_SPK_BUFF_SIZE+rptr-wptr;

	return unUsed;
}
uint32 tlkdev_spk_dataLen(void)
{
	uint16 used;
	uint32 wptr;
	uint32 rptr;
	
	wptr = sTlkDevSpkOffset;
	rptr = (audio_get_tx_dma_rptr(DMA3))-((uint32)gTlkDevSpkBuffer);
	
	if(wptr > rptr) used = wptr-rptr;
	else used = TLK_DEV_SPK_BUFF_SIZE+wptr-rptr;
	
	return used;
}
//"dataLen == 0x0000":Fills all idle buffers; "dataLen == 0xFFFF":Fill all buffers.
void tlkdev_spk_fillZero(uint16 dataLen, bool isInc)
{
	if(dataLen == 0xFFFF){
		tlkdev_spk_mute();
	}else{
		uint16 woffset;
		uint08 *pBuffer = (uint08*)gTlkDevSpkBuffer;
		uint16 tempLen = tlkdev_spk_idleLen();
		if(dataLen == 0 || dataLen > tempLen) dataLen = tempLen;
		if(dataLen == 0 || dataLen > TLK_DEV_SPK_BUFF_SIZE) return;
		woffset = sTlkDevSpkOffset;
		if(woffset+dataLen <= TLK_DEV_SPK_BUFF_SIZE) tempLen = dataLen;
		else tempLen = TLK_DEV_SPK_BUFF_SIZE-woffset;
		if(tempLen != 0) tmemset(pBuffer+woffset, 0, tempLen);
		woffset += tempLen;
		dataLen -= tempLen;
		if(dataLen != 0){
			woffset = dataLen;
			tmemset(pBuffer, 0, dataLen);
		}
		if(isInc){
			sTlkDevSpkOffset = woffset;
		} 
	}
}
bool tlkdev_spk_fillBuff(uint08 *pData, uint16 dataLen)
{
	uint32 wptr;
	uint32 rptr;
	uint16 unUsed;
	uint16 offset;
	uint08 *pBuffer = (uint08*)gTlkDevSpkBuffer;
	
	if(pData == NULL || dataLen == 0 || (dataLen & 0x01)) return true;

	if(sTlkDevSpkIsMute) sTlkDevSpkIsMute = false;

	wptr = sTlkDevSpkOffset;
	rptr = (audio_get_tx_dma_rptr(DMA3))-((uint32)gTlkDevSpkBuffer);
	

	if(rptr > wptr) unUsed = rptr-wptr;
	else unUsed = TLK_DEV_SPK_BUFF_SIZE+rptr-wptr;

//	my_dump_str_u32s(DUMP_APP_MSG, "fill1:", rptr, wptr, unUsed, dataLen);

	if(unUsed <= dataLen){
//		my_dump_str_data(DUMP_APP_MSG, "fill data: fail", 0, 0);
		return false;
	}

//	my_dump_str_data(DUMP_APP_MSG, "=== spkFillBuff", 0, 0);

//	my_dump_str_data(DUMP_APP_MSG, "fill data: ok", 0, 0);

	if(unUsed == TLK_DEV_SPK_BUFF_SIZE){
		wptr = rptr+32;
	}
	if(wptr >= TLK_DEV_SPK_BUFF_SIZE){
		wptr = 0;
	}

	if(wptr+dataLen >  TLK_DEV_SPK_BUFF_SIZE){
		offset = TLK_DEV_SPK_BUFF_SIZE-wptr;
	}else{
		offset = dataLen;
	}
	memcpy(pBuffer+wptr, pData, offset);
	if(offset < dataLen){
		memcpy(pBuffer, pData+offset, dataLen-offset);
	}
	
	wptr += dataLen;
	if(wptr >= TLK_DEV_SPK_BUFF_SIZE){
		wptr -= TLK_DEV_SPK_BUFF_SIZE;
	}
	sTlkDevSpkOffset = wptr;
		
	return true;
}

void tlkdev_spk_backReadData(uint08 *pBuffer, uint16 buffLen, uint16 offset, bool isBack)
{
	uint32 rptr;
	uint16 tempLen;

	if(buffLen == 0 || buffLen > TLK_DEV_SPK_BUFF_SIZE || offset >= TLK_DEV_SPK_BUFF_SIZE){
//		my_dump_str_data(DUMP_APP_MSG, "app_audio_backReadSpkData: fault", 0, 0);
		return;
	}
	rptr = (uint32)((audio_get_tx_dma_rptr(DMA3))-((uint32)gTlkDevSpkBuffer));
	if(offset != 0){
		if(!isBack){
			rptr += offset;
			if(rptr >= TLK_DEV_SPK_BUFF_SIZE) rptr -= TLK_DEV_SPK_BUFF_SIZE;
		}else{
			if(rptr >= offset) rptr -= offset;
			else rptr = TLK_DEV_SPK_BUFF_SIZE+rptr-offset;
		}
	}
	if(rptr >= TLK_DEV_SPK_BUFF_SIZE) rptr = 0;
//	if(rptr >= buffLen) rptr -= buffLen;
//	else rptr = TLK_DEV_SPK_BUFF_SIZE+rptr-buffLen;
		
	if(rptr+buffLen <= TLK_DEV_SPK_BUFF_SIZE) tempLen = buffLen;
	else tempLen = TLK_DEV_SPK_BUFF_SIZE-rptr;
	if(tempLen != 0) tmemcpy(pBuffer, ((uint08*)(gTlkDevSpkBuffer))+rptr, tempLen);
	if(tempLen < buffLen) tmemcpy(pBuffer+tempLen, (uint08*)gTlkDevSpkBuffer, buffLen-tempLen);

//	my_dump_str_u32s(DUMP_APP_MSG, "backRead:", offset, rptr, sAppAudioSpkOffset, tempLen);
}




