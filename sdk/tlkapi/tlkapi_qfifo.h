/********************************************************************************************************
 * @file     tlkapi_qfifo.h
 *
 * @brief    This is the header file for BTBLE SDK
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

#ifndef TLKAPI_QFIFO_H
#define TLKAPI_QFIFO_H



#define tlkapi_qfifo_size(pFifo)               ((pFifo)->size)
#define tlkapi_qfifo_count(pFifo)              ((pFifo)->numb)
#define tlkapi_qfifo_isFull(pFifo)             ((pFifo)->full || (pFifo)->numb == 0)
#define tlkapi_qfifo_isEmpty(pFifo)            (!(pFifo)->full && (pFifo)->wptr == (pFifo)->rptr)



// must align for 4-bytes, size*count<0xFFFF
typedef struct{
	uint08 full;
	uint16 numb;
	uint16 size;
	uint16 wptr;
	uint16 rptr;
	uint08 *pBuff;
}tlkapi_qfifo_t;



int  tlkapi_qfifo_init(tlkapi_qfifo_t *pFifo, uint16 numb, uint16 size, uint08 *pBuffer, uint32 buffLen);

void tlkapi_qfifo_reset(tlkapi_qfifo_t *pFifo);
void tlkapi_qfifo_clear(tlkapi_qfifo_t *pFifo);

void tlkapi_qfifo_dropBuff(tlkapi_qfifo_t *pFifo);
void tlkapi_qfifo_dropData(tlkapi_qfifo_t *pFifo);

uint16 tlkapi_qfifo_usedNum(tlkapi_qfifo_t *pFifo);
uint16 tlkapi_qfifo_idleNum(tlkapi_qfifo_t *pFifo);

uint08 *tlkapi_qfifo_getBuff(tlkapi_qfifo_t *pFifo);
uint08 *tlkapi_qfifo_getData(tlkapi_qfifo_t *pFifo);

uint08 *tlkapi_qfifo_takeBuff(tlkapi_qfifo_t *pFifo);
uint08 *tlkapi_qfifo_takeData(tlkapi_qfifo_t *pFifo);





#endif //TLKAPI_QFIFO_H

