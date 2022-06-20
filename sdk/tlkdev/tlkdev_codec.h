/********************************************************************************************************
 * @file     tlkdev_codec.h
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

#ifndef TLKDEV_CODEC_H
#define TLKDEV_CODEC_H


//Select
typedef enum{
	TLKDEV_CODEC_SELC_INNER = 0,
	TLKDEV_CODEC_SELC_OUTER = 1,
}TLKDEV_CODEC_SEL_ENUM;

typedef enum{
	TLKDEV_CODEC_MODE_SINGLE = 0,
	TLKDEV_CODEC_MODE_DOUBLE = 1,
}TLKDEV_CODEC_MODE_ENUM;


typedef struct{
	uint08 sel; //Refer to TLKDEV_CODEC_SEL_ENUM.
	uint08 mode; //Refer to TLKDEV_CODEC_MODE_ENUM.
	uint08 isInit;
	uint08 reserve;
	uint32 sampleRate;
}tlkdev_codec_t;




int tlkdev_codec_init(uint08 mode, uint08 codecSel);
int tlkdev_codec_init2(uint08 mode, uint08 codecSel);

void tlkdev_codec_reset(void);
void tlkdev_codec_setSampleRate(uint32 sampleRate);


#endif //TLKDEV_CODEC_H

