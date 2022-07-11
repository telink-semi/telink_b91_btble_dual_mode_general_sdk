/********************************************************************************************************
 * @file     tlkalg_sha1.c
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
#include "tlkalg/digest/sha/tlkalg_sha1.h"



typedef union{
    uint08 c[64];
    uint32 l[16];
}tlkalg_sha1_char64ToLong16_t;


#define TlkAlgSha1Rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))
#define TlkAlgSha1Blk0(i) (block->l[i] = (TlkAlgSha1Rol(block->l[i],24)&0xFF00FF00) | (TlkAlgSha1Rol(block->l[i],8)&0x00FF00FF))
#define TlkAlgSha1Blk(i)  (block->l[i&15] = TlkAlgSha1Rol(block->l[(i+13)&15] ^ block->l[(i+8)&15] ^ block->l[(i+2)&15] ^ block->l[i&15],1))

// (R0+R1), R2, R3, R4 are the different operations used in SHA1
#define TlkAlgSha1R0(v,w,x,y,z,i)  z += ((w&(x^y))^y) + TlkAlgSha1Blk0(i)+ 0x5A827999 + TlkAlgSha1Rol(v,5); w=TlkAlgSha1Rol(w,30);
#define TlkAlgSha1R1(v,w,x,y,z,i)  z += ((w&(x^y))^y) + TlkAlgSha1Blk(i) + 0x5A827999 + TlkAlgSha1Rol(v,5); w=TlkAlgSha1Rol(w,30);
#define TlkAlgSha1R2(v,w,x,y,z,i)  z += (w^x^y) + TlkAlgSha1Blk(i) + 0x6ED9EBA1 + TlkAlgSha1Rol(v,5); w=TlkAlgSha1Rol(w,30);
#define TlkAlgSha1R3(v,w,x,y,z,i)  z += (((w|x)&y)|(w&x)) + TlkAlgSha1Blk(i) + 0x8F1BBCDC + TlkAlgSha1Rol(v,5); w=TlkAlgSha1Rol(w,30);
#define TlkAlgSha1R4(v,w,x,y,z,i)  z += (w^x^y) + TlkAlgSha1Blk(i) + 0xCA62C1D6 + TlkAlgSha1Rol(v,5); w=TlkAlgSha1Rol(w,30);



void tlkalg_sha1_init(tlkalg_sha1_contex_t *pContext)
{
	pContext->state[0] = 0x67452301;
    pContext->state[1] = 0xEFCDAB89;
    pContext->state[2] = 0x98BADCFE;
    pContext->state[3] = 0x10325476;
    pContext->state[4] = 0xC3D2E1F0;
    pContext->count[0] = 0;
    pContext->count[1] = 0;
}
void tlkalg_sha1_transform(uint32 state[5], uint08 buffer[64])
{
	uint32 a;
    uint32 b;
    uint32 c;
    uint32 d;
    uint32 e;
    uint08 workspace[64];
    tlkalg_sha1_char64ToLong16_t* block = (tlkalg_sha1_char64ToLong16_t*) workspace;

	tmemcpy(block->l, buffer, 64);

    // Copy context->state[] to working vars
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    // 4 rounds of 20 operations each. Loop unrolled.
    TlkAlgSha1R0(a,b,c,d,e, 0); TlkAlgSha1R0(e,a,b,c,d, 1); TlkAlgSha1R0(d,e,a,b,c, 2); TlkAlgSha1R0(c,d,e,a,b, 3);
    TlkAlgSha1R0(b,c,d,e,a, 4); TlkAlgSha1R0(a,b,c,d,e, 5); TlkAlgSha1R0(e,a,b,c,d, 6); TlkAlgSha1R0(d,e,a,b,c, 7);
    TlkAlgSha1R0(c,d,e,a,b, 8); TlkAlgSha1R0(b,c,d,e,a, 9); TlkAlgSha1R0(a,b,c,d,e,10); TlkAlgSha1R0(e,a,b,c,d,11);
    TlkAlgSha1R0(d,e,a,b,c,12); TlkAlgSha1R0(c,d,e,a,b,13); TlkAlgSha1R0(b,c,d,e,a,14); TlkAlgSha1R0(a,b,c,d,e,15);
    TlkAlgSha1R1(e,a,b,c,d,16); TlkAlgSha1R1(d,e,a,b,c,17); TlkAlgSha1R1(c,d,e,a,b,18); TlkAlgSha1R1(b,c,d,e,a,19);
    TlkAlgSha1R2(a,b,c,d,e,20); TlkAlgSha1R2(e,a,b,c,d,21); TlkAlgSha1R2(d,e,a,b,c,22); TlkAlgSha1R2(c,d,e,a,b,23);
    TlkAlgSha1R2(b,c,d,e,a,24); TlkAlgSha1R2(a,b,c,d,e,25); TlkAlgSha1R2(e,a,b,c,d,26); TlkAlgSha1R2(d,e,a,b,c,27);
    TlkAlgSha1R2(c,d,e,a,b,28); TlkAlgSha1R2(b,c,d,e,a,29); TlkAlgSha1R2(a,b,c,d,e,30); TlkAlgSha1R2(e,a,b,c,d,31);
    TlkAlgSha1R2(d,e,a,b,c,32); TlkAlgSha1R2(c,d,e,a,b,33); TlkAlgSha1R2(b,c,d,e,a,34); TlkAlgSha1R2(a,b,c,d,e,35);
    TlkAlgSha1R2(e,a,b,c,d,36); TlkAlgSha1R2(d,e,a,b,c,37); TlkAlgSha1R2(c,d,e,a,b,38); TlkAlgSha1R2(b,c,d,e,a,39);
    TlkAlgSha1R3(a,b,c,d,e,40); TlkAlgSha1R3(e,a,b,c,d,41); TlkAlgSha1R3(d,e,a,b,c,42); TlkAlgSha1R3(c,d,e,a,b,43);
    TlkAlgSha1R3(b,c,d,e,a,44); TlkAlgSha1R3(a,b,c,d,e,45); TlkAlgSha1R3(e,a,b,c,d,46); TlkAlgSha1R3(d,e,a,b,c,47);
    TlkAlgSha1R3(c,d,e,a,b,48); TlkAlgSha1R3(b,c,d,e,a,49); TlkAlgSha1R3(a,b,c,d,e,50); TlkAlgSha1R3(e,a,b,c,d,51);
    TlkAlgSha1R3(d,e,a,b,c,52); TlkAlgSha1R3(c,d,e,a,b,53); TlkAlgSha1R3(b,c,d,e,a,54); TlkAlgSha1R3(a,b,c,d,e,55);
    TlkAlgSha1R3(e,a,b,c,d,56); TlkAlgSha1R3(d,e,a,b,c,57); TlkAlgSha1R3(c,d,e,a,b,58); TlkAlgSha1R3(b,c,d,e,a,59);
    TlkAlgSha1R4(a,b,c,d,e,60); TlkAlgSha1R4(e,a,b,c,d,61); TlkAlgSha1R4(d,e,a,b,c,62); TlkAlgSha1R4(c,d,e,a,b,63);
    TlkAlgSha1R4(b,c,d,e,a,64); TlkAlgSha1R4(a,b,c,d,e,65); TlkAlgSha1R4(e,a,b,c,d,66); TlkAlgSha1R4(d,e,a,b,c,67);
    TlkAlgSha1R4(c,d,e,a,b,68); TlkAlgSha1R4(b,c,d,e,a,69); TlkAlgSha1R4(a,b,c,d,e,70); TlkAlgSha1R4(e,a,b,c,d,71);
    TlkAlgSha1R4(d,e,a,b,c,72); TlkAlgSha1R4(c,d,e,a,b,73); TlkAlgSha1R4(b,c,d,e,a,74); TlkAlgSha1R4(a,b,c,d,e,75);
    TlkAlgSha1R4(e,a,b,c,d,76); TlkAlgSha1R4(d,e,a,b,c,77); TlkAlgSha1R4(c,d,e,a,b,78); TlkAlgSha1R4(b,c,d,e,a,79);

    // Add the working vars back into context.state[]
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}
void tlkalg_sha1_update(tlkalg_sha1_contex_t *pContext, uint08 *pData, uint32 dataLen)
{
	uint32 i;
    uint32 j;

    j = (pContext->count[0] >> 3) & 63;
    if((pContext->count[0] += dataLen << 3) < (dataLen << 3)){
        pContext->count[1]++;
    }

    pContext->count[1] += (dataLen >> 29);
    if((j + dataLen) > 63){
        i = 64 - j;
        tmemcpy(&pContext->buff[j], pData, i);
        tlkalg_sha1_transform(pContext->state, pContext->buff);
        for(; i + 63 < dataLen; i += 64){
            tlkalg_sha1_transform(pContext->state, (uint08*)pData + i);
        }
        j = 0;
    }else{
        i = 0;
    }

    tmemcpy(&pContext->buff[j], &pData[i], dataLen - i);
}
void tlkalg_sha1_finish(tlkalg_sha1_contex_t *pContext, tlkalg_sha1_digest_t *pDigest)
{
	uint32 i;
    uint08 finalcount[8];

    for(i=0; i<8; i++){
        finalcount[i] = (uint08)((pContext->count[(i >= 4 ? 0 : 1)]
			>> ((3-(i & 3)) * 8) ) & 255);  // Endian independent
    }
    tlkalg_sha1_update(pContext, (uint08*)"\x80", 1);
    while((pContext->count[0] & 504) != 448){
        tlkalg_sha1_update(pContext, (uint08*)"\0", 1);
    }

    tlkalg_sha1_update(pContext, finalcount, 8);  // Should cause a Sha1TransformFunction()
    for( i=0; i<TLKALG_SHA1_HASH_SIZE; i++ ){
        pDigest->value[i] = (uint08)((pContext->state[i>>2] >> ((3-(i & 3)) * 8)) & 255);
    }
}
void tlkalg_sha1_result(uint08 *pData, uint32 dataLen, tlkalg_sha1_digest_t *pDigest)
{
	tlkalg_sha1_contex_t contex;
	tlkalg_sha1_init(&contex);
	tlkalg_sha1_update(&contex, pData, dataLen);
	tlkalg_sha1_finish(&contex, pDigest);
}

