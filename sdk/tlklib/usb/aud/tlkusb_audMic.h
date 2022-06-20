/********************************************************************************************************
 * @file     tlkusb_audMic.h
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
#ifndef TLKUSB_AUDIO_MIC_H
#define TLKUSB_AUDIO_MIC_H

#if (TLKUSB_AUD_MIC_ENABLE)


#define TLKUSB_AUDMIC_VOL_MIN      ((sint16) 0x0000) /* Volume Minimum Value */
#define TLKUSB_AUDMIC_VOL_MAX      ((sint16) 0x1e00) /* Volume Maximum Value */
#define	TLKUSB_AUDMIC_VOL_RES      0x0180 /* Volume Resolution */
#define TLKUSB_AUDMIC_VOL_DEF      0x1800 /* Volume default */
#define	TLKUSB_AUDMIC_VOL_STEP     (TLKUSB_AUDMIC_VOL_MAX / TLKUSB_AUDMIC_VOL_RES)

typedef struct {
	uint08 mute;
	uint16 curVol;
	uint16 volStep;
}tlkusb_audmic_ctrl_t;



int tlkusb_audmic_init(void);


int tlkusb_audmic_d2hClassInfHandler(tlkusb_setup_req_t *pSetup, uint08 infNum);
int tlkusb_audmic_d2hClassEdpHandler(tlkusb_setup_req_t *pSetup, uint08 edpNum);

int tlkusb_audmic_h2dClassInfHandler(tlkusb_setup_req_t *pSetup, uint08 infNum);
int tlkusb_audmic_h2dClassEdpHandler(tlkusb_setup_req_t *pSetup, uint08 edpNum);


uint tlkusb_audmic_getVolume(void);
void tlkusb_audmic_setVolume(sint16 volume);
void tlkusb_audmic_enterMute(bool enable);


int tlkusb_audmic_setCmdDeal(int type);
int tlkusb_audmic_getCmdDeal(int req, int type);



#endif //#if (TLKUSB_AUD_MIC_ENABLE)

#endif //TLKUSB_AUDIO_MIC_H

