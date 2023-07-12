/********************************************************************************************************
 * @file	tlkmdi_usb.h
 *
 * @brief	This is the header file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *
 *******************************************************************************************************/
#ifndef TLKMDI_USB_H
#define TLKMDI_USB_H

#if (TLK_MDI_USB_ENABLE)




int tlkmdi_usb_init(void);


void tlkmdi_usb_shutdown(void);

void tlkmdi_usb_handler(void);



#endif //#if (TLK_MDI_USB_ENABLE)

#endif //TLKMDI_USB_H

