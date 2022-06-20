/********************************************************************************************************
 * @file     tlkalg_agc.h
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

#pragma once

#if TLK_ALG_AGC_ENABLE



extern void my_agc_init(void* agcInst,
    short targetLevelDbfs,			/// AGC TARGET,This setting value N means the actual output -N (dB) signal
									/// To correctly set this parameter, the user must take into account a margin to avoid signal saturation.
									/// range(dB):0~10            default(dB) :3
    short compressionGaindB,		/// the AGC gain,This setting value M means that the actual AGC gain is M (dB)
									/// range(dB):0~40            default(dB) :9
    unsigned int fs);

extern int my_agc_process(
    void* agcInst,
    const short* const* in_near,
	unsigned int samples,					///80:5ms,120:7.5ms,160:10ms
    short* const* out
);

#endif
