/********************************************************************************************************
 * @file     tlkdev_mic.c
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
#include "tlkdev/sys/tlkdev_mic.h"

#include "drivers.h"


extern short *tlkalg_ec_frame(void);

static uint16 sTlkDevMicROffset = 0;
uint16 gTlkDevMicBuffer[TLK_DEV_MIC_BUFF_SIZE/2]; 	



int tlkdev_mic_init(void)
{
	
	return TLK_ENONE;
}
int tlkdev_mic_open(void)
{
	return TLK_ENONE;
}
void tlkdev_mic_close(void)
{
	
}


void tlkdev_mic_reset(void)
{
	sTlkDevMicROffset = 0;
}




bool tlkdev_mic_readData(uint08 *pBuffer, uint16 buffLen, uint16 *pOffset)
{
	uint32 wptr;
	uint32 rptr;
	uint16 dataLen;
	uint16 tempLen;

	rptr = sTlkDevMicROffset;
//	wptr = (uint32)(reg_dma_dst_addr(DMA2) - (uint32)gTlkDevMicBuffer);
	wptr = (uint32)(audio_get_rx_dma_wptr(DMA2) - (uint32)gTlkDevMicBuffer);
	if(wptr >= TLK_DEV_MIC_BUFF_SIZE){
		wptr = 0;
//		my_dump_str_data(DUMP_APP_MSG, "app_audio_readMicData: fault", 0, 0);
		return false;
	}
	
	if(wptr >= rptr) dataLen = wptr-rptr;
	else dataLen = TLK_DEV_MIC_BUFF_SIZE+wptr-rptr;
	if(dataLen < buffLen) return false;
		
//	my_dump_str_u32s(DUMP_APP_MSG, "readData0:", dataLen, wptr, rptr, sTlkDevMicROffset);
	
	if(rptr+buffLen <= TLK_DEV_MIC_BUFF_SIZE) tempLen = buffLen;
	else tempLen = TLK_DEV_MIC_BUFF_SIZE-rptr;
	if(tempLen != 0) tmemcpy(pBuffer, ((uint08*)gTlkDevMicBuffer)+rptr, tempLen);
	if(tempLen == buffLen){
		rptr += tempLen;
	}else{
		rptr = buffLen-tempLen;
		tmemcpy(pBuffer+tempLen, ((uint08*)gTlkDevMicBuffer), rptr);
	}
	if(pOffset != NULL) *pOffset = dataLen;
	if(rptr >= TLK_DEV_MIC_BUFF_SIZE) rptr = 0;
//	my_dump_str_u32s(DUMP_APP_MSG, "readData1:", dataLen, wptr, rptr, sTlkDevMicROffset);
	sTlkDevMicROffset = rptr;
	return true;
}


