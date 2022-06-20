/********************************************************************************************************
 * @file     tlkmmi_audioModinf.h
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

#ifndef TLKMMI_AUDIO_MODINF_H
#define TLKMMI_AUDIO_MODINF_H

#if (TLKMMI_AUDIO_ENABLE)



typedef struct{
	int (*Start)(uint16 handle, uint32 param);
	int (*Close)(uint16 handle);
	void(*Timer)(void);
	bool(*ToNext)(void);
	bool(*ToPrev)(void);
	bool(*Switch)(uint16 handle, uint08 status);
	bool(*IsBusy)(void);
	uint(*Intval)(void); //Interval
	bool(*IrqProc)(void);
}tlkmmi_audio_modinf_t;


int tlkmmi_audio_modinfStart(TLKMMI_AUDIO_OPTYPE_ENUM optype, uint16 handle, uint32 param);
int tlkmmi_audio_modinfClose(TLKMMI_AUDIO_OPTYPE_ENUM optype, uint16 handle);

void tlkmmi_audio_modinfTimer(TLKMMI_AUDIO_OPTYPE_ENUM optype);


/******************************************************************************
 * Function: tlkmmi_audio_modinfSwitch
 * Descript: Switching audio State.
 * Params:
 *        @optype[IN]--The optype of audio.
 *        @handle[IN]--The handle of audio.
 *        @status[IN]--The status of audio.
 * Return: Operating results. true means success, others means failture.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfSwitch(TLKMMI_AUDIO_OPTYPE_ENUM optype, uint16 handle, uint08 status);

/******************************************************************************
 * Function: tlkmmi_audio_modinfToNext
 * Descript: Process the tlkmdi_audtone_isBusy function which indicate by the optype.
 * Params: 
 *        @optype[IN]--The optype of audio.
 * Return: Operating results. true means success, others means failture.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfToNext(TLKMMI_AUDIO_OPTYPE_ENUM optype);

/******************************************************************************
 * Function: tlkmmi_audio_modinfToPrev
 * Descript: .
 * Params: 
 *        @optype[IN]--The optype of audio.
 * Return: Operating results. true means success, others means failture.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfToPrev(TLKMMI_AUDIO_OPTYPE_ENUM optype);

/******************************************************************************
 * Function: tlkmmi_audio_modinfIsBusy
 * Descript: Process the tlkmdi_audtone_isBusy function which indicate by the optype.
 * Params: @optype[IN]--The optype of audio.
 * Return: Operating results. true means success, others means failture.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfIsBusy(TLKMMI_AUDIO_OPTYPE_ENUM optype);

/******************************************************************************
 * Function: tlkmmi_audio_modinfIsBusy
 * Descript: Process the tlkmdi_audtone_intval function which indicate by the optype.
 * Params: @optype[IN]--The optype of audio.
 * Return: The interval value.
 * Others: None.
*******************************************************************************/
uint tlkmmi_audio_modinfIntval(TLKMMI_AUDIO_OPTYPE_ENUM optype);

/******************************************************************************
 * Function: tlkmmi_audio_modinfIrqProc
 * Descript: Process the hantlkmdi_tone_handlerdler function which 
 *           indicate by the optype.
 * Params:@optype[IN]--The optype of audio.
 * Return: Operating results. true means success, others means failture.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfIrqProc(TLKMMI_AUDIO_OPTYPE_ENUM optype);

/******************************************************************************
 * Function: tlkmmi_audio_getModinf
 * Descript: Get the interface of Tone module 
 * Params:@optype[IN]--The optype of audio.
 * Return: The interface function means success, others means failture.
 * Others: None.
*******************************************************************************/
const tlkmmi_audio_modinf_t *tlkmmi_audio_getModinf(TLKMMI_AUDIO_OPTYPE_ENUM optype);


#endif //#if (TLKMMI_AUDIO_ENABLE)

#endif //TLKMMI_AUDIO_MODINF_H

