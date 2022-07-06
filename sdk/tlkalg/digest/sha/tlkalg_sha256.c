/********************************************************************************************************
 * @file     tlkalg_sha256.c
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
#include "tlkalg/digest/sha/tlkalg_sha256.h"


#define Sha256Ror(value, bits) (((value) >> (bits)) | ((value) << (32 - (bits))))
#define Sha256Ch(x, y, z)      (z ^ (x & (y ^ z)))
#define Sha256Maj(x, y, z)     (((x | y) & z) | (x & y))
#define Sha256S(x, n)          Sha256Ror((x),(n))
#define Sha256R(x, n)          (((x)&0xFFFFFFFFUL)>>(n))
#define Sha256Sigma0(x)        (Sha256S(x, 2) ^ Sha256S(x, 13) ^ Sha256S(x, 22))
#define Sha256Sigma1(x)        (Sha256S(x, 6) ^ Sha256S(x, 11) ^ Sha256S(x, 25))
#define Sha256Gamma0(x)        (Sha256S(x, 7) ^ Sha256S(x, 18) ^ Sha256R(x, 3))
#define Sha256Gamma1(x)        (Sha256S(x, 17) ^ Sha256S(x, 19) ^ Sha256R(x, 10))

static const uint32 scSha256Key[64] = {
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL,
    0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL,
    0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL,
    0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
    0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL,
    0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,
    0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL,
    0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL,
    0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL,
    0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
    0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
    0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};



void tlkalg_sha256_init(tlkalg_sha256_contex_t *pContext)
{
	pContext->curlen = 0;
    pContext->length = 0;
    pContext->state[0] = 0x6A09E667UL;
    pContext->state[1] = 0xBB67AE85UL;
    pContext->state[2] = 0x3C6EF372UL;
    pContext->state[3] = 0xA54FF53AUL;
    pContext->state[4] = 0x510E527FUL;
    pContext->state[5] = 0x9B05688CUL;
    pContext->state[6] = 0x1F83D9ABUL;
    pContext->state[7] = 0x5BE0CD19UL;
}
void tlkalg_sha256_transform(tlkalg_sha256_contex_t *pContext, uint08 *pData)
{
	int i;
	uint32 S[8];
    uint32 W[64];
    uint32 t0;
    uint32 t1;
    uint32 t;

    // Copy state into S
    for(i=0; i<8; i++){
        S[i] = pContext->state[i];
    }
	
    // Copy the state into 512-bits into W[0..15]
    for(i=0; i<16; i++){
		ARRAY_TO_UINT32H(pData, 4*i, W[i]);
    }

    // Fill W[16..63]
    for( i=16; i<64; i++ ){
        W[i] = Sha256Gamma1( W[i-2]) + W[i-7] + Sha256Gamma0( W[i-15] ) + W[i-16];
    }

    // Compress
    for(i=0; i<64; i++){
		t0 = S[7] + Sha256Sigma1(S[4]) + Sha256Ch(S[4], S[5], S[6]) + scSha256Key[i] + W[i];
		t1 = Sha256Sigma0(S[0]) + Sha256Maj(S[0], S[1], S[2]);
		S[3] += t0;
		S[7]  = t0 + t1;

		
        t = S[7];
        S[7] = S[6];
        S[6] = S[5];
        S[5] = S[4];
        S[4] = S[3];
        S[3] = S[2];
        S[2] = S[1];
        S[1] = S[0];
        S[0] = t;
    }

    // Feedback
    for(i=0; i<8; i++){
        pContext->state[i] = pContext->state[i] + S[i];
    }
}
void tlkalg_sha256_update(tlkalg_sha256_contex_t *pContext, uint08 *pData, uint16 dataLen)
{
	uint32 n;

	if(pContext->curlen > TLKALG_SHA256_BLOCK_SIZE) return;

    while(dataLen > 0){
        if(pContext->curlen == 0 && dataLen >= TLKALG_SHA256_BLOCK_SIZE){
           tlkalg_sha256_transform(pContext, (uint08*)pData);
           pContext->length += TLKALG_SHA256_BLOCK_SIZE * 8;
           pData = (uint08*)pData + TLKALG_SHA256_BLOCK_SIZE;
           dataLen -= TLKALG_SHA256_BLOCK_SIZE;
        }else{
		   if(dataLen < (TLKALG_SHA256_BLOCK_SIZE - pContext->curlen)) n = dataLen;
		   else n = (TLKALG_SHA256_BLOCK_SIZE - pContext->curlen);
           tmemcpy(pContext->buff + pContext->curlen, pData, n);
           pContext->curlen += n;
           pData = (uint08*)pData + n;
           dataLen -= n;
           if(pContext->curlen == TLKALG_SHA256_BLOCK_SIZE){
              tlkalg_sha256_transform(pContext, pContext->buff);
              pContext->length += 8*TLKALG_SHA256_BLOCK_SIZE;
              pContext->curlen = 0;
           }
       }
    }
}
void tlkalg_sha256_finish(tlkalg_sha256_contex_t *pContext, tlkalg_sha256_digest_t *pDigest)
{
	int i;

    if(pContext->curlen >= TLKALG_SHA256_BLOCK_SIZE) return;

    // Increase the length of the message
    pContext->length += pContext->curlen * 8;

    // Append the '1' bit
    pContext->buff[pContext->curlen++] = (uint08)0x80;

    // if the length is currently above 56 bytes we append zeros
    // then compress.  Then we can fall back to padding zeros and length
    // encoding like normal.
    if(pContext->curlen > 56 ){
        while(pContext->curlen < 64){
            pContext->buff[pContext->curlen++] = (uint08)0;
        }
        tlkalg_sha256_transform(pContext, pContext->buff);
        pContext->curlen = 0;
    }

    // Pad up to 56 bytes of zeroes
    while(pContext->curlen < 56){
        pContext->buff[pContext->curlen++] = (uint08)0;
    }

    // Store length
	ARRAY_TO_UINT32H(pContext->buff, 56, pContext->length);
    tlkalg_sha256_transform(pContext, pContext->buff);

    // Copy output
    for(i=0; i<8; i++){
		UINT32H_TO_ARRAY(pContext->state[i], pDigest->value, 4*i);
    }
}
void tlkalg_sha256_result(uint08 *pData, uint16 dataLen, tlkalg_sha256_digest_t *pDigest)
{
	tlkalg_sha256_contex_t context;
	tlkalg_sha256_init(&context);
	tlkalg_sha256_update(&context, pData, dataLen);
	tlkalg_sha256_finish(&context, pDigest);
}



