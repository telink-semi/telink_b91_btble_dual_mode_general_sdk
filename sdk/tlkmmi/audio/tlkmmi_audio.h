/********************************************************************************************************
 * @file     tlkmmi_audio.h
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

#ifndef TLKMMI_AUDIO_H
#define TLKMMI_AUDIO_H

#if (TLKMMI_AUDIO_ENABLE)


#define TLKMMI_AUDIO_TIMEOUT          100000 //200ms
#define TLKMMI_AUDIO_TIMEOUT_MS       100
#define TLKMMI_AUDIO_TIMER_TIMEOUT    (3000000/TLKMMI_AUDIO_TIMEOUT) //Prevent timer termination, which may cause problems

#define TLKMMI_AUDIO_INFO_TIMEOUT     (5000000/TLKMMI_AUDIO_TIMEOUT) //5s


#define TLKMMI_AUDIO_DBG_FLAG         (TLKMMI_AUDIO_DBG_ENABLE | TLKAPI_DBG_FLAG_ALL)
#define TLKMMI_AUDIO_DBG_SIGN         "[MMI]"



typedef enum{
	TLKMMI_AUDIO_OPTYPE_NONE = 0,
	TLKMMI_AUDIO_OPTYPE_TONE, //Play the prompt tone locally
	TLKMMI_AUDIO_OPTYPE_PLAY, //Play the music locally
	TLKMMI_AUDIO_OPTYPE_HF,
	TLKMMI_AUDIO_OPTYPE_AG,
	TLKMMI_AUDIO_OPTYPE_SCO, //Status control of SCO link establishment. SCO is commonly used in phone calls or speech recognition.
	TLKMMI_AUDIO_OPTYPE_SRC, //Connect the headset and play music to the headset
	TLKMMI_AUDIO_OPTYPE_SNK, //Connect a mobile phone and listen to music on the mobile phone.
	TLKMMI_AUDIO_OPTYPE_MAX,
}TLKMMI_AUDIO_OPTYPE_ENUM;



/******************************************************************************
 * Function: tlkmmi_audio_init
 * Descript: Initial the audio path and audio ctrl flow, set the audio paramter.
 * Params:
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_init(void);

/******************************************************************************
 * Function: tlkmmi_audio_isBusy
 * Descript:
 * Params:
 * Return: true/false
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_isBusy(void);

void tlkmmi_audio_connect(uint16 handle, uint08 ptype, uint08 usrID);

/******************************************************************************
 * Function: tlkmmi_audio_disconn
 * Descript:
 * Params:
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_disconn(uint16 handle);

/******************************************************************************
 * Function: tlkmmi_audio_insertStatus
 * Descript: Suspend the current audio task when a new audio task insert 
 *           and start running.
 * Params:
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
extern int tlkmmi_audio_insertStatus(uint16 aclHandle, uint08 optype);

/******************************************************************************
 * Function: tlkmmi_audio_removeStatus
 * Descript: Suspend the current task and delete it.Then resume the task 
 *           which been suspend before.
 * Params:
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
extern int tlkmmi_audio_removeStatus(uint16 aclHandle, uint08 optype);

/******************************************************************************
 * Function: tlkmmi_audio_isLocalPlay
 * Descript: Checks whether the current playback is local.
 * Params: None.
 * Return: TRUE-local play, false-other play.
 * Others: None.
*******************************************************************************/
extern bool tlkmmi_audio_isLocalPlay(void);

/******************************************************************************
 * Function: tlkmmi_audio_stopLocalPlay
 * Descript: Suspend the current music and adjust the audio handle and it's 
 *           operation type out of the audio status control list.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
extern void tlkmmi_audio_stopLocalPlay(void);

extern bool tlkmmi_audio_startTone(uint16 fileIndex, uint16 playCount);


#endif //#if (TLKMMI_AUDIO_ENABLE)

#endif //TLKMMI_AUDIO_H

