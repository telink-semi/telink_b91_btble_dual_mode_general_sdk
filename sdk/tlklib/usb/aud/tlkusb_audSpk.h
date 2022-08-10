/********************************************************************************************************
 * @file     tlkusb_audSpk.h
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
#ifndef TLKUSB_AUDIO_SPK_H
#define TLKUSB_AUDIO_SPK_H

#if (TLKUSB_AUD_SPK_ENABLE)

#define	TLKUSB_AUDSPK_VOL_MIN       ((sint16) 0xa000)     /* Volume Minimum Value */
#define	TLKUSB_AUDSPK_VOL_MAX       ((sint16) 0x0300)     /* Volume Maximum Value */
#define TLKUSB_AUDSPK_VOL_RES       0x0180    /* Volume Resolution */
#define TLKUSB_AUDSPK_VOL_DEF       0x8000    /* Volume default */
#define TLKUSB_AUDSPK_VOL_STEP      400


typedef struct {
	uint08 mute;
	uint16 curVol;
	uint16 volStep;
	uint32 sampleRate;
}tlkusb_audspk_ctrl_t;


int tlkusb_audspk_init(void);


int tlkusb_audspk_d2hClassInfHandler(tlkusb_setup_req_t *pSetup, uint08 infNum);
int tlkusb_audspk_d2hClassEdpHandler(tlkusb_setup_req_t *pSetup, uint08 edpNum);

int tlkusb_audspk_h2dClassInfHandler(tlkusb_setup_req_t *pSetup, uint08 infNum);
int tlkusb_audspk_h2dClassEdpHandler(tlkusb_setup_req_t *pSetup, uint08 edpNum);

uint tlkusb_audspk_getVolume(void);
void tlkusb_audspk_setVolume(uint16 volume);
void tlkusb_audspk_enterMute(bool enable);

int tlkusb_audspk_setInfCmdDeal(int type);
int tlkusb_audspk_getInfCmdDeal(int req, int type);

int tlkusb_audspk_setEdpCmdDeal(int type);



#endif //#if (TLKUSB_AUD_SPK_ENABLE)

#endif //TLKUSB_AUDIO_SPK_H

