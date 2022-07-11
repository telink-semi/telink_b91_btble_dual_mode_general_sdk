/********************************************************************************************************
 * @file     tlkusb_audDesc.h
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
#ifndef TLKUSB_AUDIO_DESC_H
#define TLKUSB_AUDIO_DESC_H

#if (TLK_USB_AUD_ENABLE)



#define TLKUSB_AUD_STRING_PRODUCT       L"Telink B91 BTBLE Audio"

#define TLKUSB_AUD_STRING_SERIAL0       L"TLSR9218-16000"
#define TLKUSB_AUD_STRING_SERIAL1       L"TLSR9218-32000"
#define TLKUSB_AUD_STRING_SERIAL2       L"TLSR9218-44100"
#define TLKUSB_AUD_STRING_SERIAL3       L"TLSR9218-48000"


typedef struct {
	tlkusb_stdConfigureDesc_t config;
	tlkusb_stdInterfaceDesc_t audCtrInf;
	tlkusb_audInterfaceAcDesc_t audCtrAcInf;
	tlkusb_audInputDesc_t   micInputDesc;
	tlkusb_audMicFeatureDesc_t micFeatureDesc;
	tlkusb_audOutputDesc_t  micOutputDesc;
	tlkusb_stdInterfaceDesc_t micSetting0Inf;
	tlkusb_stdInterfaceDesc_t micSetting1Inf;
	tlkusb_audInterfaceAsDesc_t micDatAsInf;
	tlkusb_audFormatDesc_t micFormatDesc;
	tlkusb_audSampleDesc_t micSampleDesc;
	tlkusb_audStdEndpointDesc_t micStdEdpDesc;
	tlkusb_audSpcEndpointDesc_t micSpcEdpDesc;
}tlkusb_audAudConfigDesc_t;







#endif //#if (TLK_USB_AUD_ENABLE)

#endif //TLKUSB_AUDIO_DESC_H

