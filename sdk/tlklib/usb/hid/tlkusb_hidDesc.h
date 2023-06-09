/********************************************************************************************************
 * @file     tlkusb_msc.h
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
#ifndef TLKUSB_HID_DESC_H
#define TLKUSB_HID_DESC_H

#if (TLK_USB_HID_ENABLE)




#define TLKUSB_HID_STRING_PRODUCT       L"Telink B91 BTBLE HID"
#define TLKUSB_HID_STRING_SERIAL        L"TLSR9218-HID"



typedef struct {
	tlkusb_stdConfigureDesc_t config;
	#if (TLKUSB_HID_MOUSE_ENABLE)
	tlkusb_stdInterfaceDesc_t mouseInf;
	tlkusb_HidEndpointDesc_t  mouseHid;
	tlkusb_stdEndpointDesc_t  mouseEdp;
	#endif
	#if (TLKUSB_HID_KEYBOARD_ENABLE)
	tlkusb_stdInterfaceDesc_t keyboardInf;
	tlkusb_HidEndpointDesc_t  keyboardHid;
	tlkusb_stdEndpointDesc_t  keyboardEdp;
	#endif
}tlkusb_hidConfigDesc_t;





#endif //#if (TLK_USB_HID_ENABLE)

#endif //TLKUSB_HID_DESC_H


