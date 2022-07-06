/********************************************************************************************************
 * @file     tlkalg_ec.c
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

#if (TLK_ALG_EC_ENABLE)

#include "tlkalg_ec.h"

#include "tlkalg/audio/aec/tlkalg_aec_ns.h"
#include "tlkalg/audio/agc/tlkalg_agc.h"


static SpeexPreprocessState *sTlkAlgEcNsCtrl = nullptr;
static SpeexEchoState       *sTlkAlgEcAecCtrl = nullptr;

static short sTlkAlgEcOutBuffer[256];



void tlkalg_ec_init(uint08 *pSpeexNs, uint08 *pAecm)
{
	sTlkAlgEcNsCtrl  = (SpeexPreprocessState*)pSpeexNs;
	sTlkAlgEcAecCtrl = (SpeexEchoState*)pAecm;

	if(sTlkAlgEcAecCtrl != nullptr){
		AEC_CFG_PARAS aecParas;
		aecParas.sampling_rate = 16000;
		aecParas.use_dc_notch = 1;
		aecParas.use_pre_emp = 1;
		aec_init(sTlkAlgEcAecCtrl, &aecParas, 120, 120);
	}
	if(sTlkAlgEcNsCtrl != nullptr){
		NS_CFG_PARAS nsParas;
		nsParas.noise_suppress_default = -15;
		nsParas.echo_suppress_default = -55;
		nsParas.echo_suppress_active_default = -45;
		nsParas.low_shelf_enable = 0;
		nsParas.ns_smoothness = 27853;      // QCONST16(0.85f,15)
		nsParas.ns_threshold_low = 100000.0f;
		ns_init(sTlkAlgEcNsCtrl, &nsParas, SPEEX_FRAME_SIZE, 16000);
		ns_set_parameter(sTlkAlgEcNsCtrl, SPEEX_PREPROCESS_SET_ECHO_STATE, sTlkAlgEcAecCtrl);
	}
}

short *tlkalg_ec_frame(uint08 *pMicData, uint08 *pSpkData)
{
	if(sTlkAlgEcAecCtrl != nullptr){
		aec_process_frame(sTlkAlgEcAecCtrl, (const spx_int16_t*)pMicData, (const spx_int16_t*)pSpkData, (spx_int16_t*)sTlkAlgEcOutBuffer);
	}
	if(sTlkAlgEcNsCtrl != nullptr){
		ns_process_frame(sTlkAlgEcNsCtrl, sTlkAlgEcOutBuffer);
	}
	return sTlkAlgEcOutBuffer;
}


#endif //#if (TLK_ALG_EC_ENABLE)

