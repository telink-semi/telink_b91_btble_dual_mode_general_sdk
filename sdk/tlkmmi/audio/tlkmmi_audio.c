/********************************************************************************************************
 * @file     tlkmmi_audio.c
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

#include "drivers.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdev/tlkdev_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi/tlkmmi_stdio.h"
#include "tlkmmi/tlkmmi_adapt.h"
#if (TLKMMI_AUDIO_ENABLE)
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/a2dp/btp_a2dp.h"
#include "tlkmmi/audio/tlkmmi_audio.h"
#include "tlkmmi/audio/tlkmmi_audioCtrl.h"
#include "tlkmmi/audio/tlkmmi_audioInfo.h"
#include "tlkmmi/audio/tlkmmi_audioComm.h"
#include "tlkmmi/audio/tlkmmi_audioStatus.h"
#include "tlkmmi/audio/tlkmmi_audioModinf.h"



static bool tlkmmi_audio_timer(tlkapi_timer_t *pTimer, void *pUsrArg);
#if (TLKAPI_TIMER_ENABLE)
static bool tlkmmi_audio_irqTimer(tlkapi_timer_t *pTimer, void *pUsrArg);
#endif //#if (TLKAPI_TIMER_ENABLE)


uint16 gTlkMmiAudioCurHandle;
uint08 gTlkMmiAudioCurOptype;
uint08 gTlkMmiAudioTmrState = 0;
uint08 gTlkMmiAudioTmrCount = 0;
tlkapi_timer_t gTlkMmiAudioCurTimer;
tlkapi_timer_t gTlkMmiAudioIrqTimer;

/******************************************************************************
 * Function: tlkmmi_audio_init
 * Descript: Initial the audio path and audio ctrl flow, set the audio paramter.
 * Params:
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_init(void)
{
	audio_set_codec_in_path_a_d_gain(CODEC_IN_D_GAIN_0_DB, CODEC_IN_A_GAIN_8_DB); //mic sco
	audio_set_codec_out_path_a_d_gain(CODEC_OUT_D_GAIN_0_DB, CODEC_OUT_A_GAIN_0_DB); //mic sco
	
	tlkdev_codec_init(TLKDEV_CODEC_MODE_SINGLE, TLKDEV_CODEC_SELC_INNER);
	tlkdev_codec_setSampleRate(44100);

	audio_set_codec_adc_wnf(CODEC_ADC_WNF_INACTIVE);
	audio_codec_adc_enable(0);
	
	tlkmmi_audio_infoInit();
	tlkmmi_audio_commInit();
	tlkmmi_audio_ctrlInit();
	tlkmmi_audio_statusInit();
	tlkmmi_adapt_initTimer(&gTlkMmiAudioCurTimer, tlkmmi_audio_timer, nullptr, TLKMMI_AUDIO_TIMEOUT);
	#if (TLKAPI_TIMER_ENABLE)
	tlkapi_timer_initNode(&gTlkMmiAudioIrqTimer, tlkmmi_audio_irqTimer, nullptr, TLKMMI_AUDIO_TIMEOUT);
	#endif
	gTlkMmiAudioCurOptype = TLKMMI_AUDIO_OPTYPE_NONE;
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_audio_isBusy
 * Descript:
 * Params:
 * Return: true/false
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_isBusy(void)
{
	if(gTlkMmiAudioCurOptype != 0 || gTlkMmiAudioTmrState != 0) return true;
	return false;
}

void tlkmmi_audio_connect(uint16 handle, uint08 ptype, uint08 usrID)
{
	if(ptype == BTP_PTYPE_A2DP){
		int srcIndex;
		int snkIndex;
		int playIndex;
		int dstOptype;
		srcIndex = tlkmmi_audio_statusIndexByOptype(TLKMMI_AUDIO_OPTYPE_SRC);
		snkIndex = tlkmmi_audio_statusIndexByOptype(TLKMMI_AUDIO_OPTYPE_SNK);
		playIndex = tlkmmi_audio_statusIndex(TLK_INVALID_HANDLE, TLKMMI_AUDIO_OPTYPE_PLAY);
		if(playIndex >= 0){
			tlkmmi_audio_removeStatus(TLK_INVALID_HANDLE, TLKMMI_AUDIO_OPTYPE_PLAY);
		}
		if(usrID == BTP_USRID_SERVER) dstOptype = TLKMMI_AUDIO_OPTYPE_SRC;
		else dstOptype = TLKMMI_AUDIO_OPTYPE_SNK;
		if(playIndex >= 0 && srcIndex < 0 && snkIndex < 0){
			tlkmmi_audio_modinfStart(handle, dstOptype, 0);
		}		
	}
}
/******************************************************************************
 * Function: tlkmmi_audio_disconn
 * Descript:
 * Params:
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_disconn(uint16 handle)
{
	tlkmmi_audio_removeStatusByHandle(handle);
}


static bool tlkmmi_audio_timer(tlkapi_timer_t *pTimer, void *pUsrArg)
{
//	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_timer 01: {optype-%d,handle-%d,count-%d,state-%d}", 
//		gTlkMmiAudioCurOptype, gTlkMmiAudioCurHandle, gTlkMmiAudioTmrCount, gTlkMmiAudioTmrState);
	if(gTlkMmiAudioCurOptype != TLKMMI_AUDIO_OPTYPE_NONE){
		tlkmmi_audio_modinfTimer(gTlkMmiAudioCurOptype);
		if(gTlkMmiAudioTmrCount != 0) gTlkMmiAudioTmrCount--;
		if(!tlkmmi_audio_modinfIsBusy(gTlkMmiAudioCurOptype)){
			uint08 optype = gTlkMmiAudioCurOptype;
			uint16 handle = gTlkMmiAudioCurHandle;
			tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_handler: status over");
			tlkapi_chip_switchClock(TLKAPI_CHIP_CLOCK_48M);
			gTlkMmiAudioCurOptype = TLKMMI_AUDIO_OPTYPE_NONE;
			gTlkMmiAudioCurHandle = 0;
			#if (TLKAPI_TIMER_ENABLE)
			tlkapi_timer_removeNode(&gTlkMmiAudioIrqTimer);
			#endif
			gTlkMmiAudioTmrState = 0;
			tlkmmi_audio_removeStatus(handle, optype);
//			my_dump_str_data(TLKMMI_AUDIO_DBG_ENABLE, "tlkmmi_audio_handler: optype = ", &gTlkMmiAudioCurOptype, 1);
		}else if(gTlkMmiAudioTmrState == 0){
			tlkapi_info(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_handler: start timer");
			#if (TLKAPI_TIMER_ENABLE)
			tlkapi_timer_updateNode(&gTlkMmiAudioIrqTimer, 3000, true);
			#endif
			gTlkMmiAudioTmrState = 2;
			gTlkMmiAudioTmrCount = TLKMMI_AUDIO_TIMER_TIMEOUT;
			tlkapi_chip_switchClock(TLKAPI_CHIP_CLOCK_96M);
			if(gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_PLAY || gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_SRC){
				if(gTlkMmiAudioCtrl.report.enable) tlkmmi_adapt_insertTimer(&gTlkMmiAudioCtrl.timer);
			}
		}else if(gTlkMmiAudioTmrCount == 0){
			tlkapi_fatal(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_handler: timer fault");
			#if (TLKAPI_TIMER_ENABLE)
			tlkapi_timer_updateNode(&gTlkMmiAudioIrqTimer, 3000, true);
			#endif
			gTlkMmiAudioTmrCount = TLKMMI_AUDIO_TIMER_TIMEOUT;
		}
	}
	else if(gTlkMmiAudioTmrState != 0){
		tlkapi_info(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_handler: close timer");
		if(gTlkMmiAudioTmrState == 2){
			gTlkMmiAudioCurOptype = TLKMMI_AUDIO_OPTYPE_NONE;
			gTlkMmiAudioCurHandle = 0;
			#if (TLKAPI_TIMER_ENABLE)
			tlkapi_timer_removeNode(&gTlkMmiAudioIrqTimer);
			#endif
		}
		gTlkMmiAudioTmrState = 0;
		tlkdev_spk_mute();
		tlkapi_chip_switchClock(TLKAPI_CHIP_CLOCK_48M);
	}
	if(gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_PLAY || gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_SRC){
		#if (TLK_MDI_MP3_ENABLE)
		if(tlkmdi_mp3_isUpdate()) tlkmdi_mp3_startUpdate();
		#endif
	}
		
	if(gTlkMmiAudioCurOptype != 0 || gTlkMmiAudioTmrState != 0) return true;
	else return false;
}

#if (TLKAPI_TIMER_ENABLE)
static bool tlkmmi_audio_irqTimer(tlkapi_timer_t *pTimer, void *pUsrArg)
{
	uint32 timeIntval;
	
	if(gTlkMmiAudioCurOptype == TLKMMI_AUDIO_OPTYPE_NONE) return false;
	
	if(!tlkmmi_audio_modinfIrqProc(gTlkMmiAudioCurOptype)){
		tlkdev_spk_mute();
		timeIntval = 100000;
	}else{
		timeIntval = tlkmmi_audio_modinfIntval(gTlkMmiAudioCurOptype);
		if(timeIntval == 0) timeIntval = 3000;
		else if(timeIntval < 500) timeIntval = 500;
		else if(timeIntval > 1000000) timeIntval = 1000000;
	}
	
	if(gTlkMmiAudioCurOptype != TLKMMI_AUDIO_OPTYPE_NONE){
		tlkapi_timer_updateNode(&gTlkMmiAudioIrqTimer, timeIntval, true);
		gTlkMmiAudioTmrCount = TLKMMI_AUDIO_TIMER_TIMEOUT;
	}else{
		gTlkMmiAudioTmrState = 0;
		gTlkMmiAudioTmrCount = 0;
	}

	return false;
}
#endif


#endif //#if (TLKMMI_AUDIO_ENABLE)

