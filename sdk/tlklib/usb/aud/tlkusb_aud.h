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


extern void tlkusb_audmic_autoZero(bool enable);
extern void tlkusb_audspk_autoFlush(bool enable);

extern bool tlkusb_audmic_sendData(uint08 *pData, uint16 dataLen, bool isCover);
extern uint tlkusb_audspk_readData(uint08 *pBuff, uint08 buffLen, bool isParty);




#endif //#if (TLK_USB_AUD_ENABLE)

#endif //TLKUSB_AUDIO_H

