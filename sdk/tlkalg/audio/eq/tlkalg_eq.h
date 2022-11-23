/********************************************************************************************************
 * @file     tlkalg_eq.h
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
#ifndef TLKALG_EQ_H_
#define TLKALG_EQ_H_

#if 1//(TLK_ALG_EQ_ENABLE)


#define TLKALG_EQ_DEBUG_ENABLE			1



typedef enum{
	TLKALG_EQ_SAMPLE_RATE_48K    = 0,
	TLKALG_EQ_SAMPLE_RATE_44P1K  = 1,
	TLKALG_EQ_SAMPLE_RATE_16K    = 2,
	TLKALG_EQ_SAMPLE_RATE_32K    = 3,
}TLKALG_EQ_SAMPLE_RATE_ENUM;
typedef enum{
	TLKALG_EQ_CHANNEL_LEFT       = 0,
	TLKALG_EQ_CHANNEL_RIGHT      = 1,
	TLKALG_EQ_CHANNEL_STEREO     = 2,
}TLKALG_EQ_CHANNEL_ENUM;
typedef enum{
	TLKALG_EQ_TYPE_MUSIC         = 0,
	TLKALG_EQ_TYPE_VOICE_MIC     = 1,
	TLKALG_EQ_TYPE_VOICE_SPK     = 2,
}TLKALG_EQ_TYPE_ENUM;

typedef enum{
	TLKALG_EQ_FILTER_TYPE_PEAKING        = 0,
	TLKALG_EQ_FILTER_TYPE_LOWPASS        = 1,
	TLKALG_EQ_FILTER_TYPE_HIGHPASS       = 2,
	TLKALG_EQ_FILTER_TYPE_BANDPASS       = 3,
	TLKALG_EQ_FILTER_TYPE_NORTCH         = 4,
	TLKALG_EQ_FILTER_TYPE_LOWSHELF       = 5,
	TLKALG_EQ_FILTER_TYPE_HIGHSHELF      = 6,
}TLKALG_EQ_FILTER_TYPE_ENUM;

typedef enum{
	TLKALG_EQ_MUSIC_STYLE_BALANCED       = 0,
	TLKALG_EQ_MUSIC_STYLE_ACOUSTIC       = 1,
	TLKALG_EQ_MUSIC_STYLE_BASSBOOSTER    = 2,
	TLKALG_EQ_MUSIC_STYLE_BASSREDUCER    = 3,
	TLKALG_EQ_MUSIC_STYLE_CLASSICAL      = 4,
	TLKALG_EQ_MUSIC_STYLE_DANCE          = 5,
	TLKALG_EQ_MUSIC_STYLE_DEEP           = 6,
	TLKALG_EQ_MUSIC_STYLE_ELECTRONIC     = 7,
	TLKALG_EQ_MUSIC_STYLE_FLAT           = 8,
	TLKALG_EQ_MUSIC_STYLE_HIPHOP         = 9,
	TLKALG_EQ_MUSIC_STYLE_JAZZ           = 10,
	TLKALG_EQ_MUSIC_STYLE_LATIN          = 11,
	TLKALG_EQ_MUSIC_STYLE_LOUNGE         = 12,
	TLKALG_EQ_MUSIC_STYLE_PIANO          = 13,
	TLKALG_EQ_MUSIC_STYLE_POP            = 14,
	TLKALG_EQ_MUSIC_STYLE_RANDB          = 15,
	TLKALG_EQ_MUSIC_STYLE_ROCK           = 16,
	TLKALG_EQ_MUSIC_STYLE_SMALLSPEAKER   = 17,
	TLKALG_EQ_MUSIC_STYLE_SPOKENWORD     = 18,
	TLKALG_EQ_MUSIC_STYLE_TREBLEBOOSTER  = 19,
	TLKALG_EQ_MUSIC_STYLE_TREBLEREDUCER  = 20,
	TLKALG_EQ_MUSIC_STYLE_VOCALBOOSTER   = 21,
}TLKALG_EQ_MUSIC_STYLE_ENUM;





#define TLKALG_EQ_FLASH_BANK_SIZE          0xd0
#define TLKALG_EQ_FLASH_BANK_NUM_MAX       0x07

///mode = 1;
#define TLKALG_EQ_BANK_SIZE_DEFAULT_MAX    0x50
#define TLKALG_EQ_BANK_DEFAULT_NUM_MAX     0x19
#define TLKALG_EQ_BANK_SIZE_CUSTOMER_MAX   0x50
#define TLKALG_EQ_BANK_CUSTOMER_NUM_MAX    0x19

#define TLKALG_EQ_FILTER_ANKER_MAX         8
#define TLKALG_EQ_FILTER_MAX               9
#define TLKALG_EQ_FILTER_VOICE_MAX         4
#define TLKALG_EQ_NSTAGE_VOICE_MIC_MAX     4
#define TLKALG_EQ_NSTAGE_VOICE_SPK_MAX     4
#define TLKALG_EQ_NSTAGE_MISIC_SPK_MAX     9

#define TLKALG_EQ_NSTAGE_VOICE_MIC         4
#define TLKALG_EQ_NSTAGE_VOICE_SPK         4

#define TLKALG_EQ_LEFT_SAME_WITH_RIGHT     1 //left eq as same as right

#define TLKALG_EQ_TYPE_LENTH_MAX           80
#define TLKALG_EQ_DEFAULT_TYPE_MAX         20
#define TLKALG_EQ_DEFAULT_TYPE_MASK	       0x00ff

#define TLKALG_EQ_NSTAGE_MISIC_SPK 9

#if (TLKALG_EQ_NSTAGE_VOICE_MIC_MAX < TLKALG_EQ_NSTAGE_VOICE_MIC) || (TLKALG_EQ_NSTAGE_VOICE_SPK_MAX < TLKALG_EQ_NSTAGE_VOICE_SPK) || (TLKALG_EQ_NSTAGE_MISIC_SPK_MAX < TLKALG_EQ_NSTAGE_MISIC_SPK)
#error Wrong NSTAGE_EQ MAX
#endif

typedef struct _eq_para{
	uint8_t eq_nstage;      	///	EQ number of stage.
	uint8_t eq_sample_rate;	   	/// 0:48K ;1:44.1K ;2: 16K
	uint8_t eq_channel;			/// 0:left   1:right
	uint8_t eq_type;			/// 0:TLKALG_EQ_TYPE_MUSIC  1:TLKALG_EQ_TYPE_VOICE_MIC  2:TLKALG_EQ_TYPE_VOICE_SPK
}__attribute__ ((__packed__)) eq_para_t;

typedef struct _eq_mode_para{
	signed short gain;					///Gain range(BBEE/100)		: 	-327.68db ~ +327.67db
	unsigned short freq_c;				///Center frequency range 	:	20~24000
	unsigned short filter_q;			///Q value range(HHGG/1000)	:	0~65.535
	unsigned char filter_type;			///filter type(1~8)			:
	unsigned char cvsd;					///reserved
}eq_mode_para_t;

typedef struct _eq_mode_header{
	unsigned char idx;
	unsigned char max_gain_num;
	unsigned char filter_sum;
	unsigned char all_gain;					///Full-band gain reduction range:0~25.6
}eq_mode_header_t;

typedef struct _eq_mode{
	eq_mode_header_t eq_header;						/// 4 bytes
	eq_mode_para_t eq_para[TLKALG_EQ_FILTER_MAX];	/// 8 + 9 bytes
	int CRC;										/// 4 bytes
}eq_mode_t;



void tlkalg_eq_loadParam(uint32 addr, uint16 index, uint08 flag);
void tlkalg_eq_saveParam(void);
int  tlkalg_eq_procss(TLKALG_EQ_TYPE_ENUM type, TLKALG_EQ_CHANNEL_ENUM chn, int sampleRate, sint16 *pData, int samples);
void tlkalg_eq_dataProcess(TLKALG_EQ_TYPE_ENUM type, TLKALG_EQ_CHANNEL_ENUM chn, sint16* ps, int samples);
void tlkalg_eq_setSampleRate(int samplerate, TLKALG_EQ_TYPE_ENUM type);


#endif //#if (TLK_ALG_EQ_ENABLE)

#endif //TLKALG_EQ_H

