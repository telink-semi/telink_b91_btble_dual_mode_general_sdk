/********************************************************************************************************
 * @file     tlkmmi_audioModinf.c
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
#include "tlkdev/tlkdev_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi/tlkmmi_stdio.h"
#if (TLKMMI_AUDIO_ENABLE)
#include "tlkmmi/audio/tlkmmi_audio.h"
#include "tlkmmi/audio/tlkmmi_audioCtrl.h"
#include "tlkmmi/audio/tlkmmi_audioStatus.h"
#include "tlkmmi/audio/tlkmmi_audioModinf.h"



int tlkmmi_audio_modinfStart(TLKMMI_AUDIO_OPTYPE_ENUM optype, uint16 handle, uint32 param)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf == nullptr || pModinf->Start == nullptr) return -TLK_ENOSUPPORT;
	else return pModinf->Start(handle, param);
}
int tlkmmi_audio_modinfClose(TLKMMI_AUDIO_OPTYPE_ENUM optype, uint16 handle)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf == nullptr || pModinf->Close == nullptr) return -TLK_ENOSUPPORT;
	else return pModinf->Close(handle);
}

void tlkmmi_audio_modinfTimer(TLKMMI_AUDIO_OPTYPE_ENUM optype)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf != nullptr && pModinf->Timer != nullptr){
		pModinf->Timer();
	}
}

/******************************************************************************
 * Function: tlkmmi_audio_modinfSwitch
 * Descript: Process the tlkmdi_audtone_switch function which indicate by the optype.
 * Params:@optype[IN]--The optype of audio.
 *        @handle[IN]--The handle of audio.
 *        @status[IN]--The status of audio.
 * Return: Operating results. true means success, others means failture.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfSwitch(TLKMMI_AUDIO_OPTYPE_ENUM optype, uint16 handle, uint08 status)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf == nullptr || pModinf->Switch == nullptr) return false;
	else return pModinf->Switch(handle, status);
}

/******************************************************************************
 * Function: tlkmmi_audio_modinfToNext
 * Descript: Process the tlkmdi_audtone_isBusy function which indicate by the optype.
 * Params: 
 *        @optype[IN]--The optype of audio.
 * Return: Operating results. true means success, others means failture.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfToNext(TLKMMI_AUDIO_OPTYPE_ENUM optype)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf == nullptr || pModinf->ToNext == nullptr) return false;
	else return pModinf->ToNext();
}

/******************************************************************************
 * Function: tlkmmi_audio_modinfToNext
 * Descript: Process the tlkmdi_audtone_isBusy function which indicate by the optype.
 * Params: 
 *        @optype[IN]--The optype of audio.
 * Return: Operating results. true means success, others means failture.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfToPrev(TLKMMI_AUDIO_OPTYPE_ENUM optype)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf == nullptr || pModinf->ToPrev == nullptr) return false;
	else return pModinf->ToPrev();
}

/******************************************************************************
 * Function: tlkmmi_audio_modinfIsBusy
 * Descript: Process the tlkmdi_audtone_isBusy function which indicate by the optype.
 * Params: @optype[IN]--The optype of audio.
 * Return: Operating results. true means success, others means failture.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfIsBusy(TLKMMI_AUDIO_OPTYPE_ENUM optype)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf == nullptr || pModinf->IsBusy == nullptr) return false;
	else return pModinf->IsBusy();
}

/******************************************************************************
 * Function: tlkmmi_audio_modinfIsBusy
 * Descript: Process the tlkmdi_audtone_intval function which indicate by the optype.
 * Params: @optype[IN]--The optype of audio.
 * Return: The interval value.
 * Others: None.
*******************************************************************************/
uint tlkmmi_audio_modinfIntval(TLKMMI_AUDIO_OPTYPE_ENUM optype)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf == nullptr || pModinf->Intval == nullptr) return 0;
	else return pModinf->Intval();
}

/******************************************************************************
 * Function: tlkmmi_audio_modinfIrqProc
 * Descript: Process the hantlkmdi_tone_handlerdler function which 
 *           indicate by the optype.
 * Params:@optype[IN]--The optype of audio.
 * Return: Operating results. true means success, others means failture.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfIrqProc(TLKMMI_AUDIO_OPTYPE_ENUM optype)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf == nullptr || pModinf->IrqProc == nullptr) return false;
	else return pModinf->IrqProc();
}



#if (TLKMDI_CFG_AUDTONE_ENABLE)
static const tlkmmi_audio_modinf_t sTlkMMidAudioToneModinf = {
	tlkmdi_audtone_start, //Start
	tlkmdi_audtone_close, //Close
	nullptr, //Timer
	nullptr, //ToNext
	nullptr, //ToNext
	tlkmdi_audtone_switch, //Switch
	tlkmdi_audtone_isBusy, //IsBusy
	tlkmdi_audtone_intval, //Intval
	tlkmdi_audtone_irqProc, //IrqProc
};
#endif //TLKMDI_CFG_AUDTONE_ENABLE
#if (TLKMDI_CFG_AUDPLAY_ENABLE)
static const tlkmmi_audio_modinf_t sTlkMMidAudioPlayModinf = {
	tlkmdi_audplay_start, //Start
	tlkmdi_audplay_close, //Close
	nullptr, //Timer
	tlkmdi_audplay_toNext, //ToNext
	tlkmdi_audplay_toPrev, //ToNext
	tlkmdi_audplay_switch, //Switch
	tlkmdi_audplay_isBusy, //IsBusy
	tlkmdi_audplay_intval, //Intval
	tlkmdi_audplay_irqProc, //IrqProc
};
#endif //#if (TLKMDI_CFG_AUDPLAY_ENABLE)
#if (TLKMDI_CFG_AUDHFP_ENABLE)
static const tlkmmi_audio_modinf_t sTlkMMidAudioHfModinf = {
	nullptr, //Start
	nullptr, //Close
	nullptr, //Timer
	nullptr, //ToNext
	nullptr, //ToNext
	tlkmdi_audhfp_switch, //Switch
	tlkmdi_audhfp_isBusy, //IsBusy
	tlkmdi_audhfp_intval, //Intval
	tlkmdi_audhfp_irqProc, //IrqProc
};
static const tlkmmi_audio_modinf_t sTlkMMidAudioAgModinf = {
	nullptr, //Start
	nullptr, //Close
	nullptr, //Timer
	nullptr, //ToNext
	nullptr, //ToNext
	tlkmdi_audhfp_switch, //Switch
	tlkmdi_audhfp_isBusy, //IsBusy
	tlkmdi_audhfp_intval, //Intval
	tlkmdi_audhfp_irqProc, //IrqProc
};
#endif //#if (TLKMDI_CFG_AUDHFP_ENABLE)
#if (TLKMDI_CFG_AUDSCO_ENABLE)
static const tlkmmi_audio_modinf_t sTlkMMidAudioScoModinf = {
	nullptr, //Start
	nullptr, //Close
	nullptr, //Timer
	nullptr, //ToNext
	nullptr, //ToNext
	tlkmdi_audsco_switch, //Switch
	tlkmdi_audsco_isBusy, //IsBusy
	tlkmdi_audsco_intval, //Intval
	tlkmdi_audsco_irqProc, //IrqProc
};
#endif //#if (TLKMDI_CFG_AUDSCO_ENABLE)
#if (TLKMDI_CFG_AUDSRC_ENABLE)
static const tlkmmi_audio_modinf_t sTlkMMidAudioSrcModinf = {
	tlkmdi_audsrc_start, //Start
	tlkmdi_audsrc_close, //Close
	tlkmdi_audsrc_timer, //Timer
	tlkmdi_audsrc_toNext, //ToNext
	tlkmdi_audsrc_toPrev, //ToNext
	tlkmdi_audsrc_switch, //Switch
	tlkmdi_audsrc_isBusy, //IsBusy
	tlkmdi_audsrc_intval, //Intval
	tlkmdi_audsrc_irqProc, //IrqProc
};
#endif //#if (TLKMDI_CFG_AUDSRC_ENABLE)
#if (TLKMDI_CFG_AUDSNK_ENABLE)
static const tlkmmi_audio_modinf_t sTlkMMidAudioSnkModinf = {
	tlkmdi_audsnk_start, //Start
	tlkmdi_audsnk_close, //Close
	tlkmdi_audsnk_timer, //Timer
	tlkmdi_audsnk_toNext, //ToNext
	tlkmdi_audsnk_toPrev, //ToNext
	tlkmdi_audsnk_switch, //Switch
	tlkmdi_audsnk_isBusy, //IsBusy
	tlkmdi_audsnk_intval, //Intval
	tlkmdi_audsnk_irqProc, //IrqProc
};
#endif //#if (TLKMDI_CFG_AUDSNK_ENABLE)
#if (TLKMDI_CFG_AUDUAC_ENABLE)
static const tlkmmi_audio_modinf_t sTlkMMidAudioUacModinf = {
	tlkmdi_auduac_start, //Start
	tlkmdi_auduac_close, //Close
	tlkmdi_auduac_timer, //Timer
	nullptr, //ToNext
	nullptr, //ToNext
	tlkmdi_auduac_switch, //Switch
	tlkmdi_auduac_isBusy, //IsBusy
	tlkmdi_auduac_intval, //Intval
	tlkmdi_auduac_irqProc, //IrqProc
};
#endif //#if (TLKMDI_CFG_AUDUAC_ENABLE)
static const tlkmmi_audio_modinf_t *spTlkMmiAudioModinfs[TLKMMI_AUDIO_OPTYPE_MAX] = {
	nullptr,
#if TLKMDI_CFG_AUDTONE_ENABLE
	&sTlkMMidAudioToneModinf,
#else
	nullptr,
#endif
#if TLKMDI_CFG_AUDPLAY_ENABLE
	&sTlkMMidAudioPlayModinf,
#else
	nullptr,
#endif
#if TLKMDI_CFG_AUDHFP_ENABLE
	&sTlkMMidAudioHfModinf,
	&sTlkMMidAudioAgModinf,
#else
	nullptr,
	nullptr,
#endif
#if TLKMDI_CFG_AUDSCO_ENABLE
	&sTlkMMidAudioScoModinf,
#else
	nullptr,
#endif
#if TLKMDI_CFG_AUDSRC_ENABLE
	&sTlkMMidAudioSrcModinf,
#else
	nullptr,
#endif
#if TLKMDI_CFG_AUDSNK_ENABLE
	&sTlkMMidAudioSnkModinf,
#else
	nullptr,
#endif
#if TLKMDI_CFG_AUDUAC_ENABLE
	&sTlkMMidAudioUacModinf,
#else
	nullptr,
#endif
};

/******************************************************************************
 * Function: tlkmmi_audio_getModinf
 * Descript: Get the interface of Tone module 
 * Params:@optype[IN]--The optype of audio.
 * Return: The interface function means success, others means failture.
 * Others: None.
*******************************************************************************/
const tlkmmi_audio_modinf_t *tlkmmi_audio_getModinf(TLKMMI_AUDIO_OPTYPE_ENUM optype)
{
	if(optype >= TLKMMI_AUDIO_OPTYPE_MAX) return nullptr;
	return spTlkMmiAudioModinfs[optype];
}

#endif //#if (TLKMMI_AUDIO_ENABLE)
