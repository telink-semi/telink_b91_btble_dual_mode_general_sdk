/********************************************************************************************************
 * @file     tlkapi_mem.h
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

#ifndef TLKAPI_MEM_H
#define TLKAPI_MEM_H


#if (TLKMDI_CFG_REUSE_SPKBUFF_ENABLE)
#define TLKAPI_MEM_TOTAL_SIZE         (37*1024) //MP3(36740) = 36740 -- Worst scenario: Music playing on the headphone
#else
#define TLKAPI_MEM_TOTAL_SIZE         (40*1024) //MP3(36740)+SrcEnc(4048) = 36740+4048=40788  -- Worst scenario: Music playing on the headphone
#endif

typedef ulong tlkapi_mem_t;


int tlkapi_setSysMemBuffer(bool osIsEn, uint08 *pBuffer, uint16 buffLen);

void *tlkapi_malloc(uint32 size);
void *tlkapi_calloc(uint32 size);
void *tlkapi_realloc(void *ptr, uint32 size);
void  tlkapi_free(void *ptr);
void  tlkapi_printMem(void);


tlkapi_mem_t tlkapi_mem_init(bool osIsEn, uint08 *pBuffer, uint32 buffLen);
void tlkapi_mem_deinit(tlkapi_mem_t mem);

void tlkapi_mem_clean(tlkapi_mem_t mem);
void tlkapi_mem_print(tlkapi_mem_t mem);

void *tlkapi_mem_malloc(tlkapi_mem_t mem, uint32 size);
void *tlkapi_mem_calloc(tlkapi_mem_t mem, uint32 size);
void *tlkapi_mem_realloc(tlkapi_mem_t mem, void *ptr, uint32 size);
int   tlkapi_mem_free(tlkapi_mem_t mem, void *ptr);



#endif //TLKAPI_MEM_H

