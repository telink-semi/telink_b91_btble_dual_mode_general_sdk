/********************************************************************************************************
 * @file     tlkalg_aec_ns.h
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

#ifndef AEC_NS_API_H
#define AEC_NS_API_H

#define GSCON 1
#define AECON 1

/*! Version number to ensure header and binary are matching. */
#define AEC_NS_VERSION_INT(major, minor, micro) (((major) << 16) | ((minor) << 8) | (micro))
#define AEC_NS_VERSION AEC_NS_VERSION_INT(1, 1, 9)

#ifndef _SPEEX_TYPES_H
#define _SPEEX_TYPES_H

#if defined(_WIN32) 
/* MSVC/Borland */
typedef __int32 spx_int32_t;
typedef unsigned __int32 spx_uint32_t;
typedef __int16 spx_int16_t;
typedef unsigned __int16 spx_uint16_t;
#elif __riscv
typedef short spx_int16_t;
typedef unsigned short spx_uint16_t;
typedef int spx_int32_t;
typedef unsigned int spx_uint32_t;
#endif
#endif

typedef struct {
	spx_int16_t use_pre_emp;      /* 1: enable pre-emphasis filter, 0: disable pre-emphasis filter */
	spx_int16_t use_dc_notch;     /* 1: enable DC removal filter, 0: disable DC removal filter */
	spx_int32_t sampling_rate;    /* sample rate */
	int reserved;
} AEC_CFG_PARAS;

typedef struct {
	int   low_shelf_enable;          /* 1: enable lowshelf filter, 0: disable lowshelf filter */
	int   noise_suppress_default;    /* noise suppression ratio, set to -15db by default */
	int   echo_suppress_default;  
	int   echo_suppress_active_default;
	short ns_smoothness;             /* gain smoothing factor in Q15 format */
	float ns_threshold_low;
	int   reserved;
} NS_CFG_PARAS;

#define SPEEX_SAMPLERATE 16000
#define SPEEX_FRAME_SIZE 120
//#define SPEEX_SAMPLERATE 48000
//#define SPEEX_FRAME_SIZE 96
#define SPEEX_TAIL_LENGTH SPEEX_FRAME_SIZE
#define SPEEX_WINDOW_SIZE (SPEEX_FRAME_SIZE<<1)
#define SPEEX_M ((SPEEX_TAIL_LENGTH+SPEEX_FRAME_SIZE-1)/SPEEX_FRAME_SIZE)

/** Set preprocessor denoiser state */
#define SPEEX_PREPROCESS_SET_DENOISE 0
/** Get preprocessor denoiser state */
#define SPEEX_PREPROCESS_GET_DENOISE 1

/** Set preprocessor Automatic Gain Control state */
#define SPEEX_PREPROCESS_SET_AGC 2
/** Get preprocessor Automatic Gain Control state */
#define SPEEX_PREPROCESS_GET_AGC 3

/** Set preprocessor Voice Activity Detection state */
#define SPEEX_PREPROCESS_SET_VAD 4
/** Get preprocessor Voice Activity Detection state */
#define SPEEX_PREPROCESS_GET_VAD 5

/** Set preprocessor Automatic Gain Control level (float) */
#define SPEEX_PREPROCESS_SET_AGC_LEVEL 6
/** Get preprocessor Automatic Gain Control level (float) */
#define SPEEX_PREPROCESS_GET_AGC_LEVEL 7

/** Set preprocessor dereverb state */
#define SPEEX_PREPROCESS_SET_DEREVERB 8
/** Get preprocessor dereverb state */
#define SPEEX_PREPROCESS_GET_DEREVERB 9

/** Set preprocessor dereverb level */
#define SPEEX_PREPROCESS_SET_DEREVERB_LEVEL 10
/** Get preprocessor dereverb level */
#define SPEEX_PREPROCESS_GET_DEREVERB_LEVEL 11

/** Set preprocessor dereverb decay */
#define SPEEX_PREPROCESS_SET_DEREVERB_DECAY 12
/** Get preprocessor dereverb decay */
#define SPEEX_PREPROCESS_GET_DEREVERB_DECAY 13

/** Set probability required for the VAD to go from silence to voice */
#define SPEEX_PREPROCESS_SET_PROB_START 14
/** Get probability required for the VAD to go from silence to voice */
#define SPEEX_PREPROCESS_GET_PROB_START 15

/** Set probability required for the VAD to stay in the voice state (integer percent) */
#define SPEEX_PREPROCESS_SET_PROB_CONTINUE 16
/** Get probability required for the VAD to stay in the voice state (integer percent) */
#define SPEEX_PREPROCESS_GET_PROB_CONTINUE 17

/** Set maximum attenuation of the noise in dB (negative number) */
#define SPEEX_PREPROCESS_SET_NOISE_SUPPRESS 18
/** Get maximum attenuation of the noise in dB (negative number) */
#define SPEEX_PREPROCESS_GET_NOISE_SUPPRESS 19

/** Set maximum attenuation of the residual echo in dB (negative number) */
#define SPEEX_PREPROCESS_SET_ECHO_SUPPRESS 20
/** Get maximum attenuation of the residual echo in dB (negative number) */
#define SPEEX_PREPROCESS_GET_ECHO_SUPPRESS 21

/** Set maximum attenuation of the residual echo in dB when near end is active (negative number) */
#define SPEEX_PREPROCESS_SET_ECHO_SUPPRESS_ACTIVE 22
/** Get maximum attenuation of the residual echo in dB when near end is active (negative number) */
#define SPEEX_PREPROCESS_GET_ECHO_SUPPRESS_ACTIVE 23

/** Set the corresponding echo canceller state so that residual echo suppression can be performed (NULL for no residual echo suppression) */
#define SPEEX_PREPROCESS_SET_ECHO_STATE 24
/** Get the corresponding echo canceller state */
#define SPEEX_PREPROCESS_GET_ECHO_STATE 25

/** Set maximal gain increase in dB/second (int32) */
#define SPEEX_PREPROCESS_SET_AGC_INCREMENT 26

/** Get maximal gain increase in dB/second (int32) */
#define SPEEX_PREPROCESS_GET_AGC_INCREMENT 27

/** Set maximal gain decrease in dB/second (int32) */
#define SPEEX_PREPROCESS_SET_AGC_DECREMENT 28

/** Get maximal gain decrease in dB/second (int32) */
#define SPEEX_PREPROCESS_GET_AGC_DECREMENT 29

/** Set maximal gain in dB (int32) */
#define SPEEX_PREPROCESS_SET_AGC_MAX_GAIN 30

/** Get maximal gain in dB (int32) */
#define SPEEX_PREPROCESS_GET_AGC_MAX_GAIN 31

/*  Can't set loudness */
/** Get loudness */
#define SPEEX_PREPROCESS_GET_AGC_LOUDNESS 33

/*  Can't set gain */
/** Get current gain (int32 percent) */
#define SPEEX_PREPROCESS_GET_AGC_GAIN 35

/*  Can't set spectrum size */
/** Get spectrum size for power spectrum (int32) */
#define SPEEX_PREPROCESS_GET_PSD_SIZE 37

/*  Can't set power spectrum */
/** Get power spectrum (int32[] of squared values) */
#define SPEEX_PREPROCESS_GET_PSD 39

/*  Can't set noise size */
/** Get spectrum size for noise estimate (int32)  */
#define SPEEX_PREPROCESS_GET_NOISE_PSD_SIZE 41

/*  Can't set noise estimate */
/** Get noise estimate (int32[] of squared values) */
#define SPEEX_PREPROCESS_GET_NOISE_PSD 43

/* Can't set speech probability */
/** Get speech probability in last frame (int32).  */
#define SPEEX_PREPROCESS_GET_PROB 45

/** Set preprocessor Automatic Gain Control level (int32) */
#define SPEEX_PREPROCESS_SET_AGC_TARGET 46
/** Get preprocessor Automatic Gain Control level (int32) */
#define SPEEX_PREPROCESS_GET_AGC_TARGET 47
/** Set threshold to implement NS */
#define SPEEX_PREPROCESS_SET_THRESHOLD_LOW 48
#define SPEEX_PREPROCESS_SET_THRESHOLD_HIGH 49
#define SPEEX_PREPROCESS_SET_FREEZE_WIN 50
#define SPEEX_PREPROCESS_SET_GAINMU 51

/** Internal echo canceller state. Should never be accessed directly. */
typedef struct SpeexEchoState_ SpeexEchoState;

/** State of the preprocessor (one per channel). Should never be accessed directly. */
typedef struct SpeexPreprocessState_ SpeexPreprocessState;

#if GSCON
	/*! Version number to ensure header and binary are matching. */
	#define GSC_VERSION_INT(major, minor, micro) (((major) << 16) | ((minor) << 8) | (micro))
	#define GSC_VERSION GSC_VERSION_INT(1, 0, 0)

	#define MICNUM (2)

	typedef struct gscState_ gscState;

	int gsc_get_version(void);
	int gsc_get_size();
	int gsc_state_init(gscState* st, int frame_size, int exchange_mic);
	int gsc_BeamFormer(short* x_in, short* ref_in, short* x_out, gscState* st, int was_speech);
#endif

int aec_ns_get_version(void);

#if AECON
int aec_get_size();
#endif
int ns_get_size();

#if AECON
/** Creates a new echo canceller state
 * @param frame_size Number of samples to process at one time (should correspond to 10-20 ms)
 * @param filter_length Number of samples of echo to cancel (should generally correspond to 100-500 ms)
 * @return Newly-created echo canceller state
 */
extern void aec_init(SpeexEchoState* st, AEC_CFG_PARAS* param, int frame_size, int filter_length);

/** Performs echo cancellation a frame, based on the audio sent to the speaker (no delay is added
 * to playback in this form)
 *
 * @param st Echo canceller state
 * @param rec Signal from the microphone (near end + far end echo)
 * @param play Signal played to the speaker (received from far end)
 * @param out Returns near-end signal with echo removed
 */
extern void aec_process_frame(SpeexEchoState* st, const spx_int16_t* rec, const spx_int16_t* play, spx_int16_t* out);
#endif

/** Creates a new preprocessing state. You MUST create one state per channel processed.
 * @param frame_size Number of samples to process at one time (should correspond to 10-20 ms). Must be
 * the same value as that used for the echo canceller for residual echo cancellation to work.
 * @param sampling_rate Sampling rate used for the input.
 * @return Newly created preprocessor state
*/
//extern void ns_init1(SpeexPreprocessState* st, NS_CFG_PARAS* param, int frame_size, int sampling_rate);
extern void ns_init(void* pst, void *pParam, int frame_size, int sampling_rate);

/** Preprocess a frame
 * @param st Preprocessor state
 * @param x Audio sample vector (in and out). Must be same size as specified in ns_init().
 * @return Bool value for voice activity (1 for speech, 0 for noise/silence), ONLY if VAD turned on.
*/
extern int ns_process_frame(SpeexPreprocessState* st, spx_int16_t* x);

/** Used like the ioctl function to control the preprocessor parameters
 * @param st Preprocessor state
 * @param request ioctl-type request (one of the SPEEX_PREPROCESS_* macros)
 * @param ptr Data exchanged to-from function
 * @return 0 if no error, -1 if request in unknown
*/
extern int ns_set_parameter(SpeexPreprocessState* st, int request, void* ptr);

extern float ns_get_gain_average(SpeexPreprocessState* st);

extern void ns_free(SpeexPreprocessState* st);
#endif
