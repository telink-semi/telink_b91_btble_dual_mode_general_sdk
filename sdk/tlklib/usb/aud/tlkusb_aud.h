/********************************************************************************************************
 * @file     tlkusb_aud.h
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
#ifndef TLKUSB_AUDIO_H
#define TLKUSB_AUDIO_H

#if (TLK_USB_AUD_ENABLE)



int tlkusb_aud_init(void);


int tlkusb_aud_d2hClassInfHandler(tlkusb_setup_req_t *pSetup, uint08 infNum);
int tlkusb_aud_d2hClassEdpHandler(tlkusb_setup_req_t *pSetup, uint08 edpNum);
int tlkusb_aud_h2dClassInfHandler(tlkusb_setup_req_t *pSetup, uint08 infNum);
int tlkusb_aud_h2dClassEdpHandler(tlkusb_setup_req_t *pSetup, uint08 edpNum);


#if (TLKUSB_AUD_MIC_ENABLE)
extern int tlkusb_audmic_d2hClassInfHandler(tlkusb_setup_req_t *pSetup, uint08 infNum);
extern int tlkusb_audmic_d2hClassEdpHandler(tlkusb_setup_req_t *pSetup, uint08 edpNum);
extern int tlkusb_audmic_h2dClassInfHandler(tlkusb_setup_req_t *pSetup, uint08 infNum);
extern int tlkusb_audmic_h2dClassEdpHandler(tlkusb_setup_req_t *pSetup, uint08 edpNum);
#endif //#if (TLKUSB_AUD_MIC_ENABLE)

#if (TLKUSB_AUD_SPK_ENABLE)
extern int tlkusb_audspk_d2hClassInfHandler(tlkusb_setup_req_t *pSetup, uint08 infNum);
extern int tlkusb_audspk_d2hClassEdpHandler(tlkusb_setup_req_t *pSetup, uint08 edpNum);
extern int tlkusb_audspk_h2dClassInfHandler(tlkusb_setup_req_t *pSetup, uint08 infNum);
extern int tlkusb_audspk_h2dClassEdpHandler(tlkusb_setup_req_t *pSetup, uint08 edpNum);
#endif //#if (TLKUSB_AUD_SPK_ENABLE)



#endif //#if (TLK_USB_AUD_ENABLE)

#endif //TLKUSB_AUDIO_H

