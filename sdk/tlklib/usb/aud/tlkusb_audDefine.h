/********************************************************************************************************
 * @file     tlkusb_audDefine.h
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
#ifndef TLKUSB_AUDIO_DEFINE_H
#define TLKUSB_AUDIO_DEFINE_H

#if (TLK_USB_AUD_ENABLE)


#define TLKUSB_AUD_MIC_ENABLE    (1 && TLK_USB_AUD_ENABLE)
#define TLKUSB_AUD_SPK_ENABLE    (0 && TLK_USB_AUD_ENABLE)

#define TLKUSB_AUD_MIC_RESOLUTION_BIT      16
#define TLKUSB_AUD_MIC_SAMPLE_RATE         16000
#define TLKUSB_AUD_MIC_CHANNEL_COUNT       1
#define TLKUSB_AUD_MIC_CHANNEL_LENGTH      (TLKUSB_AUD_MIC_CHANNEL_COUNT*(TLKUSB_AUD_MIC_SAMPLE_RATE*TLKUSB_AUD_MIC_RESOLUTION_BIT/1000/8))

#define TLKUSB_AUD_SPK_RESOLUTION_BIT      16
#define TLKUSB_AUD_SPK_SAMPLE_RATE         16000
#define TLKUSB_AUD_SPK_CHANNEL_COUNT       2
#define TLKUSB_AUD_SPK_CHANNEL_LENGTH      (TLKUSB_AUD_SPK_CHANNEL_COUNT*(TLKUSB_AUD_SPK_SAMPLE_RATE*TLKUSB_AUD_SPK_RESOLUTION_BIT/1000/8))



typedef enum{
	TLKUSB_AUD_INF_CTR,
	#if (TLKUSB_AUD_SPK_ENABLE)
	TLKUSB_AUD_INF_SPK,
	#endif
	#if (TLKUSB_AUD_MIC_ENABLE)
	TLKUSB_AUD_INF_MIC,
	#endif
	TLKUSB_AUD_INF_MAX,
}TLKUSB_AUD_INF_ENUM;
typedef enum{
	TLKUSB_AUD_EDP_MIC = 7,
	TLKUSB_AUD_EDP_SPK = 6,
}TLKUSB_AUD_EDP_ENUM;



enum TLKUSB_AUD_CSCP_ENUM
{
	TLKUSB_AUD_CSCP_StreamingProtocol = 0,
	TLKUSB_AUD_CSCP_ControlProtocol = 0,
	TLKUSB_AUD_CSCP_AudioClass = 1,
	TLKUSB_AUD_CSCP_ControlSubclass = 1,
	TLKUSB_AUD_CSCP_AudioStreamingSubclass,
	TLKUSB_AUD_CSCP_MIDIStreamingSubclass,
};
enum Audio_CSInterface_AC_SubTypes_t
{
	AUDIO_DSUBTYPE_CSInterface_Header = 1,
	AUDIO_DSUBTYPE_CSInterface_InputTerminal,
	AUDIO_DSUBTYPE_CSInterface_OutputTerminal,
	AUDIO_DSUBTYPE_CSInterface_Mixer,
	AUDIO_DSUBTYPE_CSInterface_Selector,
	AUDIO_DSUBTYPE_CSInterface_Feature,
	AUDIO_DSUBTYPE_CSInterface_Processing,
	AUDIO_DSUBTYPE_CSInterface_Extension,
};
enum Audio_CSInterface_AS_SubTypes_t
{
	AUDIO_DSUBTYPE_CSInterface_General = 1,
	AUDIO_DSUBTYPE_CSInterface_FormatType,
	AUDIO_DSUBTYPE_CSInterface_FormatSpecific,
};
enum Audio_CSEndpoint_SubTypes_t
{
	AUDIO_DSUBTYPE_CSEndpoint_General = 1,
};
enum TLKUSB_AUDID_ENUM{
	TLKUSB_AUDID_SPK_INPUT_TERMINAL_ID = 1,
	TLKUSB_AUDID_SPK_FEATURE_UNIT_ID,
	TLKUSB_AUDID_SPK_OUTPUT_TERMINAL_ID,
	TLKUSB_AUDID_MIC_INPUT_TERMINAL_ID,
	TLKUSB_AUDID_MIC_FEATURE_UNIT_ID,
	TLKUSB_AUDID_MIC_OUTPUT_TERMINAL_ID,
};
enum TLKUSB_AUD_SRCID_ENUM{
	TLKUSB_AUD_SRCID_SPK_FEATURE_UNIT = 1,
	TLKUSB_AUD_SRCID_SPK_OUTPUT_TERMINAL,
	TLKUSB_AUD_SRCID_MIC_FEATURE_UNIT = 4,
	TLKUSB_AUD_SRCID_MIC_OUTPUT_TERMINAL,
};
enum {
	USB_AUDIO_FORMAT_UNKNOWN = 0,
	USB_AUDIO_FORMAT_PCM,
	USB_AUDIO_FORMAT_ADPCM,
	USB_AUDIO_FORMAT_IEEE_FLOAT,
	USB_AUDIO_FORMAT_IBM_CVSD,
	USB_AUDIO_FORMAT_ALAW,
	USB_AUDIO_FORMAT_MULAW,
	USB_AUDIO_FORMAT_WMAVOICE9,
	USB_AUDIO_FORMAT_OKI_ADPCM,
	USB_AUDIO_FORMAT_DVI_ADPCM,
	USB_AUDIO_FORMAT_IMA_ADPCM,
	USB_AUDIO_FORMAT_MEDIASPACE_ADPCM,
	USB_AUDIO_FORMAT_SIERRA_ADPCM,
	USB_AUDIO_FORMAT_G723_ADPCM,
	USB_AUDIO_FORMAT_DIGISTD,
	USB_AUDIO_FORMAT_DIGIFIX,
	USB_AUDIO_FORMAT_DIALOGIC_OKI_ADPCM,
	USB_AUDIO_FORMAT_MEDIAVISION_ADPCM,
	USB_AUDIO_FORMAT_YAMAHA_ADPCM,
	USB_AUDIO_FORMAT_SONARC,
	USB_AUDIO_FORMAT_DSPGROUP_TRUESPEECH,
	USB_AUDIO_FORMAT_ECHOSC1,
	USB_AUDIO_FORMAT_AUDIOFILE_AF36,
	USB_AUDIO_FORMAT_APTX,
	USB_AUDIO_FORMAT_AUDIOFILE_AF10,
	USB_AUDIO_FORMAT_DOLBY_AC2,
	USB_AUDIO_FORMAT_GSM610,
	USB_AUDIO_FORMAT_MSNAUDIO,
	USB_AUDIO_FORMAT_ANTEX_ADPCME,
	USB_AUDIO_FORMAT_CONTROL_RES_VQLPC,
	USB_AUDIO_FORMAT_DIGIREAL,
	USB_AUDIO_FORMAT_DIGIADPCM,
	USB_AUDIO_FORMAT_CONTROL_RES_CR10,
	USB_AUDIO_FORMAT_NMS_VBXADPCM,
	USB_AUDIO_FORMAT_CS_IMAADPCM,
	USB_AUDIO_FORMAT_ECHOSC3,
	USB_AUDIO_FORMAT_ROCKWELL_ADPCM,
	USB_AUDIO_FORMAT_ROCKWELL_DIGITALK,
	USB_AUDIO_FORMAT_XEBEC,
	USB_AUDIO_FORMAT_G721_ADPCM,
	USB_AUDIO_FORMAT_G728_CELP,
	USB_AUDIO_FORMAT_MPEG,
	USB_AUDIO_FORMAT_MPEGLAYER3,
	USB_AUDIO_FORMAT_CIRRUS,
	USB_AUDIO_FORMAT_ESPCM,
	USB_AUDIO_FORMAT_VOXWARE,
	USB_AUDIO_FORMAT_WAVEFORMAT_CANOPUS_ATRAC,
	USB_AUDIO_FORMAT_G726_ADPCM,
	USB_AUDIO_FORMAT_G722_ADPCM,
	USB_AUDIO_FORMAT_DSAT,
	USB_AUDIO_FORMAT_DSAT_DISPLAY,
	USB_AUDIO_FORMAT_SOFTSOUND,
	USB_AUDIO_FORMAT_RHETOREX_ADPCM,
	USB_AUDIO_FORMAT_MSAUDIO1,
	USB_AUDIO_FORMAT_WMAUDIO2,
	USB_AUDIO_FORMAT_WMAUDIO3,
	USB_AUDIO_FORMAT_WMAUDIO_LOSSLESS,
	USB_AUDIO_FORMAT_CREATIVE_ADPCM,
	USB_AUDIO_FORMAT_CREATIVE_FASTSPEECH8,
	USB_AUDIO_FORMAT_CREATIVE_FASTSPEECH10,
	USB_AUDIO_FORMAT_QUARTERDECK,
	USB_AUDIO_FORMAT_FM_TOWNS_SND,
	USB_AUDIO_FORMAT_BTV_DIGITAL,
	USB_AUDIO_FORMAT_OLIGSM,
	USB_AUDIO_FORMAT_OLIADPCM,
	USB_AUDIO_FORMAT_OLICELP,
	USB_AUDIO_FORMAT_OLISBC,
	USB_AUDIO_FORMAT_OLIOPR,
	USB_AUDIO_FORMAT_LH_CODEC,
	USB_AUDIO_FORMAT_NORRIS,
};
#define AUDIO_EP_FULL_PACKETS_ONLY        0x80
#define AUDIO_EP_ACCEPTS_SMALL_PACKETS    (0<<7)
#define AUDIO_EP_SAMPLE_FREQ_CONTROL      0x01
#define AUDIO_EP_PITCH_CONTROL            0x02



#define AUDIO_CHANNEL_LEFT_FRONT           BIT(0)
#define AUDIO_CHANNEL_RIGHT_FRONT          BIT(1)
#define AUDIO_CHANNEL_CENTER_FRONT         BIT(2)
#define AUDIO_CHANNEL_LOW_FREQ_ENHANCE     BIT(3)
#define AUDIO_CHANNEL_LEFT_SURROUND        BIT(4)
#define AUDIO_CHANNEL_RIGHT_SURROUND       BIT(5)
#define AUDIO_CHANNEL_LEFT_OF_CENTER       BIT(6)
#define AUDIO_CHANNEL_RIGHT_OF_CENTER      BIT(7)
#define AUDIO_CHANNEL_SURROUND             BIT(8)
#define AUDIO_CHANNEL_SIDE_LEFT            BIT(9)
#define AUDIO_CHANNEL_SIDE_RIGHT           BIT(10)
#define AUDIO_CHANNEL_TOP                  BIT(11)

#define AUDIO_FEATURE_MUTE                 BIT(0)
#define AUDIO_FEATURE_VOLUME               BIT(1)
#define AUDIO_FEATURE_BASS                 BIT(2)
#define AUDIO_FEATURE_MID                  BIT(3)
#define AUDIO_FEATURE_TREBLE               BIT(4)
#define AUDIO_FEATURE_GRAPHIC_EQUALIZER    BIT(5)
#define AUDIO_FEATURE_AUTOMATIC_GAIN       BIT(6)
#define AUDIO_FEATURE_DELAY                BIT(7)
#define AUDIO_FEATURE_BASS_BOOST           BIT(8)
#define AUDIO_FEATURE_BASS_LOUDNESS        BIT(9)

#define AUDIO_TERMINAL_UNDEFINED           (0x100)
#define AUDIO_TERMINAL_STREAMING           (0x101)
#define AUDIO_TERMINAL_VENDOR              (0x1FF)
#define AUDIO_TERMINAL_IN_UNDEFINED        (0x200)
#define AUDIO_TERMINAL_IN_MIC              (0x201)
#define AUDIO_TERMINAL_IN_DESKTOP_MIC      (0x202)
#define AUDIO_TERMINAL_IN_PERSONAL_MIC     (0x203)
#define AUDIO_TERMINAL_IN_OMNIDIR_MIC      (0x204)
#define AUDIO_TERMINAL_IN_MIC_ARRAY        (0x205)
#define AUDIO_TERMINAL_IN_PROCESSING_MIC   (0x206)
#define AUDIO_TERMINAL_IN_OUT_UNDEFINED    (0x300)
#define AUDIO_TERMINAL_OUT_SPEAKER         (0x301)
#define AUDIO_TERMINAL_OUT_HEADPHONES      (0x302)
#define AUDIO_TERMINAL_OUT_HEAD_MOUNTED    (0x303)
#define AUDIO_TERMINAL_OUT_DESKTOP         (0x304)
#define AUDIO_TERMINAL_OUT_ROOM            (0x305)
#define AUDIO_TERMINAL_OUT_COMMUNICATION   (0x306)
#define AUDIO_TERMINAL_OUT_LOWFREQ         (0x307)
#define AUDIO_TERMINAL_HEADSET         	   (0x402)

enum AUDIO_USB_INF_OPCODE{
	USB_SPEAKER_INPUT_TERMINAL_ID = 1,
	USB_SPEAKER_FEATURE_UNIT_ID,
	USB_SPEAKER_OUTPUT_TERMINAL_ID,
	USB_MIC_INPUT_TERMINAL_ID,
	USB_MIC_FEATURE_UNIT_ID,
	USB_MIC_OUTPUT_TERMINAL_ID,
};
enum AUDIO_USB_EDP_OPCODE
{
	AUDIO_EPCONTROL_SamplingFreq = 1,
	AUDIO_EPCONTROL_Pitch,
};
enum Audio_ClassRequests_t
{
	AUDIO_REQ_SetCurrent = 1,
	AUDIO_REQ_SetMinimum,
	AUDIO_REQ_SetMaximum,
	AUDIO_REQ_SetResolution,
	AUDIO_REQ_SetMemory,
	AUDIO_REQ_GetCurrent = 0x81,
	AUDIO_REQ_GetMinimum,
	AUDIO_REQ_GetMaximum,
	AUDIO_REQ_GetResolution,
	AUDIO_REQ_GetMemory,
	AUDIO_REQ_GetStatus = 0xFF,
};






#endif //#if (TLK_USB_AUD_ENABLE)

#endif //TLKUSB_AUDIO_DEFINE_H

