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

#ifndef APPLICATION_TWS_EQ_H_
#define APPLICATION_TWS_EQ_H_



enum {
	EQ_SAMPLE_RATE_48K			= 0,
	EQ_SAMPLE_RATE_44P1K		= 1,
	EQ_SAMPLE_RATE_16K			= 2,
	EQ_SAMPLE_RATE_32K			= 3,

};

enum {
	EQ_CHANNEL_LEFT			= 0,
	EQ_CHANNEL_RIGHT		= 1,

};

enum {
	EQ_TYPE_MUSIC			= 0,
	EQ_TYPE_VOICE_MIC		= 1,
	EQ_TYPE_VOICE_SPK		= 2,

};

typedef struct _eq_para{
	uint8_t eq_nstage;      	///	EQ number of stage.
	uint8_t eq_sample_rate;	   	/// 0:48K ;1:44.1K ;2: 16K
	uint8_t eq_channel;			/// 0:left   1:right
	uint8_t eq_type;			/// 0:EQ_TYPE_MUSIC  1:EQ_TYPE_VOICE_MIC  2:EQ_TYPE_VOICE_SPK
}__attribute__ ((__packed__)) eq_para_t;

typedef struct _eq_sys{

	uint8_t	music_eq_en;
	uint8_t	speech_mic_eq_en;
	uint8_t	speech_spl_eq_en;

	uint8_t	music_eq_num;
	uint8_t	speech_mic_eq_num;
	uint8_t	speech_spl_eq_num;

	uint8_t music_indpd_lr;				///Independent sign bit of EQ parameters for music left and right ears
	uint8_t speech_spl_indpd_lr;		///Independent sign bit of EQ parameters for the left and right ears of the speech

}__attribute__ ((__packed__)) eq_sys_t;
#if TLK_ALG_EQ_ENABLE

#define EQ_FLASH_Bank_SIZE		0xd0
#define EQ_FLASH_Bank			0x07

#define NSTAGE_EQ_SPEECH_MIC_MAX     4
#define NSTAGE_EQ_SPEECH_SPK_MAX 	 4
#define NSTAGE_EQ_MISIC_SPK_MAX 	 9

#define nstage_eq_speech_mic 	4
#define nstage_eq_speech_spk 	4

#if SOUNDBAR_EQ_EN
	#define nstage_eq_music  5
#else
	#define nstage_eq_music  9
#endif

extern eq_para_t eq_para;
extern eq_sys_t  eq_sys_para;
extern nds_bq_df1_f32_t instance_eq_music_right;
extern nds_bq_df1_f32_t instance_eq_music_left;
extern nds_bq_df1_f32_t instance_eq_speech_mic;
extern nds_bq_df1_f32_t instance_eq_speech_speaker_right;
extern nds_bq_df1_f32_t instance_eq_speech_speaker_left;

extern float32_t state_eq_speech_mic[4 * NSTAGE_EQ_SPEECH_MIC_MAX];
extern float32_t state_eq_speech_speaker_left[4 * NSTAGE_EQ_SPEECH_SPK_MAX];
extern float32_t state_eq_speech_speaker_right[4 * NSTAGE_EQ_SPEECH_SPK_MAX];

extern float32_t state_eq_music_left[4 * NSTAGE_EQ_MISIC_SPK_MAX];
extern float32_t state_eq_music_right[4 * NSTAGE_EQ_MISIC_SPK_MAX];

extern eq_para_t eq_set_para_table[7];

void eq_state_reset_all(void);

void eq_inf_load(uint32_t adr);
int eq_coef_load_ota (eq_para_t para, u8 *p);

nds_bq_df1_f32_t* eq_proc_settimg_init(eq_para_t para);
void eq_state_reset(nds_bq_df1_f32_t *st);

void eq_proc_tws_music(eq_para_t para, signed short *ps, signed short *pd, unsigned int nsample, unsigned char shift);
void eq_proc_tws_speech(eq_para_t para, signed short *ps, signed short *pd, unsigned int nsample, unsigned char shift);

int my_debug_eq(unsigned char *p, int len);
void eq_coeff_dump(void);

#endif

#endif /* APPLICATION_TWS_EQ_H_ */
